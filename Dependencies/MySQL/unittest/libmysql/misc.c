/* Copyright (C) 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include "my_test.h"

/*
  Bug#28075 "COM_DEBUG crashes mysqld"
*/

static int test_bug28075(MYSQL *mysql)
{
  int rc;

  rc= mysql_dump_debug_info(mysql);
  check_mysql_rc(rc, mysql);

  rc= mysql_ping(mysql);
  check_mysql_rc(rc, mysql);

  return OK;
}

/*
  Bug#28505: mysql_affected_rows() returns wrong value if CLIENT_FOUND_ROWS
  flag is set.
*/

static int test_bug28505(MYSQL *mysql)
{
  my_ulonglong res;
  int rc;

  rc= mysql_query(mysql, "drop table if exists t1");
  rc= mysql_query(mysql, "create table t1(f1 int primary key)");
  rc= mysql_query(mysql, "insert into t1 values(1)");
  rc= mysql_query(mysql, "insert into t1 values(1) on duplicate key update f1=1");
  res= mysql_affected_rows(mysql);
  FAIL_UNLESS(!res, "res != 0");
  rc= mysql_query(mysql, "drop table t1");
  return OK;
}

/*
  Bug #29692  	Single row inserts can incorrectly report a huge number of 
  row insertions
*/

static int test_bug29692(MYSQL *mysql)
{
  int rc;
  rc= mysql_query(mysql, "drop table if exists t1");
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql, "create table t1(f1 int)");
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql, "insert into t1 values(1)");
  check_mysql_rc(rc, mysql);
  FAIL_UNLESS(1 == mysql_affected_rows(mysql), "affected_rows != 1");
  rc= mysql_query(mysql, "drop table t1");
  check_mysql_rc(rc, mysql);
  return OK;
}

static int bug31418_impl()
{
  my_bool is_null;
  MYSQL *mysql;
  int rc;

  /* Create a new connection. */

  mysql= test_connect(NULL);
  if (!mysql)
    return FAIL;

  /***********************************************************************
    Check that lock is free:
      - IS_FREE_LOCK() should return 1;
      - IS_USED_LOCK() should return NULL;
  ***********************************************************************/

  is_null= query_int_variable(mysql,
                              "IS_FREE_LOCK('bug31418')",
                              &rc);
  FAIL_UNLESS(!is_null && rc, "rc = 0");

  is_null= query_int_variable(mysql,
                              "IS_USED_LOCK('bug31418')",
                              &rc);
  FAIL_UNLESS(is_null, "rc = 0");

  /***********************************************************************
    Acquire lock and check the lock status (the lock must be in use):
      - IS_FREE_LOCK() should return 0;
      - IS_USED_LOCK() should return non-zero thread id;
  ***********************************************************************/

  query_int_variable(mysql, "GET_LOCK('bug31418', 1)", &rc);
  FAIL_UNLESS(rc, "rc = 0");

  is_null= query_int_variable(mysql,
                              "IS_FREE_LOCK('bug31418')",
                              &rc);
  FAIL_UNLESS(!is_null && !rc, "rc = 0");

  is_null= query_int_variable(mysql,
                              "IS_USED_LOCK('bug31418')",
                              &rc);
  FAIL_UNLESS(!is_null && rc, "rc = 0");

  /***********************************************************************
    Issue COM_CHANGE_USER command and check the lock status
    (the lock must be free):
      - IS_FREE_LOCK() should return 1;
      - IS_USED_LOCK() should return NULL;
  **********************************************************************/

  rc= mysql_change_user(mysql, username, password, schema ? schema : "test");
  check_mysql_rc(rc, mysql);

  is_null= query_int_variable(mysql,
                              "IS_FREE_LOCK('bug31418')",
                              &rc);
  FAIL_UNLESS(!is_null && rc, "rc = 0");

  is_null= query_int_variable(mysql,
                              "IS_USED_LOCK('bug31418')",
                              &rc);
  FAIL_UNLESS(is_null, "rc = 0");

  /***********************************************************************
   That's it. Cleanup.
  ***********************************************************************/

  mysql_close(mysql);
  return OK;
}

static int test_bug31418(MYSQL *mysql)
{
 int i;
  /* Run test case for BUG#31418 for three different connections. */

  for (i=0; i < 3; i++)
    if (bug31418_impl())
      return FAIL;

  return OK;
}

