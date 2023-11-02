/*

 check_tempgeom.c -- SpatiaLite Test Case

 Author: Sandro Furieri <a.furieri@lqt.it>

 ------------------------------------------------------------------------------
 
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
 
Portions created by the Initial Developer are Copyright (C) 2020
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sqlite3.h"
#include "spatialite.h"

static int
sql_test (sqlite3 * handle, const char *sql)
{
/* executing an SQL test returning a value */
    int retval = 0;
    char *err_msg = NULL;
    char **results;
    int rows;
    int columns;
    int ret = sqlite3_get_table (handle, sql,
				 &results, &rows, &columns, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "Error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -1;
	  goto stop;
      }
    if ((rows != 1) || (columns != 1))
      {
	  fprintf (stderr, "Unexpected rows/columns: %i/%i.\n", rows, columns);
	  retval = -2;
	  goto stop;
      }
    retval = atoi (results[1]);
  stop:
    sqlite3_free_table (results);
    return retval;
}

int
main (int argc, char *argv[])
{
#ifndef ENABLE_RTTOPO		/* RTTOPO is not supported: quitting */
	return 0;
#endif

    int retval = 0;
    int ret;
    sqlite3 *handle;
    const char *sql;
    sqlite3_stmt *stmt = NULL;
    char *err_msg = NULL;
    void *cache = spatialite_alloc_connection ();

    if (argc > 1 || argv[0] == NULL)
	argc = 1;		/* silencing stupid compiler warnings */

    ret =
	sqlite3_open_v2 (":memory:", &handle,
			 SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "cannot open in-memory db: %s\n",
		   sqlite3_errmsg (handle));
	  sqlite3_close (handle);
	  return -1;
      }

    spatialite_init_ex (handle, cache, 0);

    ret =
	sqlite3_exec (handle, "SELECT InitSpatialMetadataFull(1)", NULL, NULL,
		      &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "InitSpatialMetadataFull() error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -2;
	  goto stop;
      }

/* basic tests expecting SUCESS */

    ret =
	sqlite3_exec (handle, "ATTACH DATABASE ':memory:' AS t", NULL, NULL,
		      &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "ATTACH DATABASE #1 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -3;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "CREATE TABLE t.point_test (id INTEGER PRIMARY KEY, name TEXT)",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE TABLE #1 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -4;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "SELECT AddTemporaryGeometryColumn('t', 'point_test', 'geom', 4326, 'POINT', 'XY')",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "AddTemporaryGeometryColumn #1 error: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  retval = -5;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "INSERT INTO t.point_test (id, name, geom) VALUES (NULL, 'one', MakePoint(11.50, 22.50, 4326))",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO #1 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -6;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "INSERT INTO t.point_test (id, name, geom) VALUES (NULL, 'two', MakePoint(33.50, 44.50, 4326))",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO #2 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -7;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "INSERT INTO t.point_test (id, name, geom) VALUES (NULL, 'three', MakePoint(55.50, 66.50, 4326))",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO #3 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -8;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "SELECT CreateTemporarySpatialIndex('t', 'point_test', 'geom')",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CreateTemporarySpatialIndex #1 error: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  retval = -9;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "INSERT INTO t.point_test (id, name, geom) VALUES (NULL, 'four', MakePoint(77.50, 88.50, 4326))",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO #4 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -10;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "INSERT INTO t.point_test (id, name, geom) VALUES (NULL, 'five', MakePoint(99.0, -99.0, 4326))",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO #5 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -11;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "UPDATE t.point_test SET geom = MakePoint(-33.50, -44.50, 4326) WHERE id = 2",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "UPDATE #1 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -12;
	  goto stop;
      }

    ret =
	sqlite3_exec (handle,
		      "DELETE FROM t.point_test WHERE id = 3",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "DELETE #1 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  retval = -13;
	  goto stop;
      }

/* checking the Spatial Index */
    sql = "SELECT pkid, xmin, ymin FROM t.idx_point_test_geom ORDER BY pkid";
    ret = sqlite3_prepare_v2 (handle, sql, strlen (sql), &stmt, NULL);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "PREPARED STATEMENT #1 error: %s\n",
		   sqlite3_errmsg (handle));
	  sqlite3_free (err_msg);
	  retval = -14;
	  goto stop;
      }
    while (1)
      {
	  /* scrolling the result set rows */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	    {
		int id = sqlite3_column_int (stmt, 0);
		double x = sqlite3_column_double (stmt, 1);
		double y = sqlite3_column_double (stmt, 2);
		if (id >= 1 && id <= 5 && id != 3)
		  {
		      int err = 0;
		      if (id == 1)
			{
			    if (x != 11.50)
				err = 1;
			    if (y != 22.50)
				err = 1;
			}
		      if (id == 2)
			{
			    if (x != -33.50)
				err = 1;
			    if (y != -44.50)
				err = 1;
			}
		      if (id == 4)
			{
			    if (x != 77.50)
				err = 1;
			    if (y != 88.50)
				err = 1;
			}
		      if (id == 5)
			{
			    if (x != 99.0)
				err = 1;
			    if (y != -99.0)
				err = 1;
			}
		      if (err)
			{
			    fprintf (stderr,
				     "SpatialIndex error: ID=%d unexpected X=%f Y=%f\n",
				     id, x, y);
			    sqlite3_free (err_msg);
			    retval = -15;
			    goto stop;
			}
		  }
		else
		  {
		      fprintf (stderr, "SpatialIndex error: unexpected ID=%d\n",
			       id);
		      sqlite3_free (err_msg);
		      retval = -16;
		      goto stop;
		  }
	    }
	  else
	    {
		fprintf (stderr, "STEP #1 error: %s\n", err_msg);
		sqlite3_free (err_msg);
		retval = -17;
		goto stop;
	    }
      }
    sqlite3_finalize (stmt);
    stmt = NULL;

