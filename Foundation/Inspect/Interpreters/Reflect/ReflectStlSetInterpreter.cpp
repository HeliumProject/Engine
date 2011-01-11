#include "ReflectStlSetInterpreter.h"
#include "InspectReflectInit.h"

#include "Foundation/Inspect/Controls/ButtonControl.h"
#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Controls/ListControl.h"
#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/DataBinding.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ReflectStlSetInterpreter::ReflectStlSetInterpreter( Container* container )
: ReflectFieldInterpreter( container )
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates UI for the field variable specified.
// 
void ReflectStlSetInterpreter::InterpretField( const Reflect::Field* field, const std::vector<Reflect::Object*>& instances, Container* parent )
{
    if ( field->m_Flags & Reflect::FieldFlags::Hide )
    {
        return;
    }

    // create the container
    ContainerPtr container = CreateControl< Container >();
    parent->AddChild( container );

    tstring temp;
    field->GetProperty( TXT( "UIName" ), temp );
    if ( temp.empty() )
    {
        bool converted = Helium::ConvertString( field->m_Name, temp );
        HELIUM_ASSERT( converted );
    }

    container->a_Name.Set( temp );

    // create the serializers
    std::vector< Reflect::Object* >::const_iterator itr = instances.begin();
    std::vector< Reflect::Object* >::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
        Reflect::DataPtr ser = Reflect::AssertCast< Reflect::Data >( Reflect::Registry::GetInstance()->CreateInstance( field->m_DataClass ) );
        uintptr_t fieldAddress = ( uintptr_t )( *itr ) + field->m_Offset;
        ser->ConnectData( ( void* )fieldAddress );
        m_Datas.push_back( ser );
    }

    // create the list
    ListPtr list = CreateControl< List >();
    list->a_HelpText.Set( field->GetProperty( TXT( "HelpText" ) ) );
    container->AddChild( list );

    // bind the ui to the serialiers
    list->Bind( new MultiStringFormatter< Reflect::Data >( (std::vector<Reflect::Data*>&)m_Datas ) );

    // create the buttons if we are not read only
    if ( !( field->m_Flags & Reflect::FieldFlags::ReadOnly ) )
    {
        ContainerPtr buttonContainer = CreateControl< Container >();
        container->AddChild( buttonContainer );

        ButtonPtr buttonAdd = CreateControl< Button >();
        buttonContainer->AddChild( buttonAdd );
        buttonAdd->a_Label.Set( TXT( "Add" ) );
        buttonAdd->a_HelpText.Set( TXT( "Add an item to the list." ) );
        buttonAdd->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectStlSetInterpreter::OnAdd ) );
        buttonAdd->SetClientData( new ClientData( list ) );

        ButtonPtr buttonRemove = CreateControl< Button >();
        buttonContainer->AddChild( buttonRemove );
        buttonRemove->a_Label.Set( TXT( "Remove" ) );
        buttonRemove->a_HelpText.Set( TXT( "Remove the selected item(s) from the list." ) );
        buttonRemove->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &ReflectStlSetInterpreter::OnRemove ) );
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
void ReflectStlSetInterpreter::OnAdd( const ButtonClickedArgs& args )
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

///////////////////////////////////////////////////////////////////////////////
// Callback for when the remove button is pressed.  If there are any items 
// selected in the list control, they will be removed from the list.
// 
void ReflectStlSetInterpreter::OnRemove( const ButtonClickedArgs& args )
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
                std::vector< DataPtr >::const_iterator serItr = m_Datas.begin();
                std::vector< DataPtr >::const_iterator serEnd = m_Datas.end();
                for ( ; serItr != serEnd; ++serItr )
                {
                    Reflect::StlSetData* setData = Reflect::AssertCast<Reflect::StlSetData>(*serItr);
                    std::vector< ConstDataPtr > items;
                    setData->GetItems( items );
                    setData->RemoveItem( items[ *itr ] );
                }
            }

            list->a_SelectedItemIndices.Set( std::set< size_t > () );

            args.m_Control->GetCanvas()->Read();
        }
    }
}
