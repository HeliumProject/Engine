#pragma once

#include "Content/Layer.h"
#include "Content/NodeVisibility.h"

#include "Luna/API.h"
#include "SceneNode.h"

namespace Luna
{
  class Layer : public Luna::SceneNode
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Layer, Luna::SceneNode );
    static void InitializeType();
    static void CleanupType();

  public:
    Layer( Luna::Scene* scene, Content::Layer* layer );
    virtual ~Layer();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Initialize() NOC_OVERRIDE;
    virtual void Pack() NOC_OVERRIDE;

    bool IsVisible() const NOC_OVERRIDE;
    void SetVisible( bool visible );

    bool IsSelectable() const;
    void SetSelectable( bool selectable );

    const Math::Color3& GetColor() const;
    void SetColor( const Math::Color3& color );

    OS_SelectableDumbPtr GetMembers();
    bool ContainsMember( Luna::SceneNode* node ) const;

    virtual void Prune( V_SceneNodeDumbPtr& prunedNodes ) NOC_OVERRIDE;
    virtual void Insert(SceneGraph* g, V_SceneNodeDumbPtr& insertedNodes ) NOC_OVERRIDE;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;

  private:
    static void CreatePanel( CreatePanelArgs& args );
    static void BuildUnionAndIntersection( Enumerator* enumerator, const OS_SelectableDumbPtr& selection, std::string& unionStr, std::string& intersectionStr );

    Content::NodeVisibilityPtr m_VisibilityData; 
  };

  typedef Nocturnal::SmartPtr< Luna::Layer > LayerPtr;
  typedef std::vector< Luna::Layer* > V_LayerDumbPtr;
}
