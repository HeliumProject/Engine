#pragma once

#include "ExportBase.h"

namespace Helium
{
    namespace MayaContent
    {
        class MAYA_API ExportJoint : public ExportBase
        {
        public:
            ExportJoint()
            {

            }

            ExportJoint( const MObject& mayaObject, Helium::TUID& id )
                : ExportBase( mayaObject )
            {
                m_ContentObject = new Content::JointTransform( id );
            }

            // Gather the necessary maya data
            virtual void GatherMayaData( V_ExportBase &newExportObjects );
        };

        typedef Helium::SmartPtr<ExportJoint> ExportJointPtr;
    }
}