/*

 check_multiple_points.c -- SpatiaLite Test Case

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
 
Portions created by the Initial Developer are Copyright (C) 2021
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <spatialite/gaiaconfig.h>

#include "sqlite3.h"
#include "spatialite.h"

static int
do_create_helper_tables (sqlite3 * sqlite)
{
/* creating and populating the helper tables */
    const char *sql;
    int ret;
    char *err_msg = NULL;

/* valid table */
    sql = "CREATE TABLE ok_points (\n"
	"\tfid INTEGER NOT NULL,\n"
	"\tpos INTEGER NOT NULL,\n"
	"\tCONSTRAINT pk_okpts PRIMARY KEY (fid, pos))";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE ok_points error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -4;
      }
/* valid Geometry */
    sql = "SELECT AddGeometryColumn('ok_points', 'geom', 4326, 'POINT', 'XY')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE ok_points valid Geometry error: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -5;
      }
/* invalid Geometry - wrong SRID */
    sql =
	"SELECT AddGeometryColumn('ok_points', 'bad_geom1', 3003, 'POINT', 'XY')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE ok_points bad Geometry #1 error: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -6;
      }
/* invalid Geometry - wrong type */
    sql =
	"SELECT AddGeometryColumn('ok_points', 'bad_geom2', 4326, 'LINESTRING', 'XY')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE ok_points bad Geometry #2 error: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -7;
      }
/* invalid Geometry - wrong dimensions */
    sql =
	"SELECT AddGeometryColumn('ok_points', 'bad_geom3', 4326, 'POINT', 'XYZ')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE ok_points bad Geometry #3 error: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -8;
      }
/* another valid Geometry (all NULLs) */
    sql =
	"SELECT AddGeometryColumn('ok_points', 'geom_null', 4326, 'POINT', 'XY')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE ok_points NULL Geometry error: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -9;
      }

/* invalid table - not a Geometry */
    sql = "CREATE TABLE bad_points (\n"
	"\tfid INTEGER NOT NULL,\n"
	"\tpos INTEGER NOT NULL,\n"
	"\tgeom BLOB,\n" "\tCONSTRAINT pk_badpts PRIMARY KEY (fid, pos))";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE bad_points error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -10;
      }

/* valid table with duplicates */
    sql = "CREATE TABLE dup_points (\n"
	"\tpkuid INTEGER PRIMARY KEY,\n"
	"\tfid INTEGER NOT NULL,\n" "\tpos INTEGER NOT NULL)";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE dup_points error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -11;
      }
/* valid Geometry */
    sql = "SELECT AddGeometryColumn('dup_points', 'geom', 4326, 'POINT', 'XY')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "CREATE dup_points valid Geometry #1 error: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -12;
      }

/* populating OK_POINTS */
    sql =
	"INSERT INTO ok_points (fid, pos, geom) VALUES (123, 1, MakePoint(1.5, 0.5, 4326))";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO ok_points #1 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -13;
      }
    sql =
	"INSERT INTO ok_points (fid, pos, geom) VALUES (123, 3, MakePoint(2.5, 0.9, 4326))";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO ok_points #2 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -14;
      }
    sql =
	"INSERT INTO ok_points (fid, pos, geom) VALUES (123, 4, MakePoint(2.7, 2.5, 4326))";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO ok_points #3 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -15;
      }
    sql =
	"INSERT INTO ok_points (fid, pos, geom) VALUES (123, 100, MakePoint(100, 100, 4326))";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO ok_points #4 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -16;
      }
    sql =
	"INSERT INTO ok_points (fid, pos, geom) VALUES (123, 101, MakePoint(101, 101, 4326))";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO ok_points #5 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -17;
      }

/* populating DUP_POINTS */
    sql =
	"INSERT INTO dup_points (fid, pos, geom) VALUES (123, 1, MakePoint(1.5, 0.5, 4326))";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO dup_points #1 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -18;
      }
    sql =
	"INSERT INTO dup_points (fid, pos, geom) VALUES (123, 1, MakePoint(2, 2, 4326))";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "INSERT INTO dup_points #2 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -19;
      }

    return 0;
}