/*
  Altough mysql_create_db(), mysql_rm_db() are deprecated since 4.0 they
  should not crash server and should not hang in case of errors.

  Since those functions can't be seen in modern API (unless client library
  was compiled with USE_OLD_FUNCTIONS define) we use simple_command() macro.
*/
static int test_bug6081(MYSQL *mysql)
{
  int rc;

  if (mysql_get_server_version(mysql) < 50100) {
    diag("Test requires MySQL Server version 5.1 or above");
    return SKIP;
  }

  rc= simple_command(mysql, COM_DROP_DB, (uchar*) schema,
                     (ulong)strlen(schema), 0U);
  FAIL_IF(!rc, "Error expected");

  rc= simple_command(mysql, COM_CREATE_DB, (uchar*) schema,
                     (ulong)strlen(schema), 0U);
  FAIL_IF(!rc, "Error expected");
  
  rc= mysql_select_db(mysql, schema);
  check_mysql_rc(rc, mysql);
  return OK;
}

/* Query processing */

static int test_debug_example(MYSQL *mysql)
{
  int rc;
  MYSQL_RES *result;


  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_debug_example");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "CREATE TABLE test_debug_example("
                         "id INT PRIMARY KEY AUTO_INCREMENT, "
                         "name VARCHAR(20), xxx INT)");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "INSERT INTO test_debug_example (name) "
                         "VALUES ('mysql')");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "UPDATE test_debug_example SET name='updated' "
                         "WHERE name='deleted'");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "SELECT * FROM test_debug_example where name='mysql'");
  check_mysql_rc(rc, mysql);

  result= mysql_use_result(mysql);
  FAIL_IF(!result, "Invalid result set");

  while (mysql_fetch_row(result));
  mysql_free_result(result);

  rc= mysql_query(mysql, "DROP TABLE test_debug_example");
  check_mysql_rc(rc, mysql);
  return OK;
}

/*
  Test a crash when invalid/corrupted .frm is used in the
  SHOW TABLE STATUS
  bug #93 (reported by serg@mysql.com).
*/

static int test_frm_bug(MYSQL *mysql)
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  MYSQL_RES  *result;
  MYSQL_ROW  row;
  FILE       *test_file;
  char       data_dir[FN_REFLEN];
  char       test_frm[FN_REFLEN];
  int        rc;


  mysql_autocommit(mysql, TRUE);

  rc= mysql_query(mysql, "drop table if exists test_frm_bug");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "flush tables");
  check_mysql_rc(rc, mysql);

  stmt= mysql_stmt_init(mysql);
  FAIL_IF(!stmt, mysql_error(mysql));
  rc= mysql_stmt_prepare(stmt, "show variables like 'datadir'", strlen("show variables like 'datadir'"));
  check_stmt_rc(rc, stmt);

  rc= mysql_stmt_execute(stmt);
  check_stmt_rc(rc, stmt);

  memset(my_bind, '\0', sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= data_dir;
  my_bind[0].buffer_length= FN_REFLEN;
  my_bind[1]= my_bind[0];

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_stmt_rc(rc, stmt);

  rc= mysql_stmt_fetch(stmt);
  check_stmt_rc(rc, stmt);

  rc= mysql_stmt_fetch(stmt);
  FAIL_UNLESS(rc == MYSQL_NO_DATA, "rc != MYSQL_NO_DATA");

  sprintf(test_frm, "%s/%s/test_frm_bug.frm", data_dir, schema);


  if (!(test_file= my_fopen(test_frm, (int) (O_RDWR | O_CREAT), MYF(MY_WME))))
  {
    mysql_stmt_close(stmt);
    diag("Can't write to file %s -> SKIP", test_frm);
    return SKIP;
  }

  rc= mysql_query(mysql, "SHOW TABLE STATUS like 'test_frm_bug'");
  check_mysql_rc(rc, mysql);

  result= mysql_store_result(mysql);
  FAIL_IF(!result, "Invalid result set");/* It can't be NULL */

  rc= 0;
  while (mysql_fetch_row(result))
    rc++;
  FAIL_UNLESS(rc == 1, "rowcount != 0");

  mysql_data_seek(result, 0);

  row= mysql_fetch_row(result);
  FAIL_IF(!row, "couldn't fetch row");

  FAIL_UNLESS(row[17] != 0, "row[17] != 0");

  mysql_free_result(result);
  mysql_stmt_close(stmt);

  my_fclose(test_file, MYF(0));
  mysql_query(mysql, "drop table if exists test_frm_bug");
  return OK;
}

