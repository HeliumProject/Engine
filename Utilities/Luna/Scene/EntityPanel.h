#pragma once

#include "API.h"
#include "InstancePanel.h"
#include "Inspect/Data.h"
#include "Inspect/FilteredDropTarget.h"
#include "InspectReflect/ReflectInterpreter.h"
#include "Entity.h"
#include "UIToolKit/FileDialog.h"
#include "RCS/RCS.h"

namespace Luna
{
  struct EntityAssetSetChangeArgs;

  template <class T>
  struct Enabler
  {
    std::vector<T*> m_Items;

    void SetEnabled(bool enabled)
    {
      std::vector<T*>::const_iterator itr = m_Items.begin();
      std::vector<T*>::const_iterator end = m_Items.end();
      for ( ; itr != end; ++itr )
      {
        (*itr)->SetEnabled(enabled);
      }
    }
  };

  typedef Enabler<Inspect::Control> ControlEnabler;

  class EntityPanel : public InstancePanel
  {
  protected:
    Inspect::Value*     m_TextBox;
    
    Inspect::Panel*     m_LightingPanel;
    Inspect::Value*     m_DrawDistance;
    Inspect::Value*     m_UpdateDistance;
    Inspect::Choice*    m_ShaderGroup;
    Inspect::Value*     m_Attenuation;

    Inspect::CheckBox*  m_FarShadowFadeout;
    Inspect::CheckBox*  m_CastsBakedShadows;
    Inspect::CheckBox*  m_DoBakedShadowCollisions;
    Inspect::Value*     m_BakedShadowAABBExt;
    Inspect::Value*     m_BakedShadowMergeGroups;
    Inspect::CheckBox*  m_HighResShadowMap;

    Inspect::CheckBox*  m_SpecializeCheckbox;

    ControlEnabler*     m_SegmentEnabler;
    ControlEnabler*     m_ShaderGroupEnabler;

    ControlEnabler*     m_FarShadowFadeoutEnabler;
    ControlEnabler*     m_CastsBakedShadowsEnabler;
    ControlEnabler*     m_DoBakedShadowCollisionsEnabler;
    ControlEnabler*     m_BakedShadowAABBExtEnabler;
    ControlEnabler*     m_BakedShadowMergeGroupsEnabler;
    ControlEnabler*     m_HighResShadowMapEnabler;

    typedef std::vector< EntityPtr > V_EntitySmartPtr;
    V_EntitySmartPtr                m_Entities;
    Inspect::ReflectInterpreterPtr  m_CollisionReflectInterpreter;

  public:
    EntityPanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection);
    virtual ~EntityPanel();
    virtual void Create() NOC_OVERRIDE;

  protected:
    void CreateAssetType();
    void CreateClassPath();
    void CreateClassActions();
    void CreateShowFlags();
    void CreateLighting();
    void CreateCubemap();
    void CreateGameplay();
    void CreateShaderGroups();
    void CreateVisual();
    void CreateCollision();
    void CreateAttenuation();
    void CreateChildImportExport();

  protected:
    // UI callbacks

    bool OnEntityAssetChanging( const Inspect::ChangingArgs& args );
    void OnEntityAssetChanged( const Inspect::ChangeArgs& args );
    void OnEntityAssetRefresh( Inspect::Button* button );
    void OnEntityAssetEditAsset( Inspect::Button* button );
    void OnEntityAssetEditArt( Inspect::Button* button );
    void OnEntityAssetBuild( Inspect::Button* button );
    void OnEntityAssetView( Inspect::Button* button );
    void OnEntityAssetRevisionHistory( Inspect::Button* button );
    void OnEntityAssetDrop( const Inspect::FilteredDropTargetArgs& args );
    void OnCubeMapEditAsset( Inspect::Button* button );
    void OnViewUVs( Inspect::Button* button );

    // Internal Callbacks
    void EntityAssetReloaded( const EntityAssetSetChangeArgs& args );

    // Getters/Setters

    template <class AttributeType>
    bool GetOverride() const
    {
      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
        NOC_ASSERT(entity);

        if ( !entity->GetPackage<Asset::Entity>()->ContainsAttribute( Reflect::GetType<AttributeType>() ) )
        {
          return false;
        }
      }

