#include "Windows/Windows.h"
#include "CacheFiles.h"
#include "CacheFileStats.h"
#include "Platform/Mutex.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/File.h"
#include "AppUtils/AppUtils.h"
#include "Console/Console.h"
#include "Windows/Error.h"
#include "Common/Config.h"
#include "Common/CommandLine.h"
#include "Common/Environment.h"
#include "Common/String/Units.h"
#include "Common/Version.h"
#include "Dependencies/Dependencies.h"
#include "Profile/Timer.h"

#include <algorithm>
#include <vector>
#include <queue>

static bool            g_Disable = false;
static i32             g_InitCount = 0;
static u32             g_ThreadCount = 5;
static std::string     g_CacheFilesPath;
static f32             g_AverageThroughput = 0;
static u32             g_AverageThroughputSampleCount = 0;

namespace Results
{
  enum Result
  {
    Success,
    MD5Missing,
    MD5Mismatch,
    MD5WritingFailed,
    DestinationExists,
    SourceMissing,
    CopyFailed,
    Busy
  };
  const static char* Strings[]  = {
    "Success",
    "MD5 missing",
    "MD5 mismatch",
    "MD5 file could not be written",
    "Destination file exists",
    "Source file is missing",
    "Copy failed",
    "CCS busy"
  };
}
typedef Results::Result Result;

class CacheFilesException : public Nocturnal::Exception
{
public:
  CacheFilesException( Result result, const char *msgFormat, ... )
  {
    m_Result = result;

    va_list msgArgs;
    va_start( msgArgs, msgFormat );
    SetMessage( msgFormat, msgArgs );
    va_end( msgArgs );
  }

  Result GetResult() const
  {
    return m_Result;
  }

protected:
  CacheFilesException() throw() {} // hide default c_tor

private:
  Result m_Result;
};

struct TransferStats : public Nocturnal::RefCountBase<TransferStats>
{
  TransferStats()
    : m_Download( false )
    , m_Duration( 0.0f )
    , m_Speed( 0.0f )
    , m_Size( 0 )
    , m_Count( 0 )
    , m_Copied( 0 )
  {
  }

  bool  m_Download;
  f32   m_Duration;
  f32   m_Speed;
  u64   m_Size;
  u32   m_Count;
  u32   m_Copied;
};
typedef Nocturnal::SmartPtr< TransferStats > TransferStatsPtr;
typedef std::vector< TransferStatsPtr > V_TransferStats;
V_TransferStats g_TransferStats;

typedef void (*CopyFunction)( std::string sourceFile, std::string targetFile, Dependencies::DependencyInfo* fileInfo );

struct CopyJob
{
  CopyJob()
    : m_Result( Results::Success )
    , m_Function( NULL )
    , m_FileInfo( NULL )
  {

  }

  Result                  m_Result;
  CopyFunction            m_Function;
  Dependencies::DependencyInfo* m_FileInfo;
  std::string             m_Source;
  std::string             m_Target;
  TransferStatsPtr        m_Stats;
};
typedef std::vector<CopyJob> V_CopyJob;
typedef std::queue<CopyJob*> Q_CopyJob;

bool ReadFile( const std::string& path, std::string& data )
{
  FILE* f = fopen( path.c_str(), "r" );
  if ( !f )
  {
    return false;
  }

  fseek( f, 0, SEEK_END );
  u32 dataSize = ftell( f );
  fseek( f, 0, SEEK_SET );

  char* buffer = new char[ dataSize + 1 ];
  fread( buffer, dataSize, 1, f );
  fclose( f );
  buffer[ dataSize ] = 0;
  data = buffer;

  delete [] buffer;

  return true;
}

bool WriteFile( const std::string& path, const std::string& data )
{
  FILE* f = fopen( path.c_str(), "w" );
  if ( !f )
  {
    return false;
  }

  fwrite( data.c_str(), data.length(), 1, f );
  fclose( f );

  return true;
}

