/*

 virtualknn2.c -- SQLite3 extension [VIRTUAL TABLE KNN metahandler]

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
 
IMPORTANT NOTICE:

a similar module (VirtualKNN) was supported by previos versions.
 
this is a full replacement based on a completerly different algorithm
and presenting a substantially different interface.

now a MaxDistance Radius is absolutely required and the algorithm
has the capability to automatically expand it when required.

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

#ifndef OMIT_GEOS		/* GEOS is supported */

#include <spatialite/sqlite.h>

#include <spatialite/spatialite_ext.h>
#include <spatialite/gaiaaux.h>
#include <spatialite/gaiageo.h>

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp    _stricmp
#define strncasecmp    _strnicmp
#endif

static struct sqlite3_module my_knn2_module;

/******************************************************************************
/
/ VirtualTable structs
/
******************************************************************************/

typedef struct VKnn2ItemStruct
{
/* a Feature item into the KNN2 array */
    int ok;
    sqlite3_int64 rowid;
    double radius;
    double dist_crs;
    double dist_m;
} VKnn2Item;
typedef VKnn2Item *VKnn2ItemPtr;

typedef struct VKnn2ContextStruct
{
/* current KNN2 context */
    int valid;
    char *db_prefix;
    char *table_name;
    char *column_name;
    int is_geographic;
    unsigned char *blob;
    int blob_size;
    double point_x;
    double point_y;
    double radius;
    int expand;
    VKnn2ItemPtr knn2_array;
    int max_items;
    int next_item;
} VKnn2Context;
typedef VKnn2Context *VKnn2ContextPtr;

typedef struct VirtualKnn2Struct
{
/* extends the sqlite3_vtab struct */
    const sqlite3_module *pModule;	/* ptr to sqlite module: USED INTERNALLY BY SQLITE */
    int nRef;			/* # references: USED INTERNALLY BY SQLITE */
    char *zErrMsg;		/* error message: USED INTERNALLY BY SQLITE */
    sqlite3 *db;		/* the sqlite db holding the virtual table */
    VKnn2ContextPtr knn2_ctx;	/* KNN2 context */
} VirtualKnn2;
typedef VirtualKnn2 *VirtualKnn2Ptr;

typedef struct VirtualKnn2CursorStruct
{
/* extends the sqlite3_vtab_cursor struct */
    VirtualKnn2Ptr pVtab;	/* Virtual table of this cursor */
    int eof;			/* the EOF marker */
    int CurrentIndex;		/* index of the current KNN2 item */
} VirtualKnn2Cursor;
typedef VirtualKnn2Cursor *VirtualKnn2CursorPtr;

static void
vknn2_empty_context (VKnn2ContextPtr ctx)
{
/* setting an empty KNN2 context */
    if (ctx == NULL)
	return;
    ctx->valid = 0;
    ctx->db_prefix = NULL;
    ctx->table_name = NULL;
    ctx->column_name = NULL;
    ctx->is_geographic = 0;
    ctx->blob = NULL;
    ctx->blob_size = 0;
    ctx->point_x = 0.0;
    ctx->point_y = 0.0;
    ctx->radius = 0.0;
    ctx->expand = 0;
    ctx->max_items = 0;
    ctx->knn2_array = NULL;
    ctx->next_item = 0;
}

static VKnn2ContextPtr
vknn2_create_context (void)
{
/* creating an empty KNN2 context */
    VKnn2ContextPtr ctx = malloc (sizeof (VKnn2Context));
    vknn2_empty_context (ctx);
    return ctx;
}

static void
vknn2_reset_context (VKnn2ContextPtr ctx)
{
/* freeing a KNN2 context */
    if (ctx == NULL)
	return;
    if (ctx->db_prefix != NULL)
	free (ctx->db_prefix);
    if (ctx->table_name != NULL)
	free (ctx->table_name);
    if (ctx->column_name != NULL)
	free (ctx->column_name);
    if (ctx->blob != NULL)
	free (ctx->blob);
    if (ctx->knn2_array != NULL)
	free (ctx->knn2_array);
    vknn2_empty_context (ctx);
}

static void
vknn2_clear_context (VKnn2ContextPtr ctx)
{
/* clearing a KNN2 context */
    int i;
    ctx->next_item = 0;
    for (i = 0; i < ctx->max_items; i++)
      {
	  /* initializing the KNN2 array */
	  VKnn2ItemPtr item = ctx->knn2_array + i;
	  item->ok = 0;
	  item->rowid = 0;
	  item->radius = DBL_MAX;
	  item->dist_crs = DBL_MAX;
	  item->dist_m = DBL_MAX;
      }
}

static void
vknn2_init_context (VKnn2ContextPtr ctx, const char *db_prefix,
		    char *table, char *column, int is_geographic,
		    gaiaGeomCollPtr geom, double point_x, double point_y,
		    double radius, int max_items, int expand)
{
/* initializing a KNN2 context */
    int i;
    if (ctx == NULL)
	return;
    vknn2_reset_context (ctx);
    if (db_prefix == NULL)
	db_prefix = "MAIN";
    i = strlen (db_prefix);
    ctx->db_prefix = malloc (i + 1);
    strcpy (ctx->db_prefix, db_prefix);
    ctx->table_name = table;
    ctx->column_name = column;
    ctx->is_geographic = is_geographic;
    gaiaToSpatiaLiteBlobWkb (geom, &(ctx->blob), &(ctx->blob_size));
    ctx->point_x = point_x;
    ctx->point_y = point_y;
    ctx->radius = radius;
    ctx->max_items = max_items;
    ctx->expand = expand;
    ctx->knn2_array = malloc (sizeof (VKnn2Item) * max_items);
    for (i = 0; i < max_items; i++)
      {
	  /* initializing the KNN2 array */
	  VKnn2ItemPtr item = ctx->knn2_array + i;
	  item->ok = 0;
	  item->rowid = 0;
	  item->radius = DBL_MAX;
	  item->dist_crs = DBL_MAX;
	  item->dist_m = DBL_MAX;
      }
    ctx->next_item = 0;
    ctx->valid = 1;
}

