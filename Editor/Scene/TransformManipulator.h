#pragma once

#include <hash_map>

#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/Matrix4.h"

#include "Application/Undo/Queue.h"

#include "Editor/Scene/Tool.h"
#include "Editor/Scene/Scene.h"
#include "Editor/Scene/Transform.h"
#include "Editor/Scene/Manipulator.h"

namespace Helium
{
    namespace Editor
    {
        struct ManipulationStart
        {
            Math::Vector3 m_StartValue;
            Math::Matrix4 m_StartFrame;
            Math::Matrix4 m_InverseStartFrame;    
        };

        typedef std::map<ManipulatorAdapter*, ManipulationStart> M_ManipulationStart;

        class PickVisitor;

        class TransformManipulator HELIUM_ABSTRACT : public Tool, public ManiuplatorAdapterCollection
        {
        protected:
            // The mode of the manipulator (duh)
            const ManipulatorMode m_Mode;

            // Axes to manipulate
            Math::AxesFlags m_SelectedAxes;

            // Size of manipulator
            f32 m_Size;

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
            D3DMATERIAL9 m_AxisMaterial;
            D3DMATERIAL9 m_SelectedAxisMaterial;

            // Start
            M_ManipulationStart m_ManipulationStart;


            //
            // RTTI
            //

            EDITOR_DECLARE_TYPE(Editor::TransformManipulator, Tool);
            static void InitializeType();
            static void CleanupType();


            //
            // Constructor, requires a view manager to interact with, a selection to manipulate, and an undo queue to use
            //

        public:
            TransformManipulator(const ManipulatorMode mode, Editor::Scene* scene, PropertiesGenerator* generator);

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
            virtual void SelectionChanged(const OS_SelectableDumbPtr& selection);

            // setup the material in the render device for teh applicable axes
            virtual bool SetAxisMaterial(Math::AxesFlags axes);

            // get the normal to the provided axes, or camera direction if all
            virtual Math::Vector3 GetAxesNormal(Math::AxesFlags axes);

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
            // Properties
            //

            virtual void CreateProperties() HELIUM_OVERRIDE;

            f32 GetSize() const;
            void SetSize(f32 space);


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