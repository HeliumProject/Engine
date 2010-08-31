/*#include "Precompile.h"*/
#include "PropertiesManager.h"

#include "Platform/Thread.h"
#include "Platform/Platform.h"

#include "Foundation/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

using namespace Helium;
using namespace Helium::Core;

PropertyThreadArgs::PropertyThreadArgs( const PropertyThreadArgs& args )
: m_Selection( args.m_Selection )
, m_SelectionId( args.m_SelectionId )
, m_CurrentSelectionId( args.m_CurrentSelectionId )
, m_Setting( args.m_Setting )
, m_Container( args.m_Container )
, m_PropertiesCreated( args.m_PropertiesCreated )
{
}

PropertyThreadArgs::PropertyThreadArgs( const OS_SelectableDumbPtr& selection,
                                       u32 selectionId,
                                       const u32* currentSelectionId,
                                       PropertySetting setting,
                                       Inspect::Container* container,
                                       PropertiesCreatedSignature::Event& propertiesCreated )
                                       : m_SelectionId( selectionId )
                                       , m_CurrentSelectionId( currentSelectionId )
                                       , m_Setting( setting )
                                       , m_Container( container )
                                       , m_PropertiesCreated( propertiesCreated )
{
    for ( OS_SelectableDumbPtr::Iterator itr = selection.Begin(), end = selection.End(); itr != end; ++itr )
    {
        m_Selection.Append( *itr );
    }
}

EnumerateElementArgs::EnumerateElementArgs( M_ElementByType& currentElements,
                                           M_ElementsByType& commonElements,
                                           M_InterpretersByType& commonElementInterpreters )
                                           : m_CurrentElements (currentElements)
                                           , m_CommonElements (commonElements)
                                           , m_CommonElementInterpreters (commonElementInterpreters)
{

}

void EnumerateElementArgs::EnumerateElement(Reflect::Element* element, i32 includeFlags, i32 excludeFlags)
{
    // this will insert an empty map at the slot for the type of "element", or just make "b" false and return the iter at the existing one
    Helium::Insert<M_ElementByType>::Result inserted = m_CurrentElements.insert( M_ElementByType::value_type (ElementTypeFlags ( element->GetType(), includeFlags, excludeFlags ), element) );
}

PropertiesManager::PropertiesManager( PropertiesGenerator* generator )
: m_Generator( generator )
, m_Setting (PropertySettings::Intersection)
, m_SelectionDirty (false)
, m_SelectionId (0)
, m_ThreadCount (0)
{
    m_Generator->GetContainer()->GetCanvas()->AddShowListener( Inspect::CanvasShowSignature::Delegate ( this, &PropertiesManager::Show ) );
}

PropertiesManager::~PropertiesManager()
{
    m_Generator->GetContainer()->GetCanvas()->RemoveShowListener( Inspect::CanvasShowSignature::Delegate ( this, &PropertiesManager::Show ) );
}

void PropertiesManager::Show( const Inspect::CanvasShowArgs& args )
{
    if ( m_SelectionDirty )
    {
        CreateProperties();

        m_SelectionDirty = false;
    }
}

Inspect::Container* PropertiesManager::GetContainer()
{
    return NULL;
}

void PropertiesManager::SetProperties(PropertySetting setting)
{
    m_Setting = setting;

    ++m_SelectionId;

    m_SelectionDirty = true;

    CreateProperties();
}

void PropertiesManager::SetSelection(const OS_SelectableDumbPtr& selection)
{
    m_Selection = selection;

    ++m_SelectionId;

    m_SelectionDirty = true;

    CreateProperties();
}

void PropertiesManager::CreateProperties()
{
    CORE_SCOPE_TIMER( ("") );

    {
        CORE_SCOPE_TIMER( ("Reset Property State") );

        m_Generator->Reset();
    }

#ifdef INSPECT_REFACTOR
    m_PreviousScroll = m_Generator->GetContainer()->GetCanvas()->GetScroll();

    // early out if we are not visible
    if (!m_Generator->GetContainer()->GetCanvas()->GetWindow()->IsShown())
    {
        return;
    }
#endif

    // early out if we have no objects to interpret
    if ( m_Selection.Empty() )
    {
        Inspect::V_Control controls;
        FinalizeProperties( m_SelectionId, controls );
    }
    else
    {
        {
            Helium::TakeMutex mutex( m_ThreadCountMutex );
            ++m_ThreadCount;
        }

        Helium::Thread propertyThread;
        PropertyThreadArgs* propertyThreadArgs = new PropertyThreadArgs( m_Selection, m_SelectionId, &m_SelectionId, m_Setting, m_Generator->CreateControl<Inspect::Container>(), m_PropertiesCreated );
        propertyThread.CreateWithArgs( Helium::Thread::EntryHelperWithArgs<PropertiesManager, PropertyThreadArgs, &PropertiesManager::GeneratePropertiesThread>, this, propertyThreadArgs, "GeneratePropertiesThread()", -1 );
    }
}

