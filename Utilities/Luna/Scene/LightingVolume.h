#pragma once

#include "Instance.h"

#include "Attribute/AttributeHandle.h"
#include "Content/LightingVolume.h"

namespace Luna
{
  class   PrimitiveLocator;
  class   LightingVolume;
  struct  LoadArgs;
  class Light;
  typedef std::vector< Light* > V_LightDumbPtr;

  struct LightingVolumeChangeArgs
  {
    Luna::LightingVolume* m_LightingVolume;

    LightingVolumeChangeArgs( Luna::LightingVolume* volume )
      : m_LightingVolume( volume )
    {}
  };
  typedef Nocturnal::Signature< void, const LightingVolumeChangeArgs& > LightingVolumeChangeSignature;

  class LightingVolume : public Luna::Instance
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::LightingVolume, Luna::Instance );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    LightingVolume(Luna::Scene* scene);
    LightingVolume(Luna::Scene* scene, Content::LightingVolume* volume);

  public:
    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
    virtual SceneNodeTypePtr CreateNodeType( Luna::Scene* scene ) const NOC_OVERRIDE;

    // directional management
    bool ChildParentChanging( const ParentChangingArgs& args );

    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;

    static void CreatePanel( CreatePanelArgs& args );

    bool GetExcludeGlobalLights() const; 
    void SetExcludeGlobalLights( bool exclude );

    virtual void PopulateManifest( Asset::SceneManifest* manifest ) const NOC_OVERRIDE;

    void GetLinkedLights( V_LightDumbPtr& lights );
    bool IsLinkableLight( Light* light );

  public:
    ///////////////////////////////////////////////////////////////////////////
    // Returns true if the attribute specified by the template parameter is in
    // this collection.
    // 
    template < class T >
    bool GetOverride() const
    {
      const Content::LightingVolume* pkg = GetPackage< Content::LightingVolume >();
      return ( pkg->GetAttribute( Reflect::GetType< T >() ).ReferencesObject() );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Adds or removes the specified attribute from this collection.
    // 
    template < class T >
    void SetOverride( bool enable )
    {
      Content::LightingVolume* pkg = GetPackage< Content::LightingVolume >();
      if ( enable )
      {
        // This will create a new attribute or enable an existing one.
        Attribute::AttributeEditor< T > editor( pkg );
        editor.Commit();
      }
      else
      {
        pkg->RemoveAttribute( Reflect::GetType< T >() );
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Adds or removes the specified attribute from this collection.  If the 
    // attribute is being added, it's settings are populated with values from
    // the same attribute if it is found on the currently loaded level file.
    // 
    template < class T >
    void SetOverrideLevelAttribute( bool enable )
    {
      Content::LightingVolume* pkg = GetPackage< Content::LightingVolume >();
      if ( enable )
      {
        // This will create a new attribute or enable an existing one.
        Attribute::AttributeEditor< T > editor( pkg );

        // Attempt to copy any existing settings from the same attribute found
        // on the level.
        Asset::LevelAsset* level = m_Scene->GetManager()->GetCurrentLevel();
        if ( level )
        {
          Attribute::AttributePtr levelAttr = level->GetAttribute( Reflect::GetType< T >() );
          if ( levelAttr.ReferencesObject() )
          {
            T* src = Reflect::AssertCast< T >( levelAttr.Ptr() );
            T* dst = editor.operator->();
            src->CopyTo( dst );
          }
        }

        editor.Commit();
      }
      else
      {
        pkg->RemoveAttribute( Reflect::GetType< T >() );
      }
    }

    //
    // Events
    //
  protected:
    LightingVolumeChangeSignature::Event m_Changed;
  public:
    void AddChangedListener( const LightingVolumeChangeSignature::Delegate& listener )
    {
      m_Changed.Add( listener );
    }

    void RemoveChangedListener( const LightingVolumeChangeSignature::Delegate& listener )
    {
      m_Changed.Remove( listener );
    }

    virtual void PackageChanged( const Reflect::ElementChangeArgs& args ) NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr< Luna::LightingVolume > LightingVolumePtr;
  typedef std::vector< Luna::LightingVolume* > V_LightingVolumeDumbPtr;
}
