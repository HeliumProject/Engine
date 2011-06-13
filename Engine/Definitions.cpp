
#include "EnginePch.h"
#include "Engine/Definitions.h"

#include "Foundation/Reflect/Data/DataDeduction.h"
#include "Foundation/Reflect/Object.h"
//#include "Foundation/Reflect/Registry.h"
//#include "Foundation/Reflect/Serializer.h"
#include "Foundation/Reflect/ArchiveXML.h"

#include "Foundation/Log.h"

using namespace Helium;

REFLECT_DEFINE_ABSTRACT( Definitions::Definition )

struct DefinitionMetadata
{
  tstring m_GroupName;
  Definitions::DefinitionPtr m_Definition;
};

typedef std::map<tstring, DefinitionMetadata> M_Definitions;

// Every definition we know of
static M_Definitions g_DefinitionsMap;

const tstring Definitions::NULL_NAME = TXT("");
const tstring Definitions::NULL_GROUP_NAME = TXT("");

void Definitions::Definition::AcceptCompositeVisitor( Reflect::Composite& _comp )
{
  _comp.AddField( &Definition::m_Name, TXT("m_Name") );
}

Definitions::DefinitionPtr DoFind( const tstring &_name )
{
  // Try to find it
  M_Definitions::iterator iter = g_DefinitionsMap.find(_name);

  if (iter != g_DefinitionsMap.end())
  {
    // Found it!
    Definitions::DefinitionPtr ref = iter->second.m_Definition;
    return ref;
  }

  return 0;
}

void Definitions::Initialize()
{
  Reflect::RegisterClassType<Definitions::Definition>(TXT("Definition"));
}

void Definitions::Cleanup()
{
  g_DefinitionsMap.clear();

  Reflect::UnregisterClassType<Definitions::Definition>();
}

Definitions::DefinitionPtr Definitions::Create( const Reflect::Class *_class, const tstring &_name, const tstring &_group_name )
{
  HELIUM_ASSERT(_name != NULL_NAME);

  // See if we have the definition
  M_Definitions::iterator iter = g_DefinitionsMap.find(_name);

  if (iter == g_DefinitionsMap.end())
  {
    // We don't, so create it and set the namespace
    iter = g_DefinitionsMap.insert(iter, M_Definitions::value_type(_name, DefinitionMetadata()));
    iter->second.m_GroupName = _group_name;
  }
  else
  {
    // We have an object of this name already
    //if (iter->second.m_GroupName != _group_name)
    {
      Helium::Log::Warning(TXT("Duplicate definition name '%s' - Is in group '%s' but tried to add it into group '%s'"), 
        _name, iter->second.m_GroupName, _group_name);

      return DefinitionPtr();
    }
  }

  // Instantiate a new version of this definition
  Reflect::ObjectPtr object = Reflect::Registry::GetInstance()->CreateInstance(_class);
  Definitions::DefinitionPtr definition = Reflect::AssertCast<Definitions::Definition>(object.Ptr());
  iter->second.m_Definition = definition;

  // Configure it
  iter->second.m_Definition->m_Name = _name;

  // Return it
  return iter->second.m_Definition;
}

Definitions::DefinitionPtr Definitions::Find( const tstring &_name )
{
  return DoFind(_name);
}

void Definitions::Clear( const tstring &_group_name /*= NULL_GROUP_NAME*/ )
{
  for (M_Definitions::iterator iter = g_DefinitionsMap.begin();
    iter != g_DefinitionsMap.end();)
  {
    if (iter->second.m_GroupName == _group_name)
    {
      g_DefinitionsMap.erase(iter++);
    }
  }
}

void Definitions::ClearAll()
{
  g_DefinitionsMap.clear();
}

bool Definitions::Load( const tstring &_filename, const tstring &_group_name /*= NULL_GROUP_NAME*/ )
{
  V_Object objects;

  try
  {
    Reflect::ArchivePtr archive = Reflect::GetArchive( _filename );
    archive->Get( objects );
    archive->Close();
  }
  catch ( Helium::Exception& ex )
  {
    Log::Warning(TXT("Failed to load file %s: %s"), _filename, ex.What());
    return false;
  }



  //for (Reflect::V_Object::iterator iter = objects.begin(); iter != objects.end(); ++iter)
  //{
  //  // Ignore the version object that reflect puts in every file
  //  if ((*iter)->GetType() == Reflect::GetType<Reflect::Version>())
  //  {
  //    continue;
  //  }

  //  DefinitionPtr definition = Reflect::AssertCast<Definitions::Definition>(*iter);
  //  Set(definition, _group_name);
  //}

  Private::BindAll(Private::BindModes::Update);
  return true;
}

bool Definitions::Save( const tstring &_filename, const tstring &_group_name /*= NULL_GROUP_NAME*/ )
{
  Private::BindAll(Private::BindModes::PrepareToWrite);

  V_Object objects;
  Private::FindAllInGroup(_group_name, objects);

  try
  {
    Reflect::ArchivePtr archive = Reflect::GetArchive( _filename );
    archive->Put( objects );
    archive->Close();
  }
  catch ( Helium::Exception& ex )
  {
    Log::Warning(TXT("Failed to write file %s: %s"), _filename, ex.What());
    return false;
  }

  return true;
}

void Definitions::Set( const DefinitionPtr _definition, const tstring &_group_name /*= NULL_GROUP_NAME*/ )
{
  HELIUM_ASSERT(_definition->m_Name != NULL_NAME);

  M_Definitions::iterator iter = g_DefinitionsMap.find(_definition->m_Name);
  if (iter == g_DefinitionsMap.end())
  {
    iter = g_DefinitionsMap.insert(iter, M_Definitions::value_type(_definition->m_Name, DefinitionMetadata()));
    iter->second.m_GroupName = _group_name;
  }
  else
  {
    if (iter->second.m_GroupName != _group_name)
    {
      Log::Warning(TXT("Definition name '%s' - Was in group '%s' but Definitions::Set has moved it to group '%s'"), 
        _definition->m_Name, iter->second.m_GroupName, _group_name);

      iter->second.m_GroupName = _group_name;
    }
  }

  iter->second.m_Definition = _definition;
}

void Definitions::Private::FindAllInGroup( const tstring &_group_name, V_Object &_objects_list )
{
  for (M_Definitions::iterator iter = g_DefinitionsMap.begin();
    iter != g_DefinitionsMap.end(); ++iter)
  {
    if (iter->second.m_GroupName == _group_name)
    {
      _objects_list.push_back(iter->second.m_Definition);
    }
  }
}

void Definitions::Private::BindGroup( const tstring &_group_name, BindMode _bind_mode )
{
  V_Object objects;
  FindAllInGroup(_group_name, objects);

  for (V_Object::iterator iter = objects.begin();
    iter != objects.end(); ++iter)
  {
    Definitions::Definition *definition = Reflect::AssertCast<Definitions::Definition>(iter->Ptr());
    definition->BindDefinitionReferences(_bind_mode);
  }
}

void Definitions::Private::BindAll( BindMode _bind_mode )
{
  for (M_Definitions::iterator iter = g_DefinitionsMap.begin();
    iter != g_DefinitionsMap.end(); ++iter)
  {
    iter->second.m_Definition->BindDefinitionReferences(_bind_mode);
  }
}
