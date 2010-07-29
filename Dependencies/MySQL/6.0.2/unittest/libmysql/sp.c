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

/* Bug#15752 "Lost connection to MySQL server when calling a SP from C API" */

static int test_bug15752(MYSQL *mysql)
{
  int rc, i;
  const int ITERATION_COUNT= 100;
  const char *query= "CALL p1()";


  rc= mysql_query(mysql, "drop procedure if exists p1");
  check_mysql_rc(rc, mysql);
  rc= mysql_query(mysql, "create procedure p1() select 1");
  check_mysql_rc(rc, mysql);

  rc= mysql_real_query(mysql, query, strlen(query));
  check_mysql_rc(rc, mysql);
  mysql_free_result(mysql_store_result(mysql));

  rc= mysql_real_query(mysql, query, strlen(query));
  FAIL_UNLESS(rc && mysql_errno(mysql) == CR_COMMANDS_OUT_OF_SYNC, "Error expected");

  rc= mysql_next_result(mysql);
  check_mysql_rc(rc, mysql);

  mysql_free_result(mysql_store_result(mysql));

  rc= mysql_next_result(mysql);
  FAIL_IF(rc != -1, "rc != -1");

  for (i = 0; i < ITERATION_COUNT; i++)
  {
    rc= mysql_real_query(mysql, query, strlen(query));
    check_mysql_rc(rc, mysql);
    mysql_free_result(mysql_store_result(mysql));
    rc= mysql_next_result(mysql);
    check_mysql_rc(rc, mysql);
    mysql_free_result(mysql_store_result(mysql));
    rc= mysql_next_result(mysql);
    FAIL_IF(rc != -1, "rc != -1");

  }
  rc= mysql_query(mysql, "drop procedure p1");
  check_mysql_rc(rc, mysql);

  return OK;
}




struct my_tests_st my_tests[] = {
  {"test_bug15752", test_bug15752, TEST_CONNECTION_NEW, 0, NULL , NULL},
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