static void
vknn2_free_context (void *p)
{
/* freeing a KNN2 context */
    VKnn2ContextPtr ctx = (VKnn2ContextPtr) p;
    vknn2_reset_context (ctx);
    free (ctx);
}

static int
vknn2_check_view (sqlite3 * sqlite, const char *db_prefix,
		  const char *table_name, const char *geom_column,
		  char **real_table, char **real_geom, int *is_geographic)
{
/* checks if the required Spatial View is actually defined */
    sqlite3_stmt *stmt;
    char *sql_statement;
    int ret;
    int count = 0;
    char *rt = NULL;
    char *rg = NULL;
    int is_longlat = 0;

/* testing if views_geometry_columns exists */
    if (db_prefix == NULL)
      {
	  sql_statement =
	      sqlite3_mprintf ("SELECT tbl_name FROM main.sqlite_master "
			       "WHERE type = 'table' AND tbl_name = 'views_geometry_columns'");
      }
    else
      {
	  char *quoted_db = gaiaDoubleQuotedSql (db_prefix);
	  sql_statement =
	      sqlite3_mprintf ("SELECT tbl_name FROM \"%s\".sqlite_master "
			       "WHERE type = 'table' AND tbl_name = 'views_geometry_columns'",
			       quoted_db);
	  free (quoted_db);
      }
    ret =
	sqlite3_prepare_v2 (sqlite, sql_statement, strlen (sql_statement),
			    &stmt, NULL);
    sqlite3_free (sql_statement);
    if (ret != SQLITE_OK)
	return 0;
    while (1)
      {
	  /* scrolling the result set rows */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	      count++;
      }
    sqlite3_finalize (stmt);
    if (count != 1)
	return 0;
    count = 0;

/* attempting to find the RTree Geometry Column */
    if (db_prefix == NULL)
      {
	  sql_statement =
	      sqlite3_mprintf
	      ("SELECT a.f_table_name, a.f_geometry_column, SridIsGeographic(b.srid) "
	       "FROM main.views_geometry_columns AS a "
	       "JOIN main.geometry_columns AS b ON ("
	       "Upper(a.f_table_name) = Upper(b.f_table_name) AND "
	       "Upper(a.f_geometry_column) = Upper(b.f_geometry_column)) "
	       "WHERE Upper(a.view_name) = Upper(%Q) "
	       "AND Upper(a.f_geometry_column) = Upper(%Q) "
	       "AND b.spatial_index_enabled = 1", table_name, geom_column);
      }
    else
      {
	  char *quoted_db = gaiaDoubleQuotedSql (db_prefix);
	  sql_statement =
	      sqlite3_mprintf
	      ("SELECT a.f_table_name, a.f_geometry_column, SridIsGeographic(b.srid) "
	       "FROM \"%s\".views_geometry_columns AS a "
	       "JOIN \"%s\".geometry_columns AS b ON ("
	       "Upper(a.f_table_name) = Upper(b.f_table_name) AND "
	       "Upper(a.f_geometry_column) = Upper(b.f_geometry_column)) "
	       "WHERE Upper(a.view_name) = Upper(%Q) "
	       "AND Upper(a.f_geometry_column) = Upper(%Q) "
	       "AND b.spatial_index_enabled = 1",
	       quoted_db, quoted_db, table_name, geom_column);
	  free (quoted_db);
      }
    ret =
	sqlite3_prepare_v2 (sqlite, sql_statement, strlen (sql_statement),
			    &stmt, NULL);
    sqlite3_free (sql_statement);
    if (ret != SQLITE_OK)
	return 0;
    while (1)
      {
	  /* scrolling the result set rows */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	    {
		const char *v = (const char *) sqlite3_column_text (stmt, 0);
		int len = sqlite3_column_bytes (stmt, 0);
		if (rt)
		    free (rt);
		rt = malloc (len + 1);
		strcpy (rt, v);
		v = (const char *) sqlite3_column_text (stmt, 1);
		len = sqlite3_column_bytes (stmt, 1);
		if (rg)
		    free (rg);
		rg = malloc (len + 1);
		strcpy (rg, v);
		is_longlat = sqlite3_column_int (stmt, 2);
		count++;
	    }
      }
    sqlite3_finalize (stmt);
    if (count != 1)
	return 0;
    if (!validateRowid (sqlite, rt))
      {
	  free (rt);
	  free (rg);
	  return 0;
      }
    *real_table = rt;
    *real_geom = rg;
    *is_geographic = is_longlat;
    return 1;
}

