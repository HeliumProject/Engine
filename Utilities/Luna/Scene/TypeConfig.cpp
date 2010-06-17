#include "Precompile.h"
#include "TypeConfig.h"
#include "Instance.h"
#include "Entity.h"

#include "Application/UI/ImageManager.h"

#include "Foundation/Log.h"

#include "Pipeline/Content/Nodes/Instance/Instance.h"
#include "Pipeline/Asset/Classes/Entity.h"

#include <tinyxml.h>

using namespace Luna;

LUNA_DEFINE_TYPE(TypeConfig);

void TypeConfig::InitializeType()
{
  Reflect::RegisterClass< TypeConfig >( "Luna::TypeConfig" );
}

void TypeConfig::CleanupType()
{
  Reflect::UnregisterClass< TypeConfig >();
}

TypeConfig::TypeConfig()
{
  m_Color = D3DCOLOR_ARGB(1, 0, 0, 0);
  m_Solid = false;
  m_Transparent = false;
  m_IconIndex = -1;
}

int TypeConfig::Validate(Luna::SceneNode* node)
{
  int score = 0;

  if (!m_ApplicationType.empty())
  {
    if ( node->GetApplicationTypeName() != m_ApplicationType )
    {
      return -1;
    }
    else
    {
      score++;
    }
  }

  {
    std::set<std::string>::const_iterator itr = m_ContainsComponents.begin();
    std::set<std::string>::const_iterator end = m_ContainsComponents.end();
    for ( ; itr != end; ++itr )
    {
      if (ContainsComponent(node, *itr))
      {
        score++;
      }
      else
      {
        return -1;
      }
    }
  }

  {
    std::set<std::string>::const_iterator itr = m_MissingComponents.begin();
    std::set<std::string>::const_iterator end = m_MissingComponents.end();
    for ( ; itr != end; ++itr )
    {
      if (!ContainsComponent(node, *itr))
      {
        score++;
      }
      else
      {
        return -1;
      }
    }
  }

  if (!m_Location.empty() && node->HasType( Reflect::GetType<Luna::Entity>() ))
  {
    Asset::Entity* entity = node->GetPackage< Asset::Entity >();

    {
      std::string location = entity->GetEntityAssetPath();

#pragma TODO( "do we need to strip this?" )
      //FileSystem::StripPrefix( FinderSpecs::Asset::ENTITY_FOLDER.GetFolder(), location );

      if ( strstr( location.c_str(), m_Location.c_str() ) == location.c_str() )
      {
        for ( const char* ptr = m_Location.c_str(); *ptr != '\0'; ptr++ )
        {
          if ( ptr == m_Location.c_str() )
          {
            // we get one for free
            score++;
          }

          if ( *ptr == '/' )
          {
            // the rest are for slashes
            score++;
          }
        }
      }
      else
      {
        return -1;
      }
    }
  }

  return score;
}

bool TypeConfig::ContainsComponent(Luna::SceneNode* node, const std::string& name)
{
  const Reflect::Class* type = Reflect::Registry::GetInstance()->GetClass(name);

  if (type == NULL)
  {
    Log::Warning("Unable to resolve attribute name '%s'\n", name.c_str());
    return false;
  }

  const Component::ComponentPtr attr = node->GetPackage<Content::SceneNode>()->GetComponent(type->m_TypeID);

  return attr.ReferencesObject();
}

