
#pragma once

#include "Framework/Framework.h"
#include "Engine/Asset.h"

namespace Helium
{
	class SystemDefinition;

	class HELIUM_FRAMEWORK_API SystemComponent : public Asset
	{
		HELIUM_DECLARE_ASSET( Helium::SystemComponent, Helium::Asset )
		static void PopulateMetaType( Reflect::MetaStruct& comp );

	public:
		SystemComponent();

		void DoFinalize()
		{
			if ( !m_Finalized )
			{
				Finalize();

				ms_FinalizeOrder.Push( this );
				m_Finalized = true;
			}
		}

		static void CleanupAllComponents()
		{
			while ( !ms_FinalizeOrder.IsEmpty() )
			{
				SystemComponent *pComponent = ms_FinalizeOrder.Pop();
				pComponent->Cleanup();
			}
		}

	protected:
		friend SystemDefinition;

		// These are private to enforce that components only call ConditionalFinalize/ConditionalCleanup so that we can guarantee Finalize/Cleanup
		// get called exactly once

		// Two phase construction. Initialize should init any pointers that other downstream systems might reference so that in finalize,
		// you can assume any components you depend on exist (but may not yet be finalized).
		virtual void Initialize() { }
		virtual void Finalize() { }

		// Two phase destruction. Delay actually destroying memory until destroy so that downstream systems that refer to your system
		// can cleanup their references first
		virtual void Cleanup() { }
		virtual void Destroy() { }

		// Guarantee we only finalize everything exactly once
		bool m_Finalized;

		// Retain the order in which finalize() gets called so we can guarantee Cleanup gets called in the reverse order
		static DynamicArray< SystemComponent *> ms_FinalizeOrder;
	};
	typedef Helium::StrongPtr< SystemComponent > SystemComponentDefinitionPtr;

	class HELIUM_FRAMEWORK_API SystemDefinition : public Asset
	{
		HELIUM_DECLARE_ASSET( Helium::SystemDefinition, Helium::Asset )
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		void Initialize();
		void Cleanup();

		DynamicArray< SystemComponentDefinitionPtr > m_SystemComponents;
	};
	typedef Helium::StrongPtr< SystemDefinition > SystemDefinitionPtr;
}

#include "Framework/SystemDefinition.inl"