/* testing bad arguments: AddTemporaryGeometryColumn */
    sql =
	"SELECT AddTemporaryGeometryColumn(NULL, 'table', 'geom', 3003, 'POLYGON', 'XY')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #1: %d\n",
		   ret);
	  retval = -18;
	  goto stop;
      }
    sql =
	"SELECT AddTemporaryGeometryColumn('t', NULL, 'geom', 3003, 'POLYGON', 'XY')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #2: %d\n",
		   ret);
	  retval = -19;
	  goto stop;
      }
    sql =
	"SELECT AddTemporaryGeometryColumn('t', 'table', NULL, 3003, 'POLYGON', 'XY')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #3: %d\n",
		   ret);
	  retval = -20;
	  goto stop;
      }
    sql =
	"SELECT AddTemporaryGeometryColumn('t', 'table', 'geom', NULL, 'POLYGON', 'XY')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #4: %d\n",
		   ret);
	  retval = -21;
	  goto stop;
      }
    sql =
	"SELECT AddTemporaryGeometryColumn('t', 'table', 'geom', 3003, NULL, 'XY')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #5: %d\n",
		   ret);
	  retval = -22;
	  goto stop;
      }
    sql =
	"SELECT AddTemporaryGeometryColumn('t', 'table', 'geom', 3003, 'POLYGON', NULL)";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #6: %d\n",
		   ret);
	  retval = -23;
	  goto stop;
      }
    sql =
	"SELECT AddTemporaryGeometryColumn('t', 'table', 'geom', 3003, 'POLYGON', 'XY', 'true')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #7: %d\n",
		   ret);
	  retval = -24;
	  goto stop;
      }

/* testing bad arguments: CreateTemporarySpatialIndex */
    sql = "SELECT CreateTemporarySpatialIndex(NULL, 'table', 'geom')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result CreateTemporarySpatialIndex #1: %d\n",
		   ret);
	  retval = -25;
	  goto stop;
      }
    sql = "SELECT CreateTemporarySpatialIndex('t', NULL, 'geom')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result CreateTemporarySpatialIndex #2: %d\n",
		   ret);
	  retval = -26;
	  goto stop;
      }
    sql = "SELECT CreateTemporarySpatialIndex('t', 'table', NULL)";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result CreateTemporarySpatialIndex #3: %d\n",
		   ret);
	  retval = -27;
	  goto stop;
      }

/* testing invalid arguments: CreateTemporarySpatialIndex */
    sql =
	"SELECT AddTemporaryGeometryColumn('a', 'table', 'geom', 3003, 'POLYGON', 'XY')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #8: %d\n",
		   ret);
	  retval = -28;
	  goto stop;
      }
    sql =
	"SELECT AddTemporaryGeometryColumn('t', 'table', 'geom', 123456, 'POLYGON', 'XY')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #9: %d\n",
		   ret);
	  retval = -29;
	  goto stop;
      }
    sql =
	"SELECT AddTemporaryGeometryColumn('a', 'table', 'geom', 3003, 71, 'XY')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #10: %d\n",
		   ret);
	  retval = -30;
	  goto stop;
      }
    sql =
	"SELECT AddTemporaryGeometryColumn('a', 'table', 'geom', 3003, 'POLYGON', 17)";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result AddTemporaryGeometryColumn #11: %d\n",
		   ret);
	  retval = -31;
	  goto stop;
      }

/* testing invalid arguments: CreateTemporarySpatialIndex */
    sql = "SELECT CreateTemporarySpatialIndex('a', 'table', 'geom')";
    ret = sql_test (handle, sql);
    if (ret != -1)
      {
	  fprintf (stderr,
		   "Unexpected result CreateTemporarySpatialIndex #4: %d\n",
		   ret);
	  retval = -32;
	  goto stop;
      }
    sql = "SELECT CreateTemporarySpatialIndex('t', 'table', 'geom2')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result CreateTemporarySpatialIndex #5: %d\n",
		   ret);
	  retval = -33;
	  goto stop;
      }
    sql = "SELECT CreateTemporarySpatialIndex('t', 'table', 'geom')";
    ret = sql_test (handle, sql);
    if (ret != 0)
      {
	  fprintf (stderr,
		   "Unexpected result CreateTemporarySpatialIndex #6: %d\n",
		   ret);
	  retval = -34;
	  goto stop;
      }

  stop:
    if (stmt != NULL)
	sqlite3_finalize (stmt);
    ret = sqlite3_close (handle);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "sqlite3_close() error: %s\n",
		   sqlite3_errmsg (handle));
	  retval = -23;
      }

    spatialite_cleanup_ex (cache);
    spatialite_shutdown ();

    return retval;
}