static int
vknn2_check_table (sqlite3 * sqlite, const char *db_prefix,
		   const char *table_name, const char *geom_column,
		   char **real_table, char **real_geom, int *is_geographic)
{
/* checks if the required Spatial Table is actually defined */
    sqlite3_stmt *stmt;
    char *sql_statement;
    int ret;
    int count = 0;
    char *rt = NULL;
    char *rg = NULL;
    int is_longlat = 0;

    if (db_prefix == NULL)
      {
	  sql_statement =
	      sqlite3_mprintf
	      ("SELECT f_table_name, f_geometry_column, SridIsGeographic(srid) "
	       "FROM main.geometry_columns WHERE Upper(f_table_name) = Upper(%Q) AND "
	       "Upper(f_geometry_column) = Upper(%Q) AND spatial_index_enabled = 1",
	       table_name, geom_column);
      }
    else
      {
	  char *quoted_db = gaiaDoubleQuotedSql (db_prefix);
	  sql_statement =
	      sqlite3_mprintf
	      ("SELECT f_table_name, f_geometry_column, SridIsGeographic(srid) "
	       "FROM \"%s\".geometry_columns WHERE Upper(f_table_name) = Upper(%Q) AND "
	       "Upper(f_geometry_column) = Upper(%Q) AND spatial_index_enabled = 1",
	       quoted_db, table_name, geom_column);
	  free (quoted_db);
      }
    ret =
	sqlite3_prepare_v2 (sqlite, sql_statement, strlen (sql_statement),
			    &stmt, NULL);
    sqlite3_free (sql_statement);
    if (ret != SQLITE_OK)
	return 0;
    while (1)
      {
	  /* scrolling the result set rows */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	    {
		const char *v = (const char *) sqlite3_column_text (stmt, 0);
		int len = sqlite3_column_bytes (stmt, 0);
		if (rt)
		    free (rt);
		rt = malloc (len + 1);
		strcpy (rt, v);
		v = (const char *) sqlite3_column_text (stmt, 1);
		len = sqlite3_column_bytes (stmt, 1);
		if (rg)
		    free (rg);
		rg = malloc (len + 1);
		strcpy (rg, v);
		is_longlat = sqlite3_column_int (stmt, 2);
		count++;
	    }
      }
    sqlite3_finalize (stmt);
    if (count != 1)
	return vknn2_check_view (sqlite, db_prefix, table_name, geom_column,
				 real_table, real_geom, is_geographic);
    else
      {
	  *real_table = rt;
	  *real_geom = rg;
	  *is_geographic = is_longlat;
      }
    return 1;
}

static int
vknn2_find_view_rtree (sqlite3 * sqlite, const char *db_prefix,
		       const char *table_name, char **real_table,
		       char **real_geom, int *is_geographic)
{
/* attempts to find the corresponding Spatial View Geometry Column */
    sqlite3_stmt *stmt;
    char *sql_statement;
    int ret;
    int count = 0;
    char *rt = NULL;
    char *rg = NULL;
    int is_longlat = 0;

/* testing if views_geometry_columns exists */
    if (db_prefix == NULL)
      {
	  sql_statement =
	      sqlite3_mprintf ("SELECT tbl_name FROM main.sqlite_master "
			       "WHERE type = 'table' AND tbl_name = 'views_geometry_columns'");
      }
    else
      {
	  char *quoted_db = gaiaDoubleQuotedSql (db_prefix);
	  sql_statement =
	      sqlite3_mprintf ("SELECT tbl_name FROM \"%s\".sqlite_master "
			       "WHERE type = 'table' AND tbl_name = 'views_geometry_columns'",
			       quoted_db);
	  free (quoted_db);
      }
    ret =
	sqlite3_prepare_v2 (sqlite, sql_statement, strlen (sql_statement),
			    &stmt, NULL);
    sqlite3_free (sql_statement);
    if (ret != SQLITE_OK)
	return 0;
    while (1)
      {
	  /* scrolling the result set rows */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	      count++;
      }
    sqlite3_finalize (stmt);
    if (count != 1)
	return 0;
    count = 0;

/* attempting to find the RTree Geometry Column */
    if (db_prefix == NULL)
      {
	  sql_statement =
	      sqlite3_mprintf
	      ("SELECT a.f_table_name, a.f_geometry_column, SridIsGeographic(b.srid) "
	       "FROM main.views_geometry_columns AS a "
	       "JOIN main.geometry_columns AS b ON ("
	       "Upper(a.f_table_name) = Upper(b.f_table_name) AND "
	       "Upper(a.f_geometry_column) = Upper(b.f_geometry_column)) "
	       "WHERE Upper(a.view_name) = Upper(%Q) AND b.spatial_index_enabled = 1",
	       table_name);
      }
    else
      {
	  char *quoted_db = gaiaDoubleQuotedSql (db_prefix);
	  sql_statement =
	      sqlite3_mprintf
	      ("SELECT a.f_table_name, a.f_geometry_column, SridIsGeographic(b.srid) "
	       "FROM \"%s\".views_geometry_columns AS a "
	       "JOIN \"%s\".geometry_columns AS b ON ("
	       "Upper(a.f_table_name) = Upper(b.f_table_name) AND "
	       "Upper(a.f_geometry_column) = Upper(b.f_geometry_column)) "
	       "WHERE Upper(a.view_name) = Upper(%Q) AND b.spatial_index_enabled = 1",
	       quoted_db, quoted_db, table_name);
	  free (quoted_db);
      }
    ret =
	sqlite3_prepare_v2 (sqlite, sql_statement, strlen (sql_statement),
			    &stmt, NULL);
    sqlite3_free (sql_statement);
    if (ret != SQLITE_OK)
	return 0;
    while (1)
      {
	  /* scrolling the result set rows */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	    {
		const char *v = (const char *) sqlite3_column_text (stmt, 0);
		int len = sqlite3_column_bytes (stmt, 0);
		if (rt)
		    free (rt);
		rt = malloc (len + 1);
		strcpy (rt, v);
		v = (const char *) sqlite3_column_text (stmt, 1);
		len = sqlite3_column_bytes (stmt, 1);
		if (rg)
		    free (rg);
		rg = malloc (len + 1);
		strcpy (rg, v);
		is_longlat = sqlite3_column_int (stmt, 2);
		count++;
	    }
      }
    sqlite3_finalize (stmt);
    if (count != 1)
	return 0;
    *real_table = rt;
    *real_geom = rg;
    *is_geographic = is_longlat;
    return 1;
}

