#include "FileContainerInterpreter.h"
#include "InspectFileInit.h"

#include <wx/aui/aui.h>

#ifdef INSPECT_REFACTOR
#include "Application/Inspect/DragDrop/FilteredDropTarget.h"
#endif

#include "Application/Inspect/Controls/InspectLabel.h"
#include "Application/Inspect/Controls/InspectValue.h"
#include "Application/Inspect/Controls/InspectChoice.h"
#include "Application/Inspect/Controls/InspectList.h"
#include "Application/Inspect/Controls/InspectAction.h"
#include "Application/Inspect/InspectData.h"
#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Interpreters/Reflect/InspectReflectInit.h"

#include "Core/Asset/AssetClass.h"
#include "Foundation/String/Tokenize.h"
#include "Foundation/Log.h"
#include "Application/UI/FileDialog.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

FileContainerInterpreter::FileContainerInterpreter (Container* labelContainer)
: ReflectFieldInterpreter (labelContainer)
, m_List( NULL )
, m_FileFilter( TXT( "" ) )
{

}

void FileContainerInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
  m_List = NULL;

  if (field->m_Flags & FieldFlags::Hide)
  {
    return;
  }

  bool isArray = ( field->m_SerializerID == Reflect::GetType<PathArraySerializer>() ) && ( field->m_Flags & FieldFlags::Path ) != 0;
  bool isSet = ( field->m_SerializerID == Reflect::GetType<PathSetSerializer>() ) && ( field->m_Flags & FieldFlags::Path ) != 0;
  bool isContainer = isArray || isSet;

  // create the label
  ContainerPtr labelContainer = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddChild( labelContainer );
  LabelPtr label = labelContainer->GetCanvas()->Create<Label>(this);
  labelContainer->AddChild( label );
  label->SetText( field->m_UIName );

  // create the list view
  ContainerPtr listContainer = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddChild( listContainer );
  ListPtr list = m_Container->GetCanvas()->Create<List>(this);
  m_List = list;
  listContainer->AddChild( list );

  // create the buttons
  ActionPtr editButton;
  ActionPtr findButton;
  ActionPtr addButton;
  ActionPtr removeButton;
  ActionPtr upButton;
  ActionPtr downButton;
  if ( !(field->m_Flags & FieldFlags::ReadOnly) )
  {
    addButton = m_Container->GetCanvas()->Create<Action>(this);
    if ( isFileIdContainer || ( field->m_Flags & FieldFlags::FilePath ) )
    {
      tstring filter;
      field->GetProperty( TXT("FileFilter"), filter );

      // Add button - normal file open dialog
      addButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnAddFile ) );
      addButton->SetClientData( new ClientDataFilter( list, instances.front()->GetType(), filter ) );

      // Add button - opens file browser
      findButton = m_Container->GetCanvas()->Create<Action>(this);
      findButton->SetIcon( "actions/system-search" );
      findButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnFindFile ) );
      findButton->SetClientData( new ClientDataFilter( list, instances.front()->GetType(), filter ) );

      // Edit button - attempt to edit the selected file
      editButton = m_Container->GetCanvas()->Create<Action>(this);
      editButton->SetText( "Edit" );
      editButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnEdit ) );
      editButton->SetClientData( new ClientData( list ) );
    }
    else
    {
      addButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnAdd ) );
      addButton->SetClientData( new ClientData( list ) );
    }
    addButton->SetText( "Add" );

    removeButton = m_Container->GetCanvas()->Create<Action>(this);
    removeButton->SetText( "Remove" );
    removeButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnRemove ) );
    removeButton->SetClientData( new ClientData( list ) );

    if ( isArray )
    {
      upButton = m_Container->GetCanvas()->Create<Action>(this);
      upButton->SetIcon( "actions/go-up" );
      upButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnMoveUp ) );
      upButton->SetClientData( new ClientData( list ) );

      downButton = m_Container->GetCanvas()->Create<Action>(this);
      downButton->SetIcon( "actions/go-down" );
      downButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnMoveDown ) );
      downButton->SetClientData( new ClientData( list ) );
    }

#ifdef INSPECT_REFACTOR
    Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( m_FinderSpec );
    filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &FileContainerInterpreter::OnDrop ) );
    m_List->SetDropTarget( filteredDropTarget );
