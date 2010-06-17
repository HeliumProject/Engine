#pragma once

#include "Instance.h"
#include "Pipeline/Content/Nodes/Lights/Light.h"
#include "Core/Enumerator.h"
#include "Foundation/Container/BitArray.h"
#include "Primitive.h"

namespace Luna
{

  class SelectionDataObject : public Reflect::Object
  {
  public:

    OS_SelectableDumbPtr m_Selection;

    SelectionDataObject( const OS_SelectableDumbPtr& selection )
      : m_Selection( selection)
    {
    }

  };

  class PrimitiveLocator;
  class Light;
  class PrimitiveSphere;

  namespace LightSelectionChoices
  {
    enum LightSelectionChoice
    {
      RenderType = 0,
      Color      = 1,
      Intensity  = 2,
      Scale      = 3,
      LensFlare  = 4,
      PhysicalLight = 5,
      LightType    = 6,
      NumLightSelectionChoices,
    };
  };
  typedef LightSelectionChoices::LightSelectionChoice LightSelectionChoice;

  struct LightChangeArgs
  {
    Luna::Light* m_Light;

    LightChangeArgs( Luna::Light* light )
      : m_Light( light )
    {}
  };
  typedef Nocturnal::Signature< void, const LightChangeArgs& > LightChangeSignature;

  struct RealtimeLightExistenceArgs
  {
    Luna::Light* m_Light;
    bool    m_Added;

    RealtimeLightExistenceArgs( Luna::Light* light, bool added )
      : m_Light( light )
      , m_Added( added )
    {}
  };
  typedef Nocturnal::Signature< void, const RealtimeLightExistenceArgs& > RealtimeLightExistenceSignature;
  
  class LUNA_SCENE_API Light NOC_ABSTRACT : public Luna::Instance
  {
    //
    // Members
    //
  
  protected:
    Content::AreaLightType m_AreaLightType;
    //PrimitiveSphere* m_AreaLightSphere;
    PrimitiveTemplate<Position>* m_AreaLightPrim;


    BitArray m_SelectionHelper;

  public:
    static D3DMATERIAL9 s_Material;

    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Light, Luna::Instance );
    static void InitializeType();
    static void CleanupType();

    virtual void Initialize() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;

    //
    // Member functions
    //

    Light(Luna::Scene* scene, Content::Light* light);
    
    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

    Math::Color3 GetColor() const;
    void SetColor( Math::Color3 color );

    float GetIntensity() const;
    void SetIntensity( float intensity );

    f32 GetAreaLightRadius() const;
    void SetAreaLightRadius( f32 radius );

    Math::Vector2 GetAreaLightDimensions() const;
    void SetAreaLightDimensions( Math::Vector2 dim );

    f32 GetAreaLightSamplesPerMeter() const;
    void SetAreaLightSamplesPerMeter( f32 samples );

    i32 GetAreaLightType() const;
    void SetAreaLightType( i32 type );

    i32 GetRenderType() const;
    void SetRenderType( i32 renderType );

    i32 GetLensFlareType() const;
    void SetLensFlareType( i32 type );

    float GetDrawDist() const;
    void SetDrawDist( float distance );

    bool GetKillIfInactive() const;
    void SetKillIfInactive( bool b );

    bool GetVisibilityRayTest() const;
    void SetVisibilityRayTest( bool b );

    bool GetCastsShadows() const;
    void SetCastsShadows( bool b );

    bool GetAttenuate() const;
    void SetAttenuate( bool b );

    f32 GetShadowFactor() const;
    void SetShadowFactor( f32 radius );

    bool GetAllowOversized() const;
    void SetAllowOversized( bool b );

    bool GetEmitPhotons() const;
    void SetEmitPhotons( bool b );

    u32 GetNumPhotons() const;
    void SetNumPhotons( u32 numPhotons );

    Math::Color3 GetPhotonColor() const;
    void SetPhotonColor( Math::Color3 color );

    float GetPhotonIntensity() const;
    void SetPhotonIntensity( float intensity );

    const Content::V_ParametricKeyPtr& GetColorAnimation() const;
    void SetColorAnimation( const Content::V_ParametricKeyPtr& animation );

    const Content::V_ParametricKeyPtr& GetIntensityAnimation() const;
    void SetIntensityAnimation( const Content::V_ParametricKeyPtr& animation );

    f32 GetAnimationDuration() const;
    void SetAnimationDuration( f32 duration );
    
    bool GetRandomAnimOffset() const;
    void SetRandomAnimOffset( bool b );

    bool GetPhysicalLight() const;
    void SetPhysicalLight( bool b );

    bool GetSelectionHelperRenderType() const;
    void SetSelectionHelperRenderType( bool b );

    bool GetSelectionHelperColor() const;
    void SetSelectionHelperColor( bool b );

    bool GetSelectionHelperIntensity() const;
    void SetSelectionHelperIntensity( bool b );

    bool GetSelectionHelperScale() const;
    void SetSelectionHelperScale( bool b );

    bool GetSelectionHelperLensFlare() const;
    void SetSelectionHelperLensFlare( bool b );

    bool GetSelectionHelperPhysicalLight() const;
    void SetSelectionHelperPhysicalLight( bool b );

    ///////////////////////////////////////////////////////////////////////////
    // Returns true if the attribute specified by the template parameter is in
    // this collection.
    // 
    template < class T >
    bool GetOverride() const
    {
      const Content::Light* pkg = GetPackage< Content::Light >();
      return ( pkg->GetAttribute( Reflect::GetType< T >() ).ReferencesObject() );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Adds or removes the specified attribute from this collection.
    // 
    template < class T >
    void SetOverride( bool enable )
    {
      Content::Light* pkg = GetPackage< Content::Light >();
      if ( enable )
      {
        // This will create a new attribute or enable an existing one.
        Component::ComponentEditor< T > editor( pkg );
        editor.Commit();
      }
      else
      {
        pkg->RemoveAttribute( Reflect::GetType< T >() );
      }
    }


    static void OnSelectionHelper( Inspect::Button* button );


    //
    // Events
    //
  protected:
    LightChangeSignature::Event m_Changed;
  public:
    void AddChangedListener( const LightChangeSignature::Delegate& listener )
    {
      m_Changed.Add( listener );
    }

    void RemoveChangedListener( const LightChangeSignature::Delegate& listener )
    {
      m_Changed.Remove( listener );
    }

  protected:
    RealtimeLightExistenceSignature::Event m_RealtimeExistence;
  public:
    void AddRealtimeLightExistenceListener( const RealtimeLightExistenceSignature::Delegate& listener )
    {
      m_RealtimeExistence.Add( listener );
    }

    void RemoveRealtimeLightExistenceListener( const RealtimeLightExistenceSignature::Delegate& listener )
    {
      m_RealtimeExistence.Remove( listener );
    }
  };
  typedef Nocturnal::SmartPtr<Luna::Light> LightPtr;
  typedef std::vector<Luna::Light*> V_LightDumbPtr;
}