static int
vknn2_find_table (sqlite3 * sqlite, const char *db_prefix,
		  const char *table_name, char **real_table, char **real_geom,
		  int *is_geographic)
{
/* attempts to find the corresponding Spatial Table Geometry Column */
    sqlite3_stmt *stmt;
    char *sql_statement;
    int ret;
    int count = 0;
    char *rt = NULL;
    char *rg = NULL;
    int is_longlat = 0;

    if (db_prefix == NULL)
      {
	  sql_statement =
	      sqlite3_mprintf
	      ("SELECT f_table_name, f_geometry_column, SridIsGeographic(srid) "
	       " FROM main.geometry_columns WHERE Upper(f_table_name) = Upper(%Q) "
	       "AND spatial_index_enabled = 1", table_name);
      }
    else
      {
	  char *quoted_db = gaiaDoubleQuotedSql (db_prefix);
	  sql_statement =
	      sqlite3_mprintf
	      ("SELECT f_table_name, f_geometry_column, SridIsGeographic(srid) "
	       " FROM \"%s\".geometry_columns WHERE Upper(f_table_name) = Upper(%Q) "
	       "AND spatial_index_enabled = 1", quoted_db, table_name);
	  free (quoted_db);
      }
    ret =
	sqlite3_prepare_v2 (sqlite, sql_statement, strlen (sql_statement),
			    &stmt, NULL);
    sqlite3_free (sql_statement);
    if (ret != SQLITE_OK)
	return 0;
    while (1)
      {
	  /* scrolling the result set rows */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	    {
		const char *v = (const char *) sqlite3_column_text (stmt, 0);
		int len = sqlite3_column_bytes (stmt, 0);
		if (rt)
		    free (rt);
		rt = malloc (len + 1);
		strcpy (rt, v);
		v = (const char *) sqlite3_column_text (stmt, 1);
		len = sqlite3_column_bytes (stmt, 1);
		if (rg)
		    free (rg);
		rg = malloc (len + 1);
		strcpy (rg, v);
		is_longlat = sqlite3_column_int (stmt, 2);
		count++;
	    }
      }
    sqlite3_finalize (stmt);
    if (count != 1)
	return vknn2_find_view_rtree (sqlite, db_prefix, table_name,
				      real_table, real_geom, is_geographic);
    else
      {
	  *real_table = rt;
	  *real_geom = rg;
	  *is_geographic = is_longlat;
      }
    return 1;
}

static int
do_knn2_query (sqlite3_vtab_cursor * pCursor)
{
/* performing a KNN2 query */
    VirtualKnn2CursorPtr cursor = (VirtualKnn2CursorPtr) pCursor;
    VirtualKnn2Ptr knn2 = (VirtualKnn2Ptr) cursor->pVtab;
    VKnn2ContextPtr ctx = knn2->knn2_ctx;
    double radius;
    double x_radius;
    char *sql;
    char *xdb_prefix;
    char *xtable;
    char *xcolumn;
    char *idxtable;
    int ret;
    sqlite3_stmt *stmt = NULL;
    int iterations = 0;

    if (ctx->valid == 0)
	return 0;
    radius = ctx->radius;

    /* preparing the SQL query */
    xdb_prefix = gaiaDoubleQuotedSql (ctx->db_prefix);
    xtable = gaiaDoubleQuotedSql (ctx->table_name);
    xcolumn = gaiaDoubleQuotedSql (ctx->column_name);
    idxtable = sqlite3_mprintf ("DB=%s.%s", xdb_prefix, xtable);
    if (ctx->is_geographic)
      {
	  /* geographic CRS: measuring the distance in METERS via geodesic formulas */
	  sql =
	      sqlite3_mprintf
	      ("SELECT rowid, ST_Distance(?, \"%s\"), ST_Distance(?, \"%s\", 1) "
	       "FROM \"%s\".\"%s\" WHERE rowid IN (SELECT ROWID FROM SpatialIndex "
	       "WHERE f_table_name = %Q AND f_geometry_column = %Q AND "
	       "search_frame = BuildCircleMbr(?, ?, ?)) "
	       "ORDER BY 3 ASC LIMIT %d", xcolumn, xcolumn, xdb_prefix, xtable,
	       idxtable, ctx->column_name, ctx->max_items);
      }
    else
      {
	  /* planar CRS: measuring the distance in map units */
	  sql =
	      sqlite3_mprintf
	      ("SELECT rowid, ST_Distance(?, \"%s\"), ST_Distance(?, \"%s\") "
	       "FROM \"%s\".\"%s\" WHERE rowid IN (SELECT ROWID FROM SpatialIndex "
	       "WHERE f_table_name = %Q AND f_geometry_column = %Q AND "
	       "search_frame = BuildCircleMbr(?, ?, ?)) "
	       "ORDER BY 3 ASC LIMIT %d", xcolumn, xcolumn, xdb_prefix, xtable,
	       idxtable, ctx->column_name, ctx->max_items);
      }
    sqlite3_free (idxtable);
    free (xdb_prefix);
    free (xtable);
    free (xcolumn);
    ret = sqlite3_prepare_v2 (knn2->db, sql, strlen (sql), &stmt, NULL);
    sqlite3_free (sql);
    if (ret != SQLITE_OK)
	goto error;

    while (1)
      {
	  /* iterating on KNN2 queries */
	  x_radius = radius;
	  sqlite3_reset (stmt);
	  sqlite3_clear_bindings (stmt);
	  sqlite3_bind_blob (stmt, 1, ctx->blob, ctx->blob_size, SQLITE_STATIC);
	  sqlite3_bind_blob (stmt, 2, ctx->blob, ctx->blob_size, SQLITE_STATIC);
	  sqlite3_bind_double (stmt, 3, ctx->point_x);
	  sqlite3_bind_double (stmt, 4, ctx->point_y);
	  sqlite3_bind_double (stmt, 5, x_radius);
	  while (1)
	    {
		/* scrolling the result set rows */
		ret = sqlite3_step (stmt);
		if (ret == SQLITE_DONE)
		    break;	/* end of result set */
		if (ret == SQLITE_ROW)
		  {
		      VKnn2ItemPtr item = ctx->knn2_array + ctx->next_item;
		      sqlite3_int64 rowid = sqlite3_column_int64 (stmt, 0);
		      double dist_crs = sqlite3_column_double (stmt, 1);
		      double dist_m = sqlite3_column_double (stmt, 2);
		      item->rowid = rowid;
		      item->dist_crs = dist_crs;
		      item->dist_m = dist_m;
		      item->radius = radius;
		      item->ok = 1;
		      ctx->next_item += 1;
		      if (ctx->next_item >= ctx->max_items)
			  break;
		  }
		else
		    break;
	    }

	  if (ctx->next_item >= ctx->max_items)
	      break;
	  if (ctx->expand == 0)
	      break;
	  radius *= 2.0;	/* next iteration; doubling the Radius */
	  iterations++;
	  if (iterations > 16)
	      break;		/* precaution against infinive loops */
	  vknn2_clear_context (ctx);
      }
    sqlite3_finalize (stmt);
    return 1;

  error:
    if (stmt != NULL)
	sqlite3_finalize (stmt);
    return 0;
}

