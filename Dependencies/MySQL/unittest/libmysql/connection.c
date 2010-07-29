/* Copyright (C) 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA */

/**
  Some basic tests of the client API.
*/

#include "my_test.h"

static int test_bug20023(MYSQL *mysql)
{
  int sql_big_selects_orig;
  int max_join_size_orig;

  int sql_big_selects_2;
  int sql_big_selects_3;
  int sql_big_selects_4;
  int sql_big_selects_5;
  int rc;

  if (mysql_get_server_version(mysql) < 50100) {
    diag("Test requires MySQL Server version 5.1 or above");
    return SKIP;
  }

  /***********************************************************************
    Remember original SQL_BIG_SELECTS, MAX_JOIN_SIZE values.
  ***********************************************************************/

  query_int_variable(mysql,
                     "@@session.sql_big_selects",
                     &sql_big_selects_orig);

  query_int_variable(mysql,
                     "@@global.max_join_size",
                     &max_join_size_orig);

  /***********************************************************************
    Test that COM_CHANGE_USER resets the SQL_BIG_SELECTS to the initial value.
  ***********************************************************************/

  /* Issue COM_CHANGE_USER. */
  rc= mysql_change_user(mysql, username, password, schema);
  check_mysql_rc(rc, mysql);

  /* Query SQL_BIG_SELECTS. */

  query_int_variable(mysql,
                     "@@session.sql_big_selects",
                     &sql_big_selects_2);

  /* Check that SQL_BIG_SELECTS is reset properly. */

  FAIL_UNLESS(sql_big_selects_orig == sql_big_selects_2, "Different value for sql_big_select");

  /***********************************************************************
    Test that if MAX_JOIN_SIZE set to non-default value,
    SQL_BIG_SELECTS will be 0.
  ***********************************************************************/

  /* Set MAX_JOIN_SIZE to some non-default value. */

  rc= mysql_query(mysql, "SET @@global.max_join_size = 10000");
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql, "SET @@session.max_join_size = default");
  check_mysql_rc(rc, mysql);

  /* Issue COM_CHANGE_USER. */

  rc= mysql_change_user(mysql, username, password, schema);
  check_mysql_rc(rc, mysql);

  /* Query SQL_BIG_SELECTS. */

  query_int_variable(mysql,
                     "@@session.sql_big_selects",
                     &sql_big_selects_3);

  /* Check that SQL_BIG_SELECTS is 0. */

  FAIL_UNLESS(sql_big_selects_3 == 0, "big_selects != 0");

  /***********************************************************************
    Test that if MAX_JOIN_SIZE set to default value,
    SQL_BIG_SELECTS will be 1.
  ***********************************************************************/

  /* Set MAX_JOIN_SIZE to the default value (-1). */

  rc= mysql_query(mysql, "SET @@global.max_join_size = -1");
  rc= mysql_query(mysql, "SET @@session.max_join_size = default");

  /* Issue COM_CHANGE_USER. */

  rc= mysql_change_user(mysql, username, password, schema);
  check_mysql_rc(rc, mysql);

  /* Query SQL_BIG_SELECTS. */

  query_int_variable(mysql,
                     "@@session.sql_big_selects",
                     &sql_big_selects_4);

  /* Check that SQL_BIG_SELECTS is 1. */

  FAIL_UNLESS(sql_big_selects_4 == 1, "sql_big_select != 1");

  /***********************************************************************
    Restore MAX_JOIN_SIZE.
    Check that SQL_BIG_SELECTS will be the original one.
  ***********************************************************************/

  rc= mysql_query(mysql, "SET @@global.max_join_size = -1");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "SET @@session.max_join_size = default");
  check_mysql_rc(rc, mysql);

  /* Issue COM_CHANGE_USER. */

  rc= mysql_change_user(mysql, username, password, schema);
  check_mysql_rc(rc, mysql);

  /* Query SQL_BIG_SELECTS. */

  query_int_variable(mysql,
                     "@@session.sql_big_selects",
                     &sql_big_selects_5);

  /* Check that SQL_BIG_SELECTS is 1. */

  FAIL_UNLESS(sql_big_selects_5 == sql_big_selects_orig, "big_select != 1");

  /***********************************************************************
    That's it. Cleanup.
  ***********************************************************************/

  return OK;
}

