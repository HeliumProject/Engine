#include "Precompile.h"
#include "Framework/StateMachine.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

HELIUM_DEFINE_BASE_STRUCT( Helium::StateTransition );

Helium::StateTransition::StateTransition() 
: m_RequiredPredicateResult(true)
, m_MinimumTimeInState(0.0f)
, m_NextState(NULL)
{

}

void StateTransition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &StateTransition::m_NextStateName, "m_NextStateName" );
	comp.AddField( &StateTransition::m_MinimumTimeInState, "m_MinimumTimeInState" );
	comp.AddField( &StateTransition::m_RequiredPredicate, "m_RequiredPredicate" );
	comp.AddField( &StateTransition::m_RequiredPredicateResult, "m_RequiredPredicateResult" );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::State );

void State::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &State::m_Transitions, "m_Transitions" );
	comp.AddField( &State::m_StateName, "m_StateName" );
	comp.AddField( &State::m_StateFlags, "m_StateFlags" );
	comp.AddField( &State::m_OnEnterAction, "m_OnEnterAction" );
	comp.AddField( &State::m_OnExitAction, "m_OnExitAction" );
}

HELIUM_IMPLEMENT_ASSET( Helium::StateMachineDefinition, Framework, 0 );

void StateMachineDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &StateMachineDefinition::m_States, "m_States" );
	//comp.AddField( &StateMachineDefinition::m_StateFlags, "m_StateFlags" );
	comp.AddField( &StateMachineDefinition::m_StateFlagSet, "m_StateFlagSet" );
	comp.AddField( &StateMachineDefinition::m_InitialStateName, "m_InitialStateName" );
}

/// Constructor.
StateMachineDefinition::StateMachineDefinition()
{
}

/// Destructor.
StateMachineDefinition::~StateMachineDefinition()
{
}

void StateMachineDefinition::FinalizeLoad()
{
	typedef HashMap< Name, State * > HM_States;
	HM_States stateLookupMap;

	// Populate the state lookup map
	for ( DynamicArray<State>::Iterator stateIter = m_States.Begin();
		stateIter != m_States.End(); ++stateIter )
	{
		if (stateIter->m_StateName.IsEmpty())
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"StateMachine '%s' has a state with no name specified\n",
				*GetPath().ToString());

			continue;
		}

		HM_States::Iterator mapIter = stateLookupMap.Find(stateIter->m_StateName);

		if ( mapIter != stateLookupMap.End() )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"StateMachine '%s' has multiple states with the name '%s'\n",
				*GetPath().ToString(),
				*stateIter->m_StateName);

			continue;
		}

		stateLookupMap.Insert( mapIter, HM_States::ValueType( stateIter->m_StateName, &*stateIter ) );
	}

	// Set up the states/transitions
	for (DynamicArray<State>::Iterator stateIter = m_States.Begin();
		stateIter != m_States.End(); ++stateIter)
	{
		if (stateIter->m_StateName.IsEmpty())
		{
			// We gave an error message for this already
			continue;
		}

		{
			HM_States::Iterator mapIter = stateLookupMap.Find(stateIter->m_StateName);
			if (mapIter == stateLookupMap.End() || mapIter->Second() != &*stateIter)
			{
				// We had multiple states of the same name and this was a dupe, so skip it
				continue;
			}
		}

		if ( !stateIter->m_StateFlags.IsEmpty() )
		{
			if ( !m_StateFlagSet.ReferencesObject() )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					"State '%s' located in StateMachine '%s' has values in m_StateFlags, but there is no FlagSetDefinition set in the state machine.\n",
					*stateIter->m_StateName,
					*GetPath().ToString());
			}
			else
			{
				for ( DynamicArray< Name >::Iterator flagIter = stateIter->m_StateFlags.Begin();
					flagIter != stateIter->m_StateFlags.End(); ++flagIter )
				{
					StateBitmask bits = 0;
					if (m_StateFlagSet->GetFlag(*flagIter, bits))
					{
						stateIter->m_StateBitmask |= bits;
					}
					else
					{
						HELIUM_TRACE(
							TraceLevels::Warning,
							"BulletBodyComponentDefinition::FinalizeLoad - Body '%s' refers to a body flag '%s' in field m_AssignedGroupFlags that does not exist. Is the flag defined in '%s'?\n",
							*GetPath().ToString(),
							**flagIter,
							*m_StateFlagSet->GetPath().ToString());
					}
				}
			}
		}

		// Set up the transitions
		for (DynamicArray<StateTransition>::Iterator transitionIter = stateIter->m_Transitions.Begin();
			transitionIter != stateIter->m_Transitions.End(); ++transitionIter)
		{
			HM_States::Iterator mapIter = stateLookupMap.Find(transitionIter->m_NextStateName);
			if (mapIter == stateLookupMap.End())
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					"Transition in state '%s' located in state machine '%s' refers to a state named '%s', but that state is not in this state machine\n",
					*stateIter->m_StateName,
					*GetPath().ToString(),
					*transitionIter->m_NextStateName);

				continue;
			}

			transitionIter->m_NextState = mapIter->Second();
		}
	}

	// Set the initial state
	HM_States::Iterator mapIter = stateLookupMap.Find(m_InitialStateName);
	if (mapIter != stateLookupMap.End())
	{
		m_InitialState = mapIter->Second();
	}
	else
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"StateMachine '%s' refers to an initial state named '%s', but this state does not exist in the state machine.",
			*GetPath().ToString(),
			*m_InitialStateName);
	}
}