static int
vknn2_create (sqlite3 * db, void *pAux, int argc, const char *const *argv,
	      sqlite3_vtab ** ppVTab, char **pzErr)
{
/* creates the virtual table for KNN2 metahandling */
    VirtualKnn2Ptr p_vt;
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
	      ("[VirtualKNN2 module] CREATE VIRTUAL: illegal arg list {void}\n");
	  return SQLITE_ERROR;
      }
    p_vt = (VirtualKnn2Ptr) sqlite3_malloc (sizeof (VirtualKnn2));
    if (!p_vt)
	return SQLITE_NOMEM;
    p_vt->db = db;
    p_vt->pModule = &my_knn2_module;
    p_vt->nRef = 0;
    p_vt->zErrMsg = NULL;
    p_vt->knn2_ctx = vknn2_create_context ();
/* preparing the COLUMNs for this VIRTUAL TABLE */
    xname = gaiaDoubleQuotedSql (vtable);
    buf =
	sqlite3_mprintf
	("CREATE TABLE \"%s\" (db_prefix TEXT, f_table_name TEXT, "
	 "f_geometry_column TEXT, ref_geometry BLOB, radius DOUBLE, "
	 "max_items INTEGER, expand INTEGER, pos INTEGER, fid INTEGER, "
	 "distance_crs DOUBLE, distance_m DOUBLE)", xname);
    free (xname);
    free (vtable);
    if (sqlite3_declare_vtab (db, buf) != SQLITE_OK)
      {
	  sqlite3_free (buf);
	  *pzErr =
	      sqlite3_mprintf
	      ("[VirtualKNN2 module] CREATE VIRTUAL: invalid SQL statement \"%s\"",
	       buf);
	  return SQLITE_ERROR;
      }
    sqlite3_free (buf);
    *ppVTab = (sqlite3_vtab *) p_vt;
    return SQLITE_OK;
}

static int
vknn2_connect (sqlite3 * db, void *pAux, int argc, const char *const *argv,
	       sqlite3_vtab ** ppVTab, char **pzErr)
{
/* connects the virtual table - simply aliases vknn2_create() */
    return vknn2_create (db, pAux, argc, argv, ppVTab, pzErr);
}

