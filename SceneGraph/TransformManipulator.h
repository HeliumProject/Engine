#pragma once

#include <map>

#include "Math/Vector3.h"
#include "Math/Matrix4.h"

#include "Application/UndoQueue.h"

#include "SceneGraph/Tool.h"
#include "SceneGraph/Scene.h"
#include "SceneGraph/Transform.h"
#include "SceneGraph/Manipulator.h"

namespace Helium
{
    namespace SceneGraph
    {
        struct ManipulationStart
        {
            Vector3 m_StartValue;
            Matrix4 m_StartFrame;
            Matrix4 m_InverseStartFrame;    
        };

        typedef std::map<ManipulatorAdapter*, ManipulationStart> M_ManipulationStart;

        class PickVisitor;

        class HELIUM_SCENE_GRAPH_API TransformManipulator HELIUM_ABSTRACT : public Tool, public ManiuplatorAdapterCollection
        {
        protected:
            // The mode of the manipulator (duh)
            const ManipulatorMode m_Mode;

            // Axes to manipulate
            AxesFlags m_SelectedAxes;

            // Mouse input
            bool m_Left;
            bool m_Middle;
            bool m_Right;

            // Starting screen coordinates
            int m_StartX;
            int m_StartY;

            // Manipulator is active
            bool m_Manipulating;

            // Manipulator changed set values
            bool m_Manipulated;

            // Materials
            Helium::Color m_AxisMaterial;
            Helium::Color m_SelectedAxisMaterial;

            // Start
            M_ManipulationStart m_ManipulationStart;

        public:
            HELIUM_DECLARE_ABSTRACT(SceneGraph::TransformManipulator, Tool);
            static void InitializeType();
            static void CleanupType();

        public:
            TransformManipulator(const ManipulatorMode mode, SceneGraph::Scene* scene, PropertiesGenerator* generator);
            virtual ~TransformManipulator();

            virtual void Cleanup();

            virtual ManipulatorMode GetMode() HELIUM_OVERRIDE
            {
                return m_Mode;
            }


            //
            // Get/Set Pertinent Values from Manipulation Set Objects
            //

        protected:
            // update state with current selection
            virtual void SelectionChanged(const SelectionChangeArgs& args);

            // get the normal to the provided axes, or camera direction if all
            virtual Vector3 GetAxesNormal(AxesFlags axes);

            // set the final result into the object with undo/redo support
            virtual void SetResult() = 0;


            //
            // Rendering and Picking
            //

        public:
            // manage mouse events (if pick succeeded)
            virtual bool MouseDown( const MouseButtonInput& e );
            virtual void MouseUp( const MouseButtonInput& e );
            virtual void MouseMove( const MouseMoveInput& e );

            //
            // Manipulation Set, these find the primary, secondary, complete set of objects in the manpiulation set
            //  The manipulation set is the union of all IManipuatables in the main selection set
            //

            template <class T>
            T* PrimaryObject()
            {
                std::vector<T*> result (CompleteSet<T>());

                if (result.empty())
                {
                    return NULL;
                }
                else
                {
                    return result.back();
                }

                return NULL;
            }

            template <class T>
            std::vector<T*> SecondarySet()
            {
                std::vector<T*> result (CompleteSet<T>());

                result.erase( std::remove(result.begin(), result.end(), PrimaryObject<T>()) );

                return result;
            }

            template <class T>
            std::vector<T*> CompleteSet()
            {
                std::vector<T*> result;

                V_ManipulatorAdapterSmartPtr::const_iterator itr = m_ManipulatorAdapters.begin();
                V_ManipulatorAdapterSmartPtr::const_iterator end = m_ManipulatorAdapters.end();
                for ( ; itr != end; ++itr )
                {
                    ManipulatorAdapter* ptr = itr->Ptr();

                    // we *should* only have accessors of the right type, but check just in case
                    if (T::Type == ptr->GetType() || T::Type == ManipulatorAdapterTypes::ManiuplatorAdapterCollection)
                    {
                        result.push_back(static_cast<T*>(ptr));
                    }
                    else
                    {
                        HELIUM_BREAK();
                    }
                }

                return result;
            }
        };
    }
}