void StateMachineInstance::Initialize( World &world, const StateMachineDefinition *pStateMachineDefinition )
{
	HELIUM_ASSERT( pStateMachineDefinition );
	m_Definition = pStateMachineDefinition;
	m_CurrentState = pStateMachineDefinition->m_InitialState;
	OnEnterState( world, m_CurrentState );
}

void StateMachineInstance::Tick( World &world, float dt )
{
	float timeInTick = dt;

	while ( timeInTick > 0.0f )
	{
		bool m_Transitioned = false;
		for ( DynamicArray<StateTransition>::Iterator iter = m_CurrentState->m_Transitions.Begin();
			iter != m_CurrentState->m_Transitions.End(); ++iter )
		{
			float timeToConsume;
			if ( EvaluateTransition( world, *iter, timeInTick, timeToConsume ) )
			{
				timeInTick -= timeToConsume;
				m_TimeInState += timeToConsume;

				DoTransition( world, m_CurrentState, iter->m_NextState );

				m_Transitioned = true;
				break;
			}
		}

		if ( !m_Transitioned )
		{
			m_TimeInState += timeInTick;
			break;
		}
	}
}

bool StateMachineInstance::EvaluateTransition( World &world, StateTransition &transition, float dt, float &timeToConsume )
{
	if ( m_TimeInState + dt < transition.m_MinimumTimeInState )
	{
		return false;
	}

	// Do additional checking
	if ( transition.m_RequiredPredicate && !transition.m_RequiredPredicate->Evaluate( world, NULL ) )
	{
		return false;
	}

	timeToConsume = transition.m_MinimumTimeInState - m_TimeInState;
	HELIUM_ASSERT(timeToConsume >= 0.0f);
	HELIUM_ASSERT(timeToConsume <= dt);
	return true;
}

void StateMachineInstance::DoTransition( World &world, State *pOldState, State *pNewState )
{
	OnExitState( world, m_CurrentState );

	m_CurrentState = pNewState;

	OnEnterState( world, m_CurrentState );
}

void StateMachineInstance::OnEnterState( World &world, State *pState )
{
	HELIUM_TRACE(
		TraceLevels::Debug,
		"StateMachineInstance::OnEnterState %x %s\n",
		this,
		*pState->m_StateName);

	m_TimeInState = 0.0f;

	if (pState->m_OnEnterAction)
	{
		pState->m_OnEnterAction->PerformAction( world, NULL );
	}
}

void StateMachineInstance::OnExitState( World &world, State *pState )
{
	HELIUM_TRACE(
		TraceLevels::Debug,
		"StateMachineInstance::OnExitState %x %s\n",
		this,
		*pState->m_StateName);

	if (pState->m_OnExitAction)
	{
		pState->m_OnExitAction->PerformAction( world, NULL );
	}
}
