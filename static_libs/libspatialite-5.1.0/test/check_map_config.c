
/*

 check_map_config.c -- SpatiaLite Test Case

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

#include <spatialite/gaiaconfig.h>

#include "sqlite3.h"
#include "spatialite.h"

static int
execute_check (sqlite3 * sqlite, const char *sql, char **error)
{
/* executing an SQL statement returning True/False */
    sqlite3_stmt *stmt;
    int ret;
    int retcode = 0;

    if (error != NULL)
	*error = NULL;
    ret = sqlite3_prepare_v2 (sqlite, sql, strlen (sql), &stmt, NULL);
    if (ret != SQLITE_OK)
      {
	  if (error != NULL)
	      *error = sqlite3_mprintf ("%s", sqlite3_errmsg (sqlite));
	  return SQLITE_ERROR;
      }
    ret = sqlite3_step (stmt);
    if (ret == SQLITE_DONE || ret == SQLITE_ROW)
      {
	  if (sqlite3_column_int (stmt, 0) == 1)
	      retcode = 1;
      }
    sqlite3_finalize (stmt);
    if (retcode == 1)
	return SQLITE_OK;
    return SQLITE_ERROR;
}

static int
execute_check_int (sqlite3 * sqlite, const char *sql)
{
/* executing an SQL statement returning an Integer Value */
    sqlite3_stmt *stmt;
    int ret;
    int value = -1;

    ret = sqlite3_prepare_v2 (sqlite, sql, strlen (sql), &stmt, NULL);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "%s", sqlite3_errmsg (sqlite));
	  return value;
      }
    ret = sqlite3_step (stmt);
    if (ret == SQLITE_DONE || ret == SQLITE_ROW)
      {
	  if (sqlite3_column_type (stmt, 0) == SQLITE_INTEGER)
	      value = sqlite3_column_int (stmt, 0);
      }
    sqlite3_finalize (stmt);
    return value;
}

static char *
execute_check_text (sqlite3 * sqlite, const char *sql)
{
/* executing an SQL statement returning a TEXT Value */
    sqlite3_stmt *stmt;
    int ret;
    char *value = NULL;

    ret = sqlite3_prepare_v2 (sqlite, sql, strlen (sql), &stmt, NULL);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "%s", sqlite3_errmsg (sqlite));
	  return value;
      }
    ret = sqlite3_step (stmt);
    if (ret == SQLITE_DONE || ret == SQLITE_ROW)
      {
	  if (sqlite3_column_type (stmt, 0) == SQLITE_TEXT)
	    {
		int len;
		const char *val = (const char *) sqlite3_column_text (stmt, 0);
		len = strlen (val);
		value = malloc (len + 1);
		strcpy (value, val);
	    }
      }
    sqlite3_finalize (stmt);
    return value;
}

#ifdef ENABLE_LIBXML2		/* only if LIBXML2 is supported */
#ifndef OMIT_ICONV		/* only if ICONV is supported */

static unsigned char *
load_xml (const char *path, int *len)
{
/* loading an external XML */
    unsigned char *xml;
    int sz = 0;
    int rd;
    FILE *fl = fopen (path, "rb");
    if (!fl)
      {
	  fprintf (stderr, "cannot open \"%s\"\n", path);
	  return NULL;
      }
    if (fseek (fl, 0, SEEK_END) == 0)
	sz = ftell (fl);
    xml = malloc (sz + 1);
    *len = sz;
    rewind (fl);
    rd = fread (xml, 1, sz, fl);
    if (rd != sz)
      {
	  fprintf (stderr, "read error \"%s\"\n", path);
	  return NULL;
      }
    fclose (fl);
    xml[rd] = '\0';
    return xml;
}

static char *
build_hex_blob (const unsigned char *blob, int blob_len)
{
/* building an HEX blob */
    int i;
    const unsigned char *p_in = blob;
    char *hex = malloc ((blob_len * 2) + 1);
    char *p_out = hex;
    for (i = 0; i < blob_len; i++)
      {
	  sprintf (p_out, "%02x", *p_in);
	  p_in++;
	  p_out += 2;
      }
    return hex;
}

