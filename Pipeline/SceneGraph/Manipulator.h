#pragma once

#include <string>
#include <map>

#include "Foundation/Inspect/Data.h"

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/Selection.h"

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

        namespace ManipulatorSpaces
        {
            enum ManipulatorSpace
            {
                Object,
                Local,
                World,
            };
            static void ManipulatorSpaceEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(Object, TXT( "Object" ) );
                info->AddElement(Local, TXT( "Local" ) );
                info->AddElement(World, TXT( "World" ) );
            }
        }

        typedef ManipulatorSpaces::ManipulatorSpace ManipulatorSpace;

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

        class PIPELINE_API ManipulatorAdapter : public Helium::RefCountBase<ManipulatorAdapter>
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

        class PIPELINE_API ScaleManipulatorAdapter : public ManipulatorAdapter
        {
        public:
            const static ManipulatorAdapterType Type = ManipulatorAdapterTypes::ScaleManipulatorAdapter;

            virtual ManipulatorAdapterType GetType() HELIUM_OVERRIDE
            {
                return ManipulatorAdapterTypes::ScaleManipulatorAdapter;
            }

            virtual Vector3 GetPivot() = 0;

            virtual Scale GetValue() = 0;

            virtual Undo::CommandPtr SetValue(const Scale& v) = 0;
        };

        class PIPELINE_API RotateManipulatorAdapter : public ManipulatorAdapter
        {
        public:
            const static ManipulatorAdapterType Type = ManipulatorAdapterTypes::RotateManipulatorAdapter;

            virtual ManipulatorAdapterType GetType() HELIUM_OVERRIDE
            {
                return ManipulatorAdapterTypes::RotateManipulatorAdapter;
            }

            virtual Vector3 GetPivot() = 0;

            virtual EulerAngles GetValue() = 0;

            virtual Undo::CommandPtr SetValue(const EulerAngles& v) = 0;
        };

        class PIPELINE_API TranslateManipulatorAdapter : public ManipulatorAdapter
        {
        public:
            const static ManipulatorAdapterType Type = ManipulatorAdapterTypes::TranslateManipulatorAdapter;

            virtual ManipulatorAdapterType GetType() HELIUM_OVERRIDE
            {
                return ManipulatorAdapterTypes::TranslateManipulatorAdapter;
            }

            virtual Vector3 GetPivot() = 0;

            virtual Vector3 GetValue() = 0;

            virtual Undo::CommandPtr SetValue(const Vector3& v) = 0;
        };

        class PIPELINE_API ManiuplatorAdapterCollection HELIUM_ABSTRACT
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