#pragma once

#include "Framework/Framework.h"

#include "Reflect/Object.h"
#include "Reflect/Structure.h"
#include "Framework/FlagSet.h"

namespace Helium
{
	class SequenceStep;
	class SequenceInstance;

	typedef uint32_t SequenceStateBitmask;

	class HELIUM_FRAMEWORK_API SequenceStepTransition : public Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE( SequenceStepTransition );
		static void PopulateStructure( Reflect::Structure& comp );

		inline bool operator==( const SequenceStepTransition& _rhs ) const;
		inline bool operator!=( const SequenceStepTransition& _rhs ) const;

	public:

		SequenceStepTransition() { }
		virtual ~SequenceStepTransition() { }

		Name m_NextStepName;
		float m_MinimumTimeInState;

		SequenceStep *m_NextStep; // Generated based on m_NextStepName
	};

	class HELIUM_FRAMEWORK_API SequenceStep : public Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE( SequenceStep );
		static void PopulateStructure( Reflect::Structure& comp );

		inline bool operator==( const SequenceStep& _rhs ) const;
		inline bool operator!=( const SequenceStep& _rhs ) const;

	public:

		SequenceStep() { }
		virtual ~SequenceStep() { }

		DynamicArray<SequenceStepTransition> m_Transitions;
		Name m_StepName;
		DynamicArray<Name> m_StateFlags;
		SequenceStateBitmask m_StateBitmask;
	};

	/// Base type for in-world entities.
	class HELIUM_FRAMEWORK_API SequenceDefinition : public Asset
	{
		HELIUM_DECLARE_ASSET( SequenceDefinition, Asset );
		static void PopulateStructure( Reflect::Structure& comp );

	public:

		/// @name Construction/Destruction
		//@{
		SequenceDefinition();
		virtual ~SequenceDefinition();
		//@}

		virtual void FinalizeLoad();

	private:
		friend SequenceInstance;

		DynamicArray<SequenceStep> m_Steps;
		DynamicArray<Name> m_StateFlags;
		FlagSetDefinitionPtr m_StateFlagSet;
		Name m_InitialStepName;

		SequenceStep *m_InitialStep; // Generated based on name
	};
	typedef Helium::StrongPtr<SequenceDefinition> SequenceDefinitionPtr;
	typedef Helium::StrongPtr<const SequenceDefinition> ConstSequenceDefinitionPtr;

	class HELIUM_FRAMEWORK_API SequenceInstance : public Reflect::Object
	{
	public:
		SequenceInstance() { }
		virtual ~SequenceInstance() { }

		void Initialize( const SequenceDefinition *pSequenceDefinition );

		void Tick( float dt );
		void OnEnterStep( SequenceStep *pStep );
		void OnExitStep( SequenceStep *pStep );

	private:
		ConstSequenceDefinitionPtr m_Definition;

		// Current step
		SequenceStep *m_CurrentStep;
		float m_TimeInStep;

		DynamicArray<SequenceInstance> m_SubSequences;
	};
}

#include "Framework/Sequence.inl"