#include "ReflectInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectValueInterpreter.h"

#include "Foundation/Inspect/Script.h"
#include "Foundation/Inspect/Controls/ButtonControl.h"
#include "Foundation/Inspect/Controls/ValueControl.h"
#include "Foundation/Inspect/Controls/ListControl.h"
#include "Foundation/Inspect/Container.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

ReflectFieldInterpreterFactory::M_Creator ReflectFieldInterpreterFactory::m_Map;

ReflectInterpreter::ReflectInterpreter (Container* container)
: Interpreter (container)
{

}

void ReflectInterpreter::Interpret(const std::vector<Reflect::Element*>& instances, i32 includeFlags, i32 excludeFlags, bool expandPanel)
{
    m_Instances = instances;

    InterpretType(instances, m_Container, includeFlags, excludeFlags, expandPanel);
}

void ReflectInterpreter::InterpretType(const std::vector<Reflect::Element*>& instances, Container* parent, i32 includeFlags, i32 excludeFlags, bool expandPanel)
{
    const Class* typeInfo = instances[0]->GetClass();

    // create a container
    ContainerPtr container = new Container ();

    // parse
    ContainerPtr scriptOutput = new Container ();

    tstring typeInfoUI;
    typeInfo->GetProperty( TXT( "UIScript" ), typeInfoUI );
    bool result = Script::Parse(typeInfoUI, this, parent->GetCanvas(), scriptOutput);

    // compute container label
    tstring labelText;
    if (result)
    {
        V_Control::const_iterator itr = scriptOutput->GetChildren().begin();
        V_Control::const_iterator end = scriptOutput->GetChildren().end();
        for( ; itr != end; ++itr )
        {
            Label* label = Reflect::ObjectCast<Label>( *itr );
            if (label)
            {
                label->ReadStringData( labelText );

                if ( !labelText.empty() )
                {
                    break;
                }
            }
        }
    }

    if (labelText.empty())
    {
        std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
        std::vector<Reflect::Element*>::const_iterator end = instances.end();
        for ( ; itr != end; ++itr )
        {
            Reflect::Element* instance = *itr;

            if ( labelText.empty() )
            {
                labelText = instance->GetTitle();
            }
            else
            {
                if ( labelText != instance->GetTitle() )
                {
                    labelText.clear();
                    break;
                }
            }
        }

        if ( labelText.empty() )
        {
            labelText = typeInfo->m_UIName;
        }
    }

    container->a_Name.Set( labelText );

    std::map< tstring, ContainerPtr > containersMap;
    containersMap.insert( std::make_pair( TXT( "" ), container) );

    // don't bother including Element's fields
    int offset = Reflect::GetClass<Element>()->m_LastFieldID;

    // for each field in the type
    M_FieldIDToInfo::const_iterator itr = typeInfo->m_FieldIDToInfo.find(offset + 1);
    M_FieldIDToInfo::const_iterator end = typeInfo->m_FieldIDToInfo.end();
    for ( ; itr != end; ++itr )
    {
        const Field* field = itr->second;

        bool noFlags = ( field->m_Flags == 0 && includeFlags == 0xFFFFFFFF );
        bool doInclude = ( field->m_Flags & includeFlags ) != 0;
        bool dontExclude = ( excludeFlags == 0 ) || !(field->m_Flags & excludeFlags );
        bool hidden = (field->m_Flags & Reflect::FieldFlags::Hide) != 0; 

        // if we don't have flags (or we are included, and we aren't excluded) then make UI
        if ( ( noFlags || doInclude ) && ( dontExclude ) )
        {
            tstring fieldUIGroup;
            field->GetProperty( TXT( "UIGroup" ), fieldUIGroup );
            if ( !fieldUIGroup.empty() )
            {
                std::map< tstring, ContainerPtr >::iterator itr = containersMap.find( fieldUIGroup );
                if ( itr == containersMap.end() )
                {
                    // This container isn't in our list so make a new one
                    ContainerPtr newContainer = new Container ();
                    containersMap.insert( std::make_pair(fieldUIGroup, newContainer) );

                    ContainerPtr parent;
                    tstring groupName;
                    size_t idx = fieldUIGroup.find_last_of( TXT( "/" ) );
                    if ( idx != tstring::npos )
                    {
                        tstring parentName = fieldUIGroup.substr( 0, idx );
                        groupName = fieldUIGroup.substr( idx+1 );
                        if ( containersMap.find( parentName ) == containersMap.end() )
                        {          
                            parent = new Container ();

                            // create the parent hierarchy since it hasn't already been made
                            tstring currentParent = parentName;
                            for (;;)
                            {
                                idx = currentParent.find_last_of( TXT( "/" ) );
                                if ( idx == tstring::npos )
                                {
                                    // no more parents so we add it to the root
                                    containersMap.insert( std::make_pair(currentParent, parent) );
                                    parent->a_Name.Set( currentParent );
                                    containersMap[ TXT( "" ) ]->AddChild( parent );
                                    break;
                                }
                                else
                                {
                                    parent->a_Name.Set( currentParent.substr( idx+1 ) );

                                    if ( containersMap.find( currentParent ) != containersMap.end() )
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        ContainerPtr grandParent = new Container ();
                                        grandParent->AddChild( parent );
                                        containersMap.insert( std::make_pair(currentParent, parent) );

                                        parent = grandParent;
                                    }
                                    currentParent = currentParent.substr( 0, idx );
                                }
                            }
                            containersMap.insert( std::make_pair(parentName, parent) );
                        }
                        parent = containersMap[parentName];
                    }
                    else
                    {
                        parent = containersMap[ TXT( "" )];
                        groupName = fieldUIGroup;
                    }
                    newContainer->a_Name.Set( groupName );
                    parent->AddChild( newContainer );
                }

                container = containersMap[fieldUIGroup];
            }
            else
            {
                container = containersMap[ TXT( "" )];
            }


            //
            // Pointer support
            //

            if (field->m_SerializerID == Reflect::GetType<Reflect::PointerSerializer>())
            {
                if (hidden)
                {
                    continue; 
                }        

                std::vector<Reflect::Element*> fieldInstances;

                std::vector<Reflect::Element*>::const_iterator elementItr = instances.begin();
                std::vector<Reflect::Element*>::const_iterator elementEnd = instances.end();
                for ( ; elementItr != elementEnd; ++elementItr )
                {
                    uintptr fieldAddress = (uintptr)(*elementItr) + itr->second->m_Offset;

                    Element* element = *((ElementPtr*)(fieldAddress));

                    if ( element )
                    {
                        fieldInstances.push_back( element );
                    }
                }

                if ( !fieldInstances.empty() && fieldInstances.size() == instances.size() )
                {
                    InterpretType(fieldInstances, container);
                }

                continue;
            }


            //
            // Attempt to find a handler via the factory
            //

            ReflectFieldInterpreterPtr fieldInterpreter;

            for ( const Reflect::Class* type = Registry::GetInstance()->GetClass( field->m_SerializerID );
                type != Reflect::GetClass<Reflect::Element>() && !fieldInterpreter;
                type = Reflect::Registry::GetInstance()->GetClass( type->m_Base ) )
            {
                fieldInterpreter = ReflectFieldInterpreterFactory::Create( type->m_TypeID, field->m_Flags, m_Container );
            }

            if ( fieldInterpreter.ReferencesObject() )
            {
                Interpreter::ConnectInterpreterEvents( this, fieldInterpreter );
                fieldInterpreter->InterpretField( field, instances, container );
                m_Interpreters.push_back( fieldInterpreter );
                continue;
            }


            //
            // ElementArray support
            //

#pragma TODO("Move this out to an interpreter")
            if (field->m_SerializerID == Reflect::GetType<ElementArraySerializer>())
            {
                if (hidden)
                {
                    continue;
                }

                if ( instances.size() == 1 )
                {
                    uintptr fieldAddress = (uintptr)(instances.front()) + itr->second->m_Offset;

                    V_Element* elements = (V_Element*)fieldAddress;

                    if ( elements->size() > 0 )
                    {
                        ContainerPtr childContainer = new Container ();

                        tstring temp;
                        bool converted = Helium::ConvertString( field->m_UIName, temp );
                        HELIUM_ASSERT( converted );

                        childContainer->a_Name.Set( temp );

                        V_Element::const_iterator elementItr = elements->begin();
                        V_Element::const_iterator elementEnd = elements->end();
                        for ( ; elementItr != elementEnd; ++elementItr )
                        {
                            std::vector<Reflect::Element*> childInstances;
                            childInstances.push_back(*elementItr);
                            InterpretType(childInstances, childContainer);
                        }

                        container->AddChild( childContainer );
                    }
                }

                continue;
            }


            //
            // Lastly fall back to the value interpreter
            //

            const Reflect::Class* type = Registry::GetInstance()->GetClass( field->m_SerializerID );
            if ( !type->HasType( Reflect::GetType<Reflect::ContainerSerializer>() ) )
            {
                fieldInterpreter = CreateInterpreter< ReflectValueInterpreter >( m_Container );
                fieldInterpreter->InterpretField( field, instances, container );
                m_Interpreters.push_back( fieldInterpreter );
                continue;
            }
        }
    }

    // Make sure we have the base container
    container = containersMap[TXT( "" )];

    if ( !container->GetChildren().empty() )
    {
        parent->AddChild(container);
    }
}


