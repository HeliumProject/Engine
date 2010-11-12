#include "ReflectSetInterpreter.h"
#include "InspectReflectInit.h"

#include "Foundation/Inspect/Controls/ButtonControl.h"
#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Controls/ListControl.h"
#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/Data.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ReflectSetInterpreter::ReflectSetInterpreter( Container* container )
: ReflectFieldInterpreter( container )
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates UI for the field variable specified.
// 
void ReflectSetInterpreter::InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent )
{
    if ( field->m_Flags & Reflect::FieldFlags::Hide )
    {
        return;
    }

    // create the container
    ContainerPtr container = CreateControl< Container >();
    parent->AddChild( container );
    container->a_Name.Set( field->m_UIName );

    // create the serializers
    std::vector< Reflect::Element* >::const_iterator itr = instances.begin();
    std::vector< Reflect::Element* >::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
        Reflect::SerializerPtr ser = Reflect::AssertCast< Reflect::Serializer >( Reflect::Registry::GetInstance()->CreateInstance( field->m_SerializerID ) );
        uintptr_t fieldAddress = ( uintptr_t )( *itr ) + field->m_Offset;
        ser->ConnectData( ( void* )fieldAddress );
        m_Serializers.push_back( ser );
    }

    // create the list
    ListPtr list = CreateControl< List >();
    list->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
    container->AddChild( list );

    // bind the ui to the serialiers
    list->Bind( new MultiStringFormatter< Reflect::Serializer >( (std::vector<Reflect::Serializer*>&)m_Serializers ) );

    // create the buttons if we are not read only
    if ( !( field->m_Flags & Reflect::FieldFlags::ReadOnly ) )
    {
        ContainerPtr buttonContainer = CreateControl< Container >();
        container->AddChild( buttonContainer );

        ButtonPtr buttonAdd = CreateControl< Button >();
        buttonContainer->AddChild( buttonAdd );
        buttonAdd->a_Label.Set( TXT( "Add" ) );
        buttonAdd->a_HelpText.Set( TXT( "Add an item to the list." ) );
        buttonAdd->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectSetInterpreter::OnAdd ) );
        buttonAdd->SetClientData( new ClientData( list ) );

        ButtonPtr buttonRemove = CreateControl< Button >();
        buttonContainer->AddChild( buttonRemove );
        buttonRemove->a_Label.Set( TXT( "Remove" ) );
        buttonRemove->a_HelpText.Set( TXT( "Remove the selected item(s) from the list." ) );
        buttonRemove->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectSetInterpreter::OnRemove ) );
        buttonRemove->SetClientData( new ClientData( list ) );
    }

    // for now let's just disable this container if there is more than one item selected. I'm not sure if it will behave properly in this case.
    if ( instances.size() > 1 )
    {
        container->a_IsEnabled.Set( false );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the add button is pressed.  Displays a dialog that lets
// you enter a new key-value pair.  If you enter a key that already exists in
// the list, you will be asked if you want to replace it or not.
// 
void ReflectSetInterpreter::OnAdd( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        list->e_AddItem.Raise( AddItemArgs() );
        args.m_Control->GetCanvas()->Read();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the remove button is pressed.  If there are any items 
// selected in the list control, they will be removed from the list.
// 
void ReflectSetInterpreter::OnRemove( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
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
                // for each array in the selection set (the objects the array serializer is connected to)
                std::vector< SerializerPtr >::const_iterator serItr = m_Serializers.begin();
                std::vector< SerializerPtr >::const_iterator serEnd = m_Serializers.end();
                for ( ; serItr != serEnd; ++serItr )
                {
                    Reflect::SetSerializer* setSerializer = Reflect::AssertCast<Reflect::SetSerializer>(*serItr);
                    V_ConstSerializer items;
                    setSerializer->GetItems( items );
                    setSerializer->RemoveItem( items[ *itr ] );
                }
            }

            list->a_SelectedItemIndices.Set( std::set< size_t > () );

            args.m_Control->GetCanvas()->Read();
        }
    }
}
