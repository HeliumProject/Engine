set(LIBNAME litesql)

		  
##############################################################################
# litesql-util library
##############################################################################
set(litesql_util_SOURCES split.cpp 
			string.cpp
		       ../../include/litesql/split.hpp 
			../../include/litesql/string.hpp 
			)	
add_library(litesql-util STATIC ${litesql_util_SOURCES})
install(TARGETS litesql-util EXPORT litesql-util
			RUNTIME DESTINATION lib 
			ARCHIVE DESTINATION lib/static 
			COMPONENT devel)
install(EXPORT litesql-util DESTINATION lib)

##############################################################################
# litesql library
##############################################################################
set(litesql_SOURCES	database.cpp 
			expr.cpp 
			selectquery.cpp 
			persistent.cpp 
			backend.cpp 
			updatequery.cpp 
			field.cpp 
			datasource.cpp 
			compatibility.hpp 
			compatibility.cpp 
			datetime.cpp
			../../include/litesql/database.hpp 
			../../include/litesql/expr.hpp 
			../../include/litesql/selectquery.hpp 
			../../include/litesql/persistent.hpp 
			../../include/litesql/backend.hpp 
			../../include/litesql/updatequery.hpp 
			../../include/litesql/field.hpp 
			../../include/litesql/datasource.hpp 
			../../include/litesql/datetime.hpp
)
add_library(  ${LIBNAME} STATIC ${litesql_SOURCES})
install(TARGETS ${LIBNAME} EXPORT ${LIBNAME}
			RUNTIME DESTINATION lib 
			ARCHIVE DESTINATION lib/static 
			COMPONENT devel)
install(EXPORT ${LIBNAME} DESTINATION lib)

if (MSVC)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/debug/litesqld.lib DESTINATION lib/static
	COMPONENT devel)
endif(MSVC)

if (WITH_SQLITE3)
  set(${LIBNAME}_sqlite_SRC
				sqlite3_backend.cpp 
				sqlite3.hpp )
if(NOT SQLITE3_FOUND)
  list(APPEND ${LIBNAME}_sqlite_SRC 
				sqlite3.c
				sqlite3.h
				sqlite3ext.h
				)
endif(NOT SQLITE3_FOUND)

add_library(${LIBNAME}_sqlite STATIC ${${LIBNAME}_sqlite_SRC}) 

install(TARGETS ${LIBNAME}_sqlite  EXPORT ${LIBNAME}_sqlite  
			RUNTIME DESTINATION lib 
			ARCHIVE DESTINATION lib/static 
			COMPONENT devel)
install(EXPORT ${LIBNAME}_sqlite DESTINATION lib)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/debug/${LIBNAME}_sqlited.lib DESTINATION lib/static COMPONENT devel)
endif(MSVC)
endif (WITH_SQLITE3)

if (WITH_MYSQL)
  add_library(  ${LIBNAME}_mysql STATIC
        				mysql.cpp
   
  )
  install(TARGETS ${LIBNAME}_mysql  EXPORT ${LIBNAME}_mysql 
			RUNTIME DESTINATION lib 
			ARCHIVE DESTINATION lib/static 
			COMPONENT devel)
  install(EXPORT ${LIBNAME}_mysql DESTINATION lib)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/debug/${LIBNAME}_mysqld.lib DESTINATION lib/static COMPONENT devel)
endif(MSVC)

endif (WITH_MYSQL)

if (WITH_POSTGRES)
  add_library(  ${LIBNAME}_postgresql STATIC
				postgresql.cpp 
  )
  install(TARGETS ${LIBNAME}_postgresql EXPORT ${LIBNAME}_postgresql  
			RUNTIME DESTINATION lib 
			ARCHIVE DESTINATION lib/static 
			COMPONENT devel)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/debug/${LIBNAME}_postgresqld.lib DESTINATION lib/static COMPONENT devel)
endif(MSVC)
endif (WITH_POSTGRES)

if (WITH_ODBC)
  add_library(  ${LIBNAME}_odbc STATIC
      				odbc_backend.cpp 
		      		odbc_backend.hpp 
  )
  install(TARGETS ${LIBNAME}_odbc EXPORT ${LIBNAME}_odbc 
			RUNTIME DESTINATION lib 
			ARCHIVE DESTINATION lib/static 
			COMPONENT devel)
if (MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/debug/${LIBNAME}_odbcd.lib DESTINATION lib/static COMPONENT devel)
endif(MSVC)

endif (WITH_ODBC)

include_directories(../../src/library ../../include)