      return true;
    }

    template <class AttributeType, class Control, Control EntityPanel::* MemberPtr>
    void SetOverride(bool o)
    {
      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
        NOC_ASSERT(entity);

        if ( o )
        {
          if ( !entity->GetPackage<Asset::Entity>()->ContainsAttribute( Reflect::GetType<AttributeType>() ) )
          {
            entity->GetPackage<Asset::Entity>()->SetAttribute( new AttributeType () );
          }
        }
        else
        {
          if ( entity->GetPackage<Asset::Entity>()->ContainsAttribute( Reflect::GetType<AttributeType>() ) )
          {
            entity->GetPackage<Asset::Entity>()->RemoveAttribute( Reflect::GetType<AttributeType>() );
          }
        }
      }

      (this->*MemberPtr)->SetEnabled( o );
    }

    //
    template< class AttributeType, bool AttributeType::*PtrToBool >
    bool GetBoolOverride() const
    {
      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::Entity* entity = Reflect::AssertCast< Luna::Entity >( *itr );
        Asset::Entity* pkg = entity->GetPackage< Asset::Entity >();
        Nocturnal::SmartPtr< AttributeType > attrib = Reflect::ObjectCast< AttributeType >( pkg->GetAttribute( Reflect::GetType< AttributeType >() ) );
        if ( !attrib.ReferencesObject() || !( attrib->*PtrToBool ) )
        {
          return false;
        }
      }
      return true;
    }

    //
    template< class AttributeType, class Control, Control EntityPanel::*MemberPtr, bool AttributeType::*PtrToBool >
    void SetBoolOverride( bool o )
    {
      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::Entity* entity = Reflect::AssertCast< Luna::Entity >( *itr );

        Asset::Entity* pkg = entity->GetPackage<Asset::Entity>();
        if ( !pkg->ContainsAttribute( Reflect::GetType<AttributeType>() ) )
        {
          pkg->SetAttribute( new AttributeType () );
        }

        Attribute::AttributeEditor< AttributeType > attr( pkg );
        (attr.operator->())->*PtrToBool = o;
        attr.Commit();
      }

      (this->*MemberPtr)->SetEnabled( o );
    }

    template <class ValueType, class AttributeType, ValueType AttributeType::* MemberPtr>
    std::string GetValue() const
    {
      ValueType result;
      std::ostringstream str;

      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
        NOC_ASSERT(entity);

        Attribute::AttributeViewer< AttributeType > attr (entity->GetPackage<Asset::Entity>());
        if (!attr.Valid())
        {
          return "";
        }

        if ( itr == m_Selection.Begin() )
        {
          result = (attr.operator->())->*MemberPtr;
          str << (attr.operator->())->*MemberPtr;
        }
        else if ( (attr.operator->())->*MemberPtr != result )
        {
          return Inspect::MULTI_VALUE_STRING;
        }
      }

      return str.str();
    }

    template <class ValueType, class AttributeType, ValueType AttributeType::* MemberPtr>
    void SetValue(const std::string& dist)
    {
      std::istringstream str (dist);

      ValueType value;
      str >> value;

      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
        NOC_ASSERT(entity);

        Attribute::AttributeEditor< AttributeType > attr (entity->GetPackage<Asset::Entity>());
        NOC_ASSERT(attr.Valid());

        (attr.operator->())->*MemberPtr = value;

        attr.Commit();
      }
    }
    
