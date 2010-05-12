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

#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>
#include <tap.h>
#include <my_getopt.h>
#include <memory.h>
#include <errmsg.h>

#ifndef OK
# define OK 0
#endif
#ifndef FAIL
# define FAIL 1
#endif
#ifndef SKIP
# define SKIP -1
#endif
#ifndef FALSE
# define FALSE 0
#endif
#ifndef TRUE
# define TRUE 1
#endif

#define MAX_KEY MAX_INDEXES
#define MAX_KEY_LENGTH_DECIMAL_WIDTH 4          /* strlen("4096") */

#define MAX_TEST_QUERY_LENGTH 300 /* MAX QUERY BUFFER LENGTH */

#define check_mysql_rc(rc, mysql) \
if (rc)\
{\
  diag("Error (%d): %s (%d) in %s line %d", rc, mysql_error(mysql), \
       mysql_errno(mysql), __FILE__, __LINE__);\
  return(FAIL);\
}

#define check_stmt_rc(rc, stmt) \
if (rc)\
{\
  diag("Error: %s (%s: %d)", mysql_stmt_error(stmt), __FILE__, __LINE__);\
  return(FAIL);\
}

#define FAIL_IF(expr, reason)\
if (expr)\
{\
  diag("Error: %s (%s: %d)", (reason) ? reason : "", __FILE__, __LINE__);\
  return FAIL;\
}

#define FAIL_UNLESS(expr, reason)\
if (!(expr))\
{\
  diag("Error: %s (%s: %d)", reason, __FILE__, __LINE__);\
  return FAIL;\
}

/* connection options */
#define TEST_CONNECTION_DEFAULT    1 /* default connection */
#define TEST_CONNECTION_NONE       2 /* tests creates own connection */
#define TEST_CONNECTION_NEW        4 /* create a separate connection */
#define TEST_CONNECTION_DONT_CLOSE 8 /* don't close connection */

struct my_option_st
{
  enum mysql_option option;
  char              *value;
};

struct my_tests_st
{
  const char *name;
  int  (*function)(MYSQL *);
  int   connection;
  ulong connect_flags;
  struct my_option_st *options;
  char *skipmsg;
};

static char *schema = "test";
static char *hostname = 0;
static char *password = 0;
static unsigned int port = 0;
static char *socketname = 0;
static char *username = 0;

static struct my_option test_options[] =
{
  {"schema", 'd', "database to use", (uchar **) &schema, (uchar **) &schema,
   0, GET_STR, REQUIRED_ARG, 0, 0, 0, 0, 0, 0},
  {"help", '?', "Display this help and exit", 0, 0, 0, GET_NO_ARG, NO_ARG, 0,
   0, 0, 0, 0, 0},
  {"host", 'h', "Connect to host", (uchar **) &hostname, (uchar **) &hostname,
   0, GET_STR, REQUIRED_ARG, 0, 0, 0, 0, 0, 0},
  {"password", 'p',
   "Password to use when connecting to server.", (uchar **) &password, (uchar **) &password,
   0, GET_STR, OPT_ARG, 0, 0, 0, 0, 0, 0},
  {"port", 'P', "Port number to use for connection or 0 for default to, in "
   "order of preference, my.cnf, $MYSQL_TCP_PORT, "
#if MYSQL_PORT_DEFAULT == 0
   "/etc/services, "
#endif
   "built-in default (" STRINGIFY_ARG(MYSQL_PORT) ").",
   (uchar **) &port,
   (uchar **) &port, 0, GET_UINT, REQUIRED_ARG, 0, 0, 0, 0, 0, 0},
  {"socket", 'S', "Socket file to use for connection",
   (uchar **) &socketname, (uchar **) &socketname, 0, GET_STR,
   REQUIRED_ARG, 0, 0, 0, 0, 0, 0},
  {"user", 'u', "User for login if not current user", (uchar **) &username,
   (uchar **) &username, 0, GET_STR, REQUIRED_ARG, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, GET_NO_ARG, NO_ARG, 0, 0, 0, 0, 0, 0}
};

#define verify_prepare_field(result,no,name,org_name,type,table,\
                             org_table,db,length,def) \
          do_verify_prepare_field((result),(no),(name),(org_name),(type), \
                                  (table),(org_table),(db),(length),(def), \
                                  __FILE__, __LINE__)