static int
check_map_config (sqlite3 * handle, void *cache)
{
/* testing RL2 Map Configuration */
    int ret;
    char *err_msg = NULL;
    char *sql;
    unsigned char *blob;
    int blob_len;
    char *hexBlob;
    unsigned char *xml;
    int len;
    int intval;
    char *txtval;

/* testing Map Configuration */
    xml = load_xml ("mapconfig.xml", &len);
    if (xml == NULL)
	return -4;
    gaiaXmlToBlob (cache, xml, len, 1, NULL, &blob, &blob_len, NULL, NULL);
    free (xml);
    if (blob == NULL)
      {
	  fprintf (stderr, "this is not a well-formed XML !!!\n");
	  return -5;
      }
    hexBlob = build_hex_blob (blob, blob_len);
    free (blob);
    if (hexBlob == NULL)
	return -6;

/* Register Map Configuration Layer */
    sql = sqlite3_mprintf ("SELECT RL2_RegisterMapConfiguration(x%Q)", hexBlob);
    ret = execute_check (handle, sql, &err_msg);
    sqlite3_free (sql);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "Error RegisterMapConfiguration #1: %s\n\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -7;
      }

    xml = load_xml ("mapconfig2.xml", &len);
    if (xml == NULL)
	return -10;
    gaiaXmlToBlob (cache, xml, len, 1, NULL, &blob, &blob_len, NULL, NULL);
    free (xml);
    if (blob == NULL)
      {
	  fprintf (stderr, "this is not a well-formed XML !!!\n");
	  return -11;
      }
    hexBlob = build_hex_blob (blob, blob_len);
    free (blob);
    if (hexBlob == NULL)
	return -12;

    sql = sqlite3_mprintf ("SELECT RL2_RegisterMapConfiguration(x%Q)", hexBlob);
    ret = execute_check (handle, sql, &err_msg);
    sqlite3_free (sql);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "Error RegisterMapConfiguration #2: %s\n\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -13;
      }

/* Reload Map Configuration */
    sql =
	sqlite3_mprintf ("SELECT RL2_ReloadMapConfiguration(12, x%Q)", hexBlob);
    ret = execute_check (handle, sql, NULL);
    sqlite3_free (sql);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "Error MapConfiguration #1: %s\n\n",
		   "expected failure");
	  return -17;
      }

    sql =
	sqlite3_mprintf ("SELECT RL2_ReloadMapConfiguration(1, x%Q)", hexBlob);
    ret = execute_check (handle, sql, NULL);
    sqlite3_free (sql);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "Error MapConfiguration #2: %s\n\n",
		   "expected failure");
	  return -18;
      }

    sql =
	sqlite3_mprintf ("SELECT RL2_ReloadMapConfiguration('bad map', x%Q)",
			 hexBlob);
    ret = execute_check (handle, sql, NULL);
    sqlite3_free (sql);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "Error MapConfiguration #4: %s\n\n",
		   "expected failure");
	  return -20;
      }
    free (hexBlob);

    xml = load_xml ("mapconfig.xml", &len);
    if (xml == NULL)
	return -22;
    gaiaXmlToBlob (cache, xml, len, 1, NULL, &blob, &blob_len, NULL, NULL);
    free (xml);
    if (blob == NULL)
      {
	  fprintf (stderr, "this is not a well-formed XML !!!\n");
	  return -23;
      }
    hexBlob = build_hex_blob (blob, blob_len);
    free (blob);
    if (hexBlob == NULL)
	return -24;
    sql =
	sqlite3_mprintf ("SELECT RL2_ReloadMapConfiguration(1, x%Q)", hexBlob);
    ret = execute_check (handle, sql, &err_msg);
    sqlite3_free (sql);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "Error ReloadMapConfiguration #6: %s\n\n", err_msg);
	  sqlite3_free (err_msg);
	  return -25;
      }

    sql =
	sqlite3_mprintf ("SELECT RL2_ReloadMapConfiguration('my-map', x%Q)",
			 hexBlob);
    ret = execute_check (handle, sql, &err_msg);
    sqlite3_free (sql);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "Error ReloadMapConfiguration #7: %s\n\n", err_msg);
	  sqlite3_free (err_msg);
	  return -26;
      }
    free (hexBlob);

