#include "Precompile.h"
#include "AttachmentCollisionInterpreter.h"

#include "CharacterEditor.h"
#include "Editor/EditorInfo.h"
#include "Editor/SessionManager.h"

#include "Content/Scene.h"
#include "Content/CollisionPrimitive.h"
#include "Content/LooseAttachCollisionAttribute.h"

#include "Inspect/Button.h"
#include "Inspect/Canvas.h"
#include "Inspect/List.h"
#include "InspectReflect/InspectReflectInit.h"

#include "LooseAttachCollisionDialog.h"

#include <map>
using std::map;

using namespace Reflect;
using namespace Luna;
using namespace Inspect;


LAttachmentCollisionInterpreter::LAttachmentCollisionInterpreter (Container* labelContainer)
: ReflectArrayInterpreter (labelContainer)
{

}

void LAttachmentCollisionInterpreter::InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
  if (field->m_Flags & FieldFlags::Hide)
  {
    return;
  }

  // save some data for later
  m_Parent                    = parent;
  m_Field                     = field;
  m_LooseAttachAttributePtr   = Reflect::DangerousCast<Content::LooseAttachCollisionAttribute>( instances[0] );
  
  if( (m_TuidDisplay.size() == 0) || (m_DisplayToTuid.size() == 0) )
  {
    // get the luna character manager
    CharacterEditor* characterEditor = (CharacterEditor*)SessionManager::GetInstance()->LaunchEditor( Luna::EditorTypes::Character );

    if( !m_LooseAttachAttributePtr )
    {
      return;
    }
    const Content::ScenePtr scene = characterEditor->GetManager()->GetCollisionScene();

    Content::V_CollisionPrimitive primitives;
    scene->GetAll<Content::CollisionPrimitive>( primitives );

    // get all of the primitives in this model
    for( unsigned int i = 0; i < primitives.size(); i++ )
    {
      if( (primitives[i]->m_Shape >=  Content::CollisionShapes::LooseAttachmentPrimFirst) 
        && (primitives[i]->m_Shape <=  Content::CollisionShapes::LooseAttachmentPrimLast) )
      {                           
        std::string displayString = primitives[i]->m_GivenName.size() ? primitives[i]->m_GivenName : primitives[i]->m_DefaultName;  
        m_TuidDisplay[primitives[i]->m_ID]   = displayString;
        m_DisplayToTuid[displayString]       = primitives[i]->m_ID;      // i'm not really a fan of this reverse lookup      
      }
    }
  }

  // call our parents interpreter so that we display the panel
  __super::InterpretField( field, instances, parent );
}


Inspect::ActionPtr LAttachmentCollisionInterpreter::AddAddButton( Inspect::List* list )
{
  ActionPtr addButton = m_Container->GetCanvas()->Create<Action>(this);
  addButton->AddListener( ActionSignature::Delegate ( this, &LAttachmentCollisionInterpreter::MyOnAdd ) );
  addButton->SetClientData( new ClientDataControl( list ) );
  addButton->SetText( "Add" );
  
  return addButton;
}

