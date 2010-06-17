#include "FileContainerInterpreter.h"
#include "InspectFileInit.h"

#include <wx/aui/aui.h>

#include "Application/Inspect/DragDrop/FilteredDropTarget.h"
#include "Application/Inspect/Widgets/Other Controls/Label.h"
#include "Application/Inspect/Widgets/Text Controls/Value.h"
#include "Application/Inspect/Widgets/Text Controls/Choice.h"
#include "Application/Inspect/Widgets/Other Controls/List.h"
#include "Application/Inspect/Widgets/Button Controls/Action.h"
#include "Application/Inspect/Data/StringData.h"
#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Reflect/InspectReflectInit.h"

#include "Pipeline/Asset/AssetFlags.h"
#include "Foundation/String/Tokenize.h"
#include "Foundation/Log.h"
#include "Finder/Finder.h"
#include "Finder/ExtensionSpecs.h"
#include "Application/UI/FileDialog.h"

using namespace Reflect;
using namespace Inspect;
using namespace Nocturnal;

FileContainerInterpreter::FileContainerInterpreter (Container* labelContainer)
: ReflectFieldInterpreter (labelContainer)
, m_List( NULL )
, m_FinderSpec( NULL )
{

}

void FileContainerInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
  m_List = NULL;

  if (field->m_Flags & FieldFlags::Hide)
  {
    return;
  }

  return;
#pragma TODO( "support Nocturnal::S_Path and File::V_Reference" )
/*
  bool isfileidarray = ( field->m_serializerid == reflect::gettype<u64arrayserializer>() ) && ( field->m_flags & fieldflags::fileid ) != 0;
  bool isfileidset = ( field->m_serializerid == reflect::gettype<u64setserializer>() ) && ( field->m_flags & fieldflags::fileid ) != 0;
  bool isfileidcontainer = isfileidarray || isfileidset;

  // create the label
  ContainerPtr labelContainer = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddControl( labelContainer );
  LabelPtr label = labelContainer->GetCanvas()->Create<Label>(this);
  labelContainer->AddControl( label );
  label->SetText( field->m_UIName );

  // create the list view
  ContainerPtr listContainer = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddControl( listContainer );
  ListPtr list = m_Container->GetCanvas()->Create<List>(this);
  m_List = list;
  listContainer->AddControl( list );

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
      std::string specName;
      field->GetProperty( "FilterSpec", specName );
      if ( specName.empty() )
      {
        field->GetProperty( "ModifierSpec", specName );
      }
      if ( !specName.empty() )
      {
        m_FinderSpec = Finder::GetFinderSpec( specName );
      }

      // Add button - normal file open dialog
      addButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnAddFile ) );
      addButton->SetClientData( new ClientDataFilter( list, instances.front()->GetType(), specName ) );

      // Add button - opens file browser
      findButton = m_Container->GetCanvas()->Create<Action>(this);
      findButton->SetIcon( "magnify_16.png" );
      findButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnFindFile ) );
      findButton->SetClientData( new ClientDataFilter( list, instances.front()->GetType(), specName ) );

      // Edit button - attempt to edit the selected file
      editButton = m_Container->GetCanvas()->Create<Action>(this);
      editButton->SetText( "Edit" );
      editButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnEdit ) );
      editButton->SetClientData( new ClientDataControl( list ) );
    }
    else
    {
      addButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnAdd ) );
      addButton->SetClientData( new ClientDataControl( list ) );
    }
    addButton->SetText( "Add" );

    removeButton = m_Container->GetCanvas()->Create<Action>(this);
    removeButton->SetText( "Remove" );
    removeButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnRemove ) );
    removeButton->SetClientData( new ClientDataControl( list ) );

    if ( isFileIdArray )
    {
      upButton = m_Container->GetCanvas()->Create<Action>(this);
      upButton->SetIcon( "arrow_up_16.png" );
      upButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnMoveUp ) );
      upButton->SetClientData( new ClientDataControl( list ) );

      downButton = m_Container->GetCanvas()->Create<Action>(this);
      downButton->SetIcon( "arrow_down_16.png" );
      downButton->AddListener( ActionSignature::Delegate ( this, &FileContainerInterpreter::OnMoveDown ) );
      downButton->SetClientData( new ClientDataControl( list ) );
    }

    Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( m_FinderSpec );
    filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &FileContainerInterpreter::OnDrop ) );
    m_List->SetDropTarget( filteredDropTarget );
  }

  // add the buttons to the panel
  ContainerPtr buttonGroup = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddControl( buttonGroup );
  if ( addButton )
  {
    buttonGroup->AddControl( addButton );
  }
  if ( findButton )
  {
    buttonGroup->AddControl( findButton );
  }
  if ( editButton )
  {
    buttonGroup->AddControl( editButton );
  }
  if ( removeButton )
  {
    buttonGroup->AddControl( removeButton );
  }
  if ( upButton )
  {
    buttonGroup->AddControl( upButton );
  }
  if ( downButton )
  {
    buttonGroup->AddControl( downButton );
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
  Nocturnal::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Serializer>( (std::vector<Reflect::Serializer*>&)m_Serializers );
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
  */
}

