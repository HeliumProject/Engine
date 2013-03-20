#include "EnginePch.h"
#include "Engine/GameObjectPointerData.h"

#include "Reflect/ArchiveBinary.h"
#include "Reflect/ArchiveXML.h"
#include "Engine/GameObject.h"

using namespace Helium;
using namespace Helium::Reflect;

template< class ArchiveT >
void GameObjectPointerData::Serialize(ArchiveT& archive)
{
	if (m_Data->ReferencesObject())
	{
		// Valid path
		Helium::GameObject *game_object = Reflect::AssertCast<GameObject>(m_Data->Get());
		archive.WriteString(*game_object->GetPath().ToString());
	}
	else
	{
		// Empty string means null
		archive.WriteString(TXT(""));
	}
}

template< class ArchiveT >
void GameObjectPointerData::Deserialize(ArchiveT& archive)
{
	*m_Data = NULL;

	// Read the path of the object we should point to
	tstring pathString;
	archive.ReadString(pathString);
	if (!pathString.empty())
	{
		GameObjectPath path;
		path.Set(pathString.c_str());

		GameObjectLoader *loader = GameObjectLoader::GetStaticInstance();
		HELIUM_ASSERT(loader);

		size_t link_index = loader->BeginLoadObject(path);
		m_Data->SetLinkIndex(static_cast< uint32_t >(link_index));
	}
	else
	{
		// This is the null data case
		m_Data->SetLinkIndex(Invalid<uint32_t>());
	}
}
