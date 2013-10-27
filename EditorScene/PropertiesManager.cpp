#include "EditorScenePch.h"
#include "PropertiesManager.h"

#include "Platform/Atomic.h"
#include "Platform/Thread.h"

#include "Inspect/ReflectInterpreter.h"

using namespace Helium;
using namespace Helium::Editor;

PropertiesManager::PropertiesManager( PropertiesGenerator* generator, CommandQueue* commandQueue )
	: m_Generator( generator )
	, m_CommandQueue( commandQueue )
	, m_Style (PropertiesStyles::Intersection)
	, m_SelectionDirty (false)
	, m_SelectionId (0)
	, m_ThreadCount (0)
{
	m_Generator->GetContainer()->GetCanvas()->e_Show.AddMethod( this, &PropertiesManager::Show );
}

PropertiesManager::~PropertiesManager()
{
	m_Generator->GetContainer()->GetCanvas()->e_Show.RemoveMethod( this, &PropertiesManager::Show );
}

void PropertiesManager::Show( const Inspect::CanvasShowArgs& args )
{
	if ( m_SelectionDirty )
	{
		CreateProperties();

		m_SelectionDirty = false;
	}
}

void PropertiesManager::SetProperties(PropertiesStyle setting)
{
	m_Style = setting;

	++m_SelectionId;

	m_SelectionDirty = true;

	CreateProperties();
}

void PropertiesManager::SetSelection(const OS_ObjectDumbPtr& selection)
{
	m_Selection = selection;

	++m_SelectionId;

	m_SelectionDirty = true;

	CreateProperties();
}

void PropertiesManager::CreateProperties()
{
	EDITOR_SCENE_SCOPE_TIMER( ("") );

	{
		EDITOR_SCENE_SCOPE_TIMER( ("Reset Property State") );

		m_Generator->Reset();
	}

	// early out if we have no objects to interpret
	if ( m_Selection.Empty() )
	{
		Inspect::V_Control controls;
		Present( m_SelectionId, controls );
	}
	else
	{
		AtomicIncrementUnsafe( m_ThreadCount );
		Helium::CallbackThread propertyThread;

		PropertiesThreadArgs* args = new PropertiesThreadArgs( m_Style, m_SelectionId, &m_SelectionId, m_Selection );
		Helium::CallbackThread::Entry entry = Helium::CallbackThread::EntryHelperWithArgs<PropertiesManager, PropertiesThreadArgs, &PropertiesManager::GeneratePropertiesThreadEntry>;
		propertyThread.CreateWithArgs( entry, this, args, TXT( "GeneratePropertiesThreadEntry()" ), ThreadPriorities::Low );
	}
}

void PropertiesManager::GeneratePropertiesThreadEntry( PropertiesThreadArgs& args )
{
	GenerateProperties( args );
	AtomicDecrementUnsafe( m_ThreadCount );
}

class Presenter
{
public:
	Presenter( PropertiesManager* propertiesManager, uint32_t selectionId, const Inspect::V_Control& controls ) 
		: m_PropertiesManager( propertiesManager )
		, m_SelectionId( selectionId )
		, m_Controls( controls )
	{ 
	}

	void Finalize( Helium::Void )
	{
		m_PropertiesManager->Present( m_SelectionId, m_Controls );
		delete this;
	}

private:
	PropertiesManager*  m_PropertiesManager;
	uint32_t            m_SelectionId;
	Inspect::V_Control  m_Controls;
};

void PropertiesManager::GenerateProperties( PropertiesThreadArgs& args )
{
	M_ElementByType currentElements;
	M_ElementsByType commonElements;
	OS_ObjectDumbPtr selection;

	for ( OrderedSet<Reflect::ObjectPtr>::Iterator itr = args.m_Selection.Begin(), end = args.m_Selection.End(); itr != end; ++itr )
	{
		selection.Append( *itr );
	}

	//
	//  Iterates over selection, asking each to enumerate their attributes into temp members (current)
	//  Then coallate those results into an intersection member (common)
	//

	HELIUM_ASSERT( !selection.Empty() );

	{
		EDITOR_SCENE_SCOPE_TIMER( ("Selection Processing") );

		OS_ObjectDumbPtr::Iterator itr = selection.Begin();
		OS_ObjectDumbPtr::Iterator end = selection.End();
		for ( size_t index = 0; itr != end; ++itr, ++index )
		{
			if ( *args.m_CurrentSelectionId != args.m_SelectionId )
			{
				return;
			}

			currentElements.clear();

			Reflect::Object *pObject = *itr;
			HELIUM_ASSERT( pObject );

			std::pair< M_ElementByType::const_iterator, bool > inserted = 
				currentElements.insert( 
				M_ElementByType::value_type (
				ElementTypeFlags ( pObject->GetMetaClass(), 0xFFFFFFFF, 0x0 ), 
				pObject) );

#ifdef SCENE_DEBUG_PROPERTIES_GENERATOR
			HELIUM_TRACE(TraceLevels::Debug, "Object type %s:\n", pObject->GetMetaClass()->m_Name );
#endif

			if (currentElements.empty())
			{
				commonElements.clear();
			}
			else
			{
				EDITOR_SCENE_SCOPE_TIMER( ("Object Unique Reflect Property Culling") );

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
						std::pair< M_ElementsByType::iterator, bool > inserted = 
							newCommonElements.insert(M_ElementsByType::value_type( currentItr->first, std::vector<Reflect::Object*> () ));

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
							std::pair< M_ElementsByType::iterator, bool > inserted = 
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
			if ( commonElements.empty() )
			{
				break;
			}
		}
	}

	//
	//  Iterates over resultant map and causes interpretation to occur for each object in the list
	//

	Inspect::ContainerPtr container = new Inspect::Container ();

	{
		EDITOR_SCENE_SCOPE_TIMER( ("Reflect Interpret") );

		M_ElementsByType::const_iterator itr = commonElements.begin();
		M_ElementsByType::const_iterator end = commonElements.end();
		for ( ; itr != end; ++itr )
		{
			if ( *args.m_CurrentSelectionId != args.m_SelectionId )
			{
				return;
			}

			m_Generator->Interpret(itr->second, itr->first.m_IncludeFlags, itr->first.m_ExcludeFlags);
		}
	}

	// release ownership of the controls now we have passed them onto the main thread for
	//  realization and presentation to the user, this will try and unrealize the controls
	//  from a background thread, but that is okay since they haven't been realized yet :)
	Presenter* presenter = new Presenter ( this, args.m_SelectionId, container->ReleaseChildren() );

	// will cause the main thread to realize and present the controls
	m_CommandQueue->Post( VoidSignature::Delegate( presenter, &Presenter::Finalize ) );
}

void PropertiesManager::Present( uint32_t selectionId, const Inspect::V_Control& controls )
{
	if ( selectionId != m_SelectionId )
	{
		return;
	}

	EDITOR_SCENE_SCOPE_TIMER( ("Canvas Layout") );

	Inspect::Container* container = m_Generator->GetContainer();

	for ( Inspect::V_Control::const_iterator itr = controls.begin(), end = controls.end(); itr != end; ++itr )
	{
		container->AddChild( *itr );
	}

	Inspect::Canvas* canvas = container->GetCanvas();

	canvas->Realize( NULL );
}

bool PropertiesManager::IsActive()
{
	return m_ThreadCount > 0;
}

void PropertiesManager::SyncThreads()
{
	while ( IsActive() )
	{
		Thread::Sleep( 1 );
	}
}