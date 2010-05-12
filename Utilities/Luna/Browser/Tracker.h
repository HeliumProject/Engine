#pragma once

#include "API.h"
#include "AssetDB.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetFlags.h"

#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"

#include "File/Manager.h"

#include "reflect/Field.h"

#include "TUID/TUID.h"


namespace Asset
{
  class ASSETTRACKER_API AssetTracker : public Nocturnal::RefCountBase< AssetTracker >
  {
  public:
    AssetTracker();
    ~AssetTracker();

    bool Initialize();
    void Cleanup();

    bool TrackFile( const tuid id );
    bool TrackFile( const File::ManagedFilePtr& file );
    bool TrackFiles( const File::V_ManagedFilePtr& files );
    bool TrackAssetFile( const File::ManagedFilePtr& file, M_AssetFiles* assetFiles );

    void TrackEverything();

  private:
    AssetDBPtr m_AssetDB;
  };

  typedef ASSETTRACKER_API Nocturnal::SmartPtr< AssetTracker > AssetTrackerPtr;
}
