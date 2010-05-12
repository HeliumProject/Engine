#include "Precompile.h"
#include "ArrayNode.h"
#include "ArrayItemNode.h"

#include "Common/String/Tokenize.h"
#include "File/Manager.h"
#include "Console/Console.h"
#include "TUID/TUID.h"

using namespace Luna;


// Definition
LUNA_DEFINE_TYPE( Luna::ArrayNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ArrayNode::InitializeType()
{
  Reflect::RegisterClass<Luna::ArrayNode>( "Luna::ArrayNode" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ArrayNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::ArrayNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ArrayNode::ArrayNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field )
: Luna::ContainerNode( assetManager, element, field )
{
  //if ( HasFlag( Reflect::FieldFlags::FileID ) )
  //{
  //  m_Serializer->SetTranslateInputListener( Reflect::TranslateInputSignature::Delegate ( this, &ArrayNode::TranslateInputTUIDArray ) );
  //  m_Serializer->SetTranslateOutputListener( Reflect::TranslateOutputSignature::Delegate ( this, &ArrayNode::TranslateOutputTUIDArray ) );
  //}
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ArrayNode::~ArrayNode()
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates all the children of this node.
// 
void ArrayNode::CreateChildren()
{
  std::string value = GetValue();
  V_string tokens;
  ::Tokenize( value, tokens, Reflect::s_ContainerItemDelimiter );
  for each ( const std::string& str in tokens )
  {
    AddChild( new Luna::ArrayItemNode( GetAssetManager(), this, str ) );
  }
}

/////////////////////////////////////////////////////////////////////////////////
//// Converts an array of file paths into an array of tuids.
//// 
//void ArrayNode::TranslateInputTUIDArray( Reflect::TranslateInputEventArgs& args )
//{
//  if ( args.m_Serializer->GetType() == Reflect::GetType<Reflect::U64ArraySerializer>() )
//  {
//    Reflect::U64ArraySerializer* ser = Reflect::DangerousCast< Reflect::U64ArraySerializer > ( args.m_Serializer );
//
//    std::string path;
//    std::string value;
//    while ( !args.m_Stream.eof() )
//    {
//      std::getline( args.m_Stream, value );
//      if ( !path.empty() )
//      {
//        path += "\n"; // Replace newline characters in case they are the delimiters
//      }
//      path += value;
//    }
//
//    if ( !path.empty() )
//    {
//      V_string pathList;
//      ::Tokenize( path, pathList, Reflect::s_ContainerItemDelimiter );
//      V_string::const_iterator itr = pathList.begin();
//      V_string::const_iterator end = pathList.end();
//      for ( ; itr != end; ++itr )
//      {
//        tuid fileID = TUID::Null;
//        if ( !ConvertLabelToTuid( *itr, fileID ) )
//        {
//          Console::Error( "Unable to convert '%s' to a valid file ID.\n", *itr );
//        }
//
//        if ( fileID != TUID::Null )
//        {
//          ser->m_Data->push_back( fileID );
//        }
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
//// Converts an array of tuids into an array of file paths.
//// 
//void ArrayNode::TranslateOutputTUIDArray( Reflect::TranslateOutputEventArgs& args )
//{
//  if ( args.m_Serializer->GetType() == Reflect::GetType< Reflect::U64ArraySerializer >() )
//  {
//    Reflect::U64ArraySerializer* ser = Reflect::DangerousCast< Reflect::U64ArraySerializer > ( args.m_Serializer );
//
//    std::string paths;
//    V_u64::const_iterator itr = ser->m_Data->begin();
//    V_u64::const_iterator end = ser->m_Data->end();
//    for ( ; itr != end; ++itr )
//    {
//      std::string currentPath;
//      const u64& fileId = *itr;
//      if ( ConvertTuidToLabel( fileId, currentPath ) )
//      {
//        if ( !paths.empty() )
//        {
//          paths += Reflect::s_ContainerItemDelimiter;
//        }
//        paths += currentPath;
//      }
//    }
//
//    args.m_Stream << paths;
//  }
//  else
//  {
//    NOC_BREAK();
//  }
//}
