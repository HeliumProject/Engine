#include "stdafx.h"
#include "FileInterpreter.h"
#include "FileDialogButton.h"
#include "FileBrowserButton.h"

#include "Inspect/InspectInit.h"
#include "Inspect/StringData.h"
#include "Inspect/Script.h"
#include "Inspect/Action.h"
#include "Inspect/Value.h"
#include "Inspect/ClipboardDataObject.h"
#include "Inspect/ClipboardFileList.h"

#include "Asset/AssetFlags.h"
#include "File/Manager.h"
#include "Finder/Finder.h"
#include "Finder/ExtensionSpecs.h"
#include "FileSystem/FileSystem.h"
#include "UIToolKit/FileDialog.h"
#include "Console/Console.h"
#include "Common/String/Wildcard.h"
#include "Common/String/Tokenize.h"

// Using
using namespace Reflect;
using namespace Inspect;

FileInterpreter::FileInterpreter (Container* container)
: ReflectFieldInterpreter (container)
, m_FinderSpec (NULL)
{

}

void FileInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
  if (field->m_Flags & FieldFlags::Hide)
  {
    return;
  }

  //
  // Create the ui we are generating
  //

  std::vector< ContainerPtr > groups;

  ContainerPtr group = m_Container->GetCanvas()->Create<Container>(this);
  groups.push_back( group );

  bool tuidField = field->m_SerializerID == Reflect::GetType<U64Serializer>() && field->m_Flags & FieldFlags::FileID;
  bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;

  DataChangingSignature::Delegate changingDelegate;

  FileDialogButtonPtr fileDialogButton;
  FileBrowserButtonPtr browserButton;
  const bool isTuidRequired = ( field->m_Flags & FieldFlags::FileID ) == FieldFlags::FileID ;

  //
  // Parse
  //
  std::string fieldUI;
  field->GetProperty( "UIScript", fieldUI );
  bool result = Script::Parse(fieldUI, this, parent->GetCanvas(), group, field->m_Flags);

  if (!result)
  {
    if ( tuidField || field->m_SerializerID == Reflect::GetType<StringSerializer>() )
    {
      ContainerPtr valueGroup = m_Container->GetCanvas()->Create<Container>(this);
      ValuePtr value = m_Container->GetCanvas()->Create<Value>(this);
      value->SetJustification( Value::kRight );
      value->SetReadOnly( readOnly );
      valueGroup->AddControl( value );
      groups.push_back( valueGroup );

      if ( tuidField || field->m_Flags & FieldFlags::FilePath ) 
      {
        if ( !readOnly )
        {
          changingDelegate = DataChangingSignature::Delegate(this, &FileInterpreter::DataChanging);

          // File dialog button
          fileDialogButton = m_Container->GetCanvas()->Create<FileDialogButton>(this);
          fileDialogButton->SetTuidRequired( isTuidRequired );

          m_FinderSpec = NULL;
          std::string specName;
          if ( field->GetProperty( "FilterSpec", specName ) )
          {
            m_FinderSpec = Finder::GetFinderSpec( specName );
          }
          else if ( field->GetProperty( "ModifierSpec", specName ) )
          {
            m_FinderSpec = Finder::GetFinderSpec( specName );
          }

          if ( m_FinderSpec )
          {
            fileDialogButton->SetFilter( m_FinderSpec->GetDialogFilter() );
          }
          group->AddControl( fileDialogButton );

          // File search button
          browserButton = m_Container->GetCanvas()->Create<FileBrowserButton>(this);
          browserButton->SetTuidRequired( ( field->m_Flags & FieldFlags::FileID ) == FieldFlags::FileID );
          if ( m_FinderSpec )
          {
            browserButton->SetFilter( m_FinderSpec->GetDialogFilter() );
          }
          group->AddControl( browserButton );

          Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( m_FinderSpec );
          filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &FileInterpreter::OnDrop ) );

          m_Value = value;
          m_Value->SetDropTarget( filteredDropTarget );
        }

        if ( instances.size() == 1 )
        {
          // File edit button
          ActionPtr editButton = m_Container->GetCanvas()->Create<Action>(this);
          editButton->AddListener( ActionSignature::Delegate ( this, &FileInterpreter::Edit ) );
          editButton->SetText("Edit");
          group->AddControl( editButton );
        }

        if ( field->m_Flags & Asset::AssetFlags::RealTimeUpdateable )
        {
          // File refresh button
          ButtonPtr refreshButton = m_Container->GetCanvas()->Create<Button>(this);
          refreshButton->SetText("Reload");
          group->AddControl( refreshButton );
        }
      }
    }
    else
    {
      ValuePtr value = m_Container->GetCanvas()->Create<Value>( this );
      value->SetReadOnly( readOnly );
      group->AddControl( value );
    }
  }

  //
  // Setup label
  //

  LabelPtr label = NULL;

  {
    V_Control::const_iterator itr = group->GetControls().begin();
    V_Control::const_iterator end = group->GetControls().end();
    for( ; itr != end; ++itr )
    {
      Label* label = Reflect::ObjectCast<Label>( *itr );
      if (label)
      {
        break;
      }
    }
  }

  if (label == NULL)
  {
    label = group->GetCanvas()->Create<Label>(this);
    label->SetText( field->m_UIName );

    group->InsertControl(0, label);
  }

  //
  // Create type m_FinderSpecific data bound to this and additional instances
  //

  std::vector<Serializer*> ser;

  {
    std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Element*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
      SerializerPtr s = field->CreateSerializer();

      if (s->HasType(Reflect::GetType<ContainerSerializer>()))
      {
        return;
      }

      if (tuidField)
      {
        s->SetTranslateInputListener( Reflect::TranslateInputSignature::Delegate ( this, &FileInterpreter::TranslateInputTUID ) );
        s->SetTranslateOutputListener( Reflect::TranslateOutputSignature::Delegate ( this, &FileInterpreter::TranslateOutputTUID ) );
      }

      s->ConnectField(*itr, field);

      ser.push_back(s);

      m_Serializers.push_back(s);
    }
  }

  //
  // Create data and bind
  //

  Nocturnal::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Serializer>( ser );

  if (changingDelegate.Valid())
  {
    data->AddChangingListener( changingDelegate );
  }

  {
    std::vector<ContainerPtr>::const_iterator itr = groups.begin();
    std::vector<ContainerPtr>::const_iterator end = groups.end();
    for ( ; itr != end; ++itr )
    {
      (*itr)->Bind( data );
    }
  }

  //
  // Set default
  //

  if (field->m_Default != NULL)
  {
    std::stringstream outStream;

    *field->m_Default >> outStream;

    group->SetDefault(outStream.str());
  }

  //
  // Close
  //

  {
    std::vector<ContainerPtr>::const_iterator itr = groups.begin();
    std::vector<ContainerPtr>::const_iterator end = groups.end();
    for ( ; itr != end; ++itr )
    {
      parent->AddControl(*itr);
    }
  }
}

