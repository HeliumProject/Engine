#pragma once

#include "Core/API.h"
#include "Core/Content/Nodes/Instance.h"

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
            static void VolumeShapeEnumerateEnumeration( Reflect::Enumeration* info )
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
            //
            // Members
            //

        public:
            VolumeShape m_Shape;


            //
            // RTTI
            //

        public:
            REFLECT_DECLARE_CLASS(Volume, Instance);

            static void EnumerateClass( Reflect::Compositor<Volume>& comp );


            //
            // Implementation
            //

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