#pragma once

#include "Pick.h"

#include "SceneNodeType.h"

namespace Editor
{
  class PickHit;
  typedef Helium::SmartPtr< PickHit > PickHitPtr;
  typedef std::vector< PickHitPtr > V_PickHitSmartPtr;

  class HierarchyNodeType : public Editor::SceneNodeType
  {
    // 
    // Member variables
    //

  protected:
    // members
    bool m_Visible;
    bool m_Selectable;

  public:
    // materials
    D3DMATERIAL9 m_WireMaterial;
    D3DMATERIAL9 m_SolidMaterial;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( HierarchyNodeType, Editor::SceneNodeType );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    HierarchyNodeType( Editor::Scene* scene, i32 instanceType );

    virtual void Create();
    virtual void Delete();

    bool IsVisible() const;
    void SetVisible(bool value);

    bool IsSelectable() const;
    void SetSelectable( bool value );

    virtual bool IsTransparent();
  };

  typedef std::map< tstring, HierarchyNodeType* > M_HierarchyNodeTypeDumbPtr;
}