void PropertiesManager::GeneratePropertiesThread( PropertyThreadArgs& args )
{
    GenerateProperties( args );

    {
        Helium::TakeMutex mutex( m_ThreadCountMutex );
        --m_ThreadCount;
    }
}

void PropertiesManager::GenerateProperties( PropertyThreadArgs& args )
{
    M_ElementByType currentElements;
    M_ElementsByType commonElements;
    M_InterpretersByType commonElementInterpreters;
    EnumerateElementArgs enumerateElementArgs( currentElements, commonElements, commonElementInterpreters );
    OS_SelectableDumbPtr selection;

    for ( OS_SelectablePtr::Iterator itr = args.m_Selection.Begin(), end = args.m_Selection.End(); itr != end; ++itr )
    {
        selection.Append( *itr );
    }

    //
    // First Pass:
    //  Iterates over selection, asking each to enumerate their attributes into temp members (current)
    //  Then coallate those results into an intersection member (common)
    //

#pragma TODO( "selection.Empty() should always be false, so s_PanelCreators should always be used" )
    // intersection support
    M_PanelCreators intersectingPanels = selection.Empty() ? M_PanelCreators () : s_PanelCreators;

    // union support
    typedef std::map< tstring, OS_SelectableDumbPtr > M_UnionedSelections;
    M_UnionedSelections unionedSelections;
    M_PanelCreators unionedPanels;

    {
        CORE_SCOPE_TIMER( ("Selection Processing") );

        OS_SelectableDumbPtr::Iterator itr = selection.Begin();
        OS_SelectableDumbPtr::Iterator end = selection.End();
        for ( size_t index = 0; itr != end; ++itr, ++index )
        {
            if ( *args.m_CurrentSelectionId != args.m_SelectionId )
            {
                return;
            }

            currentElements.clear();

            {
                CORE_SCOPE_TIMER( ("Object Property Enumeration") );

                (*itr)->ConnectProperties(enumerateElementArgs);
            }

            M_PanelCreators currentPanels;

#ifdef SCENE_DEBUG_PROPERTIES_GENERATOR
            Log::Print("Object type %s:\n", typeid(*(*itr)).name());
#endif

            {
                CORE_SCOPE_TIMER( ("Object Panel Validation") );

                M_PanelCreators::const_iterator itrPanel = args.m_Setting == PropertySettings::Intersection ? intersectingPanels.begin() : s_PanelCreators.begin();
                M_PanelCreators::const_iterator endPanel = args.m_Setting == PropertySettings::Intersection ? intersectingPanels.end() : s_PanelCreators.end();
                for ( ; itrPanel != endPanel; ++itrPanel)
                {
                    if ( *args.m_CurrentSelectionId != args.m_SelectionId )
                    {
                        return;
                    }

                    if ((*itr)->ValidatePanel(itrPanel->first))
                    {
#ifdef SCENE_DEBUG_PROPERTIES_GENERATOR
                        Log::Print(" accepts %s\n", itrPanel->first.c_str());
#endif
                        switch (m_Setting)
                        {
                        case PropertySettings::Intersection:
                            {
                                currentPanels.insert( *itrPanel );
                                break;
                            }

                        case PropertySettings::Union:
                            {
                                unionedPanels.insert( *itrPanel );

                                Helium::Insert<M_UnionedSelections>::Result inserted = 
                                    unionedSelections.insert( M_UnionedSelections::value_type ( itrPanel->first, OS_SelectableDumbPtr () ) );

                                inserted.first->second.Append( *itr );
                            }
                        }
                    }
                    else
                    {
#ifdef SCENE_DEBUG_PROPERTIES_GENERATOR
                        Log::Print(" rejects %s\n", itrPanel->first.c_str());
#endif
                    }
                }
            }

#ifdef SCENE_DEBUG_PROPERTIES_GENERATOR
            Log::Print("\n");
#endif

            if (m_Setting == PropertySettings::Intersection)
            {
                intersectingPanels = currentPanels;
            }

            if (currentElements.empty())
            {
                commonElements.clear();
            }
            else
            {
                CORE_SCOPE_TIMER( ("Object Unique Reflect Property Culling") );

                M_ElementsByType newCommonElements;

                if (index == 0)
                {
                    M_ElementByType::const_iterator currentItr = currentElements.begin();
                    M_ElementByType::const_iterator currentEnd = currentElements.end();
                    for ( ; currentItr != currentEnd; ++currentItr )
                    {
                        if ( *args.m_CurrentSelectionId != args.m_SelectionId )
                        {
                            return;
                        }

                        // copy the shared list into the new shared map
                        Helium::Insert<M_ElementsByType>::Result inserted = 
                            newCommonElements.insert(M_ElementsByType::value_type( currentItr->first, std::vector<Reflect::Element*> () ));

                        // add this current element's instance to the new shared list
                        inserted.first->second.push_back(currentItr->second);
                    }
                }
                else
                {
                    M_ElementsByType::const_iterator sharedItr = commonElements.begin();
                    M_ElementsByType::const_iterator sharedEnd = commonElements.end();
                    for ( ; sharedItr != sharedEnd; ++sharedItr )
                    {
                        if ( *args.m_CurrentSelectionId != args.m_SelectionId )
                        {
                            return;
                        }

                        M_ElementByType::const_iterator found = currentElements.find(sharedItr->first);

                        // if we found a current element entry for this shared element
                        if (found != currentElements.end())
                        {
                            // copy the shared list into the new shared map
                            Helium::Insert<M_ElementsByType>::Result inserted = 
                                newCommonElements.insert(M_ElementsByType::value_type( sharedItr->first, sharedItr->second ));

                            // add this current element's instance to the new shared list
                            inserted.first->second.push_back(found->second);
                        }
                        else
                        {
                            // there is NO instance of this element in the current instance, let it be culled from the shared list
                        }
                    }
                }

                commonElements = newCommonElements;
            }

            // we have eliminated all the shared types, abort
            if (intersectingPanels.empty() && commonElements.empty() )
            {
                break;
            }
        }
    }


    //
    // Second Pass:
    //  Create client-constructed attribute panels
    //

    {
        CORE_SCOPE_TIMER( ("Static Panel Creation") );

        M_PanelCreators::const_iterator itr = args.m_Setting == PropertySettings::Intersection ? intersectingPanels.begin() : unionedPanels.begin();
        M_PanelCreators::const_iterator end = args.m_Setting == PropertySettings::Intersection ? intersectingPanels.end() : unionedPanels.end();
        for ( ; itr != end; ++itr )
        {
            if ( *args.m_CurrentSelectionId != args.m_SelectionId )
            {
                return;
            }

            switch ( args.m_Setting )
            {
            case PropertySettings::Intersection:
                {
                    m_Generator->Push( args.m_Container );
                    itr->second.Invoke( CreatePanelArgs (m_Generator, selection) );
                    m_Generator->Pop( false );
                    break;
                }

            case PropertySettings::Union:
                {
                    M_UnionedSelections::const_iterator found = unionedSelections.find( itr->first );

                    if (found != unionedSelections.end())
                    {
                        // this connects the invocation with the validated selection
                        m_Generator->Push( args.m_Container );
                        itr->second.Invoke( CreatePanelArgs (m_Generator, found->second) );
                        m_Generator->Pop( false );
                    }
                    else
                    {
                        // something is horribly horribly wrong
                        HELIUM_BREAK();
                    }

                    break;
                }
            }

            // if you hit then, then your custom panel creator needs work
            HELIUM_ASSERT(m_Generator->GetContainerStack().empty());
        }
    }


    //
    // Third Pass:
    //  Iterates over resultant map and causes interpretation to occur for each object in the list
    //

    {
        CORE_SCOPE_TIMER( ("Reflect Interpret") );

        M_ElementsByType::const_iterator itr = commonElements.begin();
        M_ElementsByType::const_iterator end = commonElements.end();
        for ( ; itr != end; ++itr )
        {
            if ( *args.m_CurrentSelectionId != args.m_SelectionId )
            {
                return;
            }

            Inspect::ReflectInterpreterPtr interpreter = m_Generator->CreateInterpreter<Inspect::ReflectInterpreter>( args.m_Container );

            interpreter->Interpret(itr->second, itr->first.m_IncludeFlags, itr->first.m_ExcludeFlags);

            Helium::Insert<M_InterpretersByType>::Result inserted = 
                commonElementInterpreters.insert( M_InterpretersByType::value_type(itr->first, interpreter) );
        }
    }

    PropertiesCreatedArgs propertiesCreatedArgs( this, args.m_SelectionId, args.m_Container->GetChildren() );
    m_PropertiesCreated.Raise( propertiesCreatedArgs );
}

