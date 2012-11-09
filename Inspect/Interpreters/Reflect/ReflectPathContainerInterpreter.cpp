#include "InspectPch.h"
#include "ReflectPathContainerInterpreter.h"

#include "Inspect/Controls/LabelControl.h"
#include "Inspect/Controls/ValueControl.h"
#include "Inspect/Controls/ChoiceControl.h"
#include "Inspect/Controls/ListControl.h"
#include "Inspect/Controls/ButtonControl.h"
#include "Inspect/DataBinding.h"
#include "Inspect/Inspect.h"
#include "Inspect/Interpreters/Reflect/InspectReflectInit.h"

#include "Foundation/Tokenize.h"
#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

PathContainerInterpreter::PathContainerInterpreter (Container* labelContainer)
: ReflectFieldInterpreter (labelContainer)
, m_List( NULL )
{
}

void PathContainerInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Object*>& instances, Container* parent)
{
    m_List = NULL;

    if (field->m_Flags & FieldFlags::Hide)
    {
        return;
    }

    bool isVector = ( field->m_DataClass == Reflect::GetClass<PathStlVectorData>() );
    bool isSet = ( field->m_DataClass == Reflect::GetClass<PathStlSetData>() );
    bool isContainer = isVector || isSet;

    // create the label
    ContainerPtr labelContainer = CreateControl< Container >();
    parent->AddChild( labelContainer );

    LabelPtr label = CreateControl< Label >();
    labelContainer->AddChild( label );

    tstring temp;
    field->GetProperty( TXT( "UIName" ), temp );
    if ( temp.empty() )
    {
        bool converted = Helium::ConvertString( field->m_Name, temp );
        HELIUM_ASSERT( converted );
    }

    label->BindText( temp );
    label->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );

    // only allow modification if we've only got one backing list
    label->a_IsEnabled.Set( instances.size() == 1 );

    // create the list view
    ContainerPtr listContainer = CreateControl<Container>();
    parent->AddChild( listContainer );

    ListPtr list = CreateControl< List >();
    m_List = list;
    listContainer->AddChild( list );
    m_List->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
    listContainer->a_IsEnabled.Set( instances.size() == 1 );

    // create the buttons
    ButtonPtr editButton;
    ButtonPtr findButton;
    ButtonPtr addButton;
    ButtonPtr removeButton;
    ButtonPtr upButton;
    ButtonPtr downButton;
    if ( !(field->m_Flags & FieldFlags::ReadOnly) )
    {
        tstring filter;
        field->GetProperty( TXT("FileFilter"), filter );

        addButton = CreateControl< Button >();
        if ( isContainer )
        {
            // Add button - normal file open dialog
            addButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnAddFile ) );
            addButton->SetClientData( new ClientDataFilter( list, instances.front()->GetClass(), filter ) );

            // Add button - opens file browser
            findButton = CreateControl< Button >();
            findButton->a_Icon.Set( TXT( "actions/system-search" ) );
            findButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnFindFile ) );
            findButton->SetClientData( new ClientDataFilter( list, instances.front()->GetClass(), filter ) );

            // Edit button - attempt to edit the selected file
            editButton = CreateControl< Button >();
            editButton->a_Label.Set( TXT( "Edit" ) );
            editButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnEdit ) );
            editButton->SetClientData( new ClientData( list ) );
        }
        else
        {
            addButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnAdd ) );
            addButton->SetClientData( new ClientData( list ) );
        }
        addButton->a_Label.Set( TXT( "Add" ) );
        addButton->a_HelpText.Set( TXT( "Add a file to the list." ) );

        removeButton = CreateControl< Button >();
        removeButton->a_Label.Set( TXT( "Remove" ) );
        removeButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnRemove ) );
        removeButton->SetClientData( new ClientData( list ) );
        removeButton->a_HelpText.Set( TXT( "Remove a file from the list." ) );

        if ( isVector )
        {
            upButton = CreateControl< Button >();
            upButton->a_Icon.Set( TXT( "actions/go-up" ) );
            upButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnMoveUp ) );
            upButton->SetClientData( new ClientData( list ) );
            upButton->a_HelpText.Set( TXT( "Move the selected file(s) up the list." ) );

            downButton = CreateControl< Button >();
            downButton->a_Icon.Set( TXT( "actions/go-down" ) );
            downButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnMoveDown ) );
            downButton->SetClientData( new ClientData( list ) );
            downButton->a_HelpText.Set( TXT( "Move the selected file(s) down the list." ) );
        }

        m_List->SetProperty( TXT( "FileFilter" ), filter );
    }

    // add the buttons to the container
    ContainerPtr buttonContainer = CreateControl<Container>();
    parent->AddChild( buttonContainer );
    if ( addButton )
    {
        buttonContainer->AddChild( addButton );
    }
    if ( findButton )
    {
        buttonContainer->AddChild( findButton );
    }
    if ( editButton )
    {
        buttonContainer->AddChild( editButton );
    }
    if ( removeButton )
    {
        buttonContainer->AddChild( removeButton );
    }
    if ( upButton )
    {
        buttonContainer->AddChild( upButton );
    }
    if ( downButton )
    {
        buttonContainer->AddChild( downButton );
    }

    // we only let them modify the data if we're only dealing with a single list
    buttonContainer->a_IsEnabled.Set( instances.size() == 1 );

    // create the data objects
    std::vector<Reflect::Object*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Object*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
        DataPtr s = field->CreateData();

        if ( instances.size() == 1 )
        {
            m_PathVector = Reflect::SafeCast< Reflect::PathStlVectorData >( s );
            m_PathSet = Reflect::SafeCast< Reflect::PathStlSetData >( s );
            HELIUM_ASSERT( ( m_PathVector || m_PathSet ) && !( m_PathVector && m_PathSet ) );
        }

        s->ConnectField(*itr, field);

        m_Datas.push_back(s);
    }

    // bind the ui to the data objects
    Helium::SmartPtr< MultiStringFormatter<Data> > data = new MultiStringFormatter<Data>( (std::vector<Reflect::Data*>&)m_Datas );
    list->Bind( data );
    if ( addButton && isContainer )
    {
        addButton->Bind( data );
    }

    // setup the default value
    DataPtr defaultData = field->CreateDefaultData();
    if ( defaultData )
    {
        tstringstream defaultStream;
        *defaultData >> defaultStream;
        list->a_Default.Set( defaultStream.str() );
    }
}