static int
vknn2_best_index (sqlite3_vtab * pVTab, sqlite3_index_info * pIdxInfo)
{
/* best index selection */
    int i;
    int err = 1;
    int db_prefix = 0;
    int table = 0;
    int geom_col = 0;
    int ref_geom = 0;
    int radius = 0;
    int max_items = 0;
    int expand = 0;
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    for (i = 0; i < pIdxInfo->nConstraint; i++)
      {
	  /* verifying the constraints */
	  struct sqlite3_index_constraint *p = &(pIdxInfo->aConstraint[i]);
	  if (p->usable)
	    {
		if (p->iColumn == 0 && p->op == SQLITE_INDEX_CONSTRAINT_EQ)
		    db_prefix++;
		else if (p->iColumn == 1 && p->op == SQLITE_INDEX_CONSTRAINT_EQ)
		    table++;
		else if (p->iColumn == 2 && p->op == SQLITE_INDEX_CONSTRAINT_EQ)
		    geom_col++;
		else if (p->iColumn == 3 && p->op == SQLITE_INDEX_CONSTRAINT_EQ)
		    ref_geom++;
		else if (p->iColumn == 4 && p->op == SQLITE_INDEX_CONSTRAINT_EQ)
		    radius++;
		else if (p->iColumn == 5 && p->op == SQLITE_INDEX_CONSTRAINT_EQ)
		    max_items++;
		else if (p->iColumn == 6 && p->op == SQLITE_INDEX_CONSTRAINT_EQ)
		    expand++;
	    }
      }
    if ((db_prefix == 0 || db_prefix == 1) && table == 1
	&& (geom_col == 0 || geom_col == 1) && ref_geom == 1 && radius == 1
	&& (max_items == 0 || max_items == 1) && (expand == 0 || expand == 1))
      {
	  /* this one is a valid KNN2 query */
	  int idx_num = 0x01;
	  if (db_prefix == 1)
	      idx_num |= 0x0100;
	  if (geom_col == 1)
	      idx_num |= 0x08;
	  if (max_items == 1)
	      idx_num |= 0x04;
	  if (expand == 1)
	      idx_num |= 0x02;
	  pIdxInfo->idxNum = idx_num;
	  pIdxInfo->estimatedCost = 1.0;
	  for (i = 0; i < pIdxInfo->nConstraint; i++)
	    {
		if (pIdxInfo->aConstraint[i].usable)
		  {
		      pIdxInfo->aConstraintUsage[i].argvIndex = i + 1;
		      pIdxInfo->aConstraintUsage[i].omit = 1;
		  }
	    }
	  err = 0;
      }
    if (err)
      {
	  /* illegal query */
	  pIdxInfo->idxNum = 0;
      }
    return SQLITE_OK;
}

static int
vknn2_disconnect (sqlite3_vtab * pVTab)
{
/* disconnects the virtual table */
    VirtualKnn2Ptr p_vt = (VirtualKnn2Ptr) pVTab;
    if (p_vt->knn2_ctx != NULL)
	vknn2_free_context (p_vt->knn2_ctx);
    sqlite3_free (p_vt);
    return SQLITE_OK;
}

static int
vknn2_destroy (sqlite3_vtab * pVTab)
{
/* destroys the virtual table - simply aliases vknn2_disconnect() */
    return vknn2_disconnect (pVTab);
}

static int
vknn2_open (sqlite3_vtab * pVTab, sqlite3_vtab_cursor ** ppCursor)
{
/* opening a new cursor */
    VirtualKnn2CursorPtr cursor =
	(VirtualKnn2CursorPtr) sqlite3_malloc (sizeof (VirtualKnn2Cursor));
    if (cursor == NULL)
	return SQLITE_ERROR;
    cursor->pVtab = (VirtualKnn2Ptr) pVTab;
    cursor->eof = 1;
    *ppCursor = (sqlite3_vtab_cursor *) cursor;
    return SQLITE_OK;
}

static int
vknn2_close (sqlite3_vtab_cursor * pCursor)
{
/* closing the cursor */
    sqlite3_free (pCursor);
    return SQLITE_OK;
}

static int
vknn2_get_point (gaiaGeomCollPtr geom, double *point_x, double *point_y)
{
/* checking for a valid Point Geometry */
    if (geom->FirstLinestring != NULL)
	return 0;
    if (geom->FirstPolygon != NULL)
	return 0;
    if (geom->FirstPoint == NULL)
	return 0;
    if (geom->FirstPoint != geom->LastPoint)
	return 0;
    *point_x = geom->FirstPoint->X;
    *point_y = geom->FirstPoint->Y;
    return 1;
}

