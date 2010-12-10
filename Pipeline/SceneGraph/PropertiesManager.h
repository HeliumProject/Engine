#pragma once

#include "Foundation/CommandQueue.h"
#include "Foundation/Inspect/Controls.h"

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/PropertiesGenerator.h"
#include "Pipeline/SceneGraph/Selection.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PropertiesManager;

        namespace PropertiesStyles
        {
            enum PropertiesStyle
            {
                Intersection,
                Union,
            };
        }
        typedef PropertiesStyles::PropertiesStyle PropertiesStyle;

        struct ElementTypeFlags
        {
            ElementTypeFlags( const Reflect::Class* type, int32_t includeFlags, int32_t excludeFlags )
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

            const Reflect::Class* m_Type;
            int32_t m_IncludeFlags;
            int32_t m_ExcludeFlags;
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

            void EnumerateElement(Reflect::Element* element, int32_t includeFlags = 0xFFFFFFFF, int32_t excludeFlags = 0x0 )
            {
                // this will insert an empty map at the slot for the type of "element", or just make "b" false and return the iter at the existing one
                Helium::StdInsert<M_ElementByType>::Result inserted = m_CurrentElements.insert( M_ElementByType::value_type (ElementTypeFlags ( element->GetClass(), includeFlags, excludeFlags ), element) );
            }

            M_ElementByType&        m_CurrentElements;
            M_ElementsByType&       m_CommonElements;
            M_InterpretersByType&   m_CommonElementInterpreters;
        };

        struct PropertiesCreatedArgs
        {
            PropertiesCreatedArgs( PropertiesManager* propertiesManager, uint32_t selectionId, const Inspect::V_Control& controls )
                : m_PropertiesManager( propertiesManager )
                , m_SelectionId( selectionId )
                , m_Controls( controls )
            {
            }

            PropertiesManager*  m_PropertiesManager;
            uint32_t            m_SelectionId;
            Inspect::V_Control  m_Controls;
        };

        typedef Helium::Signature< const PropertiesCreatedArgs& > PropertiesCreatedSignature;

        struct PropertiesThreadArgs
        {
            PropertiesThreadArgs( PropertiesStyle setting, uint32_t selectionId, const uint32_t* currentSelectionId, const OS_SceneNodeDumbPtr& selection )
                : m_SelectionId( selectionId )
                , m_CurrentSelectionId( currentSelectionId )
                , m_Style( setting )
            {
                for ( OS_SceneNodeDumbPtr::Iterator itr = selection.Begin(), end = selection.End(); itr != end; ++itr )
                {
                    m_Selection.Append( *itr );
                }
            }

            PropertiesStyle             m_Style;
            uint32_t                    m_SelectionId;
            const uint32_t*             m_CurrentSelectionId;
            OrderedSet<SceneNodePtr>    m_Selection;
        };

        class PIPELINE_API PropertiesManager : public Helium::RefCountBase< PropertiesManager >
        {
        public:
            PropertiesManager( PropertiesGenerator* generator, CommandQueue* commandQueue );
            ~PropertiesManager();

            void Show( const Inspect::CanvasShowArgs& args );

            void SetProperties(PropertiesStyle setting);
            void SetSelection(const OS_SceneNodeDumbPtr& selection);

            // inspect selection begin creating the property UI
            void CreateProperties();

        private:
            // the thread entry point to do the property creation work
            void GeneratePropertiesThreadEntry( PropertiesThreadArgs& args );

            // called from the thread entry function, does the property creation work
            void GenerateProperties( PropertiesThreadArgs& args );

        public:
            // display the UI (in the main UI thread)
            void Present( uint32_t selectionId, const Inspect::V_Control& controls );

            // are any threads currently active?
            bool IsActive();

            // wait for threads to complete
            void SyncThreads();

            // event to raise when the properties are done being created
            PropertiesCreatedSignature::Event e_PropertiesCreated;

        private:
            // generator container
            PropertiesGenerator*            m_Generator;

            // to defer the finalization of the properties (which are processed in a b/g thread)
            CommandQueue*                   m_CommandQueue;

            // selection to create properties for
            OS_SceneNodeDumbPtr             m_Selection;

            // do we want to be the intersection or union?
            PropertiesStyle                 m_Style;

            // dirty flag for when the selection is out of date with the canvas
            bool                            m_SelectionDirty;

            // thread info for generating properties
            uint32_t                        m_SelectionId;

            // thread count
            volatile int32_t                m_ThreadCount;
        };

        typedef Helium::SmartPtr< PropertiesManager > PropertiesManagerPtr;
    }
}