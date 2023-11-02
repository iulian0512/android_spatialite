/*

 virtualknn.c -- SQLite3 extension [VIRTUAL TABLE RTree metahandler]

 version 5.1.0, 2023 August 4

 Author: Sandro Furieri a.furieri@lqt.it

 -----------------------------------------------------------------------------
 
 Version: MPL 1.1/GPL 2.0/LGPL 2.1
 
 The contents of this file are subject to the Mozilla Public License Version
 1.1 (the "License"); you may not use this file except in compliance with
 the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/
 
Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the
License.

The Original Code is the SpatiaLite library

The Initial Developer of the Original Code is Alessandro Furieri
 
Portions created by the Initial Developer are Copyright (C) 2015-2023
the Initial Developer. All Rights Reserved.

Contributor(s):

Alternatively, the contents of this file may be used under the terms of
either the GNU General Public License Version 2 or later (the "GPL"), or
the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
in which case the provisions of the GPL or the LGPL are applicable instead
of those above. If you wish to allow use of your version of this file only
under the terms of either the GPL or the LGPL, and not to allow others to
use your version of this file under the terms of the MPL, indicate your
decision by deleting the provisions above and replace them with the notice
and other provisions required by the GPL or the LGPL. If you do not delete
the provisions above, a recipient may use your version of this file under
the terms of any one of the MPL, the GPL or the LGPL.
 
*/

/*
 
DEPRECATION:

on June 2021 this module was definitely abandoned due to efficiency
considerations and architectural issues.
 
it's now fully replaced by virtualknn2
 
this skeletal implementation is still preserved just to avoid
problems when connecting any DB-file created by previous versions
containing VirtualKNN tables.

*/

/*
 
CREDITS:

this module has been completely funded by:
Regione Toscana - Settore Sistema Informativo Territoriale ed Ambientale
(K-Nearest Neighbors [KNN] module) 

CIG: 644544015A

*/

/*

IMPORTANT NOTE: how KNN works

the KNN module is implemented on the top of an SQLite's R*Tree, and
more specifically is built around the sqlite3_rtree_query_callback()
function.

this API allows to directly explore the R*Tree hierarchy; for each
node found within the Tree the registered callback will be invoked
so to determine if the current Tree node (aka BBOX) should be further
expanded or should be ignored.
while the process goes on all Tree levels will be traversed until it
reaches Level=0 (Leaves aka Terminal Nodes) where the indexed
Geometries will be finally referenced by their individual ROWID and BBOX

in a most usual Spatial Index query the R*Tree will be simply
traversed starting from Root Nodes (higher level) and progressively
descending level by level toward Leaf Nodes (indexed Geometries); in
this specific case the BBOX to be searched is clearly defined, so the
callback function should simply check if each Node BBOX do effectively
intersect the reference BBOX and that's all.

the access strategy required by a KNN query is much more complex than
this, because there is no reference BBOX at all (the search radius is
unknown in this case); so it should be dynamically built by repeatedly
querying the R*Tree until a satisfying solution is found.

step #1
-------
we'll descend the Tree by exploring a Level at each time so to
identify the lower-level BBOX containing the reference Geometry.
if no BBOX directly contains the reference Geometry then the
nearest one will be identified.

step #2
-------
the reference Geometry will now be buffered so to completely
cover the BBOX identified in the previous step.

step #3
-------
an ordinary Spatial Index query will be now performed, using
the buffered Geometry as the search frame, so to identify
all nearest features.

step #4
-------
if the total number of nearest features identified by the
previous query is less than requested, then the buffer
radius will be repeatedly expanded until all requested
nearest features will be found.

*/

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#if defined(_WIN32) && !defined(__MINGW32__)
#include "config-msvc.h"
#else
#include "config.h"
#endif

#include <spatialite/sqlite.h>

#include <spatialite/spatialite_ext.h>
#include <spatialite/gaiaaux.h>
#include <spatialite/gaiageo.h>

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp    _stricmp
#define strncasecmp    _strnicmp
#endif

static struct sqlite3_module my_knn_module;

