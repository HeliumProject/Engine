#include "Precompile.h"
#include "LightCreateTool.h"

#include "SceneManager.h"
#include "Mesh.h"
#include "Scene.h"
#include "Light.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "ShadowDirection.h"
#include "SunLight.h"
#include "PortalLight.h"
#include "CubeMapProbe.h"
#include "AmbientLight.h"
#include "AmbientVolumeLight.h"

#include "LightingVolume.h"
#include "LightingEnvironment.h"

#include "Pick.h"

#include "Asset/CubeMapAsset.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"

#include "Content/SunShadowMergeAttribute.h"
#include "Content/GlossControlAttribute.h"
#include "Content/GroundLightAttribute.h"

#include "Asset/CubeMapAttribute.h"

using namespace Luna;

Content::LightType LightCreateTool::s_Type = Content::LightTypes::Point;

LUNA_DEFINE_TYPE(Luna::LightCreateTool);

void LightCreateTool::InitializeType()
{
  Reflect::RegisterClass< Luna::LightCreateTool >( "Luna::LightCreateTool" );
}

void LightCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::LightCreateTool >();
}

LightCreateTool::LightCreateTool(Luna::Scene* scene, Enumerator* enumerator)
: Luna::CreateTool (scene, enumerator)
{

}

LightCreateTool::~LightCreateTool()
{

}

Luna::TransformPtr LightCreateTool::CreateNode()
{
  switch(s_Type)
  {
  case Content::LightTypes::Point:
    {
      return new Luna::PointLight ( m_Scene, new Content::PointLight() );
    }

  case Content::LightTypes::Spot:
    {
      return new SpotLight( m_Scene, new Content::SpotLight() );
    }

  case Content::LightTypes::Directional:
    {
      return new Luna::DirectionalLight( m_Scene, new Content::DirectionalLight() );
    }

  case Content::LightTypes::SunShadow:
    {
      return new Luna::ShadowDirection( m_Scene, new Content::ShadowDirection() );
    }

  case Content::LightTypes::Sun:
    {
      return new SunLight( m_Scene, new Content::SunLight() );
    }

  case Content::LightTypes::LightingVolume:
    {
      Content::LightingVolumePtr pkg = new Content::LightingVolume();

      //Shadow Merge
      Attribute::AttributeEditor< Content::SunShadowMergeAttribute >    shadowMerge( pkg );

      //Gloss Control
      Attribute::AttributeEditor< Content::GlossControlAttribute >      glossControl( pkg );

      //Ground Light
      Attribute::AttributeEditor< Content::GroundLightAttribute >       groundLight( pkg );

      //Rim Light
//      Attribute::AttributeEditor< Content::RimLightAttribute >          rimLight( pkg );

      //CubeMap, only for local volumes (the global one is set on the level attributes)
      Attribute::AttributeEditor< Asset::CubeMapAttribute >             cubemap( pkg );

      // this is sort of a hack.  we assume that any lighting volume in the world
      // file (root scene) is a global volume and leave its shadow attributes enabled
      if(m_Scene->GetManager()->GetRootScene() != m_Scene)
      {
        shadowMerge->m_Enabled    = false;
        glossControl->m_Enabled   = false;
        groundLight->m_Enabled    = false;
//        rimLight->m_Enabled       = false;
        cubemap->m_Enabled        = false;
      }

      return new Luna::LightingVolume( m_Scene, pkg );
    }

  case Content::LightTypes::LightingEnvironment:
    {
      return new Luna::LightingEnvironment( m_Scene, new Content::LightingEnvironment() );
    }
  case Content::LightTypes::Portal:
    {
      return new Luna::PortalLight( m_Scene, new Content::PortalLight() );
    }
  case Content::LightTypes::AmbientDirectionals:
    {
      LPivotTransformPtr groupNode = new Luna::PivotTransform( m_Scene );

      m_Scene->AddObject( groupNode );

      Luna::DirectionalLight* light = new Luna::DirectionalLight( m_Scene, new Content::DirectionalLight() );
      light->SetName( "ambientDirectional" );
      light->SetParent( groupNode );
      m_Scene->AddObject( light );

      Math::EulerAngles rotation( 0.0f, Math::HalfPi, 0.0f );

      light = new Luna::DirectionalLight( m_Scene, new Content::DirectionalLight() );
      light->SetName( "ambientDirectional" );
      light->SetParent( groupNode );
      m_Scene->AddObject( light );

      light->SetRotate( rotation );
      light->GetPackage< Content::DirectionalLight >()->m_CastShadows = false;

      light = new Luna::DirectionalLight( m_Scene, new Content::DirectionalLight() );
      light->SetName( "ambientDirectional" );
      light->SetParent( groupNode );
      m_Scene->AddObject( light );

      rotation.angles.x = 0.0f;
      rotation.angles.y = Math::Pi;
      rotation.angles.z = 0.0f;
      light->SetRotate( rotation );
      light->GetPackage< Content::DirectionalLight >()->m_CastShadows = false;

      light = new Luna::DirectionalLight( m_Scene, new Content::DirectionalLight() );
      light->SetName( "ambientDirectional" );
      light->SetParent( groupNode );
      m_Scene->AddObject( light );

      rotation.angles.x = 0.0f;
      rotation.angles.y = Math::Pi + Math::HalfPi;
      rotation.angles.z = 0.0f;
      light->SetRotate( rotation );
      light->GetPackage< Content::DirectionalLight >()->m_CastShadows = false;

      light = new Luna::DirectionalLight( m_Scene, new Content::DirectionalLight() );
      light->SetName( "ambientDirectional" );
      light->SetParent( groupNode );
      m_Scene->AddObject( light );

      rotation.angles.x = Math::HalfPi;
      rotation.angles.y = 0.0f;
      rotation.angles.z = 0.0f;
      light->SetRotate( rotation );
      light->GetPackage< Content::DirectionalLight >()->m_CastShadows = false;

      light = new Luna::DirectionalLight( m_Scene, new Content::DirectionalLight() );
      light->SetName( "ambientDirectional" );
      light->SetParent( groupNode );
      m_Scene->AddObject( light );

      rotation.angles.x = -Math::HalfPi;
      rotation.angles.y = 0.0f;
      rotation.angles.z = 0.0f;
      light->SetRotate( rotation );
      light->GetPackage< Content::DirectionalLight >()->m_CastShadows = false;

      m_Scene->RemoveObject( groupNode );

      return groupNode;
    }
  case Content::LightTypes::CubeMapProbe:
    {
      Content::CubeMapProbe* probe = new Content::CubeMapProbe();

      std::string path;

      tuid levelID = m_Scene->GetManager()->GetCurrentLevel()->GetFileID();
      if( levelID != TUID::Null )
      {
        // try to put the new cubemap asset in the level's cubemap folder, but if you
        // can't get the level, don't worry about it...put it in the project's cubemap folder
        try
        {
          path = File::GlobalManager().GetPath( levelID );
          FileSystem::StripLeaf( path );
          path += "CubeMaps/";
        }
        catch( Nocturnal::Exception& )
        {
          path = FinderSpecs::Asset::CUBEMAP_FOLDER.GetFolder();
        }
      }
      else
      {
        path = FinderSpecs::Asset::CUBEMAP_FOLDER.GetFolder();
      }

      std::stringstream stream;
      stream << probe->m_ID;

      path += stream.str();

      tuid cubeMapID = File::GlobalManager().Open( path + FinderSpecs::Asset::CUBEMAP_DECORATION.GetDecoration() );

      tuid hdrMapID = File::GlobalManager().Open( path + ".hdr" );

      if( cubeMapID != TUID::Null )
      {
        Asset::CubeMapAssetPtr cubeMapClass = new Asset::CubeMapAsset();
        cubeMapClass->m_AssetClassID = cubeMapID;
        cubeMapClass->m_FileID = hdrMapID;
        cubeMapClass->Serialize();
        probe->m_CubeMapID = cubeMapID;
      }
      return new Luna::CubeMapProbe( m_Scene, probe );
    }
    case Content::LightTypes::Ambient:
    {
      return new Luna::AmbientLight( m_Scene, new Content::AmbientLight() );
    }
    case Content::LightTypes::AmbientVolume:
    {
      return new AmbientVolumeLight( m_Scene, new Content::AmbientVolumeLight() );
    }
  }

  return NULL;
}

