AM_CXXFLAGS=-I$(srcdir)/../../include

noinst_PROGRAMS=test-split \
		test-selectquery \
		test-datetime \
		test-update-table

test_split_SOURCES=test-split.cpp
test_split_LDADD=../library/liblitesql.la ../library/liblitesql-util.la

test_selectquery_SOURCES=test-selectquery.cpp
test_selectquery_LDADD=../library/liblitesql.la ../library/liblitesql-util.la

test_datetime_SOURCES=test-datetime.cpp
test_datetime_LDADD=../library/liblitesql.la ../library/liblitesql-util.la

test_update_table_SOURCES=test-update-table.cpp
test_update_table_LDADD=../library/liblitesql.la ../library/liblitesql-util.la

EXTRA_DIST=CMakeLists.txt
