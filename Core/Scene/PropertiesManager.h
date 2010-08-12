#pragma once

#include "Platform/Mutex.h"
#include "Application/Inspect/Controls/Controls.h"

#include "Core/API.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Core/Scene/Selection.h"

namespace Helium
{
    namespace Core
    {
        namespace PropertySettings
        {
            enum PropertySetting
            {
                Intersection,
                Union,
            };
        }
        typedef PropertySettings::PropertySetting PropertySetting;

        struct CORE_API ElementTypeFlags
        {
            i32 m_Type;

            i32 m_IncludeFlags;
            i32 m_ExcludeFlags;

            ElementTypeFlags( i32 type, i32 includeFlags, i32 excludeFlags ) : m_Type( type ), m_IncludeFlags( includeFlags ), m_ExcludeFlags( excludeFlags ) {}

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
                return m_Type == rhs.m_Type && m_IncludeFlags == rhs.m_IncludeFlags && m_ExcludeFlags == rhs.m_ExcludeFlags;
            }

            bool operator<( const ElementTypeFlags& rhs ) const
            {
                if ( m_Type != rhs.m_Type )
                    return m_Type < rhs.m_Type;

                if ( m_IncludeFlags != rhs.m_IncludeFlags )
                    return m_IncludeFlags < rhs.m_IncludeFlags;

                return m_ExcludeFlags < rhs.m_ExcludeFlags;
            }
        };

        struct CORE_API PropertiesCreatedArgs
        {
            class PropertiesManager* m_PropertiesManager;
            u32 m_SelectionId;
            Inspect::V_Control m_Controls;

            PropertiesCreatedArgs( PropertiesManager* propertiesManager, u32 selectionId, const Inspect::V_Control& controls )
                : m_PropertiesManager (propertiesManager)
                , m_SelectionId (selectionId)
                , m_Controls (controls)
            {
            }
        };

        // callback for when the properties are done
        typedef Helium::Signature< void, const PropertiesCreatedArgs& > PropertiesCreatedSignature;

        typedef Helium::OrderedSet<LSelectablePtr> OS_SelectablePtr;

        struct CORE_API PropertyThreadArgs
        {
            OS_SelectablePtr m_Selection;
            u32 m_SelectionId;
            const u32* m_CurrentSelectionId;
            PropertySetting m_Setting;
            Inspect::ContainerPtr m_Container;
            PropertiesCreatedSignature::Event& m_PropertiesCreated;

            PropertyThreadArgs( const PropertyThreadArgs& args );

            PropertyThreadArgs( const OS_SelectableDumbPtr& selection,
                u32 selectionId,
                const u32* currentSelectionId,
                PropertySetting setting,
                Inspect::Container* container,
                PropertiesCreatedSignature::Event& propertiesCreated );
        };

        typedef std::map<ElementTypeFlags, Reflect::Element*> M_ElementByType;
        typedef std::map<ElementTypeFlags, std::vector<Reflect::Element*> > M_ElementsByType;
        typedef std::map<ElementTypeFlags, Inspect::InterpreterPtr> M_InterpretersByType;
        typedef std::map<tstring, Inspect::InterpreterPtr> M_InterpretersBySymbol;

        class CORE_API EnumerateElementArgs
        {
        private:
            M_ElementByType& m_CurrentElements;
            M_ElementsByType& m_CommonElements;
            M_InterpretersByType& m_CommonElementInterpreters;

        public:
            EnumerateElementArgs( M_ElementByType& currentElements,
                M_ElementsByType& commonElements,
                M_InterpretersByType& commonElementInterpreters );

            void EnumerateElement(Reflect::Element* element, i32 includeFlags = 0xFFFFFFFF, i32 excludeFlags = 0x0 );
        };

        class CORE_API PropertiesManager : public Helium::RefCountBase< PropertiesManager > // : public Inspect::Interpreter
        {
        private:
            // generator container
            PropertiesGenerator* m_Generator;

            // selection to create properties for
            OS_SelectableDumbPtr m_Selection;

            // do we want to be the intersection or union?
            PropertySetting m_Setting;

            // dirty flag for when the selection is out of date with the canvas
            bool m_SelectionDirty;

            // thread info for generating properties
            u32 m_SelectionId;

            // event to raise when the properties are done being created
            PropertiesCreatedSignature::Event m_PropertiesCreated;

            // previous scroll
            Math::Point m_PreviousScroll;

            // thread count
            int m_ThreadCount;
            Helium::Mutex m_ThreadCountMutex;

        public:
            PropertiesManager( PropertiesGenerator* generator );
            ~PropertiesManager();

            // callback when show events occur on the canvas window
            void Show( const Inspect::CanvasShowArgs& args );

            // returns the container
            Inspect::Container* GetContainer();

            // setup properties
            void SetProperties(PropertySetting setting);

            // update selection state
            void SetSelection(const OS_SelectableDumbPtr& selection);

            // inspect selection begin creating the property UI
            void CreateProperties();

            // actually create the property UI (in a separate thread)
            void GeneratePropertiesThread( PropertyThreadArgs& args );
            void GenerateProperties( PropertyThreadArgs& args );

            // display the UI (in the main UI thread)
            void FinalizeProperties( u32 selectionId, const Inspect::V_Control& controls );

            // add a listener for when properties are created
            void AddPropertiesCreatedListener( const PropertiesCreatedSignature::Delegate& listener );

            // remove a listener for when properties are created
            void RemovePropertiesCreatedListener( const PropertiesCreatedSignature::Delegate& listener );

            // are threads active
            bool ThreadsActive();
        };

        typedef Helium::SmartPtr< PropertiesManager > PropertiesManagerPtr;

        class PropertiesCreatedCommand : public Undo::Command
        {
        public:
            PropertiesCreatedCommand( PropertiesManager* propertiesManager, u32 selectionId, const Inspect::V_Control& controls ) 
                : m_PropertiesManager( propertiesManager )
                , m_SelectionId( selectionId )
                , m_Controls( controls )
            { 
            }

            virtual void Undo() HELIUM_OVERRIDE
            {
                // this should never happen
                HELIUM_BREAK();
            }

            virtual void Redo() HELIUM_OVERRIDE
            {
                m_PropertiesManager->FinalizeProperties( m_SelectionId, m_Controls );
            }

        private:
            PropertiesManager* m_PropertiesManager;
            u32 m_SelectionId;
            Inspect::V_Control m_Controls;
        };

    }
}