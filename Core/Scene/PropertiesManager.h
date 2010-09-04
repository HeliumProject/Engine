#pragma once

#include "Foundation/CommandQueue.h"
#include "Foundation/Inspect/Controls.h"

#include "Core/API.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Core/Scene/Selection.h"

namespace Helium
{
    namespace Core
    {
        class PropertiesManager;

        namespace PropertySettings
        {
            enum PropertySetting
            {
                Intersection,
                Union,
            };
        }
        typedef PropertySettings::PropertySetting PropertySetting;

        struct ElementTypeFlags
        {
            ElementTypeFlags( i32 type, i32 includeFlags, i32 excludeFlags )
                : m_Type( type )
                , m_IncludeFlags( includeFlags )
                , m_ExcludeFlags( excludeFlags )
            {
            }

            ElementTypeFlags& operator=( const ElementTypeFlags& rhs )
            {
                if ( &rhs != this )
                {
                    m_Type = rhs.m_Type;
                    m_IncludeFlags = rhs.m_IncludeFlags;
                    m_ExcludeFlags = rhs.m_ExcludeFlags;
                }
                return *this;
            }

            bool operator==( const ElementTypeFlags& rhs ) const
            {
                return m_Type == rhs.m_Type
                    && m_IncludeFlags == rhs.m_IncludeFlags
                    && m_ExcludeFlags == rhs.m_ExcludeFlags;
            }

            bool operator<( const ElementTypeFlags& rhs ) const
            {
                if ( m_Type != rhs.m_Type )
                    return m_Type < rhs.m_Type;

                if ( m_IncludeFlags != rhs.m_IncludeFlags )
                    return m_IncludeFlags < rhs.m_IncludeFlags;

                return m_ExcludeFlags < rhs.m_ExcludeFlags;
            }

            i32 m_Type;
            i32 m_IncludeFlags;
            i32 m_ExcludeFlags;
        };

        typedef std::map< ElementTypeFlags, Reflect::Element* >                 M_ElementByType;
        typedef std::map< ElementTypeFlags, std::vector<Reflect::Element*> >    M_ElementsByType;
        typedef std::map< ElementTypeFlags, Inspect::InterpreterPtr >           M_InterpretersByType;

        struct EnumerateElementArgs
        {
            EnumerateElementArgs( M_ElementByType& currentElements, M_ElementsByType& commonElements, M_InterpretersByType& commonElementInterpreters )
                : m_CurrentElements (currentElements)
                , m_CommonElements (commonElements)
                , m_CommonElementInterpreters (commonElementInterpreters)
            {

            }

            void EnumerateElement(Reflect::Element* element, i32 includeFlags = 0xFFFFFFFF, i32 excludeFlags = 0x0 )
            {
                // this will insert an empty map at the slot for the type of "element", or just make "b" false and return the iter at the existing one
                Helium::Insert<M_ElementByType>::Result inserted = m_CurrentElements.insert( M_ElementByType::value_type (ElementTypeFlags ( element->GetType(), includeFlags, excludeFlags ), element) );
            }

            M_ElementByType&        m_CurrentElements;
            M_ElementsByType&       m_CommonElements;
            M_InterpretersByType&   m_CommonElementInterpreters;
        };

        struct PropertiesCreatedArgs
        {
            PropertiesCreatedArgs( PropertiesManager* propertiesManager, u32 selectionId, const Inspect::V_Control& controls )
                : m_PropertiesManager( propertiesManager )
                , m_SelectionId( selectionId )
                , m_Controls( controls )
            {
            }

            PropertiesManager*  m_PropertiesManager;
            u32                 m_SelectionId;
            Inspect::V_Control  m_Controls;
        };

        typedef Helium::Signature< const PropertiesCreatedArgs& > PropertiesCreatedSignature;

        struct PropertyThreadArgs
        {
            PropertyThreadArgs( const PropertyThreadArgs& args )
                : m_Selection( args.m_Selection )
                , m_SelectionId( args.m_SelectionId )
                , m_CurrentSelectionId( args.m_CurrentSelectionId )
                , m_Setting( args.m_Setting )
                , m_Container( args.m_Container )
            {
            }

            PropertyThreadArgs( const OS_SelectableDumbPtr& selection, u32 selectionId, const u32* currentSelectionId, PropertySetting setting, Inspect::Container* container )
                : m_SelectionId( selectionId )
                , m_CurrentSelectionId( currentSelectionId )
                , m_Setting( setting )
                , m_Container( container )
            {
                for ( OS_SelectableDumbPtr::Iterator itr = selection.Begin(), end = selection.End(); itr != end; ++itr )
                {
                    m_Selection.Append( *itr );
                }
            }

            OrderedSet<SelectablePtr>           m_Selection;
            u32                                 m_SelectionId;
            const u32*                          m_CurrentSelectionId;
            PropertySetting                     m_Setting;
            Inspect::ContainerPtr               m_Container;
        };

        typedef Signature< VoidSignature::Delegate > VoidDelegateSignature;

        class CORE_API PropertiesManager : public Helium::RefCountBase< PropertiesManager >
        {
        public:
            PropertiesManager( PropertiesGenerator* generator, CommandQueue* commandQueue );
            ~PropertiesManager();

            void Show( const Inspect::CanvasShowArgs& args );

            void SetProperties(PropertySetting setting);
            void SetSelection(const OS_SelectableDumbPtr& selection);

            // inspect selection begin creating the property UI
            void CreateProperties();

        private:
            // the thread entry point to do the property creation work
            void GeneratePropertiesThreadEntry( PropertyThreadArgs& args );

            // called from the thread entry function, does the property creation work
            void GenerateProperties( PropertyThreadArgs& args );

        public:
            // display the UI (in the main UI thread)
            void FinalizeProperties( u32 selectionId, const Inspect::V_Control& controls );

            bool ThreadsActive();

            // event to raise when the properties are done being created
            PropertiesCreatedSignature::Event e_PropertiesCreated;

        private:
            // generator container
            PropertiesGenerator*            m_Generator;

            // to defer the finalization of the properties (which are processed in a b/g thread)
            CommandQueue*                   m_CommandQueue;

            // selection to create properties for
            OS_SelectableDumbPtr            m_Selection;

            // do we want to be the intersection or union?
            PropertySetting                 m_Setting;

            // dirty flag for when the selection is out of date with the canvas
            bool                            m_SelectionDirty;

            // thread info for generating properties
            u32                             m_SelectionId;

            // thread count
            int                             m_ThreadCount;
            Helium::Mutex                   m_ThreadCountMutex;
        };

        typedef Helium::SmartPtr< PropertiesManager > PropertiesManagerPtr;
    }
}