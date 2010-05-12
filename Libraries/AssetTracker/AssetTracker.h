#pragma once

#include "API.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetFlags.h"

#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "File/Manager.h"
#include "reflect/Field.h"
#include "TUID/TUID.h"

namespace Asset
{
  //
  // Forwards
  //
  class AssetDB;
  typedef FILE_API Nocturnal::SmartPtr< AssetDB > AssetDBPtr;

  class AssetFile;
  typedef Nocturnal::SmartPtr< AssetFile > AssetFilePtr;
  typedef std::map< tuid, AssetFilePtr > M_AssetFiles;

  class AssetVisitor;

  struct AssetTrackerArgs
  {
    bool m_IsTracking;
    
    AssetTrackerArgs( bool isTracking )
      : m_IsTracking( isTracking )
    {
    }
  };
  typedef Nocturnal::Signature< void, const AssetTrackerArgs& > AssetTrackerSignature;

  /////////////////////////////////////////////////////////////////////////////
  class ASSETTRACKER_API AssetTracker : public Nocturnal::RefCountBase< AssetTracker >
  {
  public:
    ~AssetTracker();

    static void Initialize();
    static void Cleanup();

    static AssetTracker* GetGlobalTracker();

    static void StopCurrentThread();


    bool TrackFile( const tuid id );
    bool TrackFile( const File::ManagedFilePtr& file );
    bool TrackFiles( const File::V_ManagedFilePtr& files );
    bool TrackAssetFile( const File::ManagedFilePtr& file, M_AssetFiles* assetFiles );
    void TrackEverything();

    bool IsTracking() const;
    void StartThread();
    void StopThread();

  public:
    // Listener functions
    void OnGetAssets( Nocturnal::Void ); 
    void OnFileUpdated( const Reflect::FileAccessArgs& args ); 

  private:
    // AssetTracker is a singleton; Hide the ctor, copy ctor and assignment operator
    AssetTracker();
    AssetTracker( const AssetTracker& rhs );
    AssetTracker& operator=( const AssetTracker& rhs );

    // Thread entry points
    static DWORD WINAPI TrackEverythingThread(LPVOID pvoid);
    static DWORD WINAPI TrackFileThread(LPVOID pvoid);

  public:
    // helper functions

  private:
    AssetDBPtr            m_AssetDB;
   
    HANDLE                m_Thread;
    DWORD                 m_ThreadID;
    bool                  m_StopTracking;

    M_AssetFiles          m_AssetFiles;

  private:
    AssetTrackerSignature::Event m_Tracking;
  public:
    void AddTrackingListener( const AssetTrackerSignature::Delegate& listener )
    {
      m_Tracking.Add( listener );
    }
    void RemoveTrackingListener( const AssetTrackerSignature::Delegate& listener )
    {
      m_Tracking.Remove( listener );
    }
  };

  typedef ASSETTRACKER_API Nocturnal::SmartPtr< AssetTracker > AssetTrackerPtr;
}
