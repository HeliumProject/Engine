#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Dependencies/DependenciesExceptions.h"
#include "Pipeline/Dependencies/GraphDB.h"
#include "Pipeline/Dependencies/Info/DependencyInfo.h"
#include "Pipeline/Dependencies/Info/FileInfo.h"

#include <vector>

#include "Platform/Types.h"
#include "Foundation/Container/OrderedSet.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Platform/Mutex.h"

// Forwards
namespace CryptoPP{ class HashFilter; }

// Dependencies API
namespace Dependencies
{ 
    typedef Nocturnal::Signature< const std::string&, Nocturnal::Void > GetFormatVersionSignature;
    typedef std::map< std::string, GetFormatVersionSignature::Delegate > M_TypeNameToFormatVersionCallback;

    // Hidden Ctor/Dtor - this is a singleton class
    class PIPELINE_API DependencyGraph
    {
    private:
        DependencyGraph( const std::string& graphDBFilename, const std::string& configFolder );

    public:
        ~DependencyGraph();
        void ClearCache();

        void  RegisterInput( const DependencyInfoPtr& output, const DependencyInfoPtr& input, bool inFileIsOptional = false );

        void  RegisterInputs( const V_DependencyInfo &outputs, const V_DependencyInfo &inputs, bool inFilesAreOptional = false  );

        void  UpdateOutputs( V_DependencyInfo &outputs );

        bool  AreUpToDate( const V_string &filePaths );
        bool  AreUpToDate( const V_DependencyInfo &files );
        bool  IsUpToDate( const std::string& filePath );

        void  CreateSignature( const DependencyInfoPtr& file ); 
        void  CreateSignatures( V_DependencyInfo &listOfFiles, bool trapExceptions = false );

        // this is called by UpdateOutputs, and should not need to be called from the builders
        // public so that CacheFiles can use it if it needs to
        void  GetFileMD5( const DependencyInfoPtr& file );

        bool RegisterType( const std::string& typeName, const GetFormatVersionSignature::Delegate& callback );
        bool UnregisterType( const std::string& typeName );
        std::string GetFormatVersion( const std::string& typeName ) const;

    private:

        //
        // Members
        //

        // define the types that will be used internally to represent the graph in memory
        struct CompareFilePath
        {
            bool operator()( const std::string& rhs, const std::string& lhs ) const
            {
                return ( rhs.compare( lhs ) < 0 ) ;
            }
        };
        typedef std::map< const std::string, DependencyInfoPtr, CompareFilePath >  FileGraphCache;
        typedef std::map< std::string, DependencyInfoPtr >                        FileGraph;
        typedef std::set< DependencyInfoPtr >                                     DirtyFiles;
        typedef std::map< std::string, OS_string >                        ReregisterMap;

        Platform::Mutex       m_HighLevelMutex;

        GraphDBPtr            m_GraphDB;
        ReregisterMap         m_ReregisterMap;        // files that have been registered during this run

        FileGraphCache        m_CachedFileGraph;      // Memory Cache
        DirtyFiles            m_CachedDirtyFiles;     // set of the dirty files that will need to be updated in the db later

        M_TypeNameToFormatVersionCallback m_FormatVersionCallbacks;

        //
        // Memory cache
        //

        DependencyInfoPtr CreateCacheCopy( const DependencyInfoPtr& dependency );
        DependencyInfoPtr CacheDependency( const DependencyInfoPtr& file, bool makeDirty = true );
        void        CacheDirtyDependency( DependencyInfoPtr file );
        DependencyInfoPtr CacheRegisterDependency( const DependencyInfoPtr& file );
        DependencyInfoPtr CacheGetDependency( const std::string& filePath, bool useDB = true );
        DependencyInfoPtr CacheGetDependency( const DependencyInfoPtr& info, bool useDB = true );
        void        CacheCommitDependency( DependencyInfoPtr file, bool fileExists = true );

        void        CacheGraph( DependencyInfoPtr outFile, DependencyInfoPtr inFile, bool inFileIsOptional = false );
        void        CacheGetGraph( DependencyInfoPtr file, FileGraph& fileGraph, bool throwIfMissingInput = false, bool recurse = true, int curDepth = 0 );
        void        CacheGetInFileGraph( DependencyInfoPtr outFile, DependencyInfoPtr inFile, FileGraph& fileGraph, bool throwIfMissingInput, bool recurse, int curDepth );
        inline bool WasDependencyRemoved( const std::string& outFilePath, const std::string& inFilePath );
        void        CacheCommitGraph( const DependencyInfoPtr file, bool useTransaction = true );

        void        AppendFileToSignature( const DependencyInfoPtr& file, CryptoPP::HashFilter* hashFilter, V_string& trace );
        void        GetOrderDependentFiles( const DependencyInfoPtr& outFile, FileGraph& fileGraph, V_string& orderDependentFiles, int curDepth = 0 );
    };
}
