#pragma once

#include "Object.h"
#include "Selection.h"
#include "Viewport.h"

namespace Helium
{
    namespace Editor
    {
        class PropertiesGenerator;
        class Scene;

        class Tool HELIUM_ABSTRACT : public Object
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

            bool m_AllowSelection;

            //
            // RTTI
            //

        public:
            EDITOR_DECLARE_TYPE(Editor::Tool, Object);
            static void InitializeType();
            static void CleanupType();


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
            virtual bool ValidateSelection( OS_SelectableDumbPtr& items )
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

            //
            // Handle Mouse Input
            //

            // this returns whether the tool is taking the drag
            virtual bool MouseDown( wxMouseEvent& e )
            {
                return !AllowSelection();
            }

            virtual void MouseUp( wxMouseEvent& e )
            {

            }

            virtual void MouseMove( wxMouseEvent& e )
            {

            }

            virtual void KeyPress( wxKeyEvent& e )
            {
                e.Skip();
            }

            virtual void KeyDown( wxKeyEvent& e )
            {
                e.Skip();
            }

            virtual void KeyUp( wxKeyEvent& e )
            {
                e.Skip();
            }
        };

        typedef Helium::SmartPtr< Editor::Tool > LToolPtr;
    }
}