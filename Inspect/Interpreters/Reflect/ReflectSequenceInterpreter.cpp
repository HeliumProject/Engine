#include "InspectPch.h"
#include "ReflectStlVectorInterpreter.h"
#include "InspectReflectInit.h"

#include "Inspect/Controls/LabelControl.h"
#include "Inspect/Controls/ValueControl.h"
#include "Inspect/Controls/ChoiceControl.h"
#include "Inspect/Controls/ListControl.h"
#include "Inspect/Controls/ButtonControl.h"
#include "Inspect/DataBinding.h"
#include "Inspect/Inspect.h"

#include "Foundation/Tokenize.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

ReflectStlVectorInterpreter::ReflectStlVectorInterpreter (Container* labelContainer)
: ReflectFieldInterpreter (labelContainer)
{

}

void ReflectStlVectorInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Object*>& instances, Container* parent)
{
    if (field->m_Flags & FieldFlags::Hide)
    {
        return;
    }

    // create the label
    ContainerPtr labelContainer = CreateControl<Container>();
    parent->AddChild( labelContainer );
    LabelPtr label = CreateControl< Label >();
    label->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
    labelContainer->AddChild( label );

    tstring temp;
    field->GetProperty( TXT( "UIName" ), temp );
    if ( temp.empty() )
    {
        bool converted = Helium::ConvertString( field->m_Name, temp );
        HELIUM_ASSERT( converted );
    }

    label->BindText( temp );

    // create the list view
    ContainerPtr listContainer = CreateControl<Container>();
    parent->AddChild( listContainer );
    ListPtr list = CreateControl<List>();
    list->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
    listContainer->AddChild( list );

    // create the buttons
    ButtonPtr addButton;
    ButtonPtr removeButton;
    ButtonPtr upButton;
    ButtonPtr downButton;
    if ( !(field->m_Flags & FieldFlags::ReadOnly) )
    {
        addButton = AddAddButton( list );
        removeButton = AddRemoveButton( list );
        upButton = AddMoveUpButton( list );
        downButton = AddMoveDownButton( list );
    }

    // add the buttons to the container
    ContainerPtr buttonContainer = CreateControl<Container>();
    parent->AddChild( buttonContainer );
    if ( addButton )
    {
        buttonContainer->AddChild( addButton );
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

    // create the data objects
    std::vector<Reflect::Object*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Object*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
        Data s = field->CreateData();

        OnCreateFieldData( s );

        s->ConnectField(*itr, field);

        m_Datas.push_back(s);
    }

    // bind the ui to the data objects
    Helium::SmartPtr< MultiStringFormatter<Data> > data = new MultiStringFormatter<Reflect::Data>( (std::vector<Reflect::Data*>&)m_Datas );
    list->Bind( data );

    // setup the default value
    Data defaultData = field->CreateDefaultData();
    if (defaultData)
    {
        tstringstream defaultStream;
        *defaultData >> defaultStream;
        list->a_Default.Set( defaultStream.str() );
    }
}

ButtonPtr ReflectStlVectorInterpreter::AddAddButton( List* list )
{
    ButtonPtr addButton =CreateControl< Button >();
    addButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectStlVectorInterpreter::OnAdd ) );
    addButton->SetClientData( new ClientData( list ) );
    addButton->a_Label.Set( TXT( "Add" ) );
    addButton->a_HelpText.Set( TXT( "Add an item to the list." ) );

    return addButton;
}

ButtonPtr ReflectStlVectorInterpreter::AddRemoveButton( List* list )
{
    ButtonPtr removeButton = CreateControl< Button >();
    removeButton->a_Label.Set( TXT( "Remove" ) );
    removeButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectStlVectorInterpreter::OnRemove ) );
    removeButton->SetClientData( new ClientData( list ) );
    removeButton->a_HelpText.Set( TXT( "Remove the selected item(s) from the list." ) );

    return removeButton;
}