void TypeConfig::LoadFromFile(V_TypeConfigSmartPtr& types)
{
  TiXmlDocument doc;

#pragma TODO( "reimplement to use a valid config folder location to get this file" )
  std::string file = ""; //FinderSpecs::Luna::TYPE_CONFIGURATIONS.GetFile( FinderSpecs::Luna::GLOBAL_CONFIG_FOLDER );

  if (!doc.LoadFile( file.c_str() ))
  {
    Log::Warning("Unable to load type configuration from %s\n", file.c_str());
    return;
  }

  TiXmlElement* runtimeTypes = doc.FirstChildElement();

  if ( runtimeTypes && std::string( runtimeTypes->Value() ) == "TypeConfigurations" )
  {
    for ( TiXmlElement* current = runtimeTypes->FirstChildElement(); current != NULL; current = current->NextSiblingElement() )
    {
      if ( std::string( current->Value() ) != "TypeConfiguration" )
      {
        continue;
      }

      TypeConfigPtr t = new TypeConfig();

      const char* name = current->Attribute( "Name" );
      if ( name )
      {
        t->m_Name = name;
      }

      for ( TiXmlElement* block = current->FirstChildElement(); block != NULL; block = block->NextSiblingElement() )
      {
        if ( std::string( block->Value() ) == "Settings" )
        {
          int r = 0, g = 0, b = 0, a = 1;

          for ( TiXmlElement* setting = block->FirstChildElement(); setting != NULL; setting = setting->NextSiblingElement() )
          {
            if ( std::string( setting->Value() ) == "Icon" )
            {
              t->m_Icon = setting->GetText();

#pragma TODO( "reimplement icons as resources" )
              Nocturnal::Path iconPath( t->m_Icon );

              if ( iconPath.Exists() )
              {
                t->m_IconIndex = Nocturnal::GlobalImageManager().GetImageIndex( iconPath.Get() );
              }
              else
              {
                iconPath.Set( "" + t->m_Icon ); //FinderSpecs::Luna::GLOBAL_CONFIG_FOLDER.GetFolder();
                if ( iconPath.Exists() )
                {
                  t->m_IconIndex = Nocturnal::GlobalImageManager().GetImageIndex( iconPath.Get() );
                }
              }

              if ( t->m_IconIndex < 0 )
              {
                Log::Warning( "Unable to find icon for %s\n", t->m_Icon.c_str() );
                t->m_IconIndex = -1;
              }
              else
              {
                t->m_Icon = iconPath.Get();
              }
            }
            else if ( std::string( setting->Value() ) == "Color" )
            {
              setting->Attribute("Red", &r);
              Math::Clamp(r, 0, 255);

              setting->Attribute("Green", &g);
              Math::Clamp(g, 0, 255);

              setting->Attribute("Blue", &b);
              Math::Clamp(b, 0, 255);

              setting->Attribute("Alpha", &a);
              Math::Clamp(a, 0, 255);
            }
            else if ( std::string( setting->Value() ) == "Fill" )
            {
              if (!_stricmp(setting->GetText(), "Wire"))
              {
                t->m_Solid = false;
              }
              else if (!_stricmp(setting->GetText(), "Solid"))
              {
                t->m_Solid = true;
              }
            }
            else if ( std::string( setting->Value() ) == "Opacity" )
            {
              if (!_stricmp(setting->GetText(), "Opaque"))
              {
                t->m_Transparent = false;
              }
              else if (!_stricmp(setting->GetText(), "Transparent"))
              {
                t->m_Transparent = true;
              }
            }
          }

          if (t->m_Transparent && a == 1)
          {
            a = 128;
          }

          t->m_Color = D3DCOLOR_ARGB(a, r, g, b);
        }
        else if ( std::string( block->Value() ) == "Criteria" )
        {
          for ( TiXmlElement* criterion = block->FirstChildElement(); criterion != NULL; criterion = criterion->NextSiblingElement() )
          {
            if ( std::string( criterion->Value() ) == "ApplicationType" )
            {
              if (t->m_ApplicationType.empty())
              {
                t->m_ApplicationType = criterion->GetText();
              }
              else
              {
                Log::Error("Type '%s' has more than one ApplicationType specified\n", t->m_Name.c_str());
              }
            }
            else if ( std::string( criterion->Value() ) == "ContainsComponent" )
            {
              t->m_ContainsComponents.insert( criterion->GetText() );
            }
            else if ( std::string( criterion->Value() ) == "MissingComponent" )
            {
              t->m_MissingComponents.insert( criterion->GetText() );
            }
            else if ( std::string( criterion->Value() ) == "Location" )
            {
              if (t->m_Location.empty())
              {
                t->m_Location = criterion->GetText();

                Nocturnal::Path::Normalize( t->m_Location );

                if (!t->m_Location.empty() && t->m_Location[0]== '/')
                {
                  t->m_Location.erase(0, 1);
                }

                if (!t->m_Location.empty() && t->m_Location[t->m_Location.size()-1] == '/')
                {
                  t->m_Location.erase(t->m_Location.size()-1, 1);
                }
              }
              else
              {
                Log::Error("Type '%s' has more than on Location specified\n", t->m_Name.c_str());
              }
            }
          }
        }
      }

      if (!t->m_Name.empty())
      {
        types.push_back(t);
      }
    }
  }
}