static int test_wl4166_1(MYSQL *mysql)
{
  MYSQL_STMT *stmt;
  int        int_data;
  char       str_data[50];
  char       tiny_data;
  short      small_data;
  longlong   big_data;
  float      real_data;
  double     double_data;
  ulong      length[7];
  my_bool    is_null[7];
  MYSQL_BIND my_bind[7];
  static char *query;
  int rc;
  int i;

  if (mysql_get_server_version(mysql) < 50100) {
    diag("Test requires MySQL Server version 5.1 or above");
    return SKIP;
  }
  rc= mysql_query(mysql, "DROP TABLE IF EXISTS table_4166");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "CREATE TABLE table_4166(col1 tinyint NOT NULL, "
                         "col2 varchar(15), col3 int, "
                         "col4 smallint, col5 bigint, "
                         "col6 float, col7 double, "
                         "colX varchar(10) default NULL)");
  check_mysql_rc(rc, mysql);

  stmt= mysql_stmt_init(mysql);
  FAIL_IF(!stmt, mysql_error(mysql));
  query= "INSERT INTO table_4166(col1, col2, col3, col4, col5, col6, col7) "
          "VALUES(?, ?, ?, ?, ?, ?, ?)";
  rc= mysql_stmt_prepare(stmt, query, strlen(query));
  check_stmt_rc(rc, stmt);

  FAIL_IF(mysql_stmt_param_count(stmt) != 7, "param_count != 7");

  memset(my_bind, '\0', sizeof(my_bind));
  /* tinyint */
  my_bind[0].buffer_type= MYSQL_TYPE_TINY;
  my_bind[0].buffer= (void *)&tiny_data;
  /* string */
  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void *)str_data;
  my_bind[1].buffer_length= 1000;                  /* Max string length */
  /* integer */
  my_bind[2].buffer_type= MYSQL_TYPE_LONG;
  my_bind[2].buffer= (void *)&int_data;
  /* short */
  my_bind[3].buffer_type= MYSQL_TYPE_SHORT;
  my_bind[3].buffer= (void *)&small_data;
  /* bigint */
  my_bind[4].buffer_type= MYSQL_TYPE_LONGLONG;
  my_bind[4].buffer= (void *)&big_data;
  /* float */
  my_bind[5].buffer_type= MYSQL_TYPE_FLOAT;
  my_bind[5].buffer= (void *)&real_data;
  /* double */
  my_bind[6].buffer_type= MYSQL_TYPE_DOUBLE;
  my_bind[6].buffer= (void *)&double_data;

  for (i= 0; i < (int) array_elements(my_bind); i++)
  {
    my_bind[i].length= &length[i];
    my_bind[i].is_null= &is_null[i];
    is_null[i]= 0;
  }

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_stmt_rc(rc, stmt);

  int_data= 320;
  small_data= 1867;
  big_data= 1000;
  real_data= 2;
  double_data= 6578.001;

  /* now, execute the prepared statement to insert 10 records.. */
  for (tiny_data= 0; tiny_data < 10; tiny_data++)
  {
    length[1]= my_sprintf(str_data, (str_data, "MySQL%d", int_data));
    rc= mysql_stmt_execute(stmt);
    check_stmt_rc(rc, stmt);
    int_data += 25;
    small_data += 10;
    big_data += 100;
    real_data += 1;
    double_data += 10.09;
  }

  /* force a re-prepare with some DDL */

  rc= mysql_query(mysql,
    "ALTER TABLE table_4166 change colX colX varchar(20) default NULL");
  check_mysql_rc(rc, mysql);

  /*
    execute the prepared statement again,
    without changing the types of parameters already bound.
  */

  for (tiny_data= 50; tiny_data < 60; tiny_data++)
  {
    length[1]= my_sprintf(str_data, (str_data, "MySQL%d", int_data));
    rc= mysql_stmt_execute(stmt);
    check_stmt_rc(rc, stmt);
    int_data += 25;
    small_data += 10;
    big_data += 100;
    real_data += 1;
    double_data += 10.09;
  }

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP TABLE table_4166");
  check_mysql_rc(rc, mysql);
  return OK;
}