void PathContainerInterpreter::OnAdd( const ButtonClickedArgs& args )
{
    FileDialogArgs fileDialogArgs ( Helium::FileDialogTypes::OpenFile, TXT( "Add File" ), TXT( "*.*" ) );
    d_OpenFileDialog.Invoke( fileDialogArgs );
    if ( !fileDialogArgs.m_Result.empty() )
    {
        Reflect::PathData pathData;
        pathData.m_Data->Set( fileDialogArgs.m_Result.Get() );

        if ( m_PathVector )
        {
            m_PathVector->Insert( m_PathVector->GetSize(), &pathData );
        }
        else if ( m_PathSet )
        {
            m_PathSet->AddItem( &pathData );
        }
    }

    args.m_Control->GetCanvas()->Read();
}

void PathContainerInterpreter::OnAddFile( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();

    if ( clientData.ReferencesObject() && clientData->IsClass( Reflect::GetClass<ClientDataFilter>() ) )
    {
        ClientDataFilter* data = static_cast< ClientDataFilter* >( clientData.Ptr() );

        HELIUM_BREAK();
#pragma TODO( "Reimplement to use an event that interfaces with the File Dialog" )
        //if ( browserDlg.ShowModal() == wxID_OK )
        //{
        //  tstring filePath = browserDlg.GetPath().c_str();
        //  data->m_List->AddItem( filePath );
        //}

        args.m_Control->GetCanvas()->Read();
    }
}