typedef struct VirtualKnnStruct
{
/* extends the sqlite3_vtab struct */
    const sqlite3_module *pModule;	/* ptr to sqlite module: USED INTERNALLY BY SQLITE */
    int nRef;			/* # references: USED INTERNALLY BY SQLITE */
    char *zErrMsg;		/* error message: USE INTERNALLY BY SQLITE */
    sqlite3 *db;		/* the sqlite db holding the virtual table */
} VirtualKnn;
typedef VirtualKnn *VirtualKnnPtr;

typedef struct VirtualKnnCursorStruct
{
/* extends the sqlite3_vtab_cursor struct */
    VirtualKnnPtr pVtab;	/* Virtual table of this cursor */
} VirtualKnnCursor;
typedef VirtualKnnCursor *VirtualKnnCursorPtr;

static int
vknn_create (sqlite3 * db, void *pAux, int argc, const char *const *argv,
	     sqlite3_vtab ** ppVTab, char **pzErr)
{
/* creates the virtual table for R*Tree KNN metahandling */
    VirtualKnnPtr p_vt;
    char *buf;
    char *vtable;
    char *xname;
    if (pAux)
	pAux = pAux;		/* unused arg warning suppression */
    if (argc == 3)
      {
	  vtable = gaiaDequotedSql ((char *) argv[2]);
      }
    else
      {
	  *pzErr =
	      sqlite3_mprintf
	      ("[VirtualKNN module] CREATE VIRTUAL: illegal arg list {void}\n");
	  return SQLITE_ERROR;
      }
    p_vt = (VirtualKnnPtr) sqlite3_malloc (sizeof (VirtualKnn));
    if (!p_vt)
	return SQLITE_NOMEM;
    p_vt->db = db;
    p_vt->pModule = &my_knn_module;
    p_vt->nRef = 0;
    p_vt->zErrMsg = NULL;
/* preparing the COLUMNs for this VIRTUAL TABLE */
    xname = gaiaDoubleQuotedSql (vtable);
    buf = sqlite3_mprintf ("CREATE TABLE \"%s\" (f_table_name TEXT, "
			   "f_geometry_column TEXT, ref_geometry BLOB, max_items INTEGER, "
			   "pos INTEGER, fid INTEGER, distance DOUBLE)", xname);
    free (xname);
    free (vtable);
    if (sqlite3_declare_vtab (db, buf) != SQLITE_OK)
      {
	  sqlite3_free (buf);
	  *pzErr =
	      sqlite3_mprintf
	      ("[VirtualKNN module] CREATE VIRTUAL: invalid SQL statement \"%s\"",
	       buf);
	  return SQLITE_ERROR;
      }
    sqlite3_free (buf);
    *ppVTab = (sqlite3_vtab *) p_vt;
    return SQLITE_OK;
}

static int
vknn_connect (sqlite3 * db, void *pAux, int argc, const char *const *argv,
	      sqlite3_vtab ** ppVTab, char **pzErr)
{
/* connects the virtual table - simply aliases vknn_create() */
    return vknn_create (db, pAux, argc, argv, ppVTab, pzErr);
}

static int
vknn_best_index (sqlite3_vtab * pVTab, sqlite3_index_info * pIdxInfo)
{
/* best index selection */
    int i;
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    pIdxInfo->estimatedCost = 1.0;
    for (i = 0; i < pIdxInfo->nConstraint; i++)
      {
	  if (pIdxInfo->aConstraint[i].usable)
	    {
		pIdxInfo->aConstraintUsage[i].argvIndex = i + 1;
		pIdxInfo->aConstraintUsage[i].omit = 1;
	    }
      }
    pIdxInfo->idxNum = 0;
    return SQLITE_OK;
}

static int
vknn_disconnect (sqlite3_vtab * pVTab)
{
/* disconnects the virtual table */
    VirtualKnnPtr p_vt = (VirtualKnnPtr) pVTab;
    sqlite3_free (p_vt);
    return SQLITE_OK;
}

static int
vknn_destroy (sqlite3_vtab * pVTab)
{
/* destroys the virtual table - simply aliases vknn_disconnect() */
    return vknn_disconnect (pVTab);
}