static int
do_test_success (sqlite3 * sqlite)
{
/* tests expected to return a success */
    const char *sql;
    int ret;
    char *err_msg = NULL;
    int i;
    char **results;
    int rows;
    int columns;
    int err = 0;

    sql = "SELECT AsText(ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 1 0, 1 1, 2 1, 2 2, 3 2, 3 3)', 4326), "
	"123, 'OK_POINTS', 'GEOM', 'fid', 'pos'))";
    ret = sqlite3_get_table (sqlite, sql, &results, &rows, &columns, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "SELECT success #1 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -21;
      }
    if (rows < 1)
      {
	  fprintf (stderr, "SELECT success #1 returned no result\n");
	  return -22;
      }
    else
      {
	  for (i = 1; i <= rows; i++)
	    {
		const char *line = results[(i * columns) + 0];
		if (strcmp
		    (line,
		     "LINESTRING(0 0, 1.5 0.5, 1 1, 2.5 0.9, 2.7 2.5, 3 2, 3 3)")
		    != 0)
		  {
		      fprintf (stderr,
			       "SELECT success #1: unexpected result %s\n",
			       line);
		      err = 1;
		  }
	    }
      }
    sqlite3_free_table (results);

    if (err)
	return -23;

    sql = "SELECT AsText(ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'OK_POINTS', 'geom', 'fid', 'pos'))";
    ret = sqlite3_get_table (sqlite, sql, &results, &rows, &columns, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "SELECT success #2 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -24;
      }
    if (rows < 1)
      {
	  fprintf (stderr, "SELECT success #2 returned no result\n");
	  return -25;
      }
    else
      {
	  for (i = 1; i <= rows; i++)
	    {
		const char *line = results[(i * columns) + 0];
		if (strcmp (line, "LINESTRING(0 0, 1.5 0.5)") != 0)
		  {
		      fprintf (stderr,
			       "SELECT success #2: unexpected result %s\n",
			       line);
		      err = 1;
		  }
	    }
      }
    sqlite3_free_table (results);

    if (err)
	return -26;

    sql = "SELECT AsText(ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 1 0, 1 1, 2 1, 2 2, 3 2, 3 3)', 4326), "
	"0, 'ok_points', 'GEOM', 'fid', 'pos'))";
    ret = sqlite3_get_table (sqlite, sql, &results, &rows, &columns, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "SELECT success #3 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -27;
      }
    if (rows < 1)
      {
	  fprintf (stderr, "SELECT success #3 returned no result\n");
	  return -28;
      }
    else
      {
	  for (i = 1; i <= rows; i++)
	    {
		const char *line = results[(i * columns) + 0];
		if (strcmp
		    (line,
		     "LINESTRING(0 0, 1 0, 1 1, 2 1, 2 2, 3 2, 3 3)") != 0)
		  {
		      fprintf (stderr,
			       "SELECT success #3: unexpected result %s\n",
			       line);
		      err = 1;
		  }
	    }
      }
    sqlite3_free_table (results);

    if (err)
	return -29;

    return 0;
}

static int
do_test_wrong_args (sqlite3 * sqlite)
{
/* tests expected to return a failure - wrong arg types */
    const char *sql;
    int ret;
    char *err_msg = NULL;

/* NULL Linestring */
    sql = "SELECT ST_SetMultiplePoints(NULL, "
	"123, 'ok_points', 'geom', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST NULL arg #1 unexpected success\n");
	  return -27;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: first argument is not a valid Linestring.")
	!= 0)
      {
	  fprintf (stderr, "TEST NULL arg #1 unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -28;
      }

/* NULL PK value */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"NULL, 'ok_points', 'geom', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST arg #2 unexpected success\n");
	  return -29;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: second argument is not of the INTEGER type.")
	!= 0)
      {
	  fprintf (stderr, "TEST arg #2 unexpected exception: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -30;
      }

/* NULL table name */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, NULL, 'geom', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST arg #3 unexpected success\n");
	  return -31;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: third argument is not of the TEXT type.")
	!= 0)
      {
	  fprintf (stderr, "TEST arg #3 unexpected exception: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -32;
      }

/* NULL point name */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', NULL, 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST arg #4 unexpected success\n");
	  return -33;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: fourth argument is not of the TEXT type.")
	!= 0)
      {
	  fprintf (stderr, "TEST arg #4 unexpected exception: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -34;
      }

/* NULL PK name */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', 'geom', NULL, 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST arg #5 unexpected success\n");
	  return -35;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: fifth argument is not of the TEXT type.")
	!= 0)
      {
	  fprintf (stderr, "TEST arg #5 unexpected exception: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -36;
      }

/* NULL PK name */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', 'geom', 'fid', NULL)";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST arg #6 unexpected success\n");
	  return -37;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: sixth argument is not of the TEXT type.")
	!= 0)
      {
	  fprintf (stderr, "TEST arg #6 unexpected exception: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -38;
      }

/* Invalid BLOB Linestring */
    sql = "SELECT ST_SetMultiplePoints(zeroblob(40), "
	"123, 'ok_points', 'geom', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST invalid arg #1 unexpected success\n");
	  return -39;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: first argument is not a valid Linestring.")
	!= 0)
      {
	  fprintf (stderr, "TEST invalid arg #1 unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -40;
      }

/* MultiLinestring */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('MULTILINESTRING((0 0, 10 10), (20 20, 21 21))', 4326), "
	"123, 'ok_points', 'geom', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST MultiLinestring arg #1 unexpected success\n");
	  return -41;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: first argument is not a valid Linestring.")
	!= 0)
      {
	  fprintf (stderr,
		   "TEST MultiLinestring arg #1 unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -42;
      }

    return 0;
}

