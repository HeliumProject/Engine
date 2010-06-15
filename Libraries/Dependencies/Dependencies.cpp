#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "Foundation/Profile.h"

#include "Dependencies.h"
#include "DataInfo.h"
#include "Exceptions.h"
#include "GraphDB.h"

#include <cstdarg> 
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include <time.h>

#pragma warning ( push )
#pragma warning ( disable : 4267 ) // type conversion - possible loss of data warning
#include <cryptlib.h>
#include <md5.h>
#include <filters.h>
#include <files.h>
#include <hex.h>        // HexEncoder
#include <files.h>
#pragma warning ( pop )

#include "Foundation/Flags.h"
#include "Platform/Types.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/String/Utilities.h"

#include "Foundation/Log.h"

#define MAX_FILE_GRAPH_DEPTH 25

namespace Dependencies
{

  // we have our own FileExists function because we don't need to call FileSystem::StandardName
  // every time we do a FileExists... so this doesn't. 
  // 
  bool FileExists(const char* filename)
  {
    return (GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES); 
  }

  /////////////////////////////////////////////////////////////////////////////
  // Initialize constructor
  DependencyGraph::DependencyGraph( const std::string& graphDBFilename, const std::string& configFolder )
  {
    m_GraphDB = new GraphDB( "Global-DependencyGraphDB" );
    m_GraphDB->Open( graphDBFilename, configFolder, m_GraphDB->s_GraphDBVersion );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Default destructor
  DependencyGraph::~DependencyGraph( )
  {
    ClearCache();
    m_GraphDB = NULL;
  }


  /////////////////////////////////////////////////////////////////////////////
  // RegisterInput is used to add an output file to the graph after the built output
  // file is writen to disc
  void DependencyGraph::RegisterInput( const DependencyInfoPtr& output, const DependencyInfoPtr& input, bool inFileIsOptional )
  {
    DEPENDENCIES_SCOPE_TIMER((""));

    Platform::TakeMutex mutex( m_HighLevelMutex );

    if ( output->m_Path.empty() || input->m_Path.empty() )
    {
      throw Exception( "Cannot register dependancy with an empty file name; inFilePath: %s, outFilePath: %s", input->m_Path.c_str(), output->m_Path.c_str() );
    }

    Log::Bullet bullet( Log::Streams::Debug, Log::Levels::Verbose, "RegisterInput: %sInput %s -> Output %s\n", input->m_GraphConfigs & ConfigFlags::LeafInput ? "Leaf" : "Intermedate", input->m_Path.c_str(), output->m_Path.c_str() );

    DependencyInfoPtr outFile = CacheRegisterDependency( output );
    DependencyInfoPtr inFile  = CacheRegisterDependency( input );

    CacheGraph( outFile, inFile, inFileIsOptional );
  }


  /////////////////////////////////////
  // Register a bunch of inputs
  //
  void DependencyGraph::RegisterInputs( const V_DependencyInfo &outputs, const V_DependencyInfo &inputs, bool inFilesAreOptional )
  {
    Platform::TakeMutex mutex( m_HighLevelMutex );

    V_DependencyInfo::const_iterator outputItr = outputs.begin();
    V_DependencyInfo::const_iterator outputEnd = outputs.end();

    V_DependencyInfo::const_iterator inputItr;
    V_DependencyInfo::const_iterator inputEnd = inputs.end();
    for ( ; outputItr != outputEnd; ++outputItr )
    {
      inputItr = inputs.begin();
      for ( ; inputItr != inputEnd; ++inputItr )
      {
        RegisterInput( *outputItr, *inputItr, inFilesAreOptional );
      }
    }
  }


  /////////////////////////////////////
  // Update a bunch of outputs
  void DependencyGraph::UpdateOutputs( V_DependencyInfo &outputs )
  {
    Platform::TakeMutex mutex( m_HighLevelMutex );

    DEPENDENCIES_SCOPE_TIMER((""));

    m_GraphDB->BeginTrans();

    try
    {
      V_DependencyInfo::iterator it    = outputs.begin();
      V_DependencyInfo::iterator itEnd = outputs.end();
      for ( ; it != itEnd; ++it )
      {
        const DependencyInfoPtr& output = (*it);

        Log::Bullet updateOutputBullet( Log::Streams::Debug, Log::Levels::Verbose, "UpdateOutput %s\n", output->m_Path.c_str() );

        GetFileMD5( output );
        CacheCommitGraph( output, false );

        DependencyInfoPtr& dependencyFile = CacheGetDependency( output->m_Path );
        dependencyFile->m_IsUpToDateCached = false;
      }
    }
    catch ( ... )
    {
      m_GraphDB->RollbackTrans();
      throw;
    }

    m_GraphDB->CommitTrans();
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Gets the graph and determines if a file is up to date
  bool DependencyGraph::IsUpToDate( const std::string& filePath )
  {
    Platform::TakeMutex mutex( m_HighLevelMutex );

    DEPENDENCIES_SCOPE_TIMER((""));

    Log::Bullet isUpToDateBullet( Log::Streams::Debug, Log::Levels::Verbose, "IsUpToDate %s\n", filePath.c_str() );

    DependencyInfoPtr file = CacheGetDependency( filePath );
    if ( !file )
    {
      return false;
    }

    if ( !file->m_IsUpToDateCached )
    {
      // the file's IsUpToDate flag hasn't been cached yet
      FileGraph fileGraph;
      CacheGetGraph( file, fileGraph );
    }
    return file->m_IsUpToDate;
  }

  /////////////////////////////////////
  // Determines if a list of files are up to date
  bool DependencyGraph::AreUpToDate( const V_string &filePaths )
  {
    for each ( const std::string& filePath in filePaths )
    {
      if ( !IsUpToDate( filePath ) )
        return false;
    }
    return true;
  }

  /////////////////////////////////////
  // Determines if a list of files are up to date
  bool DependencyGraph::AreUpToDate( const V_DependencyInfo &filePaths )
  {
    for each ( const DependencyInfoPtr& file in filePaths )
    {
      if ( !IsUpToDate( file->m_Path ) )
        return false;
    }
    return true;
  }






  /////////////////////////////////////////////////////////////////////////////
  // if the MD5 for the file is not set, generate it and update the DB
  // this string will be nulled every time the file is changed
  //
  void DependencyGraph::GetFileMD5( const DependencyInfoPtr& file )
  {
    DEPENDENCIES_SCOPE_TIMER((""));

    // if this file is in the cache, use the cacheFile
    DependencyInfoPtr cacheFile = CacheGetDependency( file->m_Path );
    if ( cacheFile )
    {
      if ( !cacheFile->IsMD5Valid() )
      {
        cacheFile->GenerateMD5();
        CacheDirtyDependency( cacheFile );
      }

      // reutrn the cache file as it it more up-to-date
      *file = *cacheFile;
    }
    // we are dealing with a file that is not in the Dep system
    // just get the MD5 and return that
    else
    {
      if ( !file->IsMD5Valid() )
      {
        file->GenerateMD5();
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  void DependencyGraph::AppendFileToSignature( const DependencyInfoPtr& file, CryptoPP::HashFilter* hashFilter, V_string& trace)
  {
    if ( file->AppendToSignature( hashFilter, trace ) )
    {
      Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "CreateSignature is skipping optional input file (%s)\n", file->m_Path.c_str() );
      return;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Used to get the list of ordered dependencies
  // 
  void DependencyGraph::GetOrderDependentFiles( const DependencyInfoPtr& outFile, FileGraph& fileGraph, V_string& orderDependentFiles, int curDepth )
  {
    if ( curDepth >= MAX_FILE_GRAPH_DEPTH )
    {
      throw MaxGraphDepthException();
    }

    if ( outFile->IsLeaf() 
      || outFile->m_Dependencies.Empty() )
    {
      // add the outFile to the graph
      return;
    }

    // add the dependency paths to the list
    if ( outFile->InputOrderMatters() )
    {
      OS_string::Iterator depItr = outFile->m_Dependencies.Begin();
      OS_string::Iterator depEnd = outFile->m_Dependencies.End();
      for ( ; depItr != depEnd ; ++depItr )
      {
        orderDependentFiles.push_back( *depItr );
      }
    }

    OS_string::Iterator depItr = outFile->m_Dependencies.Begin();
    OS_string::Iterator depEnd = outFile->m_Dependencies.End();
    for ( ; depItr != depEnd ; ++depItr )
    {
      const std::string& inFilePath = (*depItr);

      FileGraph::iterator findFile = fileGraph.find( inFilePath );
      if ( findFile == fileGraph.end() )
      {
        throw Exception ( "Error while getting inFile from the fileGraph." );
      }
      GetOrderDependentFiles( findFile->second, fileGraph, orderDependentFiles, curDepth + 1 );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  // Generates a signature for an output file using the MD5's of the input files
  // and the the output file's format version.
  // Returns empty string if the file is OUT-OF-DATE
  // ALWAYS generates the sig, even if there is already one cached
  void DependencyGraph::CreateSignature( const DependencyInfoPtr& file )
  {
    Platform::TakeMutex mutex( m_HighLevelMutex );

    DEPENDENCIES_SCOPE_TIMER((""));

    Log::Bullet createSigBullet( Log::Streams::Debug, Log::Levels::Verbose, "CreateSignature %s\n", file->m_Path.c_str() );

    // clear the signature
    file->m_Signature.clear();

    DependencyInfoPtr outFile = CacheGetDependency( file->m_Path );
    if ( !outFile )
    {
      return;
    }

    // get the file graph and throw exception is a dependency is required and missing
    FileGraph fileGraph;
    CacheGetGraph( outFile, fileGraph, true );

    // only generate the signature if the file is NOT a leaf node
    if ( outFile->IsLeaf() 
      || outFile->m_Dependencies.Empty() )
    {
      return;
    }

    outFile->m_Signature.clear();

    // generate the signature:
    CryptoPP::MD5 hash;
    CryptoPP::HashFilter hashFilter( hash, new CryptoPP::HexEncoder( new CryptoPP::StringSink( outFile->m_Signature ) ) );

    /////////////////////////////////////////////////
    // Hash the entire graph
    FileGraph::iterator itGraph  = fileGraph.begin();
    FileGraph::iterator endGraph = fileGraph.end();
    for ( ; itGraph != endGraph ; ++itGraph )
    {
      DependencyInfoPtr inFile = itGraph->second;

      //skip the output file:
      if ( inFile->m_Path == file->m_Path )
      {
        continue;
      }

      AppendFileToSignature( inFile, &hashFilter, outFile->m_SignatureTrace );
    }

    /////////////////////////////////////////////////
    // Append the ordered dependencies
    V_string orderDependentFiles;
    GetOrderDependentFiles( outFile, fileGraph, orderDependentFiles );

    V_string::iterator itFilePath  = orderDependentFiles.begin();
    V_string::iterator endFilePath = orderDependentFiles.end();
    for ( ; itFilePath != endFilePath ; ++itFilePath )
    {
      FileGraph::iterator findFile = fileGraph.find( *itFilePath );
      if ( findFile == fileGraph.end() )
      {
        throw Exception ( "Error while getting inFile from the fileGraph." );
      }

      AppendFileToSignature( findFile->second, &hashFilter, outFile->m_SignatureTrace );
    }

    std::stringstream fileSig;
    fileSig << file->m_Path;
    outFile->m_SignatureTrace.push_back( file->m_Path );

    fileSig << file->m_Spec->GetFormatVersion();
    outFile->m_SignatureTrace.push_back( file->m_Spec->GetFormatVersion() );

    hashFilter.Put( ( byte const* ) fileSig.str().data(), fileSig.str().size() );
    hashFilter.MessageEnd();

    // Ensure the signature is always upper case
    toUpper( outFile->m_Signature );

    CacheDirtyDependency( outFile );

    file->m_Signature = outFile->m_Signature;
    file->m_SignatureTrace = outFile->m_SignatureTrace;
  }


  /////////////////////////////////////
  // Generate a bunch of signatures
  void DependencyGraph::CreateSignatures( V_DependencyInfo &listOfFiles, bool trapExceptions )
  {
    V_DependencyInfo::iterator it    = listOfFiles.begin();
    V_DependencyInfo::iterator itEnd = listOfFiles.end();
    for ( ; it != itEnd; ++it )
    {
      try
      {
        CreateSignature( (*it) );
      }
      catch ( const Nocturnal::Exception& e )
      {
        (*it)->m_Signature.clear();
        if ( trapExceptions )
        {
          Log::Error( "Error creating signature for file '%s': %s\n", (*it)->m_Path.c_str(), e.what() );
        }
        else
        {
          throw;
        }
      }
    }
  }

  //*************************************************************************//
  //
  //  CACHE FUNCTIONS
  //
  //*************************************************************************//




  /////////////////////////////////////////////////////////////////////////////
  // Adds/Updates the file m_CachedFileGraph, and sets the file to dirty by adding
  // it to the m_CachedDirtyFiles. Returns a pointer to the newly added file
  //
  DependencyInfoPtr DependencyGraph::CacheDependency( const DependencyInfoPtr& file, bool makeDirty )
  {
    DEPENDENCIES_SCOPE_TIMER((""));

    // store the contents of file in the cache
    m_CachedFileGraph[ file->m_Path ] = file;

    if ( makeDirty )
    {
      m_CachedDirtyFiles.insert( file );
    }

    return m_CachedFileGraph[ file->m_Path ];
  }

  /////////////////////////////////////////////////////////////////////////////
  // Sets the file to dirty by adding it to the m_CachedDirtyFiles
  //
  void DependencyGraph::CacheDirtyDependency( DependencyInfoPtr file )
  {
    DEPENDENCIES_SCOPE_TIMER((""));

    m_CachedDirtyFiles.insert( file );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Registers a new file with the Cache system
  DependencyInfoPtr DependencyGraph::CacheRegisterDependency( const DependencyInfoPtr& file )
  {
    DependencyInfoPtr regFile = CacheGetDependency( file );
    if ( !regFile )
    {
      regFile = CacheDependency( CreateCacheCopy( file ) );
    }
    else
    {
      if ( regFile->IsLeaf() != file->IsLeaf() )
      {
        Log::Warning( "An input file has been registered with the dependencies systems as both a leaf and non-leaf input file: %s\n", file->m_Path.c_str() );
      }
    }

    // update the selected bits of the dependency's info

    regFile->CacheCopy( *file );

    m_CachedDirtyFiles.insert( regFile );

    return regFile;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Tries to find the file in the cache; failing that, attempt to get the 
  // file from the GraphDB and inserts the file into the cache.
  // If successful, returns the file ptr to the DependencyInfo in the cache.
  //
  DependencyInfoPtr DependencyGraph::CacheGetDependency( const DependencyInfoPtr& info, bool useDB )
  {
    DEPENDENCIES_SCOPE_TIMER((""));

    DependencyInfoPtr file = NULL;

    FileGraphCache::iterator found = m_CachedFileGraph.find( info->m_Path );
    if ( found != m_CachedFileGraph.end() )
    {
      file = found->second;
    }
    else if ( useDB )
    {
      DependencyInfoPtr getFile;
      if( dynamic_cast< FileInfo* >( &*info ) )
      {
        getFile = new FileInfo;
      }
      else if( dynamic_cast< DataInfo* >( &*info ) )
      {
        getFile = new DataInfo;
      }
      if ( (int) m_GraphDB->SelectDependency( info->m_Path, getFile ) != SQL::InvalidRowID  )
      {
        file = CacheDependency( getFile, false );
      }
    }

    return file;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Tries to find the file in the cache; failing that, attempt to get the 
  // file from the GraphDB and inserts the file into the cache.
  // If successful, returns the file ptr to the DependencyInfo in the cache.
  //
  DependencyInfoPtr DependencyGraph::CacheGetDependency( const std::string& filePath, bool useDB )
  {
    DEPENDENCIES_SCOPE_TIMER((""));

    DependencyInfoPtr file = NULL;

    FileGraphCache::iterator found = m_CachedFileGraph.find( filePath );
    if ( found != m_CachedFileGraph.end() )
    {
      file = found->second;
    }
    else if ( useDB )
    {

      DependencyInfoPtr getFile = new FileInfo;
      if ( (int) m_GraphDB->SelectDependency( filePath, getFile ) != SQL::InvalidRowID  )
      {
        file = CacheDependency( getFile, false );
      }
    }

    return file;
  }



  /////////////////////////////////////////////////////////////////////////////
  // Writes out the updated dirty cache for the given file to the DB
  // cache always stomps DB: path, spec, 
  //
  void DependencyGraph::CacheCommitDependency( DependencyInfoPtr file, bool fileExists )
  {
    // if the file's not new AND not dirty, early out
    if ( ( file->m_RowID != SQL::InvalidRowID )
      && ( m_CachedDirtyFiles.find( file ) == m_CachedDirtyFiles.end() ) )
    {
      return;
    }

    // if there's a new spec use that one
    if ( file->m_NewSpec )
    {
      file->m_Spec          = file->m_NewSpec;
      file->m_NewSpec       = NULL;
      file->m_VersionRowID  = SQL::InvalidRowID;
    }

    if ( fileExists )
    {
      GetFileMD5( file );
    }

    file->SetInfo();
    file->m_VersionRowID = m_GraphDB->InsertVersion( *(file->m_Spec), file->m_FormatVersion );      
    file->m_RowID        = m_GraphDB->ReplaceDependency( file, file->m_VersionRowID );

    // remove the file from the dirty cache set
    m_CachedDirtyFiles.erase( file );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Adds a new file dependency to the outFile and adds both files to the cache,
  // and sets the dirty
  //
  void DependencyGraph::CacheGraph( DependencyInfoPtr outFile, DependencyInfoPtr inFile, bool inFileIsOptional )
  {
    DEPENDENCIES_SCOPE_TIMER((""));

    // insert this pair into the list of re-registered files
    ReregisterMap::iterator found = m_ReregisterMap.find( outFile->m_Path );
    if ( found == m_ReregisterMap.end() )
    {
      OS_string inFilePaths;
      inFilePaths.Append( inFile->m_Path );

      m_ReregisterMap.insert( ReregisterMap::value_type( outFile->m_Path, inFilePaths ) );
    }
    else
    {
      found->second.Append( inFile->m_Path );
    }

    outFile->m_Dependencies.Append( inFile->m_Path );

    inFile->m_GraphInfo.insert( M_GraphInfo::value_type( outFile->m_Path, GraphInfo() ) );
    inFile->m_GraphInfo[outFile->m_Path].m_IsOptional = inFileIsOptional;
    inFile->m_GraphInfo[outFile->m_Path].m_ExistedLastBuild = ( inFileIsOptional ? inFile->Exists() : true );

    m_CachedDirtyFiles.insert( outFile );
  }


  /////////////////////////////////////////////////////////////////////////////
  // GetGraph is used to determine if a file is UP-TO-DATE before using an
  // input file->
  //
  // The graph DB contains: 
  //  - the most recently cached built file and FormatVersion info for a given file/Finder::FileSpec
  //  - if a FormatVersion changes, it should invalidate all cached files that
  //    were of the previous version
  //
  // A file is OUT-OF-DATE when:
  //  - file does not exist on disc
  //  - there is no file graph data
  //  - no cached dependencies found for the given file
  //  - expected FormatVersion has changed (builder updated)
  //  - file size has changed
  //  - last modified time has changed, size has not changed,
  //    and there is no MD5 cache OR MD5 has changed
  //  - inFile's lastModified time is different than the 
  //    last time the out file was build (graph.in_last_modified)
  //  - the dependencies have changed:
  //     o the input files are order dependent and the order of dependencies has changed
  //     o the list of dependencies has changes
  //  
  // A file's dependencies are OUT-OF-DATE if:
  //  - one or more of its input dependencies is OUT-OF-DATE
  //
  // Recursive GetGraph function must be entered with a valid File
  void DependencyGraph::CacheGetGraph( DependencyInfoPtr outFile, FileGraph& fileGraph, bool throwIfMissingInput, bool recurse, int curDepth )
  {
    if ( curDepth >= MAX_FILE_GRAPH_DEPTH )
    {
      throw MaxGraphDepthException();
    }

    Log::Bullet cacheGetGraphBullet( Log::Streams::Debug, Log::Levels::Verbose, "CacheGetGraph %s\n", outFile->m_Path.c_str() );

    // determine if the outFile is out of date, this updates m_IsUpToDate
    if ( !outFile->m_IsUpToDate || !outFile->m_IsUpToDateCached )
    {
      outFile->m_IsUpToDate = !outFile->WasModified();
      if ( !outFile->m_IsUpToDate )
      {
        Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: file was modified\n" );
      }
    }

    // if the outFile is a leaf node, early out
    if ( outFile->IsLeaf() )
    {
      // add the outFile to the graph
      outFile->m_IsUpToDateCached = true;
      fileGraph[outFile->m_Path] = outFile;
      return;
    }

    // Assume that if the outFile is already cached, and it's dep list has
    // been read from DB and is not empty, that its graph is correct
    if ( !outFile->m_Dependencies.Empty() && outFile->m_WasGraphSelected )
    {
      OS_string depCopy = outFile->m_Dependencies;
      OS_string::Iterator depItr = depCopy.Begin();
      OS_string::Iterator depEnd = depCopy.End();
      for ( ; depItr != depEnd ; ++depItr )
      {
        const std::string& inFilePath = (*depItr);

        // only add inputs that have been re-ergisterd in the m_ReregisterMap
        if ( WasDependencyRemoved( outFile->m_Path, inFilePath ) )
        {
          // remove it from the list of dependencies and continue
          outFile->m_Dependencies.Remove( inFilePath );

          // OUT-OF-DATE: the list of file dependencies has changed
          outFile->m_IsUpToDate = false;
          Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: the list of file dependencies has changed\n" );

          continue;
        }
      }
    }
    else
    {
      // get the dependencies for the given outFile and add each to the graph
      OS_DependencyInfo listOfFiles;
      m_GraphDB->SelectGraph( outFile, listOfFiles );

      outFile->m_WasGraphSelected = true;

      if ( listOfFiles.Empty() )
      {
        // OUT-OF-DATE: no cached dependencies found for the given outFile
        outFile->m_IsUpToDate = false;
        Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: no cached dependencies found for the given outFile\n" );
      }
      else
      {
        // OUT-OF-DATE: the dependencies have changed
        if ( outFile->m_IsUpToDate )
        {
          ReregisterMap::iterator foundOutFileReg = m_ReregisterMap.find( outFile->m_Path );
          if ( foundOutFileReg != m_ReregisterMap.end() )
          {
            // try to early out
            if ( foundOutFileReg->second.Size() != listOfFiles.Size() )
            {
              outFile->m_IsUpToDate = false;
              Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: the dependencies have changed\n" );
            }
            else
            {
              // OUT-OF-DATE: the input files are order dependent and the order of dependencies has changed
              if ( outFile->InputOrderMatters() )
              {
                OS_string selectedFilePaths;
                OS_DependencyInfo::Iterator itr = listOfFiles.Begin();
                OS_DependencyInfo::Iterator end = listOfFiles.End();
                for ( ; itr != end ; ++itr )
                {
                  selectedFilePaths.Append( (*itr)->m_Path );
                }

                outFile->m_IsUpToDate = ( foundOutFileReg->second == selectedFilePaths );
                if ( !outFile->m_IsUpToDate )
                {
                  Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: the input files are order dependent and the order of dependencies has changed\n" );
                }
              }
              // OUT-OF-DATE: the list of file dependencies has changed
              else
              {
                OS_DependencyInfo::Iterator itr = listOfFiles.Begin();
                OS_DependencyInfo::Iterator end = listOfFiles.End();
                for ( ; itr != end ; ++itr )
                {
                  if ( !foundOutFileReg->second.Contains( (*itr)->m_Path ) )
                  {
                    outFile->m_IsUpToDate = false;
                    Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: the list of file dependencies has changed\n" );

                    break;
                  }
                }
              }
            }
          }
        }

        // add the selected files to the graph
        OS_DependencyInfo::Iterator fileItr = listOfFiles.Begin();
        OS_DependencyInfo::Iterator fileEnd = listOfFiles.End();
        for ( ; fileItr != fileEnd ; ++fileItr )
        {
          // only add inputs that have been re-ergisterd in the m_ReregisterMap
          if ( WasDependencyRemoved( outFile->m_Path, (*fileItr)->m_Path ) )
          {
            // remove it from the list of dependencies and continue
            outFile->m_Dependencies.Remove( (*fileItr)->m_Path );

            // OUT-OF-DATE: the list of file dependencies has changed
            outFile->m_IsUpToDate = false;
            Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: the list of file dependencies has changed\n" );

            continue;
          }

          // if the inFile is already in the cache, use that instead of the select result
          // otherwise add it to the cache and continue
          DependencyInfoPtr inFile = CacheGetDependency( *fileItr, false );
          if ( !inFile )
          {
            inFile = CacheDependency( (*fileItr), false );
          }
          else if ( !(*fileItr)->m_GraphInfo.empty() )
          {
            //inFile->m_GraphInfo.insert( (*fileItr)->m_GraphInfo.begin(), (*fileItr)->m_GraphInfo.end() );
            for each ( const M_GraphInfo::value_type& graphInfoPair in (*fileItr)->m_GraphInfo )
            {
              inFile->m_GraphInfo.insert( graphInfoPair );

              if ( inFile->m_GraphInfo[graphInfoPair.first].m_LastModified == 0 )
              {
                inFile->m_GraphInfo[graphInfoPair.first].m_LastModified = graphInfoPair.second.m_LastModified;
              }
            }
          }

          if ( !inFile )
          {
            throw Exception ( "Error while getting inFile from the CacheGetDependency function, call 2." );
          }

          // add the filePath to the list of dependencies
          outFile->m_Dependencies.Append( inFile->m_Path );
        }
      }
    }

    OS_string depCopy = outFile->m_Dependencies;
    OS_string::Iterator depItr = depCopy.Begin();
    OS_string::Iterator depEnd = depCopy.End();
    for ( ; depItr != depEnd ; ++depItr )
    {
      const std::string& inFilePath = (*depItr);

      DependencyInfoPtr inFile = CacheGetDependency( inFilePath );
      if ( !inFile )
      {
        throw Exception ( "Error while getting inFile from the CacheGetDependency function." );
      }

      if ( recurse || curDepth < 1 )
      {
        CacheGetInFileGraph( outFile, inFile, fileGraph, throwIfMissingInput, recurse, curDepth );
      }
    }      


    // add the outFile to the graph
    outFile->m_IsUpToDateCached = true;
    fileGraph[outFile->m_Path] = outFile;
  }


  /////////////////////////////////////
  // Returns true if the file dependency has been removed;
  // Helper for CacheGetGraph
  //
  inline bool DependencyGraph::WasDependencyRemoved( const std::string& outFilePath, const std::string& inFilePath )
  {
    ReregisterMap::iterator foundOutFileReg = m_ReregisterMap.find( outFilePath );
    if ( foundOutFileReg != m_ReregisterMap.end() )
    {
      if ( !foundOutFileReg->second.Contains( inFilePath ) )
      {
        return true;
      }
    }

    return false;
  }

  /////////////////////////////////////
  // Helper function for CacheGetGraph
  void DependencyGraph::CacheGetInFileGraph( DependencyInfoPtr outFile, DependencyInfoPtr inFile, FileGraph& fileGraph, bool throwIfMissingInput, bool recurse, int curDepth )
  {
    bool inFileExists = inFile->Exists();
    bool inFileIsOptionalAndIsMissing = false;

    M_GraphInfo::iterator foundGraphInfo = inFile->m_GraphInfo.find( outFile->m_Path );

    // OUT-OF-DATE: there is no outfile meta date for this infile
    if ( outFile->m_IsUpToDate && foundGraphInfo == inFile->m_GraphInfo.end() )
    {
      NOC_BREAK(); // how did this happen?

      outFile->m_IsUpToDate = false;
      Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: there is no outfile meta date for this infile (%s)\n", inFile->m_Path.c_str() );
    }

    // OUT-OF-DATE: the inFile's GraphInfo.m_LastModified time is != to it's current m_LastModifiedTime
    if ( outFile->m_IsUpToDate && inFile->m_LastModified != foundGraphInfo->second.m_LastModified ) 
    {
      outFile->m_IsUpToDate = false;
      Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: the inFile (%s) GraphInfo.m_LastModified time is != to it's current m_LastModifiedTime\n", inFile->m_Path.c_str() );
    }

    // determine if the input file is is optional
    if ( foundGraphInfo->second.m_IsOptional )
    {
      inFileIsOptionalAndIsMissing = !inFileExists;

      // OUT-OF-DATE: the inFile is optional and the exists state has changed since the last build
      if ( outFile->m_IsUpToDate && inFileExists != foundGraphInfo->second.m_ExistedLastBuild )
      {
        outFile->m_IsUpToDate = false;
        Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: the inFile (%s) is optional and the exists state has changed since the last build\n", inFile->m_Path.c_str() );
      }
    }
    // else throw an exception if the input file is NOT optional, and is missing
    else if ( !inFileExists && throwIfMissingInput )
    {
      throw Exception( "Required input file is missing: %s.", inFile->m_Path.c_str() );
    }

    // OUT-OF-DATE: one or more of it's dependencies' modified times is more recent
    if ( outFile->m_IsUpToDate && inFile->m_LastModified > outFile->m_LastModified )
    {
      outFile->m_IsUpToDate = false;
      Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: one or more of it's dependencies' modified times is more recent\n" );
    }

    // only get the inFile graph if it's not already in graph
    if ( fileGraph.find( inFile->m_Path ) == fileGraph.end() )
    {
      // this will set inFile->m_IsUpToDate
      CacheGetGraph( inFile, fileGraph, throwIfMissingInput, recurse, curDepth + 1 );
    }

    // OUT-OF-DATE: one or more of it's dependencies is OUT-OF-DATE
    if ( outFile->m_IsUpToDate && !inFileIsOptionalAndIsMissing && !inFile->m_IsUpToDate )
    {
      outFile->m_IsUpToDate = false;
      Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "OUT-OF-DATE: one or more of it's dependencies is OUT-OF-DATE\n" );
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Update the output file's graph
  void DependencyGraph::CacheCommitGraph( const DependencyInfoPtr file, bool useTransaction )
  {
    DEPENDENCIES_SCOPE_TIMER((""));

    if ( useTransaction )
    {
      m_GraphDB->BeginTrans();
    }

    // wrap the transaction in a try/catch block
    try
    {
      // commit the output file
      if ( file->m_Path.empty() )
      {
        throw Exception( "Cannot update an output file with an empty file name." );
      }

      // try to get the file from cache
      DependencyInfoPtr outFile = CacheGetDependency( file->m_Path, false );
      if ( !outFile )
      {
        throw Exception ( "Cannot update an output file that is not in the dependency system cache: %s", file->m_Path.c_str() );
      }

      outFile->m_Spec = file->m_Spec;
      outFile->IsLeaf( false );  // we know this is an output file
      CacheCommitDependency( outFile );


      // Only get the shallow graph, don't recurse
      FileGraph fileGraph;
      CacheGetGraph( outFile, fileGraph, false, false );

      int inFileOrderIndex = 0;
      std::string validGraphInFileIDs;  

      // update all leaf inputs
      OS_string::Iterator depItr = outFile->m_Dependencies.Begin();
      OS_string::Iterator depEnd = outFile->m_Dependencies.End();
      for ( ; depItr != depEnd ; ++depItr )
      {
        FileGraph::iterator findInFile = fileGraph.find( (*depItr) );
        if ( findInFile == fileGraph.end() )
        {
          throw Exception( "An input dependency %s is required for output file %s but is not found in the graph cache. How in the hell did we get here?!",  (*depItr).c_str(), outFile->m_Path.c_str() );
        }

        DependencyInfoPtr inFile = findInFile->second;

        // only update and register inputs that have been re-ergisterd in the m_ReregisterMap
        ReregisterMap::iterator foudOutFileReg = m_ReregisterMap.find( outFile->m_Path );
        if ( foudOutFileReg != m_ReregisterMap.end() )
        {
          if ( !foudOutFileReg->second.Contains( inFile->m_Path ) )
          {
            continue;
          }
        }

        bool inFileIsOptional = false;
        bool inFileExistedLastBuild = true;

        M_GraphInfo::iterator foundGraphInfo = inFile->m_GraphInfo.find( outFile->m_Path );
        if ( foundGraphInfo != inFile->m_GraphInfo.end() )
        {
          inFileIsOptional = foundGraphInfo->second.m_IsOptional;
          inFileExistedLastBuild = foundGraphInfo->second.m_ExistedLastBuild;
        }

        bool fileExists = inFile->Exists();

        // assert the either the inFile is there, or it's allowed to be missing
        if ( !fileExists && !inFileIsOptional )
        {
          throw Exception( "File '%s' does not exist, and is required to build '%s'\n", inFile->m_Path.c_str(), outFile->m_Path.c_str() );
        }

        // add/update the existing leaf files
        // re-get the file (the m_RodID may have changed from -1 to something reasonable)
        if ( inFile->IsLeaf() || inFile->m_RowID == SQL::InvalidRowID )
        {
          if ( fileExists || inFileIsOptional )
          {
            CacheCommitDependency( inFile, fileExists );
          }
        }

        // continue to the next file if this infile was not used in this build
        if ( inFileIsOptional && inFile->m_RowID == SQL::InvalidRowID )
        {
          continue;
        }

        // make sure we've got some valid data to pass into InsertGraph()
        if ( outFile->m_RowID == SQL::InvalidRowID || inFile->m_RowID == SQL::InvalidRowID )
        {
          throw Exception( "Invalid row id passed to InsertGraph, outFileId: %I64d  inFileId: %I64d (%s)",
            outFile->m_RowID, inFile->m_RowID, inFile->m_Path.c_str() );
        }

        // foreach (out,in) pair:
        //  - make sure the pair is in the DB
        m_GraphDB->InsertGraph( outFile->m_RowID, inFile->m_RowID, inFile->m_LastModified, inFileOrderIndex++, inFileIsOptional, inFileExistedLastBuild );

        // build up the string for checking which ids should be in the given graph
        if ( !validGraphInFileIDs.empty() )
        {
          validGraphInFileIDs += ",";
        }

        std::stringstream idStr;
        idStr << "'" << inFile->m_RowID << "'";  
        validGraphInFileIDs += idStr.str();
      }

      // removes old graph entries that are not in the list of validGraphInFileIDs
      if ( !validGraphInFileIDs.empty() )
      {
        m_GraphDB->DeleteGraphPairs( outFile->m_RowID, validGraphInFileIDs );
      }

    }
    catch( ... )
    {
      // roll back the transactio if any exception occurred
      if ( useTransaction )
      {
        m_GraphDB->RollbackTrans();
      }

      throw;
    }

    if ( useTransaction )
    {
      m_GraphDB->CommitTrans();
    }
  }




  /////////////////////////////////////////////////////////////////////////////
  // clears all cache
  void DependencyGraph::ClearCache()
  { 
    m_ReregisterMap.clear();
    m_CachedDirtyFiles.clear();
    m_CachedFileGraph.clear();
  }

  DependencyInfoPtr DependencyGraph::CreateCacheCopy( const DependencyInfoPtr& dependency )
  {
    FileInfo* fileInfo = dynamic_cast< FileInfo* >( &(*dependency) );

    if( fileInfo )
    {
      return new FileInfo( fileInfo->m_Path, *fileInfo->m_Spec );
    }

    DataInfo* dataInfo = dynamic_cast< DataInfo* >( &(*dependency) );

    if ( dataInfo )
    {
      return new DataInfo( dataInfo->m_Path, *dataInfo->m_Spec, dataInfo->m_Data, dataInfo->m_DataSize );
    }

    return NULL;
  }
}


