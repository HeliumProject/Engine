#pragma once

#include "Foundation/API.h" 

#include <hash_map>

#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Serializers.h"

#include "Component.h"

namespace Component
{
    class ComponentCollection;

    struct ComponentCollectionChanged
    {
        ComponentCollectionChanged(const ComponentCollection* collection, const ComponentBase* attribute)
            : m_Collection(collection)
            , m_Component(attribute)
        {

        }

        const ComponentCollection*  m_Collection;
        const ComponentBase*        m_Component;
    };

    typedef Helium::Signature<void, const ComponentCollectionChanged&> ComponentCollectionChangedSignature;


    class FOUNDATION_API ComponentCollection : public Reflect::Element
    {
    public:
        REFLECT_DECLARE_CLASS(ComponentCollection, Reflect::Element);
        static void EnumerateClass( Reflect::Compositor<ComponentCollection>& comp );

        ComponentCollection();
        ComponentCollection( const ComponentPtr& attr );
        virtual ~ComponentCollection(); 

        virtual void GatherSearchableProperties( Helium::SearchableProperties* properties ) const;

        //
        // Component Management
        //

        // clear all attributes
        virtual void Clear();

        // get all attributes
        const M_Component& GetComponents() const;

        // retrieve attribute from a slot
        virtual const ComponentPtr& GetComponent(i32 slotID) const;

        // casting helper will get you what you need, baby ;)
        template <class T>
        Helium::SmartPtr<T> GetComponent() const
        {
            return Reflect::ObjectCast<T>( GetComponent( Reflect::GetType<T>() ) );
        }

        // template helper function for removing by type... 
        template <class T>
        void RemoveComponent()
        {
            return RemoveComponent( Reflect::GetType<T>() ); 
        }

        // Set attribute into a slot.  If validate param is false, ValidateComponent
        // will not be called, and it is up to the caller to do any necessary
        // checking, including for duplicate attributes.
        virtual void SetComponent(const ComponentPtr& attr, bool validate = true );

        // remove attribute from a slot
        virtual void RemoveComponent(i32 slotID);

        // queries the container for the existence of the specified attribute
        virtual bool ContainsComponent(i32 slotID) const;

        // Validates the attribute for add to this attribute collection.  If the addition is
        // not valid, the return value will be false and "error" will have additional info about
        // the problem.  The steps carried out by this function are:
        // 1. Makes sure the attribute is not already in this collection.
        // 2. Gives derived classes the chance to HELIUM_OVERRIDE ValidateCompatible.
        // 3. Iterates over each attribute already in the collection and calls ValidateSibling.
        virtual bool ValidateComponent( const ComponentPtr &attr, tstring& error ) const;

        // Basic implementation just checks the behavior of the attribute, but you can HELIUM_OVERRIDE
        // this to allow exclusive attributes, or prohibit inclusive attributes
        virtual bool ValidateCompatible( const ComponentPtr& attr, tstring& error ) const;

        // Basic implementation just allows the attribute to be persisted, but you can HELIUM_OVERRIDE
        // this to avoid persisting attributes that are at a redundant state (as an example)
        virtual bool ValidatePersistent( const ComponentPtr& attr ) const;

        // Comparision API (for AssetType classification) -- IDEALLY SHOULD GO AWAY!
        bool IsSubset( const ComponentCollection* collection ) const;


        //
        // Change API
        //

    public:
        // this is called by ComponentHandle when an attribute changes, or by code at large
        virtual void ComponentChanged( const ComponentBase* attr = NULL );

    protected:
        // this is a callback called by elements being changed by procedurally generated UI (EditorProperties)
        void ComponentChanged( const Reflect::ElementChangeArgs& args )
        {
            // call into the virtual prototype in case it gets overridden in a derived class
            ComponentChanged( Reflect::ConstAssertCast<ComponentBase>(args.m_Element) );
        }


        //
        // Events
        //

    protected:
        ComponentCollectionChangedSignature::Event m_SingleComponentChanged; 
    public:
        void AddComponentChangedListener(const ComponentCollectionChangedSignature::Delegate& d)
        {
            m_SingleComponentChanged.Add(d);
        }
        void RemoveComponentChangedListener(const ComponentCollectionChangedSignature::Delegate& d)
        {
            m_SingleComponentChanged.Remove(d);
        }

    protected:
        ComponentCollectionChangedSignature::Event m_ComponentAdded; 
    public: 
        void AddComponentAddedListener(const ComponentCollectionChangedSignature::Delegate& d)
        {
            m_ComponentAdded.Add(d); 
        }

        void RemoveComponentAddedListener(const ComponentCollectionChangedSignature::Delegate& d)
        {
            m_ComponentAdded.Remove(d); 
        }

    protected:
        ComponentCollectionChangedSignature::Event m_ComponentRemoved; 
    public: 
        void AddComponentRemovedListener(const ComponentCollectionChangedSignature::Delegate& d)
        {
            m_ComponentRemoved.Add(d); 
        }

        void RemoveComponentRemovedListener(const ComponentCollectionChangedSignature::Delegate& d)
        {
            m_ComponentRemoved.Remove(d); 
        }


        //
        // Element overrides
        //

    public:
        // migrate legacy attributes
        virtual bool ProcessComponent(Reflect::ElementPtr element, const tstring& fieldName) HELIUM_OVERRIDE;

        // setup changed callback
        virtual void PreSerialize() HELIUM_OVERRIDE;
        virtual void PostDeserialize() HELIUM_OVERRIDE;

        // copy all attributes from one collection to another
        virtual void CopyTo(const Reflect::ElementPtr& destination) HELIUM_OVERRIDE;

        // helper function for CopyTo
        bool CopyComponentTo( ComponentCollection& destCollection, const ComponentPtr& destAttrib, const ComponentPtr& srcAttrib );


        //
        // Members
        //

    protected:
        // indicates if the attribute collection has been modified in memory
        bool m_Modified;

    private:
        M_Component m_Components;
    };

    typedef Helium::SmartPtr<ComponentCollection> ComponentCollectionPtr;
}