#pragma once

#include "Core/API.h"
#include "Core/Content/Nodes/ContentInstance.h"

namespace Helium
{
    namespace Content
    {
        namespace VolumeShapes
        {
            enum VolumeShape
            {
                Cube,
                Cylinder,
                Sphere,
                Capsule,
            };
            static void VolumeShapeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(Cube, TXT( "Cube" ) );
                info->AddElement(Cylinder, TXT( "Cylinder" ) );
                info->AddElement(Sphere, TXT( "Sphere" ) );
                info->AddElement(Capsule, TXT( "Capsule" ) );
            }
        }

        typedef VolumeShapes::VolumeShape VolumeShape;

        class CORE_API Volume : public Instance
        {
        public:
            VolumeShape m_Shape;

        public:
            REFLECT_DECLARE_CLASS(Volume, Instance);
            static void EnumerateClass( Reflect::Compositor<Volume>& comp );

        public:
            Volume ()
                : m_Shape (VolumeShapes::Cube)
            {

            }

            Volume (const Helium::TUID& id)
                : Instance (id)
                , m_Shape (VolumeShapes::Cube)
            {

            }

        };

        typedef Helium::SmartPtr<Volume> VolumePtr;
        typedef std::vector<VolumePtr> V_Volume;
    }
}