void PathContainerInterpreter::OnFindFile( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();

    if ( clientData.ReferencesObject() && clientData->IsClass( Reflect::GetClass<ClientDataFilter>() ) )
    {
        ClientDataFilter* data = static_cast< ClientDataFilter* >( clientData.Ptr() );

        HELIUM_BREAK();
#pragma TODO( "Reimplement to use an event that interfaces with the Vault" )
        //if ( browserDlg.ShowModal() == wxID_OK )
        //{
        //  data->m_List->AddItem( browserDlg.GetPath().c_str() );
        //}

        args.m_Control->GetCanvas()->Read();
    }
}

void PathContainerInterpreter::OnEdit( const ButtonClickedArgs& args )
{
    FileDialogArgs fileDialogArgs ( Helium::FileDialogTypes::OpenFile, TXT( "Edit File FilePath" ), TXT( "*.*" ) );
    d_OpenFileDialog.Invoke( fileDialogArgs );
    if ( !fileDialogArgs.m_Result.empty() )
    {
        Reflect::PathData pathData;
        pathData.m_Data->Set( fileDialogArgs.m_Result.Get() );

        std::vector< FilePath > paths;

        if ( m_PathSet )
        {
            for ( std::set< FilePath >::const_iterator itr = m_PathSet->m_Data->begin(), end = m_PathSet->m_Data->end(); itr != end; ++itr )
            {
                paths.push_back( *itr );
            }
        }

        for ( std::set< size_t >::const_iterator itr = m_List->a_SelectedItemIndices.Get().begin(), end = m_List->a_SelectedItemIndices.Get().end(); itr != end; ++itr )
        {
            if ( m_PathVector )
            {
                m_PathVector->Remove( *itr );
                m_PathVector->Insert( *itr, &pathData );
            }
            else if ( m_PathSet )
            {
                Reflect::PathData tempData;
                HELIUM_ASSERT( *itr < paths.size() );
                tempData.m_Data->Set( paths[ *itr ] );
                m_PathSet->RemoveItem( &tempData );
            }
        }

        if ( m_PathSet )
        {
            m_PathSet->AddItem( &pathData );
        }
    }

    args.m_Control->GetCanvas()->Read();
}

void PathContainerInterpreter::OnRemove( const ButtonClickedArgs& args )
{
    std::vector< FilePath > paths;

    if ( m_PathVector )
    {
        paths = *m_PathVector->m_Data;
    }
    else if ( m_PathSet )
    {
        for ( std::set< FilePath >::const_iterator itr = m_PathSet->m_Data->begin(), end = m_PathSet->m_Data->end(); itr != end; ++itr )
        {
            paths.push_back( *itr );
        }
    }

    std::set< size_t >::const_reverse_iterator itr = m_List->a_SelectedItemIndices.Get().rbegin(), end = m_List->a_SelectedItemIndices.Get().rend();
    for ( ; itr != end; ++itr )
    {
        paths.erase( paths.begin() + *itr );
    }

    if ( m_PathVector )
    {
        (*m_PathVector->m_Data) = paths;
    }
    else if ( m_PathSet )
    {
        std::set< FilePath > pathSet;
        for ( std::vector< FilePath >::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr )
        {
            pathSet.insert( std::set< FilePath >::value_type( *itr ) );
        }

        (*m_PathSet->m_Data) = pathSet;
    }

    args.m_Control->GetCanvas()->Read();
}

void PathContainerInterpreter::OnMoveUp( const ButtonClickedArgs& args )
{
    HELIUM_ASSERT( m_PathVector );

    std::set< size_t > temp = m_List->a_SelectedItemIndices.Get();
    m_PathVector->MoveUp( temp );

    args.m_Control->GetCanvas()->Read();
}

void PathContainerInterpreter::OnMoveDown( const ButtonClickedArgs& args )
{
    HELIUM_ASSERT( m_PathVector );

    std::set< size_t > temp = m_List->a_SelectedItemIndices.Get();
    m_PathVector->MoveDown( temp );

    args.m_Control->GetCanvas()->Read();
}
