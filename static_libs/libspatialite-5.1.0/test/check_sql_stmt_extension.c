/*

 check_sql_stmt_extension.c -- SpatiaLite Test Case

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
 
Portions created by the Initial Developer are Copyright (C) 2019
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

#include "check_sql_stmt.h"

int
main (int argc, char *argv[])
{
    int result = 0;
    void *cache = spatialite_alloc_connection ();
    struct db_conn conn;
    conn.db_path = NULL;
    conn.db_handle = NULL;
    conn.cache = cache;

/* testing in load_extension mode */
    fprintf (stderr, "\n****************** testing in load_extension mode\n\n");
    if (argc == 1)
      {
	  result = run_all_testcases (&conn, 1, 0);
      }
    else
      {
	  result = run_specified_testcases (argc, argv, &conn, 1);
      }
    close_connection (&conn);
    spatialite_cleanup_ex (conn.cache);

    spatialite_shutdown ();
    return result;
}