static int
vknn2_filter (sqlite3_vtab_cursor * pCursor, int idxNum, const char *idxStr,
	      int argc, sqlite3_value ** argv)
{
/* setting up a cursor filter */
    const char *db_prefix = NULL;
    const char *table = NULL;
    const char *geom_column = NULL;
    char *xtable = NULL;
    char *xgeom = NULL;
    int is_geographic = 0;
    int exists;
    int next_arg;
    gaiaGeomCollPtr geom = NULL;
    double point_x = 0.0;
    double point_y = 0.0;
    double radius = 0.0;
    int max_items = 3;
    int expand = 0;
    int has_db_prefix = 0;
    int has_geom_col = 0;
    int has_max_items = 0;
    int has_expand = 0;
    int ok_db_prefix = 0;
    int ok_table = 0;
    int ok_geom_col = 0;
    int ok_geom = 0;
    int ok_radius = 0;
    int ok_max_items = 0;
    int ok_expand = 0;
    VirtualKnn2CursorPtr cursor = (VirtualKnn2CursorPtr) pCursor;
    VirtualKnn2Ptr knn2 = (VirtualKnn2Ptr) cursor->pVtab;
    VKnn2ContextPtr vknn2_context = knn2->knn2_ctx;
    if (idxStr)
	idxStr = idxStr;	/* unused arg warning suppression */
    cursor->eof = 1;
    if (idxNum == 0x00)
	goto stop;
    if ((idxNum & 0x0100) == 0x0100)
	has_db_prefix = 1;
    if ((idxNum & 0x08) == 0x08)
	has_geom_col = 1;
    if ((idxNum & 0x04) == 0x04)
	has_max_items = 1;
    if ((idxNum & 0x02) == 0x02)
	has_expand = 1;

    if (has_db_prefix)
      {
	  if (argc >= 1)
	    {
		/* retrieving the DB Prefix param */
		if (sqlite3_value_type (argv[0]) == SQLITE_NULL)
		  {
		      db_prefix = NULL;
		      ok_db_prefix = 1;
		  }
		if (sqlite3_value_type (argv[0]) == SQLITE_TEXT)
		  {
		      db_prefix = (const char *) sqlite3_value_text (argv[0]);
		      ok_db_prefix = 1;
		  }
	    }
	  next_arg = 1;
      }
    else
      {
	  ok_db_prefix = 1;
	  next_arg = 0;
      }
    if (argc > next_arg)
      {
	  /* retrieving the Table param */
	  if (sqlite3_value_type (argv[next_arg]) == SQLITE_TEXT)
	    {
		table = (const char *) sqlite3_value_text (argv[next_arg]);
		ok_table = 1;
	    }
	  next_arg++;
      }
    if (has_geom_col)
      {
	  if (argc > next_arg)
	    {
		/* retrieving the Geometry Column param */
		if (sqlite3_value_type (argv[next_arg]) == SQLITE_NULL)
		  {
		      geom_column = NULL;
		      ok_geom_col = 1;
		  }
		if (sqlite3_value_type (argv[next_arg]) == SQLITE_TEXT)
		  {
		      geom_column =
			  (const char *) sqlite3_value_text (argv[next_arg]);
		      ok_geom_col = 1;
		  }
	    }
	  next_arg++;
      }
    else
	ok_geom_col = 1;
    if (argc > next_arg)
      {
	  /* retrieving the Point Geometry param */
	  if (sqlite3_value_type (argv[next_arg]) == SQLITE_BLOB)
	    {
		const unsigned char *blob;
		int size;
		blob = sqlite3_value_blob (argv[next_arg]);
		size = sqlite3_value_bytes (argv[next_arg]);
		geom = gaiaFromSpatiaLiteBlobWkb (blob, size);
		ok_geom = vknn2_get_point (geom, &point_x, &point_y);
	    }
	  next_arg++;
      }
    if (argc > next_arg)
      {
	  /* retrieving the Radius param */
	  if (sqlite3_value_type (argv[next_arg]) == SQLITE_INTEGER)
	    {
		int r = sqlite3_value_int (argv[next_arg]);
		radius = r;
		ok_radius = 1;
	    }
	  if (sqlite3_value_type (argv[next_arg]) == SQLITE_FLOAT)
	    {
		radius = sqlite3_value_double (argv[next_arg]);
		if (radius > 0.0)
		    ok_radius = 1;
	    }
	  next_arg++;
      }
    if (has_max_items)
      {
	  if (argc > next_arg)
	    {
		/* retrieving the MaxItems param */
		if (sqlite3_value_type (argv[next_arg]) == SQLITE_INTEGER)
		  {
		      max_items = sqlite3_value_int (argv[next_arg]);
		      if (max_items > 1024)
			  max_items = 1024;
		      if (max_items < 1)
			  max_items = 1;
		      ok_max_items = 1;
		  }
		next_arg++;
	    }
      }
    else
      {
	  max_items = 3;
	  ok_max_items = 1;
      }
    if (has_expand)
      {
	  if (argc > next_arg)
	    {
		/* retrieving the Expand param */
		if (sqlite3_value_type (argv[next_arg]) == SQLITE_INTEGER)
		  {
		      expand = sqlite3_value_int (argv[next_arg]);
		      ok_expand = 1;
		  }
		next_arg++;
	    }
      }
    else
      {
	  expand = 0;
	  ok_expand = 1;
      }

/* checking params for validity */
    if (ok_db_prefix && ok_table && ok_geom_col && ok_geom && ok_radius
	&& ok_max_items && ok_expand)
	;
    else
	goto stop;

    if (geom_column == NULL)
      {
	  /* attempting to automatically detect the geometry_column */
	  exists =
	      vknn2_find_table (knn2->db, db_prefix, table, &xtable, &xgeom,
				&is_geographic);
      }
    else
      {
	  /* attempting to verify table_name and geometry_column */
	  exists =
	      vknn2_check_table (knn2->db, db_prefix, table, geom_column,
				 &xtable, &xgeom, &is_geographic);
      }
    if (!exists)
	goto stop;
    vknn2_init_context (vknn2_context, db_prefix, xtable, xgeom, is_geographic,
			geom, point_x, point_y, radius, max_items, expand);

/* performing a KNN2 query */
    if (!do_knn2_query (pCursor))
	goto stop;

    if (vknn2_context->next_item == 0)
	cursor->eof = 1;
    else
	cursor->eof = 0;
    cursor->CurrentIndex = 0;

  stop:
    if (geom != NULL)
	gaiaFreeGeomColl (geom);
    return SQLITE_OK;
}

static int
vknn2_next (sqlite3_vtab_cursor * pCursor)
{
/* fetching a next row from cursor */
    VirtualKnn2CursorPtr cursor = (VirtualKnn2CursorPtr) pCursor;
    VKnn2ContextPtr ctx = cursor->pVtab->knn2_ctx;
    cursor->CurrentIndex += 1;
    if (cursor->CurrentIndex >= ctx->next_item)
	cursor->eof = 1;
    return SQLITE_OK;
}

static int
vknn2_eof (sqlite3_vtab_cursor * pCursor)
{
/* cursor EOF */
    VirtualKnn2CursorPtr cursor = (VirtualKnn2CursorPtr) pCursor;
    return cursor->eof;
}

