#pragma once

#include "Pipeline/API.h"

#include "AssetFlags.h"

#include "Attribute/AttributeCollection.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Reflect/Element.h"
#include "Reflect/Class.h"
#include "Reflect/Serializers.h"

namespace Finder
{
  class FinderSpec;
}

namespace Asset
{
  ///////////////////////////////////////////////////////////////////////////
  // This class is the data representation of a template for creating an
  // asset.  It contains all the information needed by the UI for creating
  // a wizard from this template. Each asset class may have multiple templates.
  // 
  class PIPELINE_API AssetTemplate : public Reflect::Element
  {
  public:
    std::string   m_Name;               // was m_Title
    std::string   m_Description;
    std::string   m_ModifierSpec;

    bool          m_DefaultAddSubDir;
    bool          m_ShowSubDirCheckbox;
    std::string   m_DefaultFormat;

    Attribute::AttributeCollectionPtr m_RequiredAttributes;
    Attribute::AttributeCollectionPtr m_OptionalAttributes;

  public:
    AssetTemplate( const Reflect::Composite* composite = NULL );
    virtual ~AssetTemplate();

    bool AddRequiredAttribute( const Attribute::AttributePtr& attribute );
    bool AddRequiredAttribute( const i32 typeID );
    const Attribute::M_Attribute& GetRequiredAttributes() const;

    bool AddOptionalAttribute( const Attribute::AttributePtr& attribute );
    bool AddOptionalAttribute( const i32 typeID );
    const Attribute::M_Attribute& GetOptionalAttributes() const;

    static void GetAssetTemplates( const i32 typeID, Reflect::V_Element& assetTemplates );
  public:
    REFLECT_DECLARE_CLASS( AssetTemplate, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<AssetTemplate>& comp );
  };
  typedef Nocturnal::SmartPtr< AssetTemplate > AssetTemplatePtr;
  typedef std::vector< AssetTemplatePtr > V_AssetTemplate;

}