void ReflectFieldInterpreterFactory::Register(i32 type, u32 mask, Creator creator)
{
    m_Map[ type ].push_back( std::make_pair(mask, creator) );
}

void ReflectFieldInterpreterFactory::Unregister(i32 type, u32 mask, Creator creator)
{
    M_Creator::iterator found = m_Map.find( type );
    if ( found != m_Map.end() )
    {
        std::remove( found->second.begin(), found->second.end(), std::make_pair( mask, creator ) );
    }
}

ReflectFieldInterpreterPtr ReflectFieldInterpreterFactory::Create(i32 type, u32 flags, Container* container)
{
    Creator creator = NULL;

    M_Creator::const_iterator found = m_Map.find( type );
    if ( found != m_Map.end() )
    {
        std::map<u32, Creator> results;

        V_Creator::const_iterator itr = found->second.begin();
        V_Creator::const_iterator end = found->second.end();
        for ( ; itr != end; ++itr )
        {
            if ( flags == itr->first ) // exact match
            {
                creator = itr->second;
            }
            else if ( flags && (flags & itr->first) != 0 ) // it has flags, and at least on flag is in the mask
            {
                u32 value = flags & itr->first;

                // count the number of bits set
                u32 bits = 0;
                while (value)
                {
                    value = value & (value - 1);
                    bits++;
                }

                // rank it by the number of matching flags
                results[ bits ] = itr->second;
            }
            else if ( itr->first == 0 ) // fall back to folks that don't care about flags
            {
                // rank it at the bottom
                results[ 0x0 ] = itr->second;
            }
        }

        if ( !results.empty() )
        {
            creator = results.rbegin()->second;
        }
    }

    if ( creator )
    {
        return creator( container );
    }
    else
    {
        return NULL;
    }
}

void ReflectFieldInterpreterFactory::Clear()
{
    m_Map.clear();
}