int do_verify_prepare_field(MYSQL_RES *result,
                            unsigned int no, const char *name,
                            const char *org_name,
                            enum enum_field_types type,
                            const char *table,
                            const char *org_table, const char *db,
                            unsigned long length, const char *def,
                            const char *file, int line)
{
  MYSQL_FIELD *field;
  CHARSET_INFO *cs;

  FAIL_IF(!(field= mysql_fetch_field_direct(result, no)), "FAILED to get result")
  cs= get_charset(field->charsetnr, 0);
  FAIL_UNLESS(cs, "Couldn't get character set");
  FAIL_UNLESS(strcmp(field->name, name) == 0, "field->name differs");
  FAIL_UNLESS(strcmp(field->org_name, org_name) == 0, "field->org_name differs");
  /*
    XXX: silent column specification change works based on number of
    bytes a column occupies. So CHAR -> VARCHAR upgrade is possible even
    for CHAR(2) column if its character set is multibyte.
    VARCHAR -> CHAR downgrade won't work for VARCHAR(3) as one would
    expect.
  */
  if (cs->mbmaxlen == 1)
    FAIL_UNLESS(field->type == type, "field->type differs");
  if (table)
    FAIL_UNLESS(strcmp(field->table, table) == 0, "field->table differs");
  if (org_table)
    FAIL_UNLESS(strcmp(field->org_table, org_table) == 0, "field->org_table differs");
  FAIL_UNLESS(strcmp(field->db, db) == 0, "field->db differs");
  /*
    Character set should be taken into account for multibyte encodings, such
    as utf8. Field length is calculated as number of characters * maximum
    number of bytes a character can occupy.
  */
  if (type != MYSQL_TYPE_BLOB)
    FAIL_UNLESS(field->length == length * cs->mbmaxlen, "field->length differs");

  return OK;
}

/* Prepare statement, execute, and process result set for given query */

int my_stmt_result(MYSQL *mysql, const char *buff)
{
  MYSQL_STMT *stmt;
  int        row_count= 0;
  int        rc;

  stmt= mysql_stmt_init(mysql);

  rc= mysql_stmt_prepare(stmt, buff, strlen(buff));
  FAIL_IF(rc, mysql_stmt_error(stmt));

  rc= mysql_stmt_execute(stmt);
  FAIL_IF(rc, mysql_stmt_error(stmt));

  while (mysql_stmt_fetch(stmt) != MYSQL_NO_DATA)
    row_count++;

  mysql_stmt_close(stmt);

  return row_count;
}

static my_bool
get_one_option(int optid, const struct my_option *opt __attribute__((unused)),
               char *argument)
{
  switch (optid) {
  case '?':
  case 'I':                                     /* Info */
    my_print_help(test_options);
    exit(0);
    break;
  }
  return 0;
}

/* Utility function to verify a particular column data */

int verify_col_data(MYSQL *mysql, const char *table, const char *col,
                            const char *exp_data)
{
  static char query[MAX_TEST_QUERY_LENGTH];
  MYSQL_RES *result;
  MYSQL_ROW row;
  int       rc;

  if (table && col)
  {
    sprintf(query, "SELECT %s FROM %s LIMIT 1", col, table);
    rc= mysql_query(mysql, query);
    check_mysql_rc(rc, mysql);
  }

  result= mysql_use_result(mysql);
  FAIL_IF(!result, "Invalid result set");

  if (!(row= mysql_fetch_row(result)) || !row[0]) {
    diag("Failed to get the result");
    goto error;
  }
  if(strcmp(row[0], exp_data)) {
    diag("Expected %s, got %s", exp_data, row[0]);
    goto error;
  }
  mysql_free_result(result);

  return OK;

error:
  mysql_free_result(result);
  return FAIL;
}

my_bool query_int_variable(MYSQL *con, const char *var_name, int *var_value)
{
  MYSQL_RES *rs;
  MYSQL_ROW row;

  char query_buffer[MAX_TEST_QUERY_LENGTH];

  my_bool is_null;

  sprintf(query_buffer,
          "SELECT %s",
          (const char *) var_name);

  FAIL_IF(mysql_query(con, query_buffer), "Query failed");
  FAIL_UNLESS(rs= mysql_store_result(con), "Invaliid result set");
  FAIL_UNLESS(row= mysql_fetch_row(rs), "Nothing to fetch");

  is_null= row[0] == NULL;

  if (!is_null)
    *var_value= atoi(row[0]);

  mysql_free_result(rs);

  return is_null;
}



static void get_options(int *argc, char ***argv)
{
  int ho_error;

  if ((ho_error= handle_options(argc, argv, test_options,
                                get_one_option)))
    exit(ho_error);
}

