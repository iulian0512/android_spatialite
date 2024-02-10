/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// modified from original source see README at the top level of this project

package org.spatialite;

import android.database.Cursor;
import android.util.Log;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.spatialite.database.SQLiteDatabase;

import java.util.ArrayList;
import java.util.Locale;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.MediumTest;
import androidx.test.filters.SmallTest;
import androidx.test.filters.Suppress;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class DatabaseLocaleTest {

    static {
        System.loadLibrary("android_spatialite");
    }


    private SQLiteDatabase mDatabase;

    private static final String[] STRINGS = {
        "c\u00f4t\u00e9",
        "cote",
        "c\u00f4te",
        "cot\u00e9",
        "boy",
        "dog",
        "COTE",
    };

    @Before
    public void setUp() throws Exception {
        mDatabase = SQLiteDatabase.create(null);
        mDatabase.execSQL(
                "CREATE TABLE test (id INTEGER PRIMARY KEY, data TEXT COLLATE LOCALIZED);");
    }

    private void insertStrings() {
        for (String s : STRINGS) {
            mDatabase.execSQL("INSERT INTO test (data) VALUES('" + s + "');");
        }
    }

    @After
    public void tearDown() throws Exception {
        mDatabase.close();
    }

    private String[] query(String sql) {
        Log.i("LocaleTest", "Querying: " + sql);
        Cursor c = mDatabase.rawQuery(sql, null);
        assertNotNull(c);
        ArrayList<String> items = new ArrayList<>();
        while (c.moveToNext()) {
            items.add(c.getString(0));
            Log.i("LocaleTest", "...." + c.getString(0));
        }
        String[] result = items.toArray(new String[items.size()]);
        assertEquals(STRINGS.length, result.length);
        c.close();
        return result;
    }

    @MediumTest
    @Test
    public void testLocaleInsertOrder() throws Exception {
        insertStrings();
        String[] results = query("SELECT data FROM test");
        assertEquals(STRINGS, results);
    }

    @Suppress // not supporting localized collators
    @MediumTest
    @Test
    public void testLocaleenUS() throws Exception {
        insertStrings();
        Log.i("LocaleTest", "about to call setLocale en_US");
        mDatabase.setLocale(new Locale("en", "US"));
        String[] results;
        results = query("SELECT data FROM test ORDER BY data COLLATE LOCALIZED ASC");

        // The database code currently uses PRIMARY collation strength,
        // meaning that all versions of a character compare equal (regardless
        // of case or accents), leaving the "cote" flavors in database order.
        assertEquals(results, new String[] {
                STRINGS[4],  // "boy"
                STRINGS[0],  // sundry forms of "cote"
                STRINGS[1],
                STRINGS[2],
                STRINGS[3],
                STRINGS[6],  // "COTE"
                STRINGS[5],  // "dog"
        });
    }

    @SmallTest
    @Test
    public void testHoge() throws Exception {
        Cursor cursor = null;
        try {
            String expectedString = new String(new int[] {0xFE000}, 0, 1);
            mDatabase.execSQL("INSERT INTO test(id, data) VALUES(1, '" + expectedString + "')");
            cursor = mDatabase.rawQuery("SELECT data FROM test WHERE id = 1", null);
            
            assertNotNull(cursor);
            assertTrue(cursor.moveToFirst());
            String actualString = cursor.getString(0);
            assertEquals(expectedString.length(), actualString.length());
            for (int i = 0; i < expectedString.length(); i++) {
                assertEquals((int)expectedString.charAt(i), (int)actualString.charAt(i));
            }
            assertEquals(expectedString, actualString);
        } finally {
            if (cursor != null) cursor.close();
        }
    }

    @SmallTest
    @Test
    public void testRemoveAccents() {
        // Insert a record with accented text
        mDatabase.execSQL("INSERT INTO test (id,data) VALUES (141,'álphá béttá gámmá'),(142,null)");

        // Query the database using your custom remove_accents function
        Cursor cursor = mDatabase.rawQuery("SELECT id,remove_accents(data) FROM test", null);

        // Move to the first (and only) row in the result set
        if (cursor != null && cursor.moveToFirst()) {
            int id=cursor.getInt(0);
            // Get the string from the query result
            String data = cursor.getString(1);

            // Compare the modified string to the expected string
            switch (id)
            {
                case 141:
                    assertEquals("alpha betta gamma", data);
                    break;
                case 142:
                    assertNull(data);
                    break;
            }

            // Close the cursor
            if(!cursor.isClosed())
                cursor.close();
        } else {
            throw new AssertionError("Cursor is null or empty.");
        }
    }
}
