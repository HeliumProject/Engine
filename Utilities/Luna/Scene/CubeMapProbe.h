#pragma once

#include "Instance.h"

#include "Content/CubeMapProbe.h"

namespace Luna
{
  class PrimitiveLocator;

  class CubeMapProbe : public Luna::Instance
  {
   
  private:

    mutable std::string m_CubeMapAssetPath;

    //
    // Runtime Type Info
    //
  public:
    LUNA_DECLARE_TYPE( Luna::CubeMapProbe, Luna::Instance );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    CubeMapProbe(Luna::Scene* scene);
    CubeMapProbe(Luna::Scene* scene, Content::CubeMapProbe* sky);

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;

    static void CreatePanel( CreatePanelArgs& args );

    const std::string& GetCubeMapAsset() const;
    void SetCubeMapAsset( const std::string& assetPath );

  };

  typedef Nocturnal::SmartPtr< Luna::CubeMapProbe > CubeMapProbePtr;
  typedef std::vector< CubeMapProbePtr > V_CubeMapProbe;
}

