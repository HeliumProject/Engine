#pragma once

#include "HierarchyNode.h"

#include "Pipeline/Content/Nodes/Curve/Point.h"
#include "Core/Manipulator.h"

namespace Luna
{
  class Point : public Luna::HierarchyNode
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Point, Luna::HierarchyNode );
    static void InitializeType();
    static void CleanupType();


    // 
    // Member functions
    // 

  public:
    Point(Luna::Scene* scene, Content::Point* data);
    virtual ~Point();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual tstring GetApplicationTypeName() const NOC_OVERRIDE;

    const Math::Vector3& GetPosition() const;
    void SetPosition( const Math::Vector3& value );

    f32 GetPositionX() const;
    void SetPositionX( f32 value );

    f32 GetPositionY() const;
    void SetPositionY( f32 value );

    f32 GetPositionZ() const;
    void SetPositionZ( f32 value );

    virtual void ConnectManipulator( ManiuplatorAdapterCollection* collection ) NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;
    virtual void Evaluate( GraphDirection direction ) NOC_OVERRIDE;
    virtual bool ValidatePanel( const tstring& name ) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );
  };

  class PointTranslateManipulatorAdapter : public TranslateManipulatorAdapter
  {
  protected:
    Luna::Point* m_Point;

  public:
    PointTranslateManipulatorAdapter( Luna::Point* pointComponent )
      :m_Point( pointComponent )
    {
      NOC_ASSERT( m_Point );
    }

    virtual Luna::HierarchyNode* GetNode() override
    {
      return m_Point;
    }

    virtual Math::Matrix4 GetFrame(ManipulatorSpace space) override;
    virtual Math::Matrix4 GetObjectMatrix() override;
    virtual Math::Matrix4 GetParentMatrix() override;

    virtual Math::Vector3 GetPivot() override
    {
      return m_Point->GetPosition();
    }

    virtual Math::Vector3 GetValue() override
    {
      return m_Point->GetPosition();
    }
  
    virtual Undo::CommandPtr SetValue( const Math::Vector3& v ) override
    {
      return new Undo::PropertyCommand<Math::Vector3> ( new Nocturnal::MemberProperty<Luna::Point, Math::Vector3> (m_Point, &Luna::Point::GetPosition, &Luna::Point::SetPosition), v);
    }
  };

  typedef Nocturnal::SmartPtr<Luna::Point> LPointPtr;
  typedef std::vector<LPointPtr> V_LPoint;
}