static int test_change_user(MYSQL *mysql)
{
  char buff[256];
  const char *user_pw= "mysqltest_pw";
  const char *user_no_pw= "mysqltest_no_pw";
  const char *pw= "password";
  const char *db= "mysqltest_user_test_database";
  int rc;

  DBUG_ENTER("test_change_user");

  /* Prepare environment */
  sprintf(buff, "drop database if exists %s", db);
  rc= mysql_query(mysql, buff);
  check_mysql_rc(rc, mysql)

  sprintf(buff, "create database %s", db);
  rc= mysql_query(mysql, buff);
  check_mysql_rc(rc, mysql)

  sprintf(buff,
          "grant select on %s.* to %s@'%%' identified by '%s'",
          db,
          user_pw,
          pw);
  rc= mysql_query(mysql, buff);
  check_mysql_rc(rc, mysql)

  sprintf(buff,
          "grant select on %s.* to %s@'%%'",
          db,
          user_no_pw);
  rc= mysql_query(mysql, buff);
  check_mysql_rc(rc, mysql)


  /* Try some combinations */
  rc= mysql_change_user(mysql, NULL, NULL, NULL);
  FAIL_UNLESS(rc, "Error expected");


  rc= mysql_change_user(mysql, "", NULL, NULL);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, "", "", NULL);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, "", "", "");
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, NULL, "", "");
  FAIL_UNLESS(rc, "Error expected");


  rc= mysql_change_user(mysql, NULL, NULL, "");
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, "", NULL, "");
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_pw, NULL, "");
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_pw, "", "");
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_pw, "", NULL);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_pw, NULL, NULL);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_pw, "", db);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_pw, NULL, db);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_pw, pw, db);
  check_mysql_rc(rc, mysql)

  rc= mysql_change_user(mysql, user_pw, pw, NULL);
  check_mysql_rc(rc, mysql)

  rc= mysql_change_user(mysql, user_pw, pw, "");
  check_mysql_rc(rc, mysql)

  rc= mysql_change_user(mysql, user_no_pw, pw, db);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_no_pw, pw, "");
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_no_pw, pw, NULL);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, user_no_pw, "", NULL);
  check_mysql_rc(rc, mysql)

  rc= mysql_change_user(mysql, user_no_pw, "", "");
  check_mysql_rc(rc, mysql)

  rc= mysql_change_user(mysql, user_no_pw, "", db);
  check_mysql_rc(rc, mysql)

  rc= mysql_change_user(mysql, user_no_pw, NULL, db);
  check_mysql_rc(rc, mysql)

  rc= mysql_change_user(mysql, "", pw, db);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, "", pw, "");
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, "", pw, NULL);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, NULL, pw, NULL);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, NULL, NULL, db);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, NULL, "", db);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, "", "", db);
  FAIL_UNLESS(rc, "Error expected");

  /* Cleanup the environment */

  rc= mysql_change_user(mysql, username, password, schema);
  check_mysql_rc(rc, mysql);

  sprintf(buff, "drop database %s", db);
  rc= mysql_query(mysql, buff);
  check_mysql_rc(rc, mysql)

  sprintf(buff, "drop user %s@'%%'", user_pw);
  rc= mysql_query(mysql, buff);
  check_mysql_rc(rc, mysql)

  sprintf(buff, "drop user %s@'%%'", user_no_pw);
  rc= mysql_query(mysql, buff);
  check_mysql_rc(rc, mysql)

  return OK;
}

/**
  Bug#31669 Buffer overflow in mysql_change_user()
*/

#define LARGE_BUFFER_SIZE 2048

static int test_bug31669(MYSQL *mysql)
{
  int rc;
  static char buff[LARGE_BUFFER_SIZE+1];
  static char user[USERNAME_CHAR_LENGTH+1];
  static char db[NAME_CHAR_LEN+1];
  static char query[LARGE_BUFFER_SIZE*2];

  rc= mysql_change_user(mysql, NULL, NULL, NULL);
  FAIL_UNLESS(rc, "Error expected");

  rc= mysql_change_user(mysql, "", "", "");
  FAIL_UNLESS(rc, "Error expected");

  memset(buff, 'a', sizeof(buff));

  rc= mysql_change_user(mysql, buff, buff, buff);
  FAIL_UNLESS(rc, "Error epected");

  rc = mysql_change_user(mysql, username, password, schema);
  check_mysql_rc(rc, mysql);

  memset(db, 'a', sizeof(db));
  db[NAME_CHAR_LEN]= 0;
  sprintf(query, "CREATE DATABASE IF NOT EXISTS %s", db);
  rc= mysql_query(mysql, query);
  check_mysql_rc(rc, mysql);

  memset(user, 'b', sizeof(user));
  user[USERNAME_CHAR_LENGTH]= 0;
  memset(buff, 'c', sizeof(buff));
  buff[LARGE_BUFFER_SIZE]= 0;
  sprintf(query, "GRANT ALL PRIVILEGES ON *.* TO '%s'@'%%' IDENTIFIED BY '%s' WITH GRANT OPTION", user, buff);
  rc= mysql_query(mysql, query);
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "FLUSH PRIVILEGES");
  check_mysql_rc(rc, mysql);

  rc= mysql_change_user(mysql, user, buff, db);
  check_mysql_rc(rc, mysql);

  user[USERNAME_CHAR_LENGTH-1]= 'a';
  rc= mysql_change_user(mysql, user, buff, db);
  FAIL_UNLESS(rc, "Error expected");

  user[USERNAME_CHAR_LENGTH-1]= 'b';
  buff[LARGE_BUFFER_SIZE-1]= 'd';
  rc= mysql_change_user(mysql, user, buff, db);
  FAIL_UNLESS(rc, "Error expected");

  buff[LARGE_BUFFER_SIZE-1]= 'c';
  db[NAME_CHAR_LEN-1]= 'e';
  rc= mysql_change_user(mysql, user, buff, db);
  FAIL_UNLESS(rc, "Error expected");

  db[NAME_CHAR_LEN-1]= 'a';
  rc= mysql_change_user(mysql, user, buff, db);
  FAIL_UNLESS(!rc, "Error expected");

  rc= mysql_change_user(mysql, user + 1, buff + 1, db + 1);
  FAIL_UNLESS(rc, "Error expected");

  rc = mysql_change_user(mysql, username, password, schema);
  check_mysql_rc(rc, mysql);

  sprintf(query, "DROP DATABASE %s", db);
  rc= mysql_query(mysql, query);
  check_mysql_rc(rc, mysql);

  sprintf(query, "DELETE FROM mysql.user WHERE User='%s'", user);
  rc= mysql_query(mysql, query);
  check_mysql_rc(rc, mysql);
  FAIL_UNLESS(mysql_affected_rows(mysql) == 1, "");

  return OK;
}