void FileInterpreter::TranslateInputTUID( Reflect::TranslateInputEventArgs& args )
{
  if (args.m_Serializer->GetType() == Reflect::GetType<Reflect::U64Serializer>())
  {
    U64Serializer* ser = DangerousCast< U64Serializer > ( args.m_Serializer );

    std::string path;
    std::getline( args.m_Stream, path );

    if ( !path.empty() )
    {
      tuid fileID = TUID::Null;
      if ( path.at( 0 ) == '<' && path.at( path.size() - 1 ) == '>' )
      {
        std::string fileStr = path.substr( 1, path.size() - 2 );
        bool parsed = TUID::Parse( fileStr, fileID );
        NOC_ASSERT( parsed );
      }
      else
      {
        try
        {
          fileID = File::GlobalManager().GetID( path );
        }
        catch ( const Nocturnal::Exception& e )
        {
          Console::Warning( "No TUID was found for path '%s' (Reason: %s); discarding.\n", path.c_str(), e.what() );
        }
      }

      if ( fileID != TUID::Null )
      {
        ser->m_Data.Set( fileID );
      }
    }
    else
    {
      ser->m_Data.Set( TUID::Null );
    }
  }
  else
  {
    NOC_BREAK();
  }
}

void FileInterpreter::TranslateOutputTUID( Reflect::TranslateOutputEventArgs& args )
{
  if ( args.m_Serializer->GetType() == Reflect::GetType<Reflect::U64Serializer>() )
  {
    U64Serializer* ser = DangerousCast< U64Serializer > ( args.m_Serializer );
    std::string path;

    bool set = false;

    if ( ser->m_Data.Get() != TUID::Null )
    {

      if ( File::GlobalManager().GetPath( ser->m_Data.Get(), path ) )
      {
        set = true;
      }
    }

    if ( !set && ser->m_Data.Val() != TUID::Null )
		{
      char buf[80];
      _snprintf( buf, sizeof(buf), "<"TUID_HEX_FORMAT">", ser->m_Data.Val() );
      buf[ sizeof(buf) - 1] = 0; 

      path = buf;
		}

    args.m_Stream << path;
  }
  else
  {
    NOC_BREAK();
  }
}