/* testing Name, Title and Abstract */
    sql = sqlite3_mprintf ("SELECT RL2_NumMapConfigurations()");
    intval = execute_check_int (handle, sql);
    if (intval != 2)
      {
	  fprintf (stderr,
		   "Error RL2_NumMapConfigurations() = %d (expected 2)\n",
		   intval);
	  return -27;
      }

    sql = sqlite3_mprintf ("SELECT RL2_MapConfigurationNameN(2)");
    txtval = execute_check_text (handle, sql);
    if (txtval == NULL)
      {
	  fprintf (stderr,
		   "Error RL2_MapConfigurationNameN(1): unexpected NULL\n");
	  return -28;
      }
    if (strcmp (txtval, "my-map") != 0)
      {
	  fprintf (stderr,
		   "Error RL2_MapConfigurationNameN(1): unexpected \"%s\"\n",
		   txtval);
	  free (txtval);
	  return -29;
      }
    free (txtval);

    sql = sqlite3_mprintf ("SELECT RL2_MapConfigurationTitleN(2)");
    txtval = execute_check_text (handle, sql);
    if (txtval == NULL)
      {
	  fprintf (stderr,
		   "Error RL2_MapConfigurationTitleN(1): unexpected NULL\n");
	  return -30;
      }
    if (strcmp (txtval, "another arbitrary title") != 0)
      {
	  fprintf (stderr,
		   "Error RL2_MapConfigurationTitleN(1): unexpected \"%s\"\n",
		   txtval);
	  free (txtval);
	  return -31;
      }
    free (txtval);

    sql = sqlite3_mprintf ("SELECT RL2_MapConfigurationAbstractN(2)");
    txtval = execute_check_text (handle, sql);
    if (txtval == NULL)
      {
	  fprintf (stderr,
		   "Error RL2_MapConfigurationAbstractN(1): unexpected NULL\n");
	  return -32;
      }
    if (strcmp (txtval, "an abstract as any other") != 0)
      {
	  fprintf (stderr,
		   "Error RL2_MapConfigurationAbstractN(1): unexpected \"%s\"\n",
		   txtval);
	  free (txtval);
	  return -33;
      }
    free (txtval);

/* Unregister Map Configuration */
    sql = sqlite3_mprintf ("SELECT RL2_UnRegisterMapConfiguration(5)");
    ret = execute_check (handle, sql, NULL);
    sqlite3_free (sql);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "Error UnRegisterMapConfiguration #1: %s\n\n",
		   "expected failure");
	  return -34;
      }

    sql = sqlite3_mprintf ("SELECT RL2_UnRegisterMapConfiguration('alpha')");
    ret = execute_check (handle, sql, &err_msg);
    sqlite3_free (sql);
    if (ret == SQLITE_OK)
      {
	  fprintf (stderr, "Error UnRegisterMapConfiguration #2: %s\n\n",
		   "expected failure");
	  return -35;
      }

    sql =
	sqlite3_mprintf
	("SELECT RL2_UnRegisterMapConfiguration('another-map')");
    ret = execute_check (handle, sql, &err_msg);
    sqlite3_free (sql);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "Error UnRegisterMapConfiguration #4: %s\n\n",
		   err_msg);
	  sqlite3_free (err_msg);
	  return -36;
      }

    return 0;
}

#endif
#endif

int
main (int argc, char *argv[])
{
    int ret;
    sqlite3 *handle;
    char *err_msg = NULL;
    char *sql;
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

    sql = "SELECT InitSpatialMetadata(1, 'WGS84')";
    ret = execute_check (handle, sql, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr,
		   "Unexpected InitSpatialMetadata result: %i, (%s)\n", ret,
		   err_msg);
	  sqlite3_free (err_msg);
	  return -2;
      }

#ifdef ENABLE_LIBXML2		/* only if LIBXML2 is supported */
#ifndef OMIT_ICONV		/* only if ICONV is supported */

/* creating the Styling Tables */
    sql = "SELECT CreateStylingTables(1)";
    ret = execute_check (handle, sql, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "Error CreateStylingTables %s\n\n", err_msg);
	  sqlite3_free (err_msg);
	  return -3;
      }

    ret = check_map_config (handle, cache);
    if (ret != 0)
	return -100 - ret;

#endif
#endif

    ret = sqlite3_close (handle);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "sqlite3_close() error: %s\n",
		   sqlite3_errmsg (handle));
	  return -57;
      }

    spatialite_cleanup_ex (cache);

    spatialite_shutdown ();
    return 0;
}