public:

    template <class LunaClass, class ContentClass >
    void OnExport( Inspect::Button* button )
    {
      
      std::string changelistDesc;
      changelistDesc = "Exporting " + Reflect::GetClass< ContentClass >()->m_ShortName + " from Luna Scene Editor";
      RCS::Changeset changelist( changelistDesc );

      if( m_Selection.Empty() )
        changelist.Revert();

      bool abandonShip = false;

      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        if( abandonShip )
          break;

        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
        if( entity )
        {
          Luna::Scene* scene = entity->GetScene();
          if( scene )
          {
            Editor* editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene );

            if( editor )
            {
              Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile( entity->GetClassSet()->GetEntityAsset(), true );
              std::string objectsFile = FinderSpecs::Content::OBJECTS_DECORATION.GetExportFile( entity->GetClassSet()->GetContentFile(), artFile->m_FragmentNode );
              RCS::File rcsObjectsFile( objectsFile );

              try
              {
                changelist.Open( rcsObjectsFile );

                V_Element loadedElements;
                V_Element serializeElements;

                NOC_ASSERT( FileSystem::Exists( objectsFile ) );
                
                if( FileSystem::GetSize( objectsFile ) )                
                  Archive::FromFile( objectsFile, loadedElements );

                V_Element::iterator elemItor = loadedElements.begin();
                V_Element::iterator elemEnd  = loadedElements.end();

                bool hasType = false;
                for( ; elemItor != elemEnd; ++elemItor )
                {
                  if( (*elemItor)->HasType( Reflect::GetType< ContentClass >() ) )
                  {
                    hasType = true;
                    break;
                  }
                }
                if( hasType )
                {
                  Asset::Entity* assetEntity = entity->GetPackage< Asset::Entity >();
                  
                  std::stringstream stream;
                  stream << "Object export file for " << assetEntity->GetEntityAsset()->GetShortName() 
                    << " already contains objects of type " << Reflect::GetClass< ContentClass >()->m_ShortName << std::endl;
                  stream << "Would you like to overwrite the existing objects?";                  
        
                  int id = ::MessageBox( GetActiveWindow(), stream.str().c_str(), "Overwrite?", MB_YESNOCANCEL | MB_ICONEXCLAMATION );

                  if( id == IDNO )
                  {
                    continue;
                  }
                  else if ( id == IDCANCEL )
                  {
                    abandonShip = true;
                    continue;
                  }
                  else
                  {
                    // erase existing elements of specified type from the object export file
                    elemItor = loadedElements.begin();
                    for( ; elemItor != elemEnd; ++elemItor  )
                    {
                      if( !(*elemItor)->HasType( Reflect::GetType< ContentClass >() ) )
                        serializeElements.push_back( *elemItor );
                    }
                  }                                    
                }
                // go through each of the entity's children and set child transforms such that
                // the parent's global transform acts as their origin
                const S_HierarchyNodeDumbPtr& children = entity->GetChildren();

                S_HierarchyNodeDumbPtr::const_iterator childrenItor = children.begin();
                S_HierarchyNodeDumbPtr::const_iterator childrenEnd  = children.end();
                for( ; childrenItor != childrenEnd; ++childrenItor )
                {
                  if( (*childrenItor)->HasType( Reflect::GetType< LunaClass >() ) )
                  {
                    LunaClass* object = Reflect::DangerousCast< LunaClass >( *childrenItor );
                    object->Pack(); 

                    ContentClass* contentObject = object->GetPackage< ContentClass >();
                    Nocturnal::SmartPtr< ContentClass > clonedObject = Reflect::ObjectCast< ContentClass >( contentObject->Clone() );

                    std::string name = clonedObject->GetName();

                    clonedObject->m_GlobalTransform = clonedObject->m_ObjectTransform;
                    clonedObject->m_ObjectTransform = Math::Matrix4::Identity; 
                    clonedObject->m_ParentID = TUID::Null;
                    
                    serializeElements.push_back( clonedObject );
                  }
                }
                Archive::ToFile( serializeElements, objectsFile );
              }
              catch (Nocturnal::Exception& e)
              {
                std::stringstream stream;
                stream << "Unable to export objects of type " << Reflect::GetClass< ContentClass >()->m_ShortName.c_str() 
                  << " from Luna Scene Editor: " << e.what();
                ::MessageBox( NULL, stream.str().c_str(), "Export Objects", MB_OK );   
                continue;
              }

             
            }
          }
        }
      }
      
    }

    template <class LunaClass, class ContentClass >
    void OnImport( Inspect::Button* button )
    {
      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
        if( entity )
        {
          Luna::Scene* scene = entity->GetScene();
          if( scene )
          {
            Editor* editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene );

            if( editor )
            {
              const S_HierarchyNodeDumbPtr& children = entity->GetChildren();
              S_HierarchyNodeDumbPtr::const_iterator itor = children.begin();
              S_HierarchyNodeDumbPtr::const_iterator childrenEnd  = children.end();
              for( ; itor != childrenEnd; ++itor )
              {
                if( (*itor)->HasType( Reflect::GetType<LunaClass>() ) )
                {
                  editor->PostCommand( new SceneNodeExistenceCommand( Undo::ExistenceActions::Remove, scene, *itor, false ) );
                }
              }

              Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile( entity->GetClassSet()->GetEntityAsset(), true );
              std::string lightFile = FinderSpecs::Content::OBJECTS_DECORATION.GetExportFile( entity->GetClassSet()->GetContentFile(), artFile->m_FragmentNode );

              if( FileSystem::Exists( lightFile ) )
              {
                Asset::Entity* assetEntity = entity->GetPackage< Asset::Entity >();
                editor->PostCommand( new SceneImportCommand( scene, lightFile, ImportActions::Import, ImportFlags::None, entity, Reflect::GetType< ContentClass >() ) );
              }
            }
          }
        }
      }
    }

    template <class LunaClass >
    void OnSelectChildren( Inspect::Button* button )
    {
      OS_SelectableDumbPtr objects;

      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
        if( entity )
        {
          Luna::Scene* scene = entity->GetScene();
          if( scene )
          {
            const S_HierarchyNodeDumbPtr& children = entity->GetChildren();
            S_HierarchyNodeDumbPtr::const_iterator itor = children.begin();
            S_HierarchyNodeDumbPtr::const_iterator childrenEnd  = children.end();
            for( ; itor != childrenEnd; ++itor )
            {
              if( (*itor)->HasType( Reflect::GetType< LunaClass >() ) )
              {
                objects.Append( *itor );
              }
            }
          }
        }
      }

      SceneEditor* editor = static_cast<SceneEditor*>( SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ) );
      if( editor )
      {
        Luna::Scene* scene = editor->GetSceneManager()->GetCurrentScene();
        if( scene )
        {
          editor->PostCommand( new SceneSelectCommand( scene, objects ) );
        }
      }
    }

    template <class LunaClass, class ContentClass >
    void OnExportToFile( Inspect::Button* button )
    {
      bool abandonShip = false;
      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        if( abandonShip )
          break;

        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
        if( entity )
        {
          Luna::Scene* scene = entity->GetScene();
          if( scene )
          {
            Editor* editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene );

            if( editor )
            {
              std::string filePath;
              try
              {
                std::string assetPath = entity->GetEntityAssetPath();
                FileSystem::StripLeaf( assetPath );
                UIToolKit::FileDialog dialog( NULL, "export children to file", assetPath.c_str(), wxEmptyString, FinderSpecs::Extension::REFLECT_BINARY.GetDialogFilter(), UIToolKit::FileDialogStyles::DefaultSave );
                if ( dialog.ShowModal() == wxID_OK )
                {
                  filePath = dialog.GetPath();
                  if ( !filePath.empty() )
                  {
                    std::string dummy = filePath;
                    FileSystem::StripLeaf( dummy );
                    FileSystem::MakePath( dummy );
                    FILE* file = fopen( filePath.c_str(), "r" );
                    fclose( file );

                    V_Element loadedElements;
                    V_Element serializeElements;

                    NOC_ASSERT( FileSystem::Exists( filePath ) );

                    if( FileSystem::GetSize( filePath ) )                
                      Archive::FromFile( filePath, loadedElements );

                    V_Element::iterator elemItor = loadedElements.begin();
                    V_Element::iterator elemEnd  = loadedElements.end();

                    bool hasType = false;
                    for( ; elemItor != elemEnd; ++elemItor )
                    {
                      if( (*elemItor)->HasType( Reflect::GetType< ContentClass >() ) )
                      {
                        hasType = true;
                        break;
                      }
                    }
                    if( hasType )
                    {
                      Asset::Entity* assetEntity = entity->GetPackage< Asset::Entity >();

                      std::stringstream stream;
                      stream << "Object export file for " << assetEntity->GetEntityAsset()->GetShortName() 
                        << " already contains objects of type " << Reflect::GetClass< ContentClass >()->m_ShortName << std::endl;
                      stream << "Would you like to overwrite the existing objects?";                  

                      int id = ::MessageBox( GetActiveWindow(), stream.str().c_str(), "Overwrite?", MB_YESNOCANCEL | MB_ICONEXCLAMATION );

                      if( id == IDNO )
                      {
                        continue;
                      }
                      else if ( id == IDCANCEL )
                      {
                        abandonShip = true;
                        continue;
                      }
                      else
                      {
                        // erase existing elements of specified type from the object export file
                        elemItor = loadedElements.begin();
                        for( ; elemItor != elemEnd; ++elemItor  )
                        {
                          if( !(*elemItor)->HasType( Reflect::GetType< ContentClass >() ) )
                            serializeElements.push_back( *elemItor );
                        }
                      }                                    
                    }
                    // go through each of the entity's children and set child transforms such that
                    // the parent's global transform acts as their origin
                    const S_HierarchyNodeDumbPtr& children = entity->GetChildren();

                    S_HierarchyNodeDumbPtr::const_iterator childrenItor = children.begin();
                    S_HierarchyNodeDumbPtr::const_iterator childrenEnd  = children.end();
                    for( ; childrenItor != childrenEnd; ++childrenItor )
                    {
                      if( (*childrenItor)->HasType( Reflect::GetType< LunaClass >() ) )
                      {
                        LunaClass* object = Reflect::DangerousCast< LunaClass >( *childrenItor );
                        object->Pack(); 

                        ContentClass* contentObject = object->GetPackage< ContentClass >();
                        Nocturnal::SmartPtr< ContentClass > clonedObject = Reflect::ObjectCast< ContentClass >( contentObject->Clone() );

                        std::string name = clonedObject->GetName();

                        clonedObject->m_GlobalTransform = clonedObject->m_ObjectTransform;
                        clonedObject->m_ObjectTransform = Math::Matrix4::Identity; 
                        clonedObject->m_ParentID = TUID::Null;

                        serializeElements.push_back( clonedObject );
                      }
                    }
                    Archive::ToFile( serializeElements, filePath );
                  }
                }                
              }
              catch (Nocturnal::Exception& e)
              {
                std::stringstream stream;
                stream << "Unable to export objects of type " << Reflect::GetClass< ContentClass >()->m_ShortName.c_str() 
                  << " from Luna Scene Editor: " << e.what();
                ::MessageBox( NULL, stream.str().c_str(), "Export Objects", MB_OK );   
                continue;
              }
            }
          }
        }
      }
    }
    template <class LunaClass, class ContentClass >
    void OnImportFromFile( Inspect::Button* button )
    {
      Editor* editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene );

      if( editor )
      {
        OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
        OS_SelectableDumbPtr::Iterator end = m_Selection.End();
        for ( ; itr != end; ++itr )
        {
          Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
          if( entity )
          {
            Luna::Scene* scene = entity->GetScene();
            if( scene )
            {              
              std::string assetPath = entity->GetEntityAssetPath();
              FileSystem::StripLeaf( assetPath );
              std::string filePath;
              UIToolKit::FileDialog dialog( NULL, "import children to file", assetPath.c_str(), wxEmptyString, FinderSpecs::Extension::REFLECT_BINARY.GetDialogFilter(), UIToolKit::FileDialogStyles::DefaultOpen );
              if ( dialog.ShowModal() == wxID_OK )
              {
                filePath = dialog.GetPath();
                if ( !filePath.empty() )
                {
                  if( FileSystem::Exists( filePath ) )
                  {
                    const S_HierarchyNodeDumbPtr& children = entity->GetChildren();
                    S_HierarchyNodeDumbPtr::const_iterator itor = children.begin();
                    S_HierarchyNodeDumbPtr::const_iterator childrenEnd  = children.end();
                    for( ; itor != childrenEnd; ++itor )
                    {
                      if( (*itor)->HasType( Reflect::GetType<LunaClass>() ) )
                      {
                        editor->PostCommand( new SceneNodeExistenceCommand( Undo::ExistenceActions::Remove, scene, *itor, false ) );
                      }
                    }

                    Asset::Entity* assetEntity = entity->GetPackage< Asset::Entity >();
                    editor->PostCommand( new SceneImportCommand( scene, filePath, ImportActions::Import, ImportFlags::None, entity, Reflect::GetType< ContentClass >() ) );
                  }               
                }
              }
            }
          }
        }
      }
    }

  };
}
