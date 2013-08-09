#pragma once

#include <string>
#include <map>

#include "Reflect/MetaEnum.h"
#include "Inspect/DataBinding.h"
#include "Math/Matrix4.h"

#include "SceneGraph/API.h"
#include "SceneGraph/Selection.h"

namespace Helium
{
    namespace SceneGraph
    {
        class HierarchyNode;

        namespace ManipulatorModes
        {
            enum ManipulatorMode
            {
                Scale,
                ScalePivot,

                Rotate,
                RotatePivot,

                Translate,
                TranslatePivot,
            };
        }

        typedef ManipulatorModes::ManipulatorMode ManipulatorMode;

        namespace ManipulatorAdapterTypes
        {
            enum ManipulatorAdapterType
            {
                ManiuplatorAdapterCollection,
                ScaleManipulatorAdapter,
                RotateManipulatorAdapter,
                TranslateManipulatorAdapter,
            };
        }

        typedef ManipulatorAdapterTypes::ManipulatorAdapterType ManipulatorAdapterType;

        class ManipulatorSpace
        {
        public:
            enum Enum
            {
                Object,
                Local,
                World,
            };

            REFLECT_DECLARE_ENUM( ManipulatorSpace );

            static void PopulateMetaType( Reflect::MetaEnum& info )
            {
                info.AddElement(Object, TXT( "Object" ) );
                info.AddElement(Local,  TXT( "Local" ) );
                info.AddElement(World,  TXT( "World" ) );
            }
        };

        class HELIUM_SCENE_GRAPH_API ManipulatorAdapter : public Helium::RefCountBase<ManipulatorAdapter>
        {
        public:
            const static ManipulatorAdapterType Type = ManipulatorAdapterTypes::ManiuplatorAdapterCollection;

            ManipulatorAdapter()
            {

            }

            virtual ManipulatorAdapterType GetType() = 0;
            virtual SceneGraph::HierarchyNode* GetNode() = 0;
            virtual bool AllowSelfSnap()
            {
                return false;
            }

            virtual Matrix4 GetFrame(ManipulatorSpace space) = 0;
            virtual Matrix4 GetObjectMatrix() = 0;
            virtual Matrix4 GetParentMatrix() = 0;
        };

        typedef Helium::SmartPtr<ManipulatorAdapter> ManipulatorAdapterPtr;
        typedef std::vector<ManipulatorAdapterPtr> V_ManipulatorAdapterSmartPtr;

        class HELIUM_SCENE_GRAPH_API ScaleManipulatorAdapter : public ManipulatorAdapter
        {
        public:
            const static ManipulatorAdapterType Type = ManipulatorAdapterTypes::ScaleManipulatorAdapter;

            virtual ManipulatorAdapterType GetType() HELIUM_OVERRIDE
            {
                return ManipulatorAdapterTypes::ScaleManipulatorAdapter;
            }

            virtual Vector3 GetPivot() = 0;

            virtual Scale GetValue() = 0;

            virtual UndoCommandPtr SetValue(const Scale& v) = 0;
        };

        class HELIUM_SCENE_GRAPH_API RotateManipulatorAdapter : public ManipulatorAdapter
        {
        public:
            const static ManipulatorAdapterType Type = ManipulatorAdapterTypes::RotateManipulatorAdapter;

            virtual ManipulatorAdapterType GetType() HELIUM_OVERRIDE
            {
                return ManipulatorAdapterTypes::RotateManipulatorAdapter;
            }

            virtual Vector3 GetPivot() = 0;

            virtual EulerAngles GetValue() = 0;

            virtual UndoCommandPtr SetValue(const EulerAngles& v) = 0;
        };

        class HELIUM_SCENE_GRAPH_API TranslateManipulatorAdapter : public ManipulatorAdapter
        {
        public:
            const static ManipulatorAdapterType Type = ManipulatorAdapterTypes::TranslateManipulatorAdapter;

            virtual ManipulatorAdapterType GetType() HELIUM_OVERRIDE
            {
                return ManipulatorAdapterTypes::TranslateManipulatorAdapter;
            }

            virtual Vector3 GetPivot() = 0;

            virtual Vector3 GetValue() = 0;

            virtual UndoCommandPtr SetValue(const Vector3& v) = 0;
        };

        class HELIUM_SCENE_GRAPH_API ManiuplatorAdapterCollection HELIUM_ABSTRACT
        {
        protected:
            V_ManipulatorAdapterSmartPtr m_ManipulatorAdapters;

        public:
            virtual ManipulatorMode GetMode() = 0;

            virtual void AddManipulatorAdapter(const ManipulatorAdapterPtr& accessor)
            {
                m_ManipulatorAdapters.push_back(accessor);
            }
        };
    }
}