static int test_wl4166_2(MYSQL *mysql)
{
  MYSQL_STMT *stmt;
  int        c_int;
  MYSQL_TIME d_date;
  MYSQL_BIND bind_out[2];
  int rc;

  if (mysql_get_server_version(mysql) < 50100) {
    diag("Test requires MySQL Server version 5.1 or above");
    return SKIP;
  }

  rc= mysql_query(mysql, "drop table if exists t1");
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql, "create table t1 (c_int int, d_date date)");
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql,
                  "insert into t1 (c_int, d_date) values (42, '1948-05-15')");
  check_mysql_rc(rc, mysql);

  stmt= mysql_stmt_init(mysql);
  FAIL_IF(!stmt, mysql_error(mysql));
  rc= mysql_stmt_prepare(stmt, "select * from t1", strlen("select * from t1"));
  check_stmt_rc(rc, stmt);

  memset(bind_out, '\0', sizeof(bind_out));
  bind_out[0].buffer_type= MYSQL_TYPE_LONG;
  bind_out[0].buffer= (void*) &c_int;

  bind_out[1].buffer_type= MYSQL_TYPE_DATE;
  bind_out[1].buffer= (void*) &d_date;

  rc= mysql_stmt_bind_result(stmt, bind_out);
  check_stmt_rc(rc, stmt);

  /* int -> varchar transition */

  rc= mysql_query(mysql,
                  "alter table t1 change column c_int c_int varchar(11)");
  check_mysql_rc(rc, mysql);

  rc= mysql_stmt_execute(stmt);
  check_stmt_rc(rc, stmt);

  rc= mysql_stmt_fetch(stmt);
  check_stmt_rc(rc, stmt);

  FAIL_UNLESS(c_int == 42, "c_int != 42");
  FAIL_UNLESS(d_date.year == 1948, "y!=1948");
  FAIL_UNLESS(d_date.month == 5, "m != 5");
  FAIL_UNLESS(d_date.day == 15, "d != 15");

  rc= mysql_stmt_fetch(stmt);
  FAIL_UNLESS(rc == MYSQL_NO_DATA, "rc != MYSQL_NO_DATA");

  /* varchar to int retrieval with truncation */

  rc= mysql_query(mysql, "update t1 set c_int='abcde'");
  check_mysql_rc(rc, mysql);

  rc= mysql_stmt_execute(stmt);
  check_stmt_rc(rc, stmt);

  rc= mysql_stmt_fetch(stmt);
  FAIL_IF(!rc, "Error expected");

  FAIL_UNLESS(c_int == 0, "c != 0");

  rc= mysql_stmt_fetch(stmt);
  FAIL_UNLESS(rc == MYSQL_NO_DATA, "rc != MYSQL_NO_DATA");

  /* alter table and increase the number of columns */
  rc= mysql_query(mysql, "alter table t1 add column d_int int");
  check_mysql_rc(rc, mysql);

  rc= mysql_stmt_execute(stmt);
  FAIL_IF(!rc, "Error expected");

  rc= mysql_stmt_reset(stmt);
  check_stmt_rc(rc, stmt);

  /* decrease the number of columns */
  rc= mysql_query(mysql, "alter table t1 drop d_date, drop d_int");
  check_mysql_rc(rc, mysql);

  rc= mysql_stmt_execute(stmt);
  FAIL_IF(!rc, "Error expected");

  mysql_stmt_close(stmt);
  rc= mysql_query(mysql, "drop table t1");
  check_mysql_rc(rc, mysql);

  return OK;
}


/**
  Test how warnings generated during assignment of parameters
  are (currently not) preserve in case of reprepare.
*/

