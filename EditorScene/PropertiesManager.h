#pragma once

#include "Application/CommandQueue.h"
#include "Inspect/Controls.h"

#include "EditorScene/API.h"
#include "EditorScene/PropertiesGenerator.h"
#include "EditorScene/Selection.h"

namespace Helium
{
	namespace Editor
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
			ElementTypeFlags( const Reflect::MetaClass* type, int32_t includeFlags, int32_t excludeFlags )
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

			const Reflect::MetaClass* m_Type;
			int32_t m_IncludeFlags;
			int32_t m_ExcludeFlags;
		};

		typedef std::map< ElementTypeFlags, Reflect::Object* >                 M_ElementByType;
		typedef std::map< ElementTypeFlags, std::vector<Reflect::Object*> >    M_ElementsByType;
		typedef std::map< ElementTypeFlags, Inspect::InterpreterPtr >          M_InterpretersByType;

		struct PropertiesCreatedArgs
		{
			PropertiesCreatedArgs( PropertiesManager* propertiesManager, uint32_t selectionId, const std::vector< Inspect::ControlPtr >& controls )
				: m_PropertiesManager( propertiesManager )
				, m_SelectionId( selectionId )
				, m_Controls( controls )
			{
			}

			PropertiesManager*  m_PropertiesManager;
			uint32_t            m_SelectionId;
			std::vector< Inspect::ControlPtr >  m_Controls;
		};

		typedef Helium::Signature< const PropertiesCreatedArgs& > PropertiesCreatedSignature;

		struct PropertiesThreadArgs
		{
			PropertiesThreadArgs( PropertiesStyle setting, uint32_t selectionId, const uint32_t* currentSelectionId, const OS_ObjectDumbPtr& selection )
				: m_SelectionId( selectionId )
				, m_CurrentSelectionId( currentSelectionId )
				, m_Style( setting )
			{
				for ( OS_ObjectDumbPtr::Iterator itr = selection.Begin(), end = selection.End(); itr != end; ++itr )
				{
					m_Selection.Append( *itr );
				}
			}

			PropertiesStyle                m_Style;
			uint32_t                       m_SelectionId;
			const uint32_t*                m_CurrentSelectionId;
			OrderedSet<Reflect::ObjectPtr> m_Selection;
		};

		class HELIUM_EDITOR_SCENE_API PropertiesManager : public Helium::RefCountBase< PropertiesManager >
		{
		public:
			PropertiesManager( PropertiesGenerator* generator, CommandQueue* commandQueue );
			~PropertiesManager();

			void Show( const Inspect::CanvasShowArgs& args );

			void SetProperties(PropertiesStyle setting);
			void SetSelection(const OS_ObjectDumbPtr& selection);

			// inspect selection begin creating the property UI
			void CreateProperties();

		private:
			// the thread entry point to do the property creation work
			void GeneratePropertiesThreadEntry( PropertiesThreadArgs& args );

			// called from the thread entry function, does the property creation work
			void GenerateProperties( PropertiesThreadArgs& args );

		public:
			// display the UI (in the main UI thread)
			void Present( uint32_t selectionId, const std::vector< Inspect::ControlPtr >& controls );

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
			OS_ObjectDumbPtr                m_Selection;

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