bool FileInterpreter::DataChanging( DataChangingArgs& args )
{
  std::string text;
  Reflect::Serializer::GetValue( args.m_NewValue, text );

  if ( !text.empty() )
  {
    FileSystem::CleanName( text, false );

    if ( FileSystem::IsFile( text ) )
    {
      return true;
    }

    std::string dir;
    if ( FileSystem::IsFolder( text ) )
    {
      dir = text;
    }

    // case 1: the path is right but the file is wrong
    if ( dir.empty() )
    {
      char drive[MAX_PATH], folder[MAX_PATH], file[MAX_PATH], ext[MAX_PATH];
      _splitpath(text.c_str(), drive, folder, file, ext);
      std::ostringstream directory;
      directory << drive << folder;
      if (FileSystem::Exists( directory.str() ))
      {
        dir = directory.str();
      }
    }

    // case 2: get as close as we can to a valid directory
    if (dir.empty())
    {
      std::string temp;
      const char* token = strtok(&temp[0], "/");
      while (token)
      {
        temp = temp + token + "/";
        if ((strlen(token) == 2 && token[1] == ':') || FileSystem::Exists(temp))
        {
          dir = dir + token + "/";
          token = strtok(NULL, "/");
        }
        else
        {
          token = NULL;
        }
      }
    }

    if (!dir.empty())
    {
      text = dir;
    }

    UIToolKit::FileDialog dialog ( m_Container->GetWindow(), wxFileSelectorPromptStr, text.c_str() );

    if (m_FinderSpec)
    {
      dialog.SetFilter( m_FinderSpec->GetDialogFilter() );
    }

    if ( dialog.ShowModal() == wxID_OK )
    {
      Reflect::Serializer::SetValue< std::string >( args.m_NewValue, dialog.GetPath().c_str() );
    }
  }

  return true;
}

void FileInterpreter::Edit( Button* button )
{
  StringData* data = static_cast<StringData*>(button->GetData().Ptr());

  std::string str;
  data->Get( str );

  if ( !str.empty() )
  {
    g_EditFilePath.Raise( EditFilePathArgs( str ) );
  }
}

void FileInterpreter::OnDrop( const Inspect::FilteredDropTargetArgs& args )
{
  if ( args.m_Paths.size() )
  {
    File::GlobalManager().Open( args.m_Paths[0] );
    m_Value->SetText( args.m_Paths[ 0 ] );
  }
}