static int test_wl4166_3(MYSQL *mysql)
{
  int rc;
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  MYSQL_TIME tm[1];

  if (mysql_get_server_version(mysql) < 50100) {
    diag("Test requires MySQL Server version 5.1 or above");
    return SKIP;
  }

  rc= mysql_query(mysql, "drop table if exists t1");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "create table t1 (year datetime)");
  check_mysql_rc(rc, mysql);

  stmt= mysql_stmt_init(mysql);
  FAIL_IF(!stmt, mysql_error(mysql));
  rc= mysql_stmt_prepare(stmt, "insert into t1 (year) values (?)", strlen("insert into t1 (year) values (?)"));
  check_stmt_rc(rc, stmt);

  FAIL_IF(mysql_stmt_param_count(stmt) != 1, "param_count != 1");

  memset(my_bind, '\0', sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_DATETIME;
  my_bind[0].buffer= &tm[0];

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_stmt_rc(rc, stmt);

  tm[0].year= 10000;
  tm[0].month= 1; tm[0].day= 1;
  tm[0].hour= 1; tm[0].minute= 1; tm[0].second= 1;
  tm[0].second_part= 0; tm[0].neg= 0;

  /* Cause a statement reprepare */
  rc= mysql_query(mysql, "alter table t1 add column c int");
  check_mysql_rc(rc, mysql);

  rc= mysql_stmt_execute(stmt);
  check_stmt_rc(rc, stmt);
  /*
    Sic: only one warning, instead of two. The warning
    about data truncation when assigning a parameter is lost.
    This is a bug.
  */
  FAIL_IF(mysql_warning_count(mysql) != 1, "warning count != 1");

  if (verify_col_data(mysql, "t1", "year", "0000-00-00 00:00:00")) {
    mysql_stmt_close(stmt);
    rc= mysql_query(mysql, "drop table t1");
    return FAIL;
  }

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "drop table t1");
  check_mysql_rc(rc, mysql);
  return OK;
}


/**
  Test that long data parameters, as well as parameters
  that were originally in a different character set, are
  preserved in case of reprepare.
*/

static int test_wl4166_4(MYSQL *mysql)
{
  MYSQL_STMT *stmt;
  int rc;
  const char *stmt_text;
  MYSQL_BIND bind_array[2];

  /* Represented as numbers to keep UTF8 tools from clobbering them. */
  const char *koi8= "\xee\xd5\x2c\x20\xda\xc1\x20\xd2\xd9\xc2\xc1\xcc\xcb\xd5";
  const char *cp1251= "\xcd\xf3\x2c\x20\xe7\xe0\x20\xf0\xfb\xe1\xe0\xeb\xea\xf3";
  char buf1[16], buf2[16];
  ulong buf1_len, buf2_len;

  if (mysql_get_server_version(mysql) < 50100) {
    diag("Test requires MySQL Server version 5.1 or above");
    return SKIP;
  }

  rc= mysql_query(mysql, "drop table if exists t1");
  check_mysql_rc(rc, mysql);

  /*
    Create table with binary columns, set session character set to cp1251,
    client character set to koi8, and make sure that there is conversion
    on insert and no conversion on select
  */
  rc= mysql_query(mysql,
                  "create table t1 (c1 varbinary(255), c2 varbinary(255))");
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql, "set character_set_client=koi8r, "
                         "character_set_connection=cp1251, "
                         "character_set_results=koi8r");
  check_mysql_rc(rc, mysql);

  memset(bind_array, '\0', sizeof(bind_array));

  bind_array[0].buffer_type= MYSQL_TYPE_STRING;

  bind_array[1].buffer_type= MYSQL_TYPE_STRING;
  bind_array[1].buffer= (void *) koi8;
  bind_array[1].buffer_length= strlen(koi8);

  stmt= mysql_stmt_init(mysql);
  check_stmt_rc(rc, stmt);

  stmt_text= "insert into t1 (c1, c2) values (?, ?)";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_stmt_rc(rc, stmt);

  mysql_stmt_bind_param(stmt, bind_array);

  mysql_stmt_send_long_data(stmt, 0, koi8, strlen(koi8));

  /* Cause a reprepare at statement execute */
  rc= mysql_query(mysql, "alter table t1 add column d int");
  check_mysql_rc(rc, mysql);

  rc= mysql_stmt_execute(stmt);
  check_stmt_rc(rc, stmt);

  stmt_text= "select c1, c2 from t1";

  /* c1 and c2 are binary so no conversion will be done on select */
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_stmt_rc(rc, stmt);

  rc= mysql_stmt_execute(stmt);
  check_stmt_rc(rc, stmt);

  bind_array[0].buffer= buf1;
  bind_array[0].buffer_length= sizeof(buf1);
  bind_array[0].length= &buf1_len;

  bind_array[1].buffer= buf2;
  bind_array[1].buffer_length= sizeof(buf2);
  bind_array[1].length= &buf2_len;

  mysql_stmt_bind_result(stmt, bind_array);

  rc= mysql_stmt_fetch(stmt);
  check_stmt_rc(rc, stmt);

  FAIL_UNLESS(buf1_len == strlen(cp1251), "");
  FAIL_UNLESS(buf2_len == strlen(cp1251), "");
  FAIL_UNLESS(!memcmp(buf1, cp1251, buf1_len), "");
  FAIL_UNLESS(!memcmp(buf2, cp1251, buf1_len), "");

  rc= mysql_stmt_fetch(stmt);
  FAIL_UNLESS(rc == MYSQL_NO_DATA, "");

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "drop table t1");
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql, "set names default");
  check_mysql_rc(rc, mysql);
  return OK;
}

