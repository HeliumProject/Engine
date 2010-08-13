#pragma once

#include "Core/Content/Nodes/ContentCurve.h"

#include "ExportBase.h"

namespace Helium
{
    namespace MayaContent
    {
        class MAYA_API ExportCurve : public ExportBase
        {
        public:
            ExportCurve( const MObject& mayaCurve, const Helium::TUID& id ) 
                : ExportBase( mayaCurve )
            {
                m_ContentObject = new Content::Curve( id );
            }

            virtual void GatherMayaData( V_ExportBase& newExportObjects ) HELIUM_OVERRIDE;
        };
    }
}