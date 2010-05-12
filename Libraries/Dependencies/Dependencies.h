#pragma once

#include "API.h"
#include "Exceptions.h"
#include "DependencyInfo.h"
#include "FileInfo.h"

#include <vector>

#include "Common/Types.h"
#include "Common/Container/OrderedSet.h"
#include "Common/Memory/SmartPtr.h"

#include "TUID/TUID.h"

// Forwards
namespace CryptoPP{ class HashFilter; }

// Dependencies API
namespace Dependencies
{ 
  void DEPENDENCIES_API Initialize();
  void DEPENDENCIES_API Cleanup();

  // static global Dependency Graph
  DEPENDENCIES_API class DependencyGraph &Graph();
  
  class GraphDB;
  typedef Nocturnal::SmartPtr< GraphDB > GraphDBPtr;

  // Hidden Ctor/Dtor - this is a singleton class
  class DEPENDENCIES_API DependencyGraph
  {
  private:
    DependencyGraph();

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

    // provide access to the ctor for the static global Dependency Graph
    friend void ::Dependencies::Initialize();
    friend DependencyGraph& ::Dependencies::Graph();    

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
   

    GraphDBPtr            m_GraphDB;
    ReregisterMap         m_ReregisterMap;        // files that have been registered during this run

    FileGraphCache        m_CachedFileGraph;      // Memory Cache
    DirtyFiles            m_CachedDirtyFiles;     // set of the dirty files that will need to be updated in the db later

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