/**
  Test that COM_REFRESH issues a implicit commit.
*/

static int test_wl4284_1(MYSQL *mysql)
{
  int rc;
  MYSQL_ROW row;
  MYSQL_RES *result;

  if (mysql_get_server_version(mysql) < 60000) {
    diag("Test requires MySQL Server version 6.0 or above");
    return SKIP;
  }

  /* set AUTOCOMMIT to OFF */
  rc= mysql_autocommit(mysql, FALSE);
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS trans");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "CREATE TABLE trans (a INT) ENGINE= InnoDB");
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "INSERT INTO trans VALUES(1)");
  check_mysql_rc(rc, mysql);

  rc= mysql_refresh(mysql, REFRESH_GRANT | REFRESH_TABLES);
  check_mysql_rc(rc, mysql);

  rc= mysql_rollback(mysql);
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "SELECT * FROM trans");
  check_mysql_rc(rc, mysql);

  result= mysql_use_result(mysql);
  FAIL_IF(!result, "Invalid result set");

  row= mysql_fetch_row(result);
  FAIL_IF(!row, "Can't fetch row");

  mysql_free_result(result);

  /* set AUTOCOMMIT to OFF */
  rc= mysql_autocommit(mysql, FALSE);
  check_mysql_rc(rc, mysql);

  rc= mysql_query(mysql, "DROP TABLE trans");
  check_mysql_rc(rc, mysql);

  return OK;
}


struct my_tests_st my_tests[] = {
  {"test_bug28075", test_bug28075, TEST_CONNECTION_DEFAULT, 0,  NULL, NULL},
  {"test_bug28505", test_bug28505, TEST_CONNECTION_DEFAULT, 0,  NULL, NULL},
  {"test_debug_example", test_debug_example, TEST_CONNECTION_DEFAULT, 0,  NULL, NULL},
  {"test_bug29692", test_bug29692, TEST_CONNECTION_NEW, CLIENT_FOUND_ROWS,  NULL, NULL},
  {"test_bug31418", test_bug31418, TEST_CONNECTION_DEFAULT, 0,  NULL, NULL},
  {"test_bug6081", test_bug6081, TEST_CONNECTION_DEFAULT, 0,  NULL, NULL},
  {"test_frm_bug", test_frm_bug, TEST_CONNECTION_NEW, 0,  NULL, NULL},
  {"test_wl4166_1", test_wl4166_1, TEST_CONNECTION_NEW, 0,  NULL, NULL},
  {"test_wl4166_2", test_wl4166_2, TEST_CONNECTION_NEW, 0,  NULL, NULL},
  {"test_wl4166_3", test_wl4166_3, TEST_CONNECTION_NEW, 0,  NULL, NULL},
  {"test_wl4166_4", test_wl4166_4, TEST_CONNECTION_NEW, 0,  NULL, NULL},
  {"test_wl4284_1", test_wl4284_1, TEST_CONNECTION_NEW, 0,  NULL, NULL},
  {NULL, NULL, 0, 0, NULL, 0}
};


int main(int argc, char **argv)
{
  if (argc > 1)
    get_options(&argc, &argv);

  get_envvars();

  run_tests(my_tests);

  return(exit_status());
}