static int
do_test_bad_args (sqlite3 * sqlite)
{
/* tests expected to return a failure - bad arguments */
    const char *sql;
    int ret;
    char *err_msg = NULL;

/* bad table name */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'not_existing', 'geom', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST bad table name unexpected success\n");
	  return -43;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: no such table \"not_existing\".") != 0)
      {
	  fprintf (stderr, "TEST bad table name unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -44;
      }

/* bad points name */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', 'bad_geom', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST bad points name unexpected success\n");
	  return -45;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: table \"ok_points\" has no column \"bad_geom\".")
	!= 0)
      {
	  fprintf (stderr, "TEST bad points name unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -46;
      }

/* bad PK name */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', 'geom', 'bad_fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST bad PK name unexpected success\n");
	  return -47;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: table \"ok_points\" has no column \"bad_fid\".")
	!= 0)
      {
	  fprintf (stderr, "TEST bad PK name unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -48;
      }

/* bad position name */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', 'geom', 'fid', 'bad_pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST bad position name unexpected success\n");
	  return -49;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: table \"ok_points\" has no column \"bad_pos\".")
	!= 0)
      {
	  fprintf (stderr, "TEST bad position name unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -50;
      }

/* not a registerd Geometry */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'bad_points', 'geom', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST not registered geometry unexpected success\n");
	  return -51;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: \"bad_points\".\"geom\" is not a registered Geometry.")
	!= 0)
      {
	  fprintf (stderr,
		   "TEST not registered geometry unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -52;
      }

/* mismatching SRID */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', 'bad_geom1', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST mismatching SRID unexpected success\n");
	  return -53;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: \"ok_points\".\"bad_geom1\" mismatching SRID.")
	!= 0)
      {
	  fprintf (stderr, "TEST mismatching SRID unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -54;
      }

/* invalid geommetry type */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', 'bad_geom2', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST invalid geometry type unexpected success\n");
	  return -55;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: \"ok_points\".\"bad_geom2\" is not a Geometry of the POINT type.")
	!= 0)
      {
	  fprintf (stderr,
		   "TEST invalid geometry type unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -56;
      }

/* invalid dimensions */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', 'bad_geom3', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST invalid dimensions unexpected success\n");
	  return -57;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: \"ok_points\".\"bad_geom3\" mismatching dimensions.")
	!= 0)
      {
	  fprintf (stderr, "TEST invalid dimensions unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -58;
      }

/* duplicate points */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'dup_points', 'geom', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST duplicate points unexpected success\n");
	  return -59;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: duplicate position found.") != 0)
      {
	  fprintf (stderr, "TEST duplicate points unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -60;
      }

/* NULL geometry */
    sql = "SELECT ST_SetMultiplePoints("
	"GeomFromText('LINESTRING(0 0, 10 10)', 4326), "
	"123, 'ok_points', 'geom_null', 'fid', 'pos')";
    ret = sqlite3_exec (sqlite, sql, NULL, NULL, &err_msg);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "TEST NULL geometry unexpected success\n");
	  return -61;
      }
    if (strcmp
	(err_msg,
	 "SetMultiplePoints() exception: illegal Geometry found.") != 0)
      {
	  fprintf (stderr, "TEST NULL geometry unexpected exception: %s\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -62;
      }

    return 0;
}

int
main (int argc, char *argv[])
{
    int retcode = 0;
    int ret;
    sqlite3 *handle;
    char *err_msg = NULL;
    void *cache = spatialite_alloc_connection ();

    ret =
	sqlite3_open_v2 (":memory:", &handle,
			 SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "cannot open \":memory:\" database: %s\n",
		   sqlite3_errmsg (handle));
	  sqlite3_close (handle);
	  return -1;
      }

    spatialite_init_ex (handle, cache, 0);

    ret = sqlite3_exec (handle, "PRAGMA foreign_keys=1", NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "PRAGMA foreign_keys=1 error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  sqlite3_close (handle);
	  return -2;
      }

    ret =
	sqlite3_exec (handle, "SELECT InitSpatialMetadataFull(1)", NULL, NULL,
		      &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "InitSpatialMetadataFull() error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  sqlite3_close (handle);
	  return -3;
      }

/* creating and populating the helper tables */
    ret = do_create_helper_tables (handle);
    if (ret != 0)
      {
	  retcode = ret;
	  goto end;
      }

/* tests expected to return a success */
    ret = do_test_success (handle);
    if (ret != 0)
      {
	  retcode = ret;
	  goto end;
      }

/* tests expected to return a failure - wrong arg types */
    ret = do_test_wrong_args (handle);
    if (ret != 0)
      {
	  retcode = ret;
	  goto end;
      }

/* tests expected to return a failure - bad arguments */
    ret = do_test_bad_args (handle);
    if (ret != 0)
      {
	  retcode = ret;
	  goto end;
      }

  end:
    sqlite3_close (handle);
    spatialite_cleanup_ex (cache);
    if (argc > 1 || argv[0] == NULL)
	argc = 1;		/* silencing stupid compiler warnings */

    spatialite_shutdown ();
    return retcode;
}
