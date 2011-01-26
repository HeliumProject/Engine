/* LiteSQL 
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

/** \file backend.hpp
    Classes Backend, Backend::Cursor and Backend::Result
*/
#ifndef litesql_backend_hpp
#define litesql_backend_hpp
#include <memory>
#include "litesql/types.hpp"
namespace litesql {


    /** An abstract base class for interfacing with relational databases */
    class Backend {
        public:
            /** An abstract base class for cursors that iterate result sets
              returned by relational database */
            class Cursor {
                public:
                    /** empty */
                    virtual ~Cursor() {}
                    /** if inherited Cursor can cache its data to speed up 
                      iteration, this method is used to set cache size. 
                      All backends do not react to this request.
                      */
                    virtual void setCacheSize(int s) {}
                    /** returns one result row. empty row means that result set is 
                     *  iterated through */
                    virtual Record fetchOne()=0;  
            };
            class Result {
                public:
                    /** empty */
                    virtual ~Result() {}
                    /** returns number of columns (fields) in result set */
                    virtual size_t fieldNum() const=0;
                    /** returns names of columns (fields) of the result set */
                    virtual Record fields() const=0;
                    /** returns number of rows (records) in result set */
                    virtual size_t recordNum() const=0;
                    /** returns result set */
                    virtual Records records() const=0;

            };
            /** empty */
            virtual ~Backend() {}
            /** return true if backend supports CREATE SEQUENCE - 
              SQL-statements */
            virtual bool supportsSequences() const {
                return false;
            }
            /** backend may want to set an AUTO_INCREMENT-attribute for table's primary 
              key field. this method is to deliver the details to database schema */
            virtual LITESQL_String getRowIDType() const {
                return LITESQL_L("INTEGER PRIMARY KEY");
            }
            /** if backend supports this, new primary key of the last insert 
              is returned */
            virtual LITESQL_String getInsertID() const { return LITESQL_L(""); }
            /** begin SQL transaction, may or may not have an effect */
            virtual void begin() const {}
            /** commit SQL transaction */
            virtual void commit() const {}
            /** rollback SQL transaction */
            virtual void rollback() const {}
            /** executes SQL-query 
              \param query SQL-query to execute 
              \return Result-object which holds result set of query */
            virtual Result* execute(const LITESQL_String& query) const = 0;
            /** executes SQL-query
              \param query SQL-query to execute 
              \return Cursor-object which can be used to iterate result set 
              row by row without loading everything to memory */
            virtual Cursor* cursor(const LITESQL_String& query) const = 0;
            /** executes multiple INSERT-statements and assigns same 'row id'
              for first field of every record
              \param tables destination tables for insert operation
              \param fields record of field names per table
              \param values record of values per table
              \param sequence sequence where row id-numbers are pulled
              \return new row id */
            virtual LITESQL_String groupInsert(Record tables, Records fields, Records values,
                    const LITESQL_String& sequence) const;
    /** returns a backend according to Backendtype in type, parameters are specific to backend and are separated by semicolon.
      \param type type of the database backend (supported are : LITESQL_L("mysql"),LITESQL_L("postgresql"),LITESQL_L("sqlite3"),LITESQL_L("odbc")
      \param connInfo database connection specific parameters (parameters are separated by semicolon)
       @throw DatabaseError if no backend is found
      */
    static Backend* getBackend(const LITESQL_String& type,const LITESQL_String& connInfo);
      
    };
}

#endif