bool WriteFile( const std::string& path, const V_string& data )
{
  FILE* f = fopen( path.c_str(), "w" );
  if ( !f )
  {
    return false;
  }

  V_string::const_iterator itr = data.begin();
  V_string::const_iterator end = data.end();
  for ( ; itr != end; ++itr )
  {
    fprintf( f, "%s\n", itr->c_str() );
  }

  fclose( f );

  return true;
}

void TouchFile( const std::string& path )
{
  HANDLE handle = CreateFile(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if ( handle == INVALID_HANDLE_VALUE )
  {
    Console::Warning( Console::Levels::Verbose, "Could not open file '%s' to update timestamp: %s\n", path.c_str(), Windows::GetErrorString().c_str() );
  }
  else
  {
    SYSTEMTIME st;
    ::GetSystemTime( &st ); // gets current time

    FILETIME ft;
    ::SystemTimeToFileTime( &st, &ft );  // converts to file time format

    if ( !::SetFileTime( handle, (LPFILETIME) NULL, (LPFILETIME) NULL, &ft ) )
    {
      Console::Warning( Console::Levels::Verbose, "Could not update timestamp on file '%s': %s\n", path.c_str(), Windows::GetErrorString().c_str() );
    }

    ::CloseHandle( handle );
  }
}

void TryMakePath( const std::string& targetFile )
{
  std::string path = targetFile;
  FileSystem::StripLeaf( path );

  try
  {
    FileSystem::MakePath( path );
  }
  catch ( const std::exception& ex )
  {
    // This can fail with an exception if another thread is attempting to make the same path at the same time
    //  so just check to see if the folder actually exists, and if it does it doesn't matter who created it
    if ( !FileSystem::Exists( path ) )
    {
      throw CacheFilesException( Results::CopyFailed, "Failed to create path '%s': %s", path.c_str(), ex.what() );
    }
  }
}

void TryCopyFile( const std::string& sourceFile, const std::string& targetFile )
{
  // Faster than FileSystem::Exists
  WIN32_FILE_ATTRIBUTE_DATA sourceStats;
  if ( !::GetFileAttributesEx( sourceFile.c_str(), GetFileExInfoStandard, &sourceStats ) )
  {
    throw CacheFilesException( Results::SourceMissing, "Source does not exist: '%s'", sourceFile.c_str() );
  }

  std::string computerName;
  Nocturnal::GetEnvVar( "COMPUTERNAME", computerName );

  std::string tempTargetFile = targetFile + "." + computerName;
  if ( !::CopyFile( sourceFile.c_str(), tempTargetFile.c_str(), false ) )
  {
    throw CacheFilesException( Results::CopyFailed, "Failed to copy '%s' to '%s': %s", sourceFile.c_str(), tempTargetFile.c_str(), Windows::GetErrorString().c_str() );
  }

  if ( !::MoveFileEx( tempTargetFile.c_str(), targetFile.c_str(), MOVEFILE_REPLACE_EXISTING ) )
  {
    throw CacheFilesException( Results::CopyFailed, "Failed to move '%s' to '%s': %s", tempTargetFile.c_str(), targetFile.c_str(), Windows::GetErrorString().c_str() );
  }
}

void CopyFileToServer( std::string sourceFile, std::string targetFile, Dependencies::DependencyInfo* fileInfo )
{
  TryMakePath( targetFile );

  std::string md5File = targetFile;
  FileSystem::StripLeaf( md5File );
  md5File += "md5.txt";

  std::string traceFile = targetFile;
  FileSystem::StripLeaf( traceFile );
  traceFile += "trace.txt";

  std::string toolsFile = targetFile;
  FileSystem::StripLeaf( toolsFile );
  toolsFile += "tools.txt";

  // Faster than FileSystem::Exists
  WIN32_FILE_ATTRIBUTE_DATA targetStats;
  
  // file already exists in CCS
  if ( ::GetFileAttributesEx( targetFile.c_str(), GetFileExInfoStandard, &targetStats ) )
  {
    //
    // Try and detect an MD5 mismatch and warn the user
    //

    std::string storedMD5;
    if ( !ReadFile( md5File, storedMD5 ) )
    {
      Console::Debug( "MD5 info does not exist for file: %s (local: %s), generating...\n", sourceFile.c_str(), targetFile.c_str() );

      // this is slow
      std::string localmd5;
      FileSystem::File::GenerateMD5( targetFile, storedMD5 );

      // write the md5 information
      if ( !WriteFile( md5File, storedMD5 ) )
      {
        throw CacheFilesException( Results::MD5WritingFailed, "Failed to write md5 file '%s' for file '%s'\n", md5File.c_str(), targetFile.c_str() );
      }
    }

    if ( fileInfo->m_MD5 != storedMD5 )
    {
      Console::Level level = AppUtils::IsToolsBuilder() ? Console::Levels::Default : Console::Levels::Verbose;
      Console::Warning( level, "MD5 Mismatch: %s (target: %s): Local (built) MD5 (%s) did not match expected MD5 (%s)\n", sourceFile.c_str(), targetFile.c_str(), fileInfo->m_MD5.c_str(), storedMD5.c_str() );
    }
  }
  else // file has not been added to CCS yet
  {
    // do the actual file movement
    TryCopyFile( sourceFile, targetFile );

    // write the md5 information
    WriteFile( md5File, fileInfo->m_MD5 );

    // write the trace information
    WriteFile( traceFile, fileInfo->m_SignatureTrace );

    // write the tools information
    WriteFile( toolsFile, NOCTURNAL_VERSION_STRING );
  }

  // set the last modified time one the target (server) file, this is so we can easily automate cleanup of files based on how recently they've been accessed
  TouchFile( targetFile );
}

void CopyFileFromServer( std::string sourceFile, std::string targetFile, Dependencies::DependencyInfo* fileInfo )
{
  TryMakePath( targetFile );

  std::string md5File = sourceFile;
  FileSystem::StripLeaf( md5File );
  md5File += "md5.txt";

  // do the actual file movement
  TryCopyFile( sourceFile, targetFile );

  std::string storedMD5;
  if ( ReadFile( md5File, storedMD5 ) )
  {
    std::string localmd5;
    FileSystem::File::GenerateMD5( targetFile, localmd5 );

    if ( localmd5 != storedMD5 )
    {
      throw CacheFilesException( Results::MD5Mismatch, "Failed to download file: %s (local: %s): Local MD5 (%s) != Expected MD5 (%s)\n", sourceFile.c_str(), targetFile.c_str(), localmd5.c_str(), storedMD5.c_str() );
    }
  }
  else
  {
    throw CacheFilesException( Results::MD5Missing, "MD5 info does not exist for file: %s (local: %s)\n", sourceFile.c_str(), targetFile.c_str() );
  }

  // set the last modified time one the source (server) file, this is so we can easily automate cleanup of files based on how recently they've been accessed
  TouchFile( sourceFile );

  // set the last modified time on the target file this so the dependencies system works correctly
  TouchFile( targetFile );
}

DWORD WINAPI CopyThread( LPVOID lpParam )
{
  Platform::Locker<Q_CopyJob>* queue = (Platform::Locker<Q_CopyJob>*)lpParam;

  while ( 1 )
  {
    CopyJob* job = NULL;

    {
      Platform::Locker<Q_CopyJob>::Handle jobs = queue->Lock();
      if ( !jobs->empty() )
      {
        job = jobs->front();
        jobs->pop();
      }
    }

    if ( !job )
    {
      break;
    }

    try
    {
      job->m_Result = Results::Success;

      try
      {
        job->m_Function( job->m_Source, job->m_Target, job->m_FileInfo );
      }
      catch ( const CacheFilesException& ex )
      {
        // it's not actually worth a warning in the case where the file is missing from the server, that
        // is expected and acceptable behavior
        if ( !( job->m_Function == CopyFileFromServer && ex.GetResult() == Results::SourceMissing ) )
        {
          Console::Warning( "Error copying file '%s' to '%s': %s\n", job->m_Source.c_str(), job->m_Target.c_str(), ex.what() );
          NOC_ASSERT( ex.GetResult() != Results::Success ); // this would not make sense...
        }

        job->m_Result = ex.GetResult();
      }
      catch ( const std::exception& ex )
      {
        Console::Warning( "Error copying file '%s' to '%s': %s\n", job->m_Source.c_str(), job->m_Target.c_str(), ex.what() );
        job->m_Result = Results::CopyFailed; // a general failure
      }

      job->m_FileInfo->m_Downloaded = job->m_Result == Results::Success;

      if ( job->m_Result == Results::Success )
      {
        job->m_FileInfo->SetInfo();
      }
    }
    catch ( const std::exception& ex )
    {
      Console::Warning( "Unhandled exception in CacheFiles::CopyThread: %s\n", ex.what() );
    }
    catch ( ... )
    {
      Console::Warning( "Unhandled exception in CacheFiles::CopyThread\n" );
    }
  }

  return TRUE;
}

bool CopyFiles( Dependencies::DependencyGraph& depGraph, const Dependencies::V_DependencyInfo& files, bool toServer )
{
  if ( g_Disable )
  {
    Console::Print( Console::Levels::Extreme, "Built file copy is disabled\n" );
    return false;
  }

  Profile::Timer timer;
  Console::Bullet bullet (files.size() >= 10 ? "Copying %d files\n" : NULL, files.size());

  // track the stats for this session
  TransferStatsPtr stats = new TransferStats ();

  // the jobs live here
  V_CopyJob jobs;

  // the queue of pointers
  Platform::Locker<Q_CopyJob> queue;

  // the handles to our worker threads
  std::vector<HANDLE> threads;

  // create a job for each of the files
  Dependencies::V_DependencyInfo::const_iterator fileItr = files.begin();
  Dependencies::V_DependencyInfo::const_iterator fileEnd = files.end();
  for ( ; fileItr != fileEnd; ++fileItr )
  {
    Dependencies::DependencyInfo* file ( *fileItr );

    if ( file->m_Signature.empty() || ( !toServer && file->m_IsUpToDate ) )
    {
      continue;
    }

    // alloc the job
    jobs.push_back( CopyJob() );
    CopyJob& job = jobs.back();

    // set file and stat info
    job.m_FileInfo = file;
    job.m_Stats = stats;

    // setup network file path
    std::string serverFile = CacheFiles::GetOutputFilename( file );

    // set job data
    std::string localFile = file->m_Path;
    if ( toServer )
    {
      job.m_Function = &CopyFileToServer;
      job.m_Source = localFile;
      job.m_Target = serverFile;

      if ( job.m_FileInfo->m_MD5.empty() )
      {
        // If you hit this you've probably failed to call UpdateOutputs before this call to Put()
        // Not really a problem, but not good either
        NOC_BREAK();

        depGraph.GetFileMD5( job.m_FileInfo );
      }
    }
    else
    {
      job.m_Function = &CopyFileFromServer;
      job.m_Source = serverFile;
      job.m_Target = localFile;
    }
  }

  V_CopyJob::iterator jobItr = jobs.begin();
  V_CopyJob::iterator jobEnd = jobs.end();
  for ( ; jobItr != jobEnd; ++jobItr )
  {
    CopyJob& job ( *jobItr );

    // add the job to the work queue
    queue.Lock()->push( &job );
  }

  // create our worker threads
  for ( u32 i=0; i<g_ThreadCount; ++i )
  {
    threads.push_back( ::CreateThread( NULL, NULL, &CopyThread, &queue, NULL, NULL ) );
  }

  // spin until the queue is empty
  u32 lastModulus = 0;
  u32 lastCopiesLeft = 0;
  while ( 1 )
  {
    u32 size = (u32)queue.Lock()->size();
    if ( size == 0 )
    {
      break;
    }

    const u32 modulus = size > 1000 ? 1000 : ( size > 100 ) ? 100 : 10;
    if ( (lastCopiesLeft / modulus) != (size / modulus) || lastModulus != modulus )
    {
      u32 toGo = (size / modulus) * modulus;
      if ( toGo )
      {
        Console::Print("%d to go\n", toGo );
        lastCopiesLeft = size;
        lastModulus = modulus;
      }
    }

    Sleep( 100 );
  }

  // wait for our threads to finish (they will terminate when the queue is empty)
  ::WaitForMultipleObjects( g_ThreadCount, &threads.front(), TRUE, INFINITE );

  // close thread handles
  std::vector<HANDLE>::const_iterator threadItr = threads.begin();
  std::vector<HANDLE>::const_iterator threadEnd = threads.end();
  for ( ; threadItr != threadEnd; ++threadItr )
  {
    ::CloseHandle( *threadItr );
  }

  bool copied = !jobs.empty();
  jobItr = jobs.begin();
  jobEnd = jobs.end();
  for ( ; jobItr != jobEnd; ++jobItr )
  {
    CopyJob& job ( *jobItr );

    std::stringstream str;
    str << (toServer ? "[Upload]" : "[Download]") << " ";
    str << (job.m_Result == Results::Success ? "[Copied]" : "[Did not copy]");
    str << " " << job.m_Source << " --> " << job.m_Target;

    if ( !job.m_Result == Results::Success )
    {
      str << " (" << Results::Strings[job.m_Result] << ")";
    }

    str << std::endl;

    Console::Print( Console::Levels::Verbose, str.str().c_str() );

    copied &= job.m_Result == Results::Success;

    if ( job.m_Result == Results::Success )
    {
      stats->m_Size += job.m_FileInfo->m_Size;
      stats->m_Copied++;
    }
  }

  std::string copyType;
  if ( toServer )
  {
    if ( copied )
    {
      copyType = "Upload Success";
    }
    else
    {
      copyType = "MD5 Check";
    }
  }
  else
  {
    if ( copied )
    {
      copyType = "Download Success";
    }
    else
    {
      copyType = "Download Failure";
    }
  }

  f32 elapsed = timer.Elapsed();
  if ( !copyType.empty() )
  {
    Console::Profile( "[%s] of %d files took: %f ms\n", copyType.c_str(), files.size(), elapsed );
  }

  stats->m_Download = !toServer;
  stats->m_Duration = elapsed;
  stats->m_Speed = ( ( (f32) stats->m_Size / 1024 ) / ( elapsed / 1000.0f ) );
  stats->m_Count = (u32)files.size();

  Console::Print( Console::Levels::Verbose, "CCS: Total Files: %d  Copied: %d  Size: %s  Speed: %s/s\n", stats->m_Count, stats->m_Copied, Nocturnal::BytesToString( stats->m_Size ).c_str(), Nocturnal::BytesToString( (u64) stats->m_Speed * 1024 ).c_str() );

  f32 previousTotal = g_AverageThroughput * g_AverageThroughputSampleCount;
  f32 currentTotal = previousTotal + stats->m_Speed * 1000.f;
  g_AverageThroughput = currentTotal / (float)++g_AverageThroughputSampleCount;

  // send the stats to the database (during cleanup)
  g_TransferStats.push_back( stats );

  return copied;
}

void CacheFiles::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
#ifdef IG_CCS_STORE_PRODUCTION
    bool production = true;
#else
    bool production = false;
#endif

    if ( AppUtils::IsToolsBuilder() )
    {
      production = false;
    }

    // this won't touch g_ThreadCount if this var doesn't exist
    Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"CCS_THREAD_COUNT", g_ThreadCount );

    char* envVar = NOCTURNAL_STUDIO_PREFIX"CCS_STORE_PRODUCTION";
    if ( !production )
    {
      envVar = NOCTURNAL_STUDIO_PREFIX"CCS_STORE_NONPRODUCTION";
    }

    Nocturnal::GetEnvVar( envVar, g_CacheFilesPath );

    // legacy support, in case the environment variable is not set
    if ( g_CacheFilesPath.empty() )
    {
      Console::Warning( "%s is not defined in your environment, disabling sharing of cached built data\n", envVar );
      g_Disable = true;
    }
    else
    {
      // clean environment variable path
      FileSystem::CleanName( g_CacheFilesPath );
      FileSystem::GuaranteeSlash( g_CacheFilesPath );

      if ( Nocturnal::GetCmdLineFlag( "disable_cache_files" ) )
      {
        g_Disable = true;
      }
    }

    if ( g_Disable )
    {
      Console::Warning( "CCS is disabled\n" );
    }
    else
    {
      Console::Print( Console::Levels::Verbose, "CCS Enabled [ %s | %d threads ]\n", production ? "production" : "non-production", g_ThreadCount );
    }
  }
}