void LAttachmentCollisionInterpreter::MyOnAdd( Button* button )
{
  if( !m_LooseAttachAttributePtr )
  {
    return;
  }

  CharacterEditor* characterEditor = (CharacterEditor*)SessionManager::GetInstance()->LaunchEditor( Luna::EditorTypes::Character );
  const Content::ScenePtr scene = characterEditor->GetManager()->GetCollisionScene();

  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( !(clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() )) )
  {
    return;
  }

  // the list of selected collision volumes
  ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );    
  List* list = static_cast< List* >( data->m_Control );

  Content::V_CollisionPrimitive primitives;
  scene->GetAll<Content::CollisionPrimitive>( primitives );

  LooseAttachCollisionDialog dlg( m_Parent->GetWindow() );

  // get list of selected and unselected items
  const V_tuid& currentItems = m_LooseAttachAttributePtr->GetCollisionTuids( );

  wxArrayString  wxarraySelectedItems;
  wxArrayString  wxarrayUnselectedItems;

  // get all of the primitives in this model
  for( unsigned int i = 0; i < primitives.size(); i++ )
  {

    if( (primitives[i]->m_Shape <  Content::CollisionShapes::LooseAttachmentPrimFirst) 
      || (primitives[i]->m_Shape >  Content::CollisionShapes::LooseAttachmentPrimLast) )
    {    
      continue;
    }

    // add it to the correct location
    V_tuid::const_iterator itor     = currentItems.begin();
    V_tuid::const_iterator itorEnd  = currentItems.end();
    bool found = false;

    while( itor != itorEnd )
    {
      if( *itor == primitives[i]->m_ID )
      {
        found = true;
        break;
      }

      itor++;
    }

    if( found )
    {
      wxarraySelectedItems.Add( wxString( m_TuidDisplay[ primitives[i]->m_ID ].c_str() ) );
    }
    else
    {
      wxarrayUnselectedItems.Add( wxString( m_TuidDisplay[ primitives[i]->m_ID ].c_str() ) );
    }
  }

  dlg.m_listSelectedCollisionVolumes->Append( wxarraySelectedItems );
  dlg.m_listUnselectedCollisionVolumes->Append( wxarrayUnselectedItems );

  if( dlg.ShowModal() == wxID_OK )
  {
    //list->AddItem( input );
    list->Freeze();

    V_string currentListItems = list->GetItems();

    // remove all the current items
    for( V_string::iterator itr = currentListItems.begin(); itr != currentListItems.end(); itr++ )
    {
      list->RemoveItem(*itr);
    }    

    // add the selected items
    for( unsigned int i = 0; i < dlg.m_listSelectedCollisionVolumes->GetCount(); i++ )
    {
      list->AddItem( dlg.m_listSelectedCollisionVolumes->GetString(i).c_str() );
    }

    list->Thaw();
  }   

  // refresh the window
  m_Parent->Read();
}

void LAttachmentCollisionInterpreter::OnCreateFieldSerializer( SerializerPtr & s )
{
  if( ( m_Field->m_SerializerID == Reflect::GetType<U64ArraySerializer>() ) && ( m_Field->m_Flags & FieldFlags::NodeID ) != 0 )
  {
    s->SetTranslateInputListener( Reflect::TranslateInputSignature::Delegate ( this, &LAttachmentCollisionInterpreter::TranslateInputTUIDContainer ) );
    s->SetTranslateOutputListener( Reflect::TranslateOutputSignature::Delegate ( this, &LAttachmentCollisionInterpreter::TranslateOutputTUIDContainer ) );
  }
}

void LAttachmentCollisionInterpreter::TranslateInputTUIDContainer( Reflect::TranslateInputEventArgs& args )
{
  if( args.m_Serializer->GetType() == Reflect::GetType<U64ArraySerializer>() )
  { 
    U64ArraySerializer* ser = AssertCast< U64ArraySerializer > ( args.m_Serializer );
    
    std::string volumeDisplayName;
   
    while ( !args.m_Stream.eof() )
    {
      std::getline(args.m_Stream, volumeDisplayName);

      if( volumeDisplayName == "" )
      {
        continue;
      }
      
      u64 assetID = m_DisplayToTuid[volumeDisplayName];
      
      ser->m_Data->push_back( assetID );
    }
  }
}

void LAttachmentCollisionInterpreter::TranslateOutputTUIDContainer( Reflect::TranslateOutputEventArgs& args )
{
  if( args.m_Serializer->GetType() == Reflect::GetType<U64ArraySerializer>() )
  {
    U64ArraySerializer* ser = AssertCast< U64ArraySerializer > ( args.m_Serializer );
    V_u64::iterator itr = ser->m_Data->begin();
    V_u64::iterator end = ser->m_Data->end();

    for ( ; itr != end; ++itr )
    {
      const u64& collisionVolumeID = *itr;
      args.m_Stream << m_TuidDisplay[collisionVolumeID];
      args.m_Stream << "\n";                              // add newline as the seperator
    }
    
  }
}