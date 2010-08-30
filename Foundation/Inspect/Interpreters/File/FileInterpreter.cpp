#include "FileInterpreter.h"

#include "Foundation/Inspect/InspectInit.h"
#include "Foundation/Inspect/InspectScript.h"
#include "Foundation/Inspect/InspectData.h"
#include "Foundation/Inspect/Controls/InspectButton.h"
#include "Foundation/Inspect/Controls/InspectValue.h"

#include "Core/Asset/AssetClass.h"
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

    ContainerPtr container = new Container ();
    groups.push_back( container );

    bool pathField = field->m_SerializerID == Reflect::GetType< PathSerializer >();
    bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;

    DataChangingSignature::Delegate changingDelegate;

    FileDialogButtonPtr fileDialogButton;

    //
    // Parse
    //
    tstring fieldUI;
    field->GetProperty( TXT( "UIScript" ), fieldUI );
    bool result = Script::Parse(fieldUI, this, parent->GetCanvas(), container, field->m_Flags);

    if (!result)
    {
        if ( pathField || field->m_SerializerID == Reflect::GetType<StringSerializer>() )
        {
            ContainerPtr valueContainer = new Container ();
            ValuePtr value = CreateControl< Value >();
            value->a_Justification.Set( Justifications::Right );
            value->a_IsReadOnly.Set( readOnly );
            valueContainer->AddChild( value );
            groups.push_back( valueContainer );

            if ( !readOnly )
            {
                changingDelegate = DataChangingSignature::Delegate(this, &FileInterpreter::DataChanging);

                // File dialog button
                fileDialogButton = CreateControl< FileDialogButton >();

                field->GetProperty( TXT( "FileFilter" ), m_FileFilter );

                if ( !m_FileFilter.empty() )
                {
                    fileDialogButton->a_Filter.Set( m_FileFilter );
                }
                container->AddChild( fileDialogButton );

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
                ButtonPtr editButton = CreateControl< Button >();
                editButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &FileInterpreter::Edit ) );
                editButton->a_Label.Set( TXT( "Edit" ) );
                container->AddChild( editButton );
            }
        }
    }
    else
    {
        ValuePtr value = CreateControl< Value >();
        value->a_IsReadOnly.Set( readOnly );
        container->AddChild( value );
    }


    //
    // Setup label
    //

    LabelPtr label = NULL;

    {
        V_Control::const_iterator itr = container->GetChildren().begin();
        V_Control::const_iterator end = container->GetChildren().end();
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
        label = CreateControl< Label >();
        tstring temp;
        bool converted = Helium::ConvertString( field->m_UIName, temp );
        HELIUM_ASSERT( converted );

        label->BindText( temp );

        container->InsertChild(0, label);
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
        container->a_Default.Set( temp );
    }

    //
    // Close
    //

    {
        std::vector<ContainerPtr>::const_iterator itr = groups.begin();
        std::vector<ContainerPtr>::const_iterator end = groups.end();
        for ( ; itr != end; ++itr )
        {
            parent->AddChild(*itr);
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

        path.TrimToExisting();

        FileDialogArgs fileDialogArgs( Helium::FileDialogTypes::OpenFile, TXT( "Path Does Not Exist" ), m_FileFilter, path );
        path = d_FindMissingFile.Invoke( fileDialogArgs );
        Reflect::Serializer::SetValue< tstring >( args.m_NewValue, path.Get() );
    }

    return true;
}

void FileInterpreter::Edit( const ButtonClickedArgs& args )
{
    tstring str;
    args.m_Control->ReadStringData( str );

    if ( !str.empty() )
    {
        g_EditFilePath.Raise( EditFilePathArgs( str ) );
    }
}

#ifdef INSPECT_REFACTOR

void FileInterpreter::OnDrop( const Inspect::FilteredDropTargetArgs& args )
{
    if ( args.m_Paths.size() )
    {
        m_Value->SetText( args.m_Paths[ 0 ] );
    }
}

#endif