static int
vknn_open (sqlite3_vtab * pVTab, sqlite3_vtab_cursor ** ppCursor)
{
/* opening a new cursor */
    VirtualKnnCursorPtr cursor =
	(VirtualKnnCursorPtr) sqlite3_malloc (sizeof (VirtualKnnCursor));
    if (cursor == NULL)
	return SQLITE_ERROR;
    cursor->pVtab = (VirtualKnnPtr) pVTab;
    *ppCursor = (sqlite3_vtab_cursor *) cursor;
    return SQLITE_OK;
}

static int
vknn_close (sqlite3_vtab_cursor * pCursor)
{
/* closing the cursor */
    sqlite3_free (pCursor);
    return SQLITE_OK;
}

static int
vknn_filter (sqlite3_vtab_cursor * pCursor, int idxNum, const char *idxStr,
	     int argc, sqlite3_value ** argv)
{
/* setting up a cursor filter */
    if (pCursor || idxStr || idxNum || argc || argv)
	idxStr = idxStr;	/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn_next (sqlite3_vtab_cursor * pCursor)
{
/* fetching a next row from cursor */
    if (pCursor)
	pCursor = pCursor;	/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn_eof (sqlite3_vtab_cursor * pCursor)
{
/* cursor EOF */
    if (pCursor)
	pCursor = pCursor;	/* unused arg warning suppression */
    return 1;
}

static int
vknn_column (sqlite3_vtab_cursor * pCursor, sqlite3_context * pContext,
	     int column)
{
/* fetching value for the Nth column */
    if (pCursor || column)
	pCursor = pCursor;	/* unused arg warning suppression */
    sqlite3_result_null (pContext);
    return SQLITE_OK;
}

static int
vknn_rowid (sqlite3_vtab_cursor * pCursor, sqlite_int64 * pRowid)
{
/* fetching the ROWID */
    if (pCursor)
	pCursor = pCursor;	/* unused arg warning suppression */
    *pRowid = 0;
    return SQLITE_OK;
}

static int
vknn_update (sqlite3_vtab * pVTab, int argc, sqlite3_value ** argv,
	     sqlite_int64 * pRowid)
{
/* generic update [INSERT / UPDATE / DELETE */
    if (pRowid || argc || argv || pVTab)
	pRowid = pRowid;	/* unused arg warning suppression */
/* read only datasource */
    return SQLITE_READONLY;
}

static int
vknn_begin (sqlite3_vtab * pVTab)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn_sync (sqlite3_vtab * pVTab)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn_commit (sqlite3_vtab * pVTab)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn_rollback (sqlite3_vtab * pVTab)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn_rename (sqlite3_vtab * pVTab, const char *zNew)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    if (zNew)
	zNew = zNew;		/* unused arg warning suppression */
    return SQLITE_ERROR;
}

static int
spliteKnnInit (sqlite3 * db)
{
    int rc = SQLITE_OK;
    my_knn_module.iVersion = 1;
    my_knn_module.xCreate = &vknn_create;
    my_knn_module.xConnect = &vknn_connect;
    my_knn_module.xBestIndex = &vknn_best_index;
    my_knn_module.xDisconnect = &vknn_disconnect;
    my_knn_module.xDestroy = &vknn_destroy;
    my_knn_module.xOpen = &vknn_open;
    my_knn_module.xClose = &vknn_close;
    my_knn_module.xFilter = &vknn_filter;
    my_knn_module.xNext = &vknn_next;
    my_knn_module.xEof = &vknn_eof;
    my_knn_module.xColumn = &vknn_column;
    my_knn_module.xRowid = &vknn_rowid;
    my_knn_module.xUpdate = &vknn_update;
    my_knn_module.xBegin = &vknn_begin;
    my_knn_module.xSync = &vknn_sync;
    my_knn_module.xCommit = &vknn_commit;
    my_knn_module.xRollback = &vknn_rollback;
    my_knn_module.xFindFunction = NULL;
    my_knn_module.xRename = &vknn_rename;
    sqlite3_create_module_v2 (db, "VirtualKNN", &my_knn_module, NULL, 0);
    return rc;
}

SPATIALITE_PRIVATE int
virtual_knn_extension_init (void *xdb)
{
    sqlite3 *db = (sqlite3 *) xdb;
    return spliteKnnInit (db);
}
