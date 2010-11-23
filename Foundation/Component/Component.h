#pragma once

#include "Foundation/API.h" 
#include "Foundation/Automation/Event.h"
#include "Foundation/Component/ComponentExceptions.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/SerializerDeduction.h"

#include "Foundation/Component/SearchableProperties.h"

namespace Helium
{
    namespace Component
    {
        class ComponentCategory;
        class ComponentCollection;

        namespace ComponentUsages
        {
            enum ComponentUsage
            {
                // Can exist only in a Class of Assets/Entities/etc.
                Class,

                // Can exist only in an instance of an Asset/Entity/etc.
                Instance,

                // Can exist in either an Instance or Class:
                //   o in the class its the default value for all instances of that class,
                //   o in the instance, its the overrides for that attribute for that instance
                Overridable,  
            };
        }
        typedef ComponentUsages::ComponentUsage ComponentUsage;

        namespace ComponentBehaviors
        {
            enum ComponentBehavior
            {
                // Can inherently be added to any collection.
                Inclusive,

                // Cannot inherently be added to any collection (the collection's validation
                // code must explicitly let this attribute in).
                Exclusive
            };
        }
        typedef ComponentBehaviors::ComponentBehavior ComponentBehavior;


        //
        // Abstract Base Class - Subclass this for structured attribute data you can add to a collection
        //

        class FOUNDATION_API ComponentBase : public Reflect::Element
        {
        private:
            REFLECT_DECLARE_ABSTRACT( ComponentBase, Reflect::Element );
            static void EnumerateClass( Reflect::Compositor<ComponentBase>& comp );

        public:
            ComponentBase();
            virtual ~ComponentBase();


            // 
            // Component slots
            // 

            // Components are designed so that there can only be one of each kind in a
            // collection.  This function specifies what slot in the collection this
            // attribute should occupy.  By default, every attribute will occupy an
            // individual slot.  Derived classes can override this function to specify
            // what slot they belong to.
            virtual int32_t GetSlot() const
            {
                return GetType();
            }


            //
            // Component Usage API
            //

            virtual ComponentUsage GetComponentUsage() const = 0;

            virtual ComponentBehavior GetComponentBehavior() const
            {
                return ComponentBehaviors::Inclusive;
            }

            // 
            // Collection information
            // 

            // Get the collection we are an attribute of
            ComponentCollection* GetCollection() const;

            // Set the collection we are an attribute of
            void SetCollection(ComponentCollection* collection);

            // Check to make sure that this attribute can be added a collection with the sibling.
            // Return false if the attribute canot be added to that collection, and fill out the error 
            // string with a meaningful explanation that will be shown to the user.
            virtual bool ValidateSibling( const ComponentBase* attribute, tstring& error ) const;

            virtual void GatherSearchableProperties( Helium::SearchableProperties* properties ) const
            {
            }

        protected:
            // the collection we are an attribute of
            ComponentCollection* m_Collection;

        public:
            // should we utilized or ignored?
            bool m_IsEnabled;
        };

        typedef Helium::StrongPtr< ComponentBase > ComponentPtr;
        typedef std::vector< ComponentPtr > V_Component;
        typedef std::map< Reflect::TypeID, ComponentPtr > M_Component;
    }

}