#endif
  }

  // add the buttons to the panel
  ContainerPtr buttonGroup = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddChild( buttonGroup );
  if ( addButton )
  {
    buttonGroup->AddChild( addButton );
  }
  if ( findButton )
  {
    buttonGroup->AddChild( findButton );
  }
  if ( editButton )
  {
    buttonGroup->AddChild( editButton );
  }
  if ( removeButton )
  {
    buttonGroup->AddChild( removeButton );
  }
  if ( upButton )
  {
    buttonGroup->AddChild( upButton );
  }
  if ( downButton )
  {
    buttonGroup->AddChild( downButton );
  }

  // create the serializers
  std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
  std::vector<Reflect::Element*>::const_iterator end = instances.end();
  for ( ; itr != end; ++itr )
  {
    SerializerPtr s = field->CreateSerializer();

    if ( isFileIdContainer )
    {
      s->SetTranslateInputListener( Reflect::TranslateInputSignature::Delegate ( this, &FileContainerInterpreter::TranslateInputTUIDContainer ) );
      s->SetTranslateOutputListener( Reflect::TranslateOutputSignature::Delegate ( this, &FileContainerInterpreter::TranslateOutputTUIDContainer ) );
    }

    s->ConnectField(*itr, field);

    m_Serializers.push_back(s);
  }

  // bind the ui to the serializers
  Helium::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Serializer>( (std::vector<Reflect::Serializer*>&)m_Serializers );
  list->Bind( data );
  if ( addButton && ( isFileIdContainer || ( field->m_Flags & FieldFlags::FilePath ) ) )
  {
    addButton->Bind( data );
  }

  // setup the default value
  if (field->m_Default != NULL)
  {
    std::stringstream outStream;
    *field->m_Default >> outStream;
    list->SetDefault(outStream.str());
  }
}

void FileContainerInterpreter::OnAdd( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
  {
    ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
    wxTextEntryDialog dlg( m_Container->GetCanvas()->GetControl(), TXT( "" ), TXT( "Add" ) );
    if ( dlg.ShowModal() == wxID_OK )
    {
      tstring input = dlg.GetValue().c_str();
      if ( !input.empty() )
      {
        List* list = static_cast< List* >( data->m_Control );
        list->AddItem( input );
      }
    }

    button->GetCanvas()->Read();
  }
}

void FileContainerInterpreter::OnAddFile( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataFilter>() ) )
  {
    ClientDataFilter* data = static_cast< ClientDataFilter* >( clientData.Ptr() );

    HELIUM_BREAK();
#pragma TODO( "Reimplement to use an event that interfaces with the File Dialog" )
    //if ( browserDlg.ShowModal() == wxID_OK )
    //{
    //  tstring filePath = browserDlg.GetPath().c_str();
    //  data->m_List->AddItem( filePath );
    //}

    button->GetCanvas()->Read();
  }
}

void FileContainerInterpreter::OnFindFile( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataFilter>() ) )
  {
    ClientDataFilter* data = static_cast< ClientDataFilter* >( clientData.Ptr() );

    HELIUM_BREAK();
#pragma TODO( "Reimplement to use an event that interfaces with the Vault" )
    //if ( browserDlg.ShowModal() == wxID_OK )
    //{
    //  data->m_List->AddItem( browserDlg.GetPath().c_str() );
    //}

    button->GetCanvas()->Read();
  }
}

void FileContainerInterpreter::OnEdit( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
  {
    ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    const std::vector< tstring >& selectedItems = list->GetSelectedItems();

    // It would be nice to iterate over the selection here, but this is not safe since
    // each call to open can destroy this control and invalidate our iterator.
    if ( !selectedItems.empty() )
    {
      g_EditFilePath.Raise( EditFilePathArgs( *( selectedItems.begin() ) ) );
    }
  }
}

void FileContainerInterpreter::OnRemove( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
  {
    ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    const std::vector< tstring >& selectedItems = list->GetSelectedItems();
    if ( !selectedItems.empty() )
    {
      std::vector< tstring >::const_iterator itr = selectedItems.begin();
      std::vector< tstring >::const_iterator end = selectedItems.end();
      list->Freeze();
      for ( ; itr != end; ++itr )
      {
        const tstring& selection = *itr;
        list->RemoveItem( selection );
      }
      list->Thaw();
    }
  }
}

void FileContainerInterpreter::OnMoveUp( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
  {
    ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    list->MoveSelectedItems( Inspect::MoveDirections::Up );
  }
}

void FileContainerInterpreter::OnMoveDown( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
  {
    ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    list->MoveSelectedItems( Inspect::MoveDirections::Down );
  }
}

void FileContainerInterpreter::OnDrop( const Inspect::FilteredDropTargetArgs& args )
{
  if ( args.m_Paths.size() )
  {
    m_List->Freeze();
    for ( std::vector< tstring >::const_iterator itr = args.m_Paths.begin(), end = args.m_Paths.end();
      itr != end; ++itr )
    {
      m_List->AddItem( *itr );
    }
    m_List->Read();
    m_List->Thaw();
  }
}
