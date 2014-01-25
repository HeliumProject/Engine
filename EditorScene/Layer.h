#pragma once

#include "EditorScene/API.h"
#include "EditorScene/SceneNode.h"
#include "EditorScene/PropertiesGenerator.h"

namespace Helium
{
	namespace Editor
	{
		class HELIUM_EDITOR_SCENE_API Layer : public SceneNode
		{
		public:
			HELIUM_DECLARE_CLASS( Layer, Editor::SceneNode );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

		public:
			Layer();
			~Layer();

			virtual void Initialize() HELIUM_OVERRIDE;

			bool IsVisible() const HELIUM_OVERRIDE;
			void SetVisible( bool visible );

			bool IsSelectable() const;
			void SetSelectable( bool selectable );

			const Color& GetColor() const;
			void SetColor( const Color& color );

			OS_SceneNodeDumbPtr GetMembers();
			bool ContainsMember( Editor::SceneNode* node ) const;

			virtual void ConnectDescendant( Editor::SceneNode* descendant ) HELIUM_OVERRIDE;
			virtual void DisconnectDescendant( Editor::SceneNode* descendant ) HELIUM_OVERRIDE;

			virtual void Insert(Graph* g, V_SceneNodeDumbPtr& insertedNodes ) HELIUM_OVERRIDE;
			virtual void Prune( V_SceneNodeDumbPtr& prunedNodes ) HELIUM_OVERRIDE;

		protected:
			// Reflected
			bool           m_Visible;
			bool           m_Selectable;
			std::set<TUID> m_Members;
			Color          m_Color;
		};

		typedef Helium::StrongPtr< Editor::Layer > LayerPtr;
		typedef std::vector< Editor::Layer* > V_LayerDumbPtr;
		typedef std::map< std::string, Editor::Layer* > M_LayerDumbPtr;
	}
}