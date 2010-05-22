#include "StdAfx.h"

#include "Resolver.h"

#include "Windows/Thread.h"

namespace File
{
    ///////////////////////////////////////////////
    // File Table

    static const char* s_InsertFileSQL = "INSERT INTO file( created, modified, id, relative, path, user ) VALUES ( ?, ?, ?, ?, ?, ? );";
    static const char* s_UpdateFileSQL = "UPDATE file SET modified=?, relative=?, path=?, user=? WHERE id=?;";
    static const char* s_DeleteFileSQL = "DELETE FROM file WHERE id=?;";
    static const char* s_SelectFileByIdSQL = "SELECT * FROM file WHERE id=? ORDER BY created ASC LIMIT 1;";
    static const char* s_SelectFileByPathSQL = "SELECT * FROM file WHERE path LIKE ? ESCAPE '@' ORDER BY created ASC LIMIT 1;";

    /////////////////////////////////////////////////////////////////////////////
    // Ctor - Initialiaze the CacheDb
    Resolver::Resolver( const std::string& databaseFile, const std::string& configPath )
        : SQL::SQLiteDB( "Resolver" )
    {
        Nocturnal::Path databasePath( databaseFile );
        Nocturnal::Path configPathObject( configPath );

        Open( databasePath.Get(), configPathObject.Get(), GetVersion() );
    }


    /////////////////////////////////////////////////////////////////////////////
    // Dtor - Closes the CacheDB
    Resolver::~Resolver()
    {
    }


    /////////////////////////////////////////////////////////////////////////////
    // Prepares all of the statements.
    void Resolver::PrepareStatements()
    {
        FILE_SCOPE_TIMER((""));

        Windows::TakeSection critSection( *m_GeneralCriticalSection );

        m_SqlInsertStatementHandle          = m_DBManager->CreateStatement( s_InsertFileSQL, "lllltt" );
        m_SqlUpdateStatementHandle          = m_DBManager->CreateStatement( s_UpdateFileSQL, "llttl" );
        m_SqlSelectIdStatementHandle        = m_DBManager->CreateStatement( s_SelectFileByIdSQL, "l" );
        m_SqlSelectPathStatementHandle      = m_DBManager->CreateStatement( s_SelectFileByPathSQL, "t" );
        m_SqlDeleteStatementHandle          = m_DBManager->CreateStatement( s_DeleteFileSQL, "l" );
    }

    //*************************************************************************//
    //
    //  API FUNCTIONS
    //
    //*************************************************************************//


