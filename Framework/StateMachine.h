#pragma once

#include "Framework/Framework.h"

#include "Reflect/Object.h"
#include "Reflect/MetaStruct.h"
#include "Framework/FlagSet.h"
#include "Framework/Action.h"
#include "Framework/Predicate.h"

namespace Helium
{
	class State;
	class StateMachineInstance;

	typedef uint32_t StateBitmask;

	class HELIUM_FRAMEWORK_API StateTransition : public Reflect::Struct
	{
		HELIUM_DECLARE_BASE_STRUCT( StateTransition );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		inline bool operator==( const StateTransition& _rhs ) const;
		inline bool operator!=( const StateTransition& _rhs ) const;

	public:
		StateTransition();
		virtual ~StateTransition() { }

		Name m_NextStateName;
		float m_MinimumTimeInState;

		PredicatePtr m_RequiredPredicate;
		bool m_RequiredPredicateResult;

		State *m_NextState; // Generated based on m_NextStateName
	};

	class HELIUM_FRAMEWORK_API State : public Reflect::Struct
	{
		HELIUM_DECLARE_BASE_STRUCT( State );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		inline bool operator==( const State& _rhs ) const;
		inline bool operator!=( const State& _rhs ) const;

	public:

		State() : m_StateBitmask(0) { }
		virtual ~State() { }

		DynamicArray<StateTransition> m_Transitions;
		Name m_StateName;
		DynamicArray<Name> m_StateFlags;

		StateBitmask m_StateBitmask; // Generated based on m_StateFlags

		ActionPtr m_OnEnterAction;
		ActionPtr m_OnExitAction;
	};

	/// Base type for in-world entities.
	class HELIUM_FRAMEWORK_API StateMachineDefinition : public Asset
	{
		HELIUM_DECLARE_ASSET( StateMachineDefinition, Asset );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

	public:

		/// @name Construction/Destruction
		//@{
		StateMachineDefinition();
		virtual ~StateMachineDefinition();
		//@}

		virtual void FinalizeLoad();

	private:
		friend StateMachineInstance;

		DynamicArray<State> m_States;
		FlagSetDefinitionPtr m_StateFlagSet;
		Name m_InitialStateName;

		State *m_InitialState; // Generated based on name
	};
	typedef Helium::StrongPtr<StateMachineDefinition> StateMachineDefinitionPtr;
	typedef Helium::StrongPtr<const StateMachineDefinition> ConstStateMachineDefinitionPtr;

	class HELIUM_FRAMEWORK_API StateMachineInstance : public Reflect::Object
	{
	public:
		StateMachineInstance() { }
		virtual ~StateMachineInstance() { }

		void Initialize( World &world, const StateMachineDefinition *pStateMachineDefinition );

		void Tick( World &pWorld, float dt );

		bool EvaluateTransition( World &world, StateTransition &transition, float dt, float &timeToConsume );
		void DoTransition( World &world, State *pOldState, State *pNewState );
		void OnEnterState( World &world, State *pState );
		void OnExitState( World &world, State *pState );

		StateBitmask GetCurrentFlags() const { return m_CurrentState ? m_CurrentState->m_StateBitmask : 0; }

	private:
		ConstStateMachineDefinitionPtr m_Definition;

		// Current state
		State *m_CurrentState;
		float m_TimeInState;

		DynamicArray<StateMachineInstance> m_SubStateMachines;
	};
}

#include "Framework/StateMachine.inl"