int check_variable(MYSQL *mysql, char *variable, char *value)
{
  char query[MAX_TEST_QUERY_LENGTH];
  MYSQL_RES *result;
  MYSQL_ROW row;

  sprintf(query, "SELECT %s", variable);
  result= mysql_store_result(mysql);
  if (!result)
    return FAIL;

  if ((row = mysql_fetch_row(result)))
    if (strcmp(row[0], value) == 0) {
      mysql_free_result(result);
      return OK;
    }
  mysql_free_result(result);
  return FAIL;
}

/* 
 * function *test_connect
 *
 * returns a new connection. This function will be called, if the test doesn't
 * use default_connection.
 */
MYSQL *test_connect(struct my_tests_st *test) {
  MYSQL *mysql;
  char query[255];

  if (!(mysql = mysql_init(NULL))) {
    diag("%s", "mysql_init failed - exiting");
    return(NULL);
  }

  /* option handling */
  if (test && test->options) {
    int i=0;

    while (test->options[i].option)
    {
      if (mysql_options(mysql, test->options[i].option, test->options[i].value)) {
        diag("Couldn't set option %d. Error (%d) %s", test->options[i].option,
                      mysql_errno(mysql), mysql_error(mysql));
        mysql_close(mysql);
        return(NULL);
      }
      i++;
    }
  }

  if (!(mysql_real_connect(mysql, hostname, username, password,
                           NULL, port, socketname, (test) ? test->connect_flags:0)))
  {
    diag("Couldn't establish connection to server %s. Error (%d): %s", 
                   hostname, mysql_errno(mysql), mysql_error(mysql));
    mysql_close(mysql);
    return(NULL);
  }

  /* change database or create if it doesn't exist */
  if (mysql_select_db(mysql, schema)) {
    if(mysql_errno(mysql) == 1049) {
      sprintf(query, "CREATE DATABASE %s", schema);
      if (mysql_query(mysql, query)) {
        diag("Can't create database %s", schema);
        mysql_close(mysql);
        return NULL;
      }
    } else {
      diag("Error (%d): %s", mysql_errno(mysql), mysql_error(mysql));
      mysql_close(mysql);
      return NULL;
    }
  }

  return(mysql);
}

static int reset_connection(MYSQL *mysql) {
  int rc;

  rc= mysql_change_user(mysql, username, password, schema);
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql, "SET table_type='MyISAM'");
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql, "SET sql_mode=''");
  check_mysql_rc(rc, mysql);

  return OK;
}

/*
 * function get_envvars((
 *
 * checks for connection related environment variables
 */
void get_envvars() {
  char  *envvar;

  if (!hostname && (envvar= getenv("MYSQL_TEST_HOST")))
    hostname= envvar;
  if (!username && (envvar= getenv("MYSQL_TEST_USER")))
    username= envvar;
  if (!password && (envvar= getenv("MYSQL_TEST_PASSWD")))
    password= envvar;
  if (!schema && (envvar= getenv("MYSQL_TEST_DB")))
    schema= envvar;
  if (!port && (envvar= getenv("MYSQL_TEST_PORT")))
    port= atoi(envvar);
  if (!socketname && (envvar= getenv("MYSQL_TEST_SOCKET")))
    socketname= envvar;
}

void run_tests(struct my_tests_st *test) {
  int i, rc, total=0;
  MYSQL *mysql, *mysql_default= NULL;  /* default connection */


  while (test[total].function)
    total++;
  plan(total);

  mysql_default= test_connect(NULL);
  diag("Testing against MySQL Server %s", mysql_get_server_info(mysql_default));

  for (i=0; i < total; i++) {
    if (!test[i].skipmsg) {

      mysql= mysql_default;
      if (test[i].connection & TEST_CONNECTION_NEW)
        mysql= test_connect(&test[i]);
      if (test[i].connection & TEST_CONNECTION_NONE)
        mysql= NULL;

      /* run test */
      rc= test[i].function(mysql);

      if (rc == SKIP)
        skip(1, test[i].name);
      else
        ok(rc == OK, test[i].name);

      /* if test failed, close and reopen default connection to prevent
         errors for further tests */
      if ((rc == FAIL || mysql_errno(mysql_default)) && (test[i].connection & TEST_CONNECTION_DEFAULT)) {
        mysql_close(mysql_default);
        mysql_default= test_connect(&test[i]);
      }
      /* clear connection: reset default connection or close extra connection */
      else if (test[i].connection & TEST_CONNECTION_DEFAULT)  {
          if (reset_connection(mysql))
            return; /* default doesn't work anymore */
      }
      else if (mysql && !(test[i].connection & TEST_CONNECTION_DONT_CLOSE))
          mysql_close(mysql);
    } else {
      skip(1, test[i].skipmsg);
    }
  }
}