static int
vknn2_column (sqlite3_vtab_cursor * pCursor, sqlite3_context * pContext,
	      int column)
{
/* fetching value for the Nth column */
    VirtualKnn2CursorPtr cursor = (VirtualKnn2CursorPtr) pCursor;
    VKnn2ContextPtr ctx = cursor->pVtab->knn2_ctx;
    VKnn2ItemPtr item = NULL;
    if (cursor || column)
	cursor = cursor;	/* unused arg warning suppression */
    if (column)
	column = column;	/* unused arg warning suppression */
    if (cursor->CurrentIndex < ctx->next_item)
	item = ctx->knn2_array + cursor->CurrentIndex;
    if (column == 0)
      {
	  /* the DB Prefix column */
	  sqlite3_result_text (pContext, ctx->db_prefix,
			       strlen (ctx->db_prefix), SQLITE_STATIC);
      }
    else if (column == 1)
      {
	  /* the Table Name column */
	  sqlite3_result_text (pContext, ctx->table_name,
			       strlen (ctx->table_name), SQLITE_STATIC);
      }
    else if (column == 2)
      {
	  /* the GeometryColumn Name column */
	  sqlite3_result_text (pContext, ctx->column_name,
			       strlen (ctx->column_name), SQLITE_STATIC);
      }
    else if (column == 3)
      {
	  /* the Reference Geometry column */
	  sqlite3_result_blob (pContext, ctx->blob, ctx->blob_size,
			       SQLITE_STATIC);
      }
    else if (column == 4)
      {
	  /* the Radius column */
	  sqlite3_result_double (pContext, item->radius);
      }
    else if (column == 5)
      {
	  /* the Max Items column */
	  sqlite3_result_int (pContext, ctx->max_items);
      }
    else if (column == 6)
      {
	  /* the Expand column */
	  sqlite3_result_int (pContext, ctx->expand);
      }
    else if (column == 7)
      {
	  /* the index column */
	  sqlite3_result_int (pContext, cursor->CurrentIndex + 1);
      }
    else if ((column == 8 || column == 9 || column == 10) && item != NULL)
      {
	  if (column == 8)
	    {
		/* the RowID column */
		sqlite3_result_int64 (pContext, item->rowid);
	    }
	  else if (column == 9)
	    {
		/* the Distance (CRS units) column */
		sqlite3_result_double (pContext, item->dist_crs);
	    }
	  else if (column == 10)
	    {
		/* the Distance (meters) column */
		sqlite3_result_double (pContext, item->dist_m);
	    }
	  else
	      sqlite3_result_null (pContext);
      }
    else
	sqlite3_result_null (pContext);
    return SQLITE_OK;
}

static int
vknn2_rowid (sqlite3_vtab_cursor * pCursor, sqlite_int64 * pRowid)
{
/* fetching the ROWID */
    VirtualKnn2CursorPtr cursor = (VirtualKnn2CursorPtr) pCursor;
    *pRowid = cursor->CurrentIndex;
    return SQLITE_OK;
}

static int
vknn2_update (sqlite3_vtab * pVTab, int argc, sqlite3_value ** argv,
	      sqlite_int64 * pRowid)
{
/* generic update [INSERT / UPDATE / DELETE */
    if (pRowid || argc || argv || pVTab)
	pRowid = pRowid;	/* unused arg warning suppression */
/* read only datasource */
    return SQLITE_READONLY;
}

static int
vknn2_begin (sqlite3_vtab * pVTab)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn2_sync (sqlite3_vtab * pVTab)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn2_commit (sqlite3_vtab * pVTab)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn2_rollback (sqlite3_vtab * pVTab)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    return SQLITE_OK;
}

static int
vknn2_rename (sqlite3_vtab * pVTab, const char *zNew)
{
/* BEGIN TRANSACTION */
    if (pVTab)
	pVTab = pVTab;		/* unused arg warning suppression */
    if (zNew)
	zNew = zNew;		/* unused arg warning suppression */
    return SQLITE_ERROR;
}

static int
spliteKnn2Init (sqlite3 * db)
{
    int rc = SQLITE_OK;
    my_knn2_module.iVersion = 1;
    my_knn2_module.xCreate = &vknn2_create;
    my_knn2_module.xConnect = &vknn2_connect;
    my_knn2_module.xBestIndex = &vknn2_best_index;
    my_knn2_module.xDisconnect = &vknn2_disconnect;
    my_knn2_module.xDestroy = &vknn2_destroy;
    my_knn2_module.xOpen = &vknn2_open;
    my_knn2_module.xClose = &vknn2_close;
    my_knn2_module.xFilter = &vknn2_filter;
    my_knn2_module.xNext = &vknn2_next;
    my_knn2_module.xEof = &vknn2_eof;
    my_knn2_module.xColumn = &vknn2_column;
    my_knn2_module.xRowid = &vknn2_rowid;
    my_knn2_module.xUpdate = &vknn2_update;
    my_knn2_module.xBegin = &vknn2_begin;
    my_knn2_module.xSync = &vknn2_sync;
    my_knn2_module.xCommit = &vknn2_commit;
    my_knn2_module.xRollback = &vknn2_rollback;
    my_knn2_module.xFindFunction = NULL;
    my_knn2_module.xRename = &vknn2_rename;
    sqlite3_create_module_v2 (db, "VirtualKNN2", &my_knn2_module, NULL, 0);
    return rc;
}

SPATIALITE_PRIVATE int
virtual_knn2_extension_init (void *xdb)
{
    sqlite3 *db = (sqlite3 *) xdb;
    return spliteKnn2Init (db);
}

#endif /* end GEOS conditional */