void LightCreateTool::CreateProperties()
{
  m_Enumerator->PushPanel("Light", true);
  {
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel("Type");

      Inspect::Choice* choice = m_Enumerator->AddChoice<int>( new Nocturnal::MemberProperty<Luna::LightCreateTool, int> (this, &LightCreateTool::GetLightType, &LightCreateTool::SetLightType) );
      choice->SetDropDown( true );

      Inspect::V_Item items;
      {
        {
          std::ostringstream str;
          str << Content::LightTypes::Point;
          items.push_back( Inspect::Item( "Point", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::Spot;
          items.push_back( Inspect::Item( "Spot", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::Directional;
          items.push_back( Inspect::Item( "Directional", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::Sun;
          items.push_back( Inspect::Item( "Sun", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::Portal;
          items.push_back( Inspect::Item( "Portal", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::LightingVolume;
          items.push_back( Inspect::Item( "Lighting Volume", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::LightingEnvironment;
          items.push_back( Inspect::Item( "Lighting Environment", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::AmbientDirectionals;
          items.push_back( Inspect::Item( "AmbientDirectionals", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::CubeMapProbe;
          items.push_back( Inspect::Item( "CubeMapProbe", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::Ambient;
          items.push_back( Inspect::Item( "Ambient", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::AmbientVolume;
          items.push_back( Inspect::Item( "AmbientVolume", str.str() ) );
        }

        {
          std::ostringstream str;
          str << Content::LightTypes::SunShadow;
          items.push_back( Inspect::Item( "Sun Shadow Direction", str.str() ) );
        }
      }
      choice->SetItems( items );
    }
    m_Enumerator->Pop();

    __super::CreateProperties();
  }

  m_Enumerator->Pop();
}

i32 LightCreateTool::GetLightType() const
{
  return s_Type;
}

void LightCreateTool::SetLightType(i32 value)
{
  s_Type = static_cast< Content::LightType > (value);

  Place(Math::Matrix4::Identity);
}