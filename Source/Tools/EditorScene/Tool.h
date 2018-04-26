#pragma once

#include "Reflect/Object.h"

#include "EditorScene/API.h"
#include "EditorScene/Input.h"
#include "EditorScene/Selection.h"
#include "EditorScene/Viewport.h"

namespace Helium
{
	namespace Editor
	{
		class PropertiesGenerator;
		class Scene;

		struct PickArgs
		{
			PickVisitor* m_PickVisitor;

			PickArgs( PickVisitor* visitor )
				: m_PickVisitor( visitor )
			{

			}
		};
		typedef Helium::Signature< PickArgs& > PickSignature;

		class HELIUM_EDITOR_SCENE_API Tool : public Reflect::Object
		{
			//
			// Members
			//

		protected:
			// The 3d view
			Viewport* m_View;

			// The attribute generator to use
			PropertiesGenerator* m_Generator;

			// The scene to edit
			Scene* m_Scene;

			// Allow the selection set to be changed
			bool m_AllowSelection;

			// Pick the world for intersections
			PickSignature::Event m_PickWorld;

		public:
			HELIUM_DECLARE_ABSTRACT( Tool, Reflect::Object );


			//
			// Implementation
			//

		public:
			Tool( Editor::Scene* scene, PropertiesGenerator* generator );

			// Create and Initialize
			virtual bool Initialize()
			{
				return true;
			}

			// Cleanup
			virtual void Cleanup()
			{

			}

			// Reset scaling
			virtual void ResetSize()
			{

			}

			// Scale to a target factor (camera was zoomed)
			virtual void ScaleTo( float factor )
			{

			}

			// Route interface callbacks to me
			virtual bool AllowSelection()
			{
				return m_AllowSelection;
			}

			// Validate any selection if not in InterfaceOverride
			virtual bool ValidateSelection( OS_ObjectDumbPtr& items )
			{
				return true;
			}

			// Create attribute UI
			virtual void CreateProperties()
			{

			}

			// Eval Nodes
			virtual void Evaluate()
			{

			}

			// Render UI
			virtual void Draw( BufferedDrawer* pDrawer )
			{
				return;
			}

			// Pick
			virtual bool Pick( PickVisitor* pick )
			{
				return false;
			}

			// Pick the world
			PickSignature::Event& PickWorld()
			{
				return m_PickWorld;
			}

			//
			// Handle Mouse Input
			//

			virtual bool MouseDown( const MouseButtonInputEvent& e )
			{
				if ( !AllowSelection() )
				{
					return true; // the tool is accepting the drag
				}

				return false; // do not skip, our caller will always handle this input
			}

			virtual void MouseUp( const MouseButtonInputEvent& e )
			{
				// do not skip, our caller will always handle this input
			}

			virtual void MouseMove( const MouseMoveInputEvent& e )
			{
				// do not skip, our caller will always handle this input
			}

			virtual void KeyPress( const KeyboardInputEvent& e )
			{
				e.Skip();
			}

			virtual void KeyDown( const KeyboardInputEvent& e )
			{
				e.Skip();
			}

			virtual void KeyUp( const KeyboardInputEvent& e )
			{
				e.Skip();
			}
		};

		typedef Helium::StrongPtr< Editor::Tool > ToolPtr;
	}
}