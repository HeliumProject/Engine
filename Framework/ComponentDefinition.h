
#pragma once

#include "Framework/Framework.h"
#include "Framework/Components.h"
#include "Engine/Asset.h"

namespace Helium
{
	// Generally components are created by filling out a component definition and using that to allocate a component.
	// In order to support the case where components depend on each other's presence to finish initialization, we use 
	// two phases. (CreateComponent, then FinalizeComponent) Definitions can reference other definitions, so we retain
	// a reference to the created component to help in wiring components directly to components
	//
	// TODO: Support abstract Assets so we can make a few of these functions pure virtual
	class HELIUM_FRAMEWORK_API ComponentDefinition : public Reflect::Object
	{
	public:
		HELIUM_DECLARE_CLASS(Helium::ComponentDefinition, Reflect::Object);
		
		// Allocates a component. Initialization is not complete without calling FinalizeComponent()
		inline Helium::Component *CreateComponent(struct Components::IHasComponents &target) const;

		// Implemented by child classes to allocate a component of the appropriate type and return it
		inline virtual Helium::Component *CreateComponentInternal(struct Components::IHasComponents &rHasComponents) const;

		// Implemented by child classes to finish setting up the component.
		inline virtual void FinalizeComponent() const;

		// Gets the component that this definition generated previously
		inline Helium::Component *GetCreatedComponent() const;

		void Clear() const { m_Instance.Reset(NULL); }

	private:
		mutable Helium::ComponentPtr<Component> m_Instance;
	};
	typedef Helium::StrongPtr<ComponentDefinition> ComponentDefinitionPtr;

	template <
		class ComponentT, 
		class ComponentDefinitionT
	>
	class ComponentDefinitionHelper : public Helium::ComponentDefinition
	{
		Helium::Component *CreateComponentInternal(struct Components::IHasComponents &rHasComponents) const
		{
			ComponentT *c = rHasComponents.VirtualGetComponentManager()->Allocate<ComponentT>(&rHasComponents, rHasComponents.VirtualGetComponents());
			c->Initialize( *Reflect::AssertCast<ComponentDefinitionT>(this) );
			return c;
		}

		virtual void FinalizeComponent() const
		{

		}
	};

	template <
		class ComponentT, 
		class ComponentDefinitionT
	>
	class ComponentDefinitionHelperWithFinalize : public Helium::ComponentDefinition
	{
		Helium::Component *CreateComponentInternal(struct Components::IHasComponents &rHasComponents) const
		{
			ComponentT *c = rHasComponents.VirtualGetComponentManager()->Allocate<ComponentT>(&rHasComponents, rHasComponents.VirtualGetComponents());
			c->Initialize( *Reflect::AssertCast<ComponentDefinitionT>(this) );
			return c;
		}

		virtual void FinalizeComponent() const
		{
			Component *c = GetCreatedComponent();
			ComponentT *pComponent = static_cast<ComponentT *>(c);
			pComponent->Finalize( *Reflect::AssertCast<ComponentDefinitionT>(this) );
		}
	};

	template <
		class ComponentT, 
		class ComponentDefinitionT
	>
	class ComponentDefinitionHelperFinalizeOnly : public Helium::ComponentDefinition
	{
		Helium::Component *CreateComponentInternal(struct Components::IHasComponents &rHasComponents) const
		{
			ComponentT *c = rHasComponents.VirtualGetComponentManager()->Allocate<ComponentT>(&rHasComponents, rHasComponents.VirtualGetComponents());
			return c;
		}

		virtual void FinalizeComponent() const
		{
			Component *c = GetCreatedComponent();
			ComponentT *pComponent = static_cast<ComponentT *>(c);
			pComponent->Finalize( *Reflect::AssertCast<ComponentDefinitionT>(this) );
		}
	};
	
#if 0
	// Due to MS compiler bugs, this will crash the compiler if you try to compile it (C1001). So will have to make individual classes for this.. urgh
	// If you ever bring this back don't forget Clear();
	template <
		class ComponentT, 
		class ComponentDefinitionT, 
		void (ComponentT::* InitFn)(const ComponentDefinition &), 
		void (ComponentT::* FinalizeFn)(const ComponentDefinition &)
	>
	class ComponentDefinitionHelperT : public Helium::ComponentDefinition
	{
		Helium::Component *CreateComponentInternal(struct Components::IHasComponents &rHasComponents) const
		{
			ComponentT *c = rHasComponents.VirtualGetComponentManager()->Allocate<ComponentT>(&rHasComponents, rHasComponents.VirtualGetComponents());
			//if (InitFn)
			//{
				//(*c.*InitFn)( *Reflect::AssertCast<ComponentDefinitionT>(this) );
			//}
			return c;
		}

		virtual void FinalizeComponent() const
		{
			//if (FinalizeFn)
			//{
				Component *c = GetCreatedComponent();
				ComponentT *pComponent = static_cast<ComponentT *>(c);
				//(*pComponent.*FinalizeFn)( *Reflect::AssertCast<ComponentDefinitionT>(this) );
			//}
		}
	};
#endif
}

#include "Framework/ComponentDefinition.inl"
