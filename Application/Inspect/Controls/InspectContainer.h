#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectControl.h"

namespace Helium
{
    namespace Inspect
    {
        //
        // Contains other controls and distributes layout logic
        //

        class APPLICATION_API Container : public Reflect::ConcreteInheritor<Container, Control>
        {
        public:
            Container();
            virtual ~Container();

            const V_Control& GetChildren() const
            {
                return m_Children;
            }

            void AddChild(Control* control);
            void InsertChild(int index, Control* control);
            void RemoveChild(Control* control);
            void Clear();

            // recusively binds contained controls to data
            virtual void Bind(const DataPtr& data) HELIUM_OVERRIDE;

            // realize control
            virtual void Realize(Container* parent) HELIUM_OVERRIDE;

            // populate
            virtual void Populate() HELIUM_OVERRIDE;

            // refreshes the UI state from data
            virtual void Read() HELIUM_OVERRIDE;

            // updates the data based on the state of the UI
            virtual bool Write() HELIUM_OVERRIDE;

            //
            // Events
            //

            ControlSignature::Event& ControlAdded()
            {
                return m_ControlAdded;
            }

            ControlSignature::Event& ControlRemoved()
            {
                return m_ControlRemoved;
            }

        private:
            void AddListeners( Control* control );
            void RemoveListeners( Control* control );
            void IsEnabledChanged( const Attribute<bool>::ChangeArgs& args );
            void IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args );
            void IsFrozenChanged( const Attribute<bool>::ChangeArgs& args );
            void IsHiddenChanged( const Attribute<bool>::ChangeArgs& args );

        protected:
            // the children controls
            V_Control m_Children;

            // on add/remove
            ControlSignature::Event m_ControlAdded;
            ControlSignature::Event m_ControlRemoved;
        };

        typedef Helium::SmartPtr<Container> ContainerPtr;
        typedef std::vector<ContainerPtr> V_Container;
    }
}