ButtonPtr ReflectStlVectorInterpreter::AddMoveUpButton( List* list )
{
    ButtonPtr upButton = CreateControl< Button >();
    upButton->a_Icon.Set( TXT( "actions/go-up" ) );
    upButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectStlVectorInterpreter::OnMoveUp ) );
    upButton->SetClientData( new ClientData( list ) );
    upButton->a_HelpText.Set( TXT( "Move the selected item(s) up the list." ) );

    return upButton;
}

ButtonPtr ReflectStlVectorInterpreter::AddMoveDownButton( List* list )
{
    ButtonPtr downButton = CreateControl< Button >();
    downButton->a_Icon.Set( TXT( "actions/go-down" ) );
    downButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectStlVectorInterpreter::OnMoveDown ) );
    downButton->SetClientData( new ClientData( list ) );
    downButton->a_HelpText.Set( TXT( "Move the selected item(s) down the list." ) );

    return downButton;
}

void ReflectStlVectorInterpreter::OnAdd( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->IsClass( Reflect::GetClass<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        list->e_AddItem.Raise( AddItemArgs() );
        args.m_Control->GetCanvas()->Read();
    }
}

void ReflectStlVectorInterpreter::OnRemove( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->IsClass( Reflect::GetClass<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        const std::set< size_t >& selectedItemIndices = list->a_SelectedItemIndices.Get();
        if ( !selectedItemIndices.empty() )
        {
            // for each item in the array to remove (by index)
            std::set< size_t >::const_reverse_iterator itr = selectedItemIndices.rbegin();
            std::set< size_t >::const_reverse_iterator end = selectedItemIndices.rend();
            for ( ; itr != end; ++itr )
            {
                // for each array in the selection set (the objects the array data is connected to)
                std::vector< Data >::const_iterator serItr = m_Datas.begin();
                std::vector< Data >::const_iterator serEnd = m_Datas.end();
                for ( ; serItr != serEnd; ++serItr )
                {
                    Reflect::StlVectorData* arrayData = Reflect::AssertCast<Reflect::StlVectorData>(*serItr);

                    arrayData->Remove( *itr );
                }
            }

            list->a_SelectedItemIndices.Set( std::set< size_t > () );

            args.m_Control->GetCanvas()->Read();
        }
    }
}

void ReflectStlVectorInterpreter::OnMoveUp( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->IsClass( Reflect::GetClass<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        std::set< size_t > selectedItemIndices = list->a_SelectedItemIndices.Get();
        if ( !selectedItemIndices.empty() )
        {
            // for each array in the selection set (the objects the array data is connected to)
            std::vector< Data >::const_iterator serItr = m_Datas.begin();
            std::vector< Data >::const_iterator serEnd = m_Datas.end();
            for ( ; serItr != serEnd; ++serItr )
            {
                Reflect::StlVectorData* arrayData = Reflect::AssertCast<Reflect::StlVectorData>(*serItr);

                arrayData->MoveUp( selectedItemIndices );
            }

            list->a_SelectedItemIndices.Set( selectedItemIndices );

            args.m_Control->GetCanvas()->Read();
        }
    }
}

void ReflectStlVectorInterpreter::OnMoveDown( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->IsClass( Reflect::GetClass<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        std::set< size_t > selectedItemIndices = list->a_SelectedItemIndices.Get();
        if ( !selectedItemIndices.empty() )
        {
            // for each array in the selection set (the objects the array data is connected to)
            std::vector< Data >::const_iterator serItr = m_Datas.begin();
            std::vector< Data >::const_iterator serEnd = m_Datas.end();
            for ( ; serItr != serEnd; ++serItr )
            {
                Reflect::StlVectorData* arrayData = Reflect::AssertCast<Reflect::StlVectorData>(*serItr);

                arrayData->MoveDown( selectedItemIndices );
            }

            list->a_SelectedItemIndices.Set( selectedItemIndices );

            args.m_Control->GetCanvas()->Read();
        }
    }
}