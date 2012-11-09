#include "InspectPch.h"
#include "ReflectPathInterpreter.h"

#include "Inspect/Inspect.h"
#include "Inspect/Script.h"
#include "Inspect/DataBinding.h"
#include "Inspect/Controls/ButtonControl.h"
#include "Inspect/Controls/ValueControl.h"

#include "Foundation/Log.h"
#include "Foundation/Wildcard.h"
#include "Foundation/Tokenize.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

PathInterpreter::PathInterpreter (Container* container)
: ReflectFieldInterpreter (container)
, m_FileFilter( TXT( "" ) )
{

}

void PathInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Object*>& instances, Container* parent)
{
    if (field->m_Flags & FieldFlags::Hide)
    {
        return;
    }

    //
    // Create the ui we are generating
    //

    std::vector< ContainerPtr > groups;

    ContainerPtr container = CreateControl<Container>();
    groups.push_back( container );

    bool pathField = field->m_DataClass == Reflect::GetClass< PathData >();
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
        if ( pathField || field->m_DataClass == Reflect::GetClass<StlStringData>() )
        {
            ContainerPtr valueContainer = CreateControl<Container>();
            ValuePtr value = CreateControl< Value >();
            value->a_Justification.Set( Justifications::Right );
            value->a_IsReadOnly.Set( readOnly );
            value->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
            valueContainer->AddChild( value );
            groups.push_back( valueContainer );

            if ( !readOnly )
            {
                changingDelegate = DataChangingSignature::Delegate(this, &PathInterpreter::DataChanging);

                // File dialog button
                fileDialogButton = CreateControl< FileDialogButton >();
                fileDialogButton->a_HelpText.Set( TXT( "Open a file dialog to choose a new file." ) );

                field->GetProperty( TXT( "FileFilter" ), m_FileFilter );

                if ( !m_FileFilter.empty() )
                {
                    fileDialogButton->a_Filter.Set( m_FileFilter );
                }
                container->AddChild( fileDialogButton );

                value->SetProperty( TXT( "FileFilter" ), m_FileFilter );

                m_Value = value;
            }

            if ( instances.size() == 1 )
            {
                // File edit button
                ButtonPtr editButton = CreateControl< Button >();
                editButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathInterpreter::Edit ) );
                editButton->a_Label.Set( TXT( "Edit" ) );
                editButton->a_HelpText.Set( TXT( "Attempt to edit the file using its associated default application." ) );
                container->AddChild( editButton );
            }
        }
    }
    else
    {
        ValuePtr value = CreateControl< Value >();
        value->a_IsReadOnly.Set( readOnly );
        value->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
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
            Label* label = Reflect::SafeCast<Label>( *itr );
            if (label)
            {
                break;
            }
        }
    }

    if (!label.ReferencesObject())
    {
        label = CreateControl< Label >();

        tstring temp;
        field->GetProperty( TXT( "UIName" ), temp );
        if ( temp.empty() )
        {
            bool converted = Helium::ConvertString( field->m_Name, temp );
            HELIUM_ASSERT( converted );
        }

        label->BindText( temp );
        label->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );

        container->InsertChild(0, label);
    }

    //
    // Create type m_FinderSpecific data bound to this and additional instances
    //

    std::vector<Data*> ser;

    {
        std::vector<Reflect::Object*>::const_iterator itr = instances.begin();
        std::vector<Reflect::Object*>::const_iterator end = instances.end();
        for ( ; itr != end; ++itr )
        {
            DataPtr s = field->CreateData();

            if (s->IsClass(Reflect::GetClass<ContainerData>()))
            {
                return;
            }

            s->ConnectField(*itr, field);

            ser.push_back(s);

            m_Datas.push_back(s);
        }
    }

    //
    // Create data and bind
    //

    Helium::SmartPtr< MultiStringFormatter<Data> > data = new MultiStringFormatter<Data>( ser );

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

    DataPtr defaultData = field->CreateDefaultData();
    if (defaultData.ReferencesObject())
    {
        tstringstream defaultStream;
        *defaultData >> defaultStream;
        container->a_Default.Set( defaultStream.str() );
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

void PathInterpreter::DataChanging( const DataChangingArgs& args )
{
    tstring text;
    Reflect::Data::GetValue( args.m_NewValue, text );

    if ( !text.empty() )
    {
        Helium::FilePath path( text );

        if ( path.IsFile() )
        {
            return;
        }

        path.TrimToExisting();

        FileDialogArgs fileDialogArgs( Helium::FileDialogTypes::OpenFile, TXT( "FilePath Does Not Exist" ), m_FileFilter, path );
        d_FindMissingFile.Invoke( fileDialogArgs );
        Reflect::Data::SetValue< tstring >( args.m_NewValue, fileDialogArgs.m_Result.Get() );
    }
}

void PathInterpreter::Edit( const ButtonClickedArgs& args )
{
    tstring str;
    args.m_Control->ReadStringData( str );

    if ( !str.empty() )
    {
        g_EditFilePath.Raise( EditFilePathArgs( str ) );
    }
}
