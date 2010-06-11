#include "Precompile.h"
#include "SimpleFieldNode.h"
#include "FieldFactory.h"

#include "Foundation/Log.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::SimpleFieldNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void SimpleFieldNode::InitializeType()
{
  Reflect::RegisterClass<Luna::SimpleFieldNode>( "Luna::SimpleFieldNode" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void SimpleFieldNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::SimpleFieldNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor.
// 
SimpleFieldNode::SimpleFieldNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field )
: Luna::FieldNode( assetManager, element, field )
{
  //if ( HasFlag( Reflect::FieldFlags::FileID ) )
  //{
  //  m_Serializer->SetTranslateInputListener( Reflect::TranslateInputSignature::Delegate ( this, &SimpleFieldNode::TranslateInputTUID ) );
  //  m_Serializer->SetTranslateOutputListener( Reflect::TranslateOutputSignature::Delegate ( this, &SimpleFieldNode::TranslateOutputTUID ) );
  //}

  SetName( field->m_UIName + ": " + GetValue() );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SimpleFieldNode::~SimpleFieldNode()
{
}

/////////////////////////////////////////////////////////////////////////////////
//// Convert a string into a TUID.
//// 
//void SimpleFieldNode::TranslateInputTUID( Reflect::TranslateInputEventArgs& args )
//{
//  if (args.m_Serializer->GetType() == Reflect::GetType<Reflect::U64Serializer>())
//  {
//    Reflect::U64Serializer* ser = DangerousCast< U64Serializer > ( args.m_Serializer );
//
//    std::string path;
//    std::getline( args.m_Stream, path );
//
//    if ( !path.empty() )
//    {
//      tuid fileID = UID::TUID::Null;
//      if ( !ConvertLabelToTuid( path, fileID ) )
//      {
//        Log::Error( "No TUID was found for path '%s' (Reason: %s); discarding.\n", path.c_str() );
//      }
//
//      if ( fileID != UID::TUID::Null )
//      {
//        ser->m_Data.Set( fileID );
//      }
//    }
//  }
//  else
//  {
//    NOC_BREAK();
//  }
//}
//
/////////////////////////////////////////////////////////////////////////////////
//// Convert a TUID into a string.
//// 
//void SimpleFieldNode::TranslateOutputTUID( Reflect::TranslateOutputEventArgs& args )
//{
//  if ( args.m_Serializer->GetType() == Reflect::GetType<Reflect::U64Serializer>() )
//  {
//    Reflect::U64Serializer* ser = DangerousCast< Reflect::U64Serializer > ( args.m_Serializer );
//    std::string path;
//
//    ConvertTuidToLabel( ser->m_Data.Get(), path );
//    args.m_Stream << path;
//  }
//  else
//  {
//    NOC_BREAK();
//  }
//}
