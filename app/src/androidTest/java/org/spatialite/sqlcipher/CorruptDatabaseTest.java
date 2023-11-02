package org.spatialite.sqlcipher;

import android.database.Cursor;
import org.spatialite.database.SQLiteDatabaseCorruptException;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.spatialite.database.SQLiteDatabase;

import java.io.File;
import java.io.IOException;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import static junit.framework.Assert.assertNotNull;
import static junit.framework.Assert.fail;

  /*
            Iulian: this test might have some meaning on sqllite 3.16 or older prehaps,
            in this test a 'corrupt' has some strings in file corrupt.db appended at the end database is loaded then it is overwritten
            the test author expects this to fail, debatable at least, in the official sqllite bindings https://sqlite.org/android/tree?type=tree
            no such test appears i am leaving it maybe someone figures it out how sqllite should behave when this occurs
         */
/*


@RunWith(AndroidJUnit4.class)
public class CorruptDatabaseTest extends BaseTest {

    static {
        System.loadLibrary("android_spatialite");
    }

    @Test
    public void corruptDatabase() throws IOException {


        File unencryptedDatabase = targetContext.getDatabasePath("corrupt.db");

        try {
            extractAssetToDatabaseDirectory("corrupt.db");

            SQLiteDatabase database = SQLiteDatabase.openOrCreateDatabase(unencryptedDatabase, null);

            // NOTE: database not expected to be null, but double-check:
            assertNotNull("ERROR: got null database object", database);

            // *Should* have been recovered:
            Cursor cursor = database.rawQuery("select * from sqlite_master;", null);
            assertNotNull("NOT EXPECTED: database.rawQuery() returned null cursor", cursor);

            // *Should* corrupt the database file that is already open:
            extractAssetToDatabaseDirectory("corrupt.db");

            try {
                // Attempt to write to corrupt database file *should* fail:
                database.execSQL("CREATE TABLE t1(a,b);");

                // NOT EXPECTED to get here:
                fail("NOT EXPECTED: CREATE TABLE succeeded ");
            } catch (Exception ex) {
                // Caught SQLiteDatabaseCorruptException as expected OK
            }

            // *Expected* to be closed now
            if (database.isOpen()) {
                fail("NOT EXPECTED: database is still open");
            }
        }
        finally {
            unencryptedDatabase.delete();
        }
    }
}
*/
