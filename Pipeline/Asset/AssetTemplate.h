#pragma once

#include "Pipeline/API.h"

#include "AssetFlags.h"

#include "Pipeline/Component/ComponentCollection.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Serializers.h"

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
    tstring   m_Name;               // was m_Title
    tstring   m_Description;
    tstring   m_FileFilter;

    bool          m_DefaultAddSubDir;
    bool          m_ShowSubDirCheckbox;
    tstring   m_DefaultFormat;

    Component::ComponentCollectionPtr m_RequiredComponents;
    Component::ComponentCollectionPtr m_OptionalComponents;

  public:
    AssetTemplate( const Reflect::Composite* composite = NULL );
    virtual ~AssetTemplate();

    bool AddRequiredComponent( const Component::ComponentPtr& attribute );
    bool AddRequiredComponent( const i32 typeID );
    const Component::M_Component& GetRequiredComponents() const;

    bool AddOptionalComponent( const Component::ComponentPtr& attribute );
    bool AddOptionalComponent( const i32 typeID );
    const Component::M_Component& GetOptionalComponents() const;

    static void GetAssetTemplates( const i32 typeID, Reflect::V_Element& assetTemplates );
  public:
    REFLECT_DECLARE_CLASS( AssetTemplate, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<AssetTemplate>& comp );
  };
  typedef Nocturnal::SmartPtr< AssetTemplate > AssetTemplatePtr;
  typedef std::vector< AssetTemplatePtr > V_AssetTemplate;

}