void FileContainerInterpreter::OnAdd( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    wxTextEntryDialog dlg( m_Container->GetCanvas()->GetControl(), "", "Add" );
    if ( dlg.ShowModal() == wxID_OK )
    {
      std::string input = dlg.GetValue().c_str();
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

    FileDialog browserDlg( button->GetCanvas()->GetControl() );

    if ( !data->m_FinderSpec.empty() )
    {
      const Finder::FinderSpec* spec = Finder::GetFinderSpec( data->m_FinderSpec );
      browserDlg.SetFilter( spec->GetDialogFilter() );
    }
    else
    {
      browserDlg.SetFilter( FinderSpecs::Extension::ALL_FILTER.GetDialogFilter() );
    }

    if ( browserDlg.ShowModal() == wxID_OK )
    {
      std::string filePath = browserDlg.GetPath().c_str();
      data->m_List->AddItem( filePath );
    }

    button->GetCanvas()->Read();
  }
}

void FileContainerInterpreter::OnFindFile( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataFilter>() ) )
  {
    ClientDataFilter* data = static_cast< ClientDataFilter* >( clientData.Ptr() );

      NOC_BREAK();
#pragma TODO( "Reimplement to use the Vault" )
    //File::FileBrowser browserDlg( button->GetCanvas()->GetControl(), wxID_ANY, "Add Asset to List" );

    //if ( !data->m_FinderSpec.empty() )
    //{
    //  const Finder::FinderSpec* spec = Finder::GetFinderSpec( data->m_FinderSpec );
    //  browserDlg.SetFilter( *spec );
    //  browserDlg.SetFilterIndex( *spec );
    //}
    //else
    //{
    //  browserDlg.SetFilter( FinderSpecs::Extension::ALL_FILTER );
    //}

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
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    const V_string& selectedItems = list->GetSelectedItems();

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
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    const V_string& selectedItems = list->GetSelectedItems();
    if ( !selectedItems.empty() )
    {
      V_string::const_iterator itr = selectedItems.begin();
      V_string::const_iterator end = selectedItems.end();
      list->Freeze();
      for ( ; itr != end; ++itr )
      {
        const std::string& selection = *itr;
        list->RemoveItem( selection );
      }
      list->Thaw();
    }
  }
}

void FileContainerInterpreter::OnMoveUp( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    list->MoveSelectedItems( Inspect::MoveDirections::Up );
  }
}

void FileContainerInterpreter::OnMoveDown( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    list->MoveSelectedItems( Inspect::MoveDirections::Down );
  }
}

void FileContainerInterpreter::OnDrop( const Inspect::FilteredDropTargetArgs& args )
{
  if ( args.m_Paths.size() )
  {
    m_List->Freeze();
    for ( V_string::const_iterator itr = args.m_Paths.begin(), end = args.m_Paths.end();
      itr != end; ++itr )
    {
      m_List->AddItem( *itr );
    }
    m_List->Read();
    m_List->Thaw();
  }
}
