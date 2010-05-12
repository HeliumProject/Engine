#include "Precompile.h"
#include "FieldFactory.h"

#include "AssetReferenceNode.h"
#include "AssetUtils.h"
#include "ElementArrayNode.h"
#include "FieldFileReference.h"
#include "FileArrayNode.h"
#include "SimpleFieldNode.h"

#include "Asset/AssetFlags.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor (private)
// 
FieldFactory::FieldFactory()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
FieldFactory::~FieldFactory()
{
}

///////////////////////////////////////////////////////////////////////////////
// This is a singleton class.  Returns the one and only instance.
// 
Luna::FieldFactory* FieldFactory::GetInstance()
{
  static Luna::FieldFactory theFieldFactory;
  return &theFieldFactory;
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the fields of the specified element and creates nodes as
// necessary, adding them to the parent.
// 
void FieldFactory::CreateChildFieldNodes( Luna::AssetNode* parent, Reflect::Element* element, Luna::AssetManager* assetManager )
{
  const Reflect::Class* classInfo = element->GetClass();
  int offset = Reflect::GetClass< Reflect::Element >()->m_LastFieldID;

  Reflect::M_FieldIDToInfo::const_iterator fieldItr = classInfo->m_FieldIDToInfo.find( offset + 1 );
  Reflect::M_FieldIDToInfo::const_iterator fieldEnd = classInfo->m_FieldIDToInfo.end();
  for ( ; fieldItr != fieldEnd; ++fieldItr )
  {
    AssetNodePtr itemToInsert;
    const Reflect::Field* field = fieldItr->second;
    const bool isHierarchy = ( field->m_Flags & Asset::AssetFlags::Hierarchy ) != 0;
    if ( isHierarchy )
    {
      AssetNodePtr node = CreateFieldNode( assetManager, element, field );
      if ( node.ReferencesObject() )
      {
        node->CreateChildren();
        parent->AddChild( node );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to create the right kind of node.
// 
AssetNodePtr FieldFactory::CreateFieldNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field )
{
  AssetNodePtr node;
  const Reflect::Class* serializerClass = Reflect::Registry::GetInstance()->GetClass( field->m_SerializerID );

  if ( serializerClass->HasType( Reflect::GetType< Reflect::ContainerSerializer >() ) )
  {
    // Container

    if ( serializerClass->HasType( Reflect::GetType< Reflect::ElementArraySerializer >() ) )
    {
      node = new Luna::ElementArrayNode( assetManager, element, field );
    }
    else if ( serializerClass->HasType( Reflect::GetType< Reflect::ArraySerializer >() ) )
    {
      if ( Luna::FileArrayNode::IsFileArray( element, field ) )
      {
        node = new Luna::FileArrayNode( assetManager, element, field );
      }
      else
      {
        node = new Luna::ArrayNode( assetManager, element, field );
      }
    }
    else
    {
      // Not supported yet.
      NOC_BREAK();
    }
  }
  else
  {
    // Non-container

    if ( Luna::IsAssetFileReference( element, field ) )
    {
      Reflect::U64SerializerPtr serializer = Reflect::AssertCast< Reflect::U64Serializer >( field->CreateSerializer( element ) );
      tuid fileID = serializer->m_Data.Get();
      Luna::AssetReferenceNodePtr refNode = new Luna::AssetReferenceNode( assetManager, fileID, field );
      refNode->AssociateField( element, field );
      node = refNode;
    }
    else if ( Luna::IsFileReference( element, field ) )
    {
      Reflect::U64SerializerPtr serializer = Reflect::AssertCast< Reflect::U64Serializer >( field->CreateSerializer( element ) );
      tuid fileID = serializer->m_Data.Get();
      node = new Luna::FieldFileReference( assetManager, element, field, fileID );
    }
    else
    {
      node = new Luna::SimpleFieldNode( assetManager, element, field );
    }
  }
  return node;
}