void PropertiesManager::FinalizeProperties( u32 selectionId, const Inspect::V_Control& controls )
{
    if ( selectionId != m_SelectionId )
    {
        return;
    }

    CORE_SCOPE_TIMER( ("Canvas Layout") );

    for ( Inspect::V_Control::const_iterator itr = controls.begin(), end = controls.end(); itr != end; ++itr )
    {
        m_Generator->GetContainer()->AddChild( *itr );
    }

#ifdef INSPECT_REFACTOR
    m_Generator->GetContainer()->GetCanvas()->Freeze();
    m_Generator->GetContainer()->GetCanvas()->Layout(); // Realize now? -Geoff
#endif

#ifdef INSPECT_REFACTOR
    m_Generator->GetContainer()->GetCanvas()->SetScroll( m_PreviousScroll );
#endif

    m_Generator->GetContainer()->GetCanvas()->Read();

#ifdef INSPECT_REFACTOR
    m_Generator->GetContainer()->GetCanvas()->Thaw();
#endif
}

void PropertiesManager::AddPropertiesCreatedListener( const PropertiesCreatedSignature::Delegate& listener )
{
    m_PropertiesCreated.Add( listener );
}

void PropertiesManager::RemovePropertiesCreatedListener( const PropertiesCreatedSignature::Delegate& listener )
{
    m_PropertiesCreated.Remove( listener );
}

bool PropertiesManager::ThreadsActive()
{
    Helium::TakeMutex mutex( m_ThreadCountMutex );
    return m_ThreadCount > 0;
}
