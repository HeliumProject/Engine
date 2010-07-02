#pragma once

#include "Pipeline/API.h"
#include "Foundation/Reflect/Serializers.h"
#include "Foundation/Component/ComponentCollection.h"
#include "Foundation/TUID.h"


namespace Content
{
  class ContentVisitor;
  class Scene;

  //
  // This is the base class of all nodes in the XML hierarchy.
  //  It encapsulates the ID (TUID/Uuid) of each SceneNode.
  //

  class PIPELINE_API SceneNode : public Component::ComponentCollection
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    // The ID of the node
    Nocturnal::TUID m_ID;

    // A generated name of the node
    tstring m_DefaultName;

    // A user created name (can be empty)
    tstring m_GivenName;

    // Should the name change when the object does?
    bool m_UseGivenName;

    // utility
    bool m_Selected;

    SceneNode ()
      : m_ID (Nocturnal::TUID::Generate())
      , m_Selected( false )
      , m_UseGivenName( false )
    {

    }

    SceneNode (const Nocturnal::TUID& id)
      : m_ID (id)
      , m_Selected( false )
      , m_UseGivenName( false )
    {

    }

    REFLECT_DECLARE_ABSTRACT(SceneNode, Component::ComponentCollection);

    static void EnumerateClass( Reflect::Compositor<SceneNode>& comp );

    // Called after scene is loaded; can be used to fix up legacy data. Add newly created nodes to elements.
    virtual void PostLoad( Reflect::V_Element& elements );

    // migrate m_Name
    virtual bool ProcessComponent( Reflect::ElementPtr element, const tstring& fieldName ) NOC_OVERRIDE;

    // invalidates class attributes
    virtual bool ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const NOC_OVERRIDE;

    const tstring& GetName() const
    {
      return (!m_GivenName.empty() && m_UseGivenName) ? m_GivenName : m_DefaultName;
    }
  };

  typedef Nocturnal::SmartPtr<SceneNode> SceneNodePtr;
  typedef std::vector<SceneNodePtr> V_SceneNode;
}