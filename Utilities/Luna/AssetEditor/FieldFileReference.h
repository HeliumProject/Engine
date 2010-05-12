#pragma once

#include "FieldNode.h"

// Forwards
namespace UIToolKit
{
  struct PreferenceChangedArgs;
}

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Node that represents a file path, connected to a member variable on
  // an element.
  // 
  class FieldFileReference : public Luna::FieldNode
  {
  protected:
    tuid m_FileID;
    bool m_UseLabelPrefix;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::FieldFileReference, Luna::FieldNode );
    static void InitializeType();
    static void CleanupType();

  public:
    FieldFileReference( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field, const tuid& fileID );
    virtual ~FieldFileReference();

    void SetUseLabelPrefix( bool useLabelPrefix );
    virtual tuid GetFileID() const;
    virtual void ActivateItem() NOC_OVERRIDE;

  protected:
    std::string MakeLabel() const;
    void PreferenceChanged( const Reflect::ElementChangeArgs& args );
  };

  typedef Nocturnal::SmartPtr< Luna::FieldFileReference > FieldFileReferencePtr;
}