    /////////////////////////////////////////////////////////////////////////////
    // Insert a single file table row into the Cache DB, returns the tuid of the 
    // entry
    //
    void Resolver::Insert( const File::Reference& fileRef )
    {
        FILE_SCOPE_TIMER((""));

        Windows::TakeSection critSection( *m_GeneralCriticalSection );

        //////////////////////////
        // try to find the existing file by id first
        File::Reference existing( fileRef );

        if ( Find( existing ) )
        {
            if ( fileRef.GetPath() == existing.GetPath() )
            {
                return;
            }
            else
            {
                // There is no use case for inserting an existing ID with a different path,
                // it would mean that there was an ID collision
                throw DuplicateEntryException( fileRef.GetId(), fileRef.GetPath().c_str() );
            }
        }

        {
            FILE_SCOPE_TIMER(("Adding file to DB"));

            bool relative = !fileRef.GetRelativePath().empty();
            std::string path = relative ? fileRef.GetRelativePath() : fileRef.GetPath();

            //////////////////////////
            // otherwise, add the new file to the DB
            if ( m_DBManager->ExecStatement( m_SqlInsertStatementHandle, ( i64 ) fileRef.m_CreationTime, ( i64 ) fileRef.m_ModifiedTime, ( i64 ) fileRef.GetId(), (i64) relative, path.c_str(), fileRef.m_LastUsername.c_str() ) != SQLITE_OK )
            {
                throw Exception( m_DBManager->GetLastErrMsg().c_str() );
            }
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // Replace a single file table row into the Cache DB
    void Resolver::Update( const File::Reference& fileRef )
    {
        FILE_SCOPE_TIMER((""));

        Windows::TakeSection critSection( *m_GeneralCriticalSection );

        bool relative = !fileRef.GetRelativePath().empty();
        std::string oldPath = fileRef.GetPath();
        
        File::Reference existing( fileRef );

        do
        {
            if ( m_DBManager->ExecSQLVMPrintF(
                s_UpdateFileSQL,
                ( i64 ) fileRef.m_ModifiedTime,
                ( i64 ) relative,
                fileRef.GetPath().c_str(),
                fileRef.m_LastUsername,
                ( i64 ) existing.GetId() ) != SQLITE_OK )
            {
                throw Exception( m_DBManager->GetLastErrMsg().c_str() );
            }

            existing.m_Id = TUID::Null;
            existing.Set( oldPath );
        } while( Find( existing ) );
    }


    /////////////////////////////////////////////////////////////////////////////
    // Actually deletes a single file from the Cache DB
    void Resolver::Delete( const File::Reference& file )
    {
        FILE_SCOPE_TIMER((""));

        Windows::TakeSection critSection( *m_GeneralCriticalSection );

        File::Reference existing( file );

        if ( !Find( existing ) )
        {
            return;
        }

        if ( m_DBManager->ExecStatement( m_SqlDeleteStatementHandle, ( i64 ) file.GetId() ) != SQLITE_DONE )
        {
            throw Exception( m_DBManager->GetLastErrMsg().c_str() );
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Populate the ReferencePtr with entry data from the Cache DB,
    // returns true if a file was found
    //
    bool Resolver::Find( File::Reference& file )
    {
        FILE_SCOPE_TIMER((""));

        Windows::TakeSection critSection( *m_GeneralCriticalSection );

        SQL::StmtHandle stmt;
        int sqlResult = SQLITE_DONE;

        bool relative = !file.GetRelativePath().empty();

        if ( file.GetId() != TUID::Null )
        {
            sqlResult = m_DBManager->ExecStatement( m_SqlSelectIdStatementHandle, ( i64 ) file.GetId() );
            stmt = m_SqlSelectIdStatementHandle;
        }
        else if ( !file.GetRelativePath().empty() )
        {
            sqlResult = m_DBManager->ExecStatement( m_SqlSelectPathStatementHandle, file.GetRelativePath().c_str() );
            stmt = m_SqlSelectPathStatementHandle;
        }
        else if( !file.GetPath().empty() )
        {
            sqlResult = m_DBManager->ExecStatement( m_SqlSelectPathStatementHandle, file.GetPath().c_str() );
            stmt = m_SqlSelectPathStatementHandle;
        }
        else
        {
            throw Exception( "Neither path nor id set in File::Reference passed to Resolver::Find." );
        }

        if ( sqlResult == SQLITE_DONE )
        {
            return false;
        }
        else if ( sqlResult != SQLITE_ROW )
        {
            throw Exception( m_DBManager->GetLastErrMsg().c_str() );
        }

        m_DBManager->GetColumnI64(  stmt,  0, ( i64& ) file.m_CreationTime );
        m_DBManager->GetColumnI64(  stmt,  1, ( i64& ) file.m_ModifiedTime );
        m_DBManager->GetColumnI64(  stmt,  2, ( i64& ) file.m_Id );

        std::string path;
        m_DBManager->GetColumnText( stmt,  3,          path );

        if ( relative )
        {
            file.Set( file.s_GlobalBaseDirectory + file.m_RelativePath );
        }
        else
        {
            file.Set( path );
        }

        m_DBManager->GetColumnText( stmt,  4,          file.m_LastUsername );

        m_DBManager->ResetStatement( stmt );
        return true;
    }

    bool Resolver::Find( const std::string& query, File::S_Reference& files )
    {
        FILE_SCOPE_TIMER((""));

        Windows::TakeSection critSection( *m_GeneralCriticalSection );

        SQL::StmtHandle stmt = m_SqlSelectPathStatementHandle;
        int sqlResult = m_DBManager->ExecStatement( stmt, query.c_str() );;

        if ( sqlResult == SQLITE_DONE )
        {
            return false;
        }
        else if ( sqlResult != SQLITE_ROW )
        {
            throw Exception( m_DBManager->GetLastErrMsg().c_str() );
        }

        do
        {
            File::ReferencePtr file = new File::Reference();

            m_DBManager->GetColumnI64(  stmt,  0, ( i64& ) file->m_CreationTime );
            m_DBManager->GetColumnI64(  stmt,  1, ( i64& ) file->m_ModifiedTime );
            m_DBManager->GetColumnI64(  stmt,  2, ( i64& ) file->m_Id );

            bool relative = false;
            m_DBManager->GetColumnI64(  stmt,  3, ( i64& ) relative ); 

            std::string path;
            m_DBManager->GetColumnText( stmt,  4,          path );
            
            if ( relative )
            {
                file->Set( file->s_GlobalBaseDirectory + path );
            }
            else
            {
                file->Set( path );
            }

            m_DBManager->GetColumnText( stmt,  5,          file->m_LastUsername );

            files.insert( file );

        } while( m_DBManager->StepStatement( stmt ) == SQLITE_ROW );

        return true;

    }

    bool Resolver::Find( const std::string& query, File::V_Reference& files )
    {
        File::S_Reference filesSet;

        bool result = Find( query, filesSet );

        if ( result )
        {
            File::S_Reference::iterator itr = filesSet.begin();
            File::S_Reference::iterator end = filesSet.end();
            for( ; itr != end; ++itr )
            {
                files.push_back( *itr );
            }
        }

        return result;
    }
} // namespace File