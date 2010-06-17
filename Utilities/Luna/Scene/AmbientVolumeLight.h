#pragma once

#include "Light.h"
#include "Pipeline/Content/Nodes/Lights/AmbientVolumeLight.h"

namespace Luna
{
  class PrimitiveSphere;

  class LUNA_SCENE_API AmbientVolumeLight : public Light
  {
    //
    // Members
    //
  protected:
    PrimitiveTemplate<Position>* m_VolumeLightInnerPrim;
    PrimitiveTemplate<Position>* m_VolumeLightOuterPrim;

    bool m_FirstEntry;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( AmbientVolumeLight, Light );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    AmbientVolumeLight(Scene* scene);
    AmbientVolumeLight(Scene* scene, Content::AmbientVolumeLight* light);
    virtual ~AmbientVolumeLight();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Initialize() NOC_OVERRIDE;
    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;
    virtual void SetScale( const Math::Scale& value );



    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

    float GetInnerRadius() const;
    void SetInnerRadius( float radius );

    float GetOuterRadius() const;
    void SetOuterRadius( float radius );    

    i32 GetVolumeLightType() const;
    void SetVolumeLightType( i32 type );

  private:

    void SwitchVolumeLightType( int type );
  };
}
