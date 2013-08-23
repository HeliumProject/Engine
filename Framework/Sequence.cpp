#include "FrameworkPch.h"
#include "Framework/Sequence.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

HELIUM_DEFINE_BASE_STRUCT( Helium::SequenceStepTransition );

void Helium::SequenceStepTransition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &SequenceStepTransition::m_NextStepName, "m_NextStepName" );
	comp.AddField( &SequenceStepTransition::m_MinimumTimeInState, "m_MinimumTimeInState" );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::SequenceStep );

void Helium::SequenceStep::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &SequenceStep::m_Transitions, "m_Transitions" );
	comp.AddField( &SequenceStep::m_StepName, "m_StepName" );
	comp.AddField( &SequenceStep::m_StateFlags, "m_StateFlags" );
}

HELIUM_IMPLEMENT_ASSET( Helium::SequenceDefinition, Framework, 0 );

void Helium::SequenceDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &SequenceDefinition::m_Steps, "m_Steps" );
	comp.AddField( &SequenceDefinition::m_StateFlags, "m_StateFlags" );
	comp.AddField( &SequenceDefinition::m_StateFlagSet, "m_StateFlagSet" );
	comp.AddField( &SequenceDefinition::m_InitialStepName, "m_InitialStepName" );
}

/// Constructor.
SequenceDefinition::SequenceDefinition()
{
}

/// Destructor.
SequenceDefinition::~SequenceDefinition()
{
}

void Helium::SequenceDefinition::FinalizeLoad()
{
	typedef HashMap< Name, SequenceStep * > HM_SequenceSteps;
	HM_SequenceSteps stepLookupMap;

	// Populate the step lookup map
	for ( DynamicArray<SequenceStep>::Iterator stepIter = m_Steps.Begin();
		stepIter != m_Steps.End(); ++stepIter )
	{
		if (stepIter->m_StepName.IsEmpty())
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"Sequence '%s' a step with no name specified\n",
				*GetPath().ToString());

			continue;
		}

		HM_SequenceSteps::Iterator mapIter = stepLookupMap.Find(stepIter->m_StepName);

		if ( mapIter != stepLookupMap.End() )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"Sequence '%s' has multiple steps with the name '%s'\n",
				*GetPath().ToString(),
				*stepIter->m_StepName);

			continue;
		}

		stepLookupMap.Insert( mapIter, HM_SequenceSteps::ValueType( stepIter->m_StepName, &*stepIter ) );
	}

	// Set up the states/transitions
	for (DynamicArray<SequenceStep>::Iterator stepIter = m_Steps.Begin();
		stepIter != m_Steps.End(); ++stepIter)
	{
		if (stepIter->m_StepName.IsEmpty())
		{
			// We gave an error message for this already
			continue;
		}

		{
			HM_SequenceSteps::Iterator mapIter = stepLookupMap.Find(stepIter->m_StepName);
			if (mapIter == stepLookupMap.End() || mapIter->Second() != &*stepIter)
			{
				// We had multiple steps of the same name and this was a dupe, so skip it
				continue;
			}
		}

		if ( !stepIter->m_StateFlags.IsEmpty() )
		{
			if ( !m_StateFlagSet.ReferencesObject() )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					"Step '%s' located in sequence '%s' has values in m_StateFlags, but there is no FlagSetDefinition set in the sequence.\n",
					*stepIter->m_StepName,
					*GetPath().ToString());
			}
			else
			{
				for ( DynamicArray< Name >::Iterator flagIter = m_StateFlags.Begin();
					flagIter != m_StateFlags.End(); ++flagIter )
				{
					SequenceStateBitmask bits = 0;
					if (m_StateFlagSet->GetFlag(*flagIter, bits))
					{
						stepIter->m_StateBitmask |= bits;
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
		for (DynamicArray<SequenceStepTransition>::Iterator transitionIter = stepIter->m_Transitions.Begin();
			transitionIter != stepIter->m_Transitions.End(); ++transitionIter)
		{
			HM_SequenceSteps::Iterator mapIter = stepLookupMap.Find(transitionIter->m_NextStepName);
			if (mapIter == stepLookupMap.End())
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					"Transition in step '%s' located in sequence '%s' refers to a step named '%s', but that step is not in this sequence\n",
					*stepIter->m_StepName,
					*GetPath().ToString(),
					*transitionIter->m_NextStepName);

				continue;
			}

			transitionIter->m_NextStep = mapIter->Second();
		}
	}

	// Set the initial step
	HM_SequenceSteps::Iterator mapIter = stepLookupMap.Find(m_InitialStepName);
	if (mapIter != stepLookupMap.End())
	{
		m_InitialStep = mapIter->Second();
	}
	else
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"Sequence '%s' refers to an initial state named '%s', but this step does not exist in the sequence.",
			*GetPath().ToString(),
			*m_InitialStepName);
	}
}

void Helium::SequenceInstance::Initialize( const SequenceDefinition *pSequenceDefinition )
{
	m_CurrentStep = pSequenceDefinition->m_InitialStep;
	OnEnterStep( m_CurrentStep );
}

void Helium::SequenceInstance::Tick( float dt )
{
	float timeInTick = dt;

	while ( timeInTick > 0.0f )
	{
		for ( DynamicArray<SequenceStepTransition>::Iterator iter = m_CurrentStep->m_Transitions.Begin();
			iter != m_CurrentStep->m_Transitions.End(); ++iter )
		{
			if ( m_TimeInStep + timeInTick > iter->m_MinimumTimeInState )
			{
				// Do additional checking
				if ( false )
				{
					continue;
				}

				timeInTick -= m_TimeInStep + timeInTick - iter->m_MinimumTimeInState;
				m_CurrentStep = iter->m_NextStep;
				break;
			}
		}
	}
}

void Helium::SequenceInstance::OnEnterStep( SequenceStep *pStep )
{

}

void Helium::SequenceInstance::OnExitStep( SequenceStep *pStep )
{

}
