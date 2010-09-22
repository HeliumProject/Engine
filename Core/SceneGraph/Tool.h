#pragma once

#include "Foundation/Input.h"

#include "Core/API.h"
#include "Foundation/Reflect/Object.h"
#include "Core/SceneGraph/Selection.h"
#include "Core/SceneGraph/Viewport.h"

namespace Helium
{
    namespace SceneGraph
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

        class CORE_API Tool HELIUM_ABSTRACT : public Reflect::Object
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
            REFLECT_DECLARE_ABSTRACT( Tool, Reflect::Object );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            Tool( SceneGraph::Scene* scene, PropertiesGenerator* generator );

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
            virtual bool ValidateSelection( OS_SceneNodeDumbPtr& items )
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
            virtual void Draw( DrawArgs* args )
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

            virtual bool MouseDown( const MouseButtonInput& e )
            {
                if ( !AllowSelection() )
                {
                    return true; // the tool is accepting the drag
                }

                return false; // do not skip, our caller will always handle this input
            }

            virtual void MouseUp( const MouseButtonInput& e )
            {
                // do not skip, our caller will always handle this input
            }

            virtual void MouseMove( const MouseMoveInput& e )
            {
                // do not skip, our caller will always handle this input
            }

            virtual void KeyPress( const KeyboardInput& e )
            {
                e.Skip();
            }

            virtual void KeyDown( const KeyboardInput& e )
            {
                e.Skip();
            }

            virtual void KeyUp( const KeyboardInput& e )
            {
                e.Skip();
            }
        };

        typedef Helium::SmartPtr< SceneGraph::Tool > ToolPtr;
    }
}