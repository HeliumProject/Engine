#include "FileInterpreter.h"
#include "FileDialogButton.h"
#include "FileBrowserButton.h"

#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/InspectScript.h"
#include "Application/Inspect/InspectData.h"
#include "Application/Inspect/Controls/InspectAction.h"
#include "Application/Inspect/Controls/InspectValue.h"
#include "Application/Inspect/Clipboard/ClipboardDataObject.h"
#include "Application/Inspect/Clipboard/ClipboardFileList.h"

#include "Core/Asset/AssetClass.h"
#include "Application/UI/FileDialog.h"
#include "Foundation/Log.h"
#include "Foundation/String/Wildcard.h"
#include "Foundation/String/Tokenize.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

FileInterpreter::FileInterpreter (Container* container)
: ReflectFieldInterpreter (container)
, m_FileFilter( TXT( "" ) )
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

    bool pathField = field->m_SerializerID == Reflect::GetType< PointerSerializer >() && field->m_Flags & FieldFlags::Path;
    bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;

    DataChangingSignature::Delegate changingDelegate;

    FileDialogButtonPtr fileDialogButton;
    FileBrowserButtonPtr browserButton;

    //
    // Parse
    //
    tstring fieldUI;
    field->GetProperty( TXT( "UIScript" ), fieldUI );
    bool result = Script::Parse(fieldUI, this, parent->GetCanvas(), group, field->m_Flags);

    if (!result)
    {
        if ( pathField || field->m_SerializerID == Reflect::GetType<StringSerializer>() )
        {
            ContainerPtr valueGroup = m_Container->GetCanvas()->Create<Container>(this);
            ValuePtr value = m_Container->GetCanvas()->Create<Value>(this);
            value->SetJustification( Value::kRight );
            value->SetReadOnly( readOnly );
            valueGroup->AddControl( value );
            groups.push_back( valueGroup );

            if ( pathField || field->m_Flags & FieldFlags::FilePath ) 
            {
                if ( !readOnly )
                {
                    changingDelegate = DataChangingSignature::Delegate(this, &FileInterpreter::DataChanging);

                    // File dialog button
                    fileDialogButton = m_Container->GetCanvas()->Create<FileDialogButton>(this);

                    field->GetProperty( TXT( "FileFilter" ), m_FileFilter );

                    if ( !m_FileFilter.empty() )
                    {
                        fileDialogButton->SetFilter( m_FileFilter );
                    }
                    group->AddControl( fileDialogButton );

                    // File search button
                    browserButton = m_Container->GetCanvas()->Create<FileBrowserButton>(this);
                    if ( !m_FileFilter.empty() )
                    {
                        browserButton->SetFilter( m_FileFilter );
                    }
                    group->AddControl( browserButton );

#ifdef INSPECT_REFACTOR
                    Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( m_FileFilter );
                    filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &FileInterpreter::OnDrop ) );
                    value->SetDropTarget( filteredDropTarget );
#endif
                    m_Value = value;
                }

                if ( instances.size() == 1 )
                {
                    // File edit button
                    ActionPtr editButton = m_Container->GetCanvas()->Create<Action>(this);
                    editButton->AddListener( ActionSignature::Delegate ( this, &FileInterpreter::Edit ) );
                    editButton->SetText( TXT( "Edit" ) );
                    group->AddControl( editButton );
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
        tstring temp;
        bool converted = Helium::ConvertString( field->m_UIName, temp );
        HELIUM_ASSERT( converted );

        label->SetText( temp );

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

            s->ConnectField(*itr, field);

            ser.push_back(s);

            m_Serializers.push_back(s);
        }
    }

    //
    // Create data and bind
    //

    Helium::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Serializer>( ser );

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
        tstringstream outStream;
        *field->m_Default >> outStream;

        tstring temp;
        bool converted = Helium::ConvertString( outStream.str().c_str(), temp );
        HELIUM_ASSERT( converted );
        group->SetDefault( temp );
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

bool FileInterpreter::DataChanging( DataChangingArgs& args )
{
    tstring text;
    Reflect::Serializer::GetValue( args.m_NewValue, text );

    if ( !text.empty() )
    {
        Helium::Path path( text );

        if ( path.IsFile() )
        {
            return true;
        }

        tstring dir;
        if ( path.IsDirectory() )
        {
            dir = path.Get();
        }

        // case 1: the path is right but the file is wrong
        if ( dir.empty() )
        {
            tchar drive[MAX_PATH], folder[MAX_PATH], file[MAX_PATH], ext[MAX_PATH];
            _tsplitpath(text.c_str(), drive, folder, file, ext);
            tostringstream directory;
            directory << drive << folder;
            if ( Helium::Path( directory.str() ).Exists() )
            {
                dir = directory.str();
            }
        }

        // case 2: get as close as we can to a valid directory
        if (dir.empty())
        {
            tstring temp;
            const tchar* token = _tcstok(&temp[0], TXT( "/" ) );
            while (token)
            {
                temp = temp + token + TXT( "/" );
                if ((_tcslen(token) == 2 && token[1] == ':') || Helium::Path( temp ).Exists() )
                {
                    dir = dir + token + TXT( "/" );
                    token = _tcstok(NULL, TXT( "/" ) );
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

        Helium::FileDialog dialog ( m_Container->GetWindow(), wxFileSelectorPromptStr, text.c_str() );

        if ( !m_FileFilter.empty() )
        {
            dialog.SetFilter( m_FileFilter );
        }

        if ( dialog.ShowModal() == wxID_OK )
        {
            const wxChar* str = dialog.GetPath().c_str();
            Reflect::Serializer::SetValue< tstring >( args.m_NewValue, str );
        }
    }

    return true;
}

void FileInterpreter::Edit( Button* button )
{
    StringData* data = static_cast<StringData*>(button->GetData().Ptr());

    tstring str;
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
        m_Value->SetText( args.m_Paths[ 0 ] );
    }
}