void CacheFiles::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    if ( !g_Disable )
    {
      CacheFileStats::Initialize();

      if ( CacheFileStats::IsConnected() )
      {
        CacheFileStats::BeginTransaction();

        bool success = true;

        V_TransferStats::iterator packageItr = g_TransferStats.begin();
        V_TransferStats::iterator packageEnd = g_TransferStats.end();
        for ( ; packageItr != packageEnd && success; ++packageItr )
        {
          const TransferStatsPtr& stats = *packageItr;
          success &= CacheFileStats::UpdateStats( stats->m_Download, stats->m_Size, stats->m_Count, stats->m_Speed, stats->m_Duration );
        }

        if ( success )
        {
          CacheFileStats::CommitTransaction();
          g_TransferStats.clear();
        }
        else
        {
          CacheFileStats::RollbackTransaction();
        }

        Console::Print( "CCS statistics sent\n" );
      }
      else
      {
        Console::Warning( "Could not connect to CCS statistics database\n" );
      }

      CacheFileStats::Cleanup();

      Console::Print( "CCS average throughput is %s/s (%d samples)\n", Nocturnal::BytesToString( (u64)g_AverageThroughput ).c_str(), g_AverageThroughputSampleCount );
    }
  }
}

const std::string& CacheFiles::GetCacheFilesPath()
{
  return g_CacheFilesPath;
}

