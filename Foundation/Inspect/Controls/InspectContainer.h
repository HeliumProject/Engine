#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Controls/InspectControl.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar CONTAINER_ATTR_NAME[] = TXT( "name" );

        //
        // Contains other controls and distributes layout logic
        //

        class FOUNDATION_API Container : public Reflect::ConcreteInheritor<Container, Control>
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

            const tstring& GetPath() const
            {
                if ( m_Path.empty() )
                {
                    BuildPath( m_Path );
                }

                return m_Path;
            }

            void BuildPath(tstring& path) const
            {
                if (m_Parent)
                {
                    m_Parent->BuildPath(path);
                }

                path += TXT( "|" ) + a_Name.Get();
            }

            // recusively binds contained controls to data
            virtual void Bind(const DataPtr& data) HELIUM_OVERRIDE;

            // process properties coming from script
            virtual bool Process(const tstring& key, const tstring& value) HELIUM_OVERRIDE;

            // populate
            virtual void Populate() HELIUM_OVERRIDE;

            // refreshes the UI state from data
            virtual void Read() HELIUM_OVERRIDE;

            // updates the data based on the state of the UI
            virtual bool Write() HELIUM_OVERRIDE;

            Attribute<tstring>                  a_Name;

            mutable ControlSignature::Event     e_ControlAdded;
            mutable ControlSignature::Event     e_ControlRemoved;

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

            // the path of the container
            mutable tstring m_Path;
        };

        typedef Helium::SmartPtr<Container> ContainerPtr;
        typedef std::vector<ContainerPtr> V_Container;
    }
}