#pragma once

#include "Luna/API.h"
#include "Editor/Document.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Luna
{
  class AssetClass;
  typedef Nocturnal::SmartPtr< Luna::AssetClass > AssetClassPtr;

  class LFrame;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for files edited by the asset editor.  Handles RCS prompts (in the
  // base class) and stores a pointer to the asset class that this file is associated
  // with.
  // 
  class AssetDocument : public Document
  {
  private:
    Luna::AssetClassPtr m_AssetClass;

    // RTTI
    LUNA_DECLARE_TYPE( AssetDocument, Document );
    static void InitializeType();
    static void CleanupType();

  public:
    AssetDocument( Luna::AssetClass* asset );
    virtual ~AssetDocument();
    Luna::AssetClass* GetAssetClass() const;
  };

  typedef Nocturnal::SmartPtr< AssetDocument > AssetDocumentPtr;
}