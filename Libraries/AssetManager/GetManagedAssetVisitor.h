#pragma once

#include "API.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetFlags.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "File/Manager.h"
#include "Reflect/Field.h"
#include "Reflect/Version.h"

namespace AssetManager 
{
  //
  // Forwards
  //  
  class ManagedAsset;
  class ManagedAssetFile;
  class ManagedFolder;

  /////////////////////////////////////////////////////////////////////////////
  class GetManagedAssetVisitor : public Reflect::Visitor
  {
  public:
    ManagedAsset*   m_ManagedAsset;
    u32             m_AssetFlags;

    GetManagedAssetVisitor( ManagedAsset* managedAsset, u32 assetFlags = (u32) ( Reflect::FieldFlags::FileID | Asset::AssetFlags::ManageField ) );

    virtual ~GetManagedAssetVisitor()
    {
    }

    virtual bool VisitField( Reflect::Element* element, const Reflect::Field* field ) NOC_OVERRIDE;

  protected:
    virtual void GetManagedAssetFile( const Reflect::ElementPtr& element, const Reflect::Field* field, tuid id, ManagedAssetFile* managedAssetFile );

    void ProcessIdField( const Reflect::ElementPtr& element, const Reflect::Field* field, tuid id );
    void BestGuessNewAssetPath( ManagedAsset* managedAsset, ManagedAssetFile* managedAssetFile, bool addDirectory );
  };


} // namespace AssetManager
