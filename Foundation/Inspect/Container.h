#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar_t CONTAINER_ATTR_NAME[] = TXT( "name" );

        ///////////////////////////////////////////////////////////////////////
        namespace ContainerHint
        {
            enum Hints
            {
                Advanced         = 1 << 0,
            };

            const uint32_t Default = 0;
        }
        typedef uint32_t ContainerHints;

        ///////////////////////////////////////////////////////////////////////
        // Contains other controls and distributes layout logic
        //
        class FOUNDATION_API Container : public Control
        {
        public:
            REFLECT_DECLARE_CLASS( Container, Control );

            Container();
            ~Container();

            const V_Control& GetChildren() const
            {
                return m_Children;
            }

            inline V_Control ReleaseChildren()
            {
                V_Control children = m_Children;

                while ( !m_Children.empty() )
                {
                    RemoveChild( m_Children.front() );
                }

                m_Children.clear();

                return children;
            }

            virtual void AddChild(Control* control);
            virtual void InsertChild(int index, Control* control);
            virtual void RemoveChild(Control* control);
            virtual void Clear();

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

            ContainerHints GetHints() const;
            void SetHints( const ContainerHints hints );
            void AddHints( const ContainerHints hints );

            // recusively binds contained controls to data
            virtual void Bind(const DataBindingPtr& data) HELIUM_OVERRIDE;

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
            void IsEnabledChanged( const Attribute<bool>::ChangeArgs& args );
            void IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args );
            void IsFrozenChanged( const Attribute<bool>::ChangeArgs& args );
            void IsHiddenChanged( const Attribute<bool>::ChangeArgs& args );

        protected:
            // the children controls
            V_Control m_Children;

            // the path of the container
            mutable tstring m_Path;

            ContainerHints m_Hints;
        };

        typedef Helium::StrongPtr<Container> ContainerPtr;
        typedef std::vector<ContainerPtr> V_Container;
    }
}