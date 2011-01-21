#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar_t CONTAINER_ATTR_NAME[] = TXT( "name" );
        const static tchar_t CONTAINER_ATTR_ICON[] = TXT( "icon" );

        ///////////////////////////////////////////////////////////////////////
        namespace UIHint
        {
            enum UIHints
            {
                Advanced = 1 << 0,
                Popup = 1 << 1,
            };

            const uint32_t Default = 0;
        }
        typedef uint32_t UIHints;

        ///////////////////////////////////////////////////////////////////////
        // Contains other controls and distributes layout logic
        //
        class FOUNDATION_API Container : public Control
        {
        public:
            REFLECT_DECLARE_OBJECT( Container, Control );

            Container();
            ~Container();

            const V_Control& GetChildren() const
            {
                return m_Children;
            }

            inline V_Control ReleaseChildren()
            {
                HELIUM_ASSERT( !this->IsRealized() );
                V_Control children = m_Children;
                Clear();
                return children;
            }

            virtual void AddChild( Control* control );
            virtual void InsertChild( int index, Control* control );
            virtual void RemoveChild( Control* control );
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

            UIHints GetUIHints() const;
            void SetUIHints( const UIHints hints );

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

            Attribute< tstring > a_Name;
            Attribute< tstring > a_Icon;

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

            UIHints m_UIHints;
        };

        typedef Helium::StrongPtr<Container> ContainerPtr;
        typedef std::vector<ContainerPtr> V_Container;
    }
}