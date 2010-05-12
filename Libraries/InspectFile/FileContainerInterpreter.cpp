#include "stdafx.h"
#include "FileContainerInterpreter.h"
#include "InspectFileInit.h"

#include <wx/aui/aui.h>

#include "Inspect/FilteredDropTarget.h"
#include "Inspect/Label.h"
#include "Inspect/Value.h"
#include "Inspect/Choice.h"
#include "Inspect/List.h"
#include "Inspect/Action.h"
#include "Inspect/StringData.h"
#include "Inspect/InspectInit.h"
#include "InspectReflect/InspectReflectInit.h"

#include "Asset/AssetFlags.h"
#include "Common/String/Tokenize.h"
#include "Console/Console.h"
#include "File/Manager.h"
#include "FileBrowser/FileBrowser.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/ExtensionSpecs.h"
#include "UIToolKit/FileDialog.h"

using namespace Reflect;
using namespace Inspect;
using namespace UIToolKit;

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

  bool isFileIdArray = ( field->m_SerializerID == Reflect::GetType<U64ArraySerializer>() ) && ( field->m_Flags & FieldFlags::FileID ) != 0;
  bool isFileIdSet = ( field->m_SerializerID == Reflect::GetType<U64SetSerializer>() ) && ( field->m_Flags & FieldFlags::FileID ) != 0;
  bool isFileIdContainer = isFileIdArray || isFileIdSet;

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
}

void FileContainerInterpreter::TranslateInputTUIDContainer( Reflect::TranslateInputEventArgs& args )
{
  bool isArraySerializer = args.m_Serializer->GetType() == Reflect::GetType<U64ArraySerializer>();
  bool isSetSerializer = args.m_Serializer->GetType() == Reflect::GetType<U64SetSerializer>();

  if ( isArraySerializer || isSetSerializer )
  {
    std::string path;
    std::string value;
    while ( !args.m_Stream.eof() )
    {
      std::getline(args.m_Stream, value);
      if ( !path.empty() )
      {
        path += "\n"; // Replace newline characters in case they are the delimiters
      }
      path += value;
    }

    if ( !path.empty() )
    {
      V_string pathList;
      ::Tokenize( path, pathList, s_ContainerItemDelimiter );
      V_string::const_iterator itr = pathList.begin();
      V_string::const_iterator end = pathList.end();
      for ( ; itr != end; ++itr )
      {
        tuid fileID = TUID::Null;

        if ( itr->at( 0 ) == '<' && itr->at( itr->size() - 1 ) == '>' )
        {
          std::string fileStr = (*itr).substr( 1, itr->size() - 2 );
          bool parsed = TUID::Parse( fileStr, fileID );
          NOC_ASSERT(parsed);
        }
        else
        {
          try
          {
            fileID = File::GlobalManager().GetID( *itr );
          }
          catch ( const Nocturnal::Exception& e )
          {
            Console::Warning( "No path was found for TUID '%s' (Reason: %s); discarding.\n", (*itr).c_str(), e.what() );
          }
        }

        if ( fileID != TUID::Null )
        {
          if ( isArraySerializer )
          {
            U64ArraySerializer* ser = AssertCast< U64ArraySerializer > ( args.m_Serializer );
          ser->m_Data->push_back( fileID );
        }
          else if ( isSetSerializer )
          {
            U64SetSerializer* ser = AssertCast< U64SetSerializer > ( args.m_Serializer );
            ser->m_Data->insert( fileID );
      }
    }
  }
    }
  }
  else
  {
    NOC_BREAK();
  }
}

void FileContainerInterpreter::TranslateOutputTUID( std::string& path, const u64& fileId )
  {
      bool set = false;

      if ( fileId != TUID::Null )
      {
        if ( !path.empty() )
        {
          path += s_ContainerItemDelimiter;
        }

        std::string filePath;
        if ( File::GlobalManager().GetPath( fileId, filePath ) )
        {
          path += filePath;
          set = true;
        }
      }

      if ( !set )
      {
        char buf[80];
        _snprintf( buf, sizeof(buf), "<"TUID_HEX_FORMAT">", fileId );
        buf[ sizeof(buf) - 1] = 0; 

        path += buf;
      }
    }

void FileContainerInterpreter::TranslateOutputTUIDContainer( Reflect::TranslateOutputEventArgs& args )
{
  bool isArraySerializer = args.m_Serializer->GetType() == Reflect::GetType<U64ArraySerializer>();
  bool isSetSerializer = args.m_Serializer->GetType() == Reflect::GetType<U64SetSerializer>();

  if ( isArraySerializer )
  {
    U64ArraySerializer* ser = AssertCast< U64ArraySerializer > ( args.m_Serializer );
    V_u64::iterator itr = ser->m_Data->begin();
    V_u64::iterator end = ser->m_Data->end();

    std::string path;
    for ( ; itr != end; ++itr )
    {
      const u64& fileId = *itr;
      TranslateOutputTUID( path, fileId );
    }
    args.m_Stream << path;
  }
  else if ( isSetSerializer )
  {
    U64SetSerializer* ser = AssertCast< U64SetSerializer > ( args.m_Serializer );
    S_u64::iterator itr = ser->m_Data->begin();
    S_u64::iterator end = ser->m_Data->end();

    std::string path;
    for ( ; itr != end; ++itr )
    {
      const u64& fileId = *itr;
      TranslateOutputTUID( path, fileId );
    }
    args.m_Stream << path;
  }
  else
  {
    NOC_BREAK();
  }
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
      try
      {
        File::GlobalManager().Open( filePath );
      }
      catch ( const File::Exception& e )
      {
        Console::Error( "%s\n", e.what() );
        return;
      }
      
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

    File::FileBrowser browserDlg( button->GetCanvas()->GetControl(), wxID_ANY, "Add Asset to List" );
    browserDlg.SetTuidRequired( true );

    if ( !data->m_FinderSpec.empty() )
    {
      const Finder::FinderSpec* spec = Finder::GetFinderSpec( data->m_FinderSpec );
      browserDlg.SetFilter( *spec );
      browserDlg.SetFilterIndex( *spec );
    }
    else
    {
      browserDlg.SetFilter( FinderSpecs::Extension::ALL_FILTER );
    }

    if ( browserDlg.ShowModal() == wxID_OK )
    {
      data->m_List->AddItem( browserDlg.GetPath().c_str() );
    }

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
      // Make sure all the files are in the file resolver
      File::GlobalManager().Open( *itr );
      m_List->AddItem( *itr );
    }
    m_List->Read();
    m_List->Thaw();
  }
}