std::string CacheFiles::GetOutputFilename( const Dependencies::DependencyInfoPtr& file )
{
  const std::string& signature = file->m_Signature;
  std::string subdir = signature.substr( 0, 2 );
  std::string serverFile = g_CacheFilesPath + subdir + '/' + signature + '/' + signature + ".file";
  FileSystem::CleanName( serverFile ); // make path lower case and fix slash directions
  return serverFile;
}

bool CacheFiles::Get( Dependencies::DependencyGraph& depGraph, const Dependencies::V_DependencyInfo& files )
{
  NOC_ASSERT( g_InitCount );
  return CopyFiles( depGraph, files, false );
}

bool CacheFiles::Get( Dependencies::DependencyGraph& depGraph, const Dependencies::FileInfoPtr& file )
{
  Dependencies::V_DependencyInfo files;
  files.push_back( file );
  return Get( depGraph, files );
}

void CacheFiles::Put( Dependencies::DependencyGraph& depGraph, const Dependencies::V_DependencyInfo& files )
{
  NOC_ASSERT( g_InitCount );
  CopyFiles( depGraph, files, true );
}

void CacheFiles::Put( Dependencies::DependencyGraph& depGraph, const Dependencies::FileInfoPtr& file )
{
  Dependencies::V_DependencyInfo files;
  files.push_back( file );
  return Put( depGraph, files );
}