/**
     Bug# 33831 mysql_real_connect() should fail if
     given an already connected MYSQL handle.
*/

static int test_bug33831(MYSQL *mysql)
{
  FAIL_IF(mysql_real_connect(mysql, hostname, username,
                             password, schema, port, socketname, 0), 
         "Error expected");
  
  return OK;
}

/* Test MYSQL_OPT_RECONNECT, Bug#15719 */

static int test_opt_reconnect(MYSQL *mysql)
{
  my_bool my_true= TRUE;
  int rc;

  mysql= mysql_init(NULL);
  FAIL_IF(!mysql, "not enough memory");

  FAIL_UNLESS(mysql->reconnect == 0, "reconnect != 0");

  rc= mysql_options(mysql, MYSQL_OPT_RECONNECT, &my_true);
  check_mysql_rc(rc, mysql);

  FAIL_UNLESS(mysql->reconnect == 1, "reconnect != 1");

  if (!(mysql_real_connect(mysql, hostname, username,
                           password, schema, port,
                           socketname, 0)))
  {
    diag("connection failed");
    mysql_close(mysql);
    return FAIL;
  }

  FAIL_UNLESS(mysql->reconnect == 1, "reconnect != 1");

  mysql_close(mysql);

  mysql= mysql_init(NULL);
  FAIL_IF(!mysql, "not enough memory");

  FAIL_UNLESS(mysql->reconnect == 0, "reconnect != 0");

  if (!(mysql_real_connect(mysql, hostname, username,
                           password, schema, port,
                           socketname, 0)))
  {
    diag("connection failed");
    mysql_close(mysql);
    return FAIL;
  }

  FAIL_UNLESS(mysql->reconnect == 0, "reconnect != 0");

  mysql_close(mysql);
  return OK;
}

static int test_compress(MYSQL *mysql)
{
  MYSQL_RES *res;
  MYSQL_ROW row;
  int rc;

  mysql= mysql_init(NULL);
  FAIL_IF(!mysql, "not enough memory");

  /* use compressed protocol */
  rc= mysql_options(mysql, MYSQL_OPT_COMPRESS, NULL);



  if (!(mysql_real_connect(mysql, hostname, username,
                           password, schema, port,
                           socketname, 0)))
  {
    diag("connection failed");
    return FAIL;
  }

  rc= mysql_query(mysql, "SHOW STATUS LIKE 'compression'");
  check_mysql_rc(rc, mysql);
  res= mysql_store_result(mysql);
  row= mysql_fetch_row(res);
  FAIL_UNLESS(strcmp(row[1], "ON") == 0, "Compression off");
  mysql_free_result(res);

  mysql_close(mysql);
  return OK;
}

struct my_tests_st my_tests[] = {
  {"test_bug20023", test_bug20023, TEST_CONNECTION_NEW, 0, NULL,  NULL},
  {"test_bug31669", test_bug31669, TEST_CONNECTION_NEW, 0, NULL,  NULL},
  {"test_bug33831", test_bug33831, TEST_CONNECTION_NEW, 0, NULL,  NULL},
  {"test_change_user", test_change_user, TEST_CONNECTION_NEW, 0, NULL,  NULL},
  {"test_opt_reconnect", test_opt_reconnect, TEST_CONNECTION_NONE, 0, NULL,  NULL},
  {"test_compress", test_compress, TEST_CONNECTION_NONE, 0, NULL,  NULL},
  {NULL, NULL, 0, 0, NULL, NULL}
};


int main(int argc, char **argv)
{
  if (argc > 1)
    get_options(&argc, &argv);

  get_envvars();

  run_tests(my_tests);

  return(exit_status());
}
