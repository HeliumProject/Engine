#pragma once

#include "ExportBase.h"
#include "Pipeline/Content/Nodes/Point.h"

#include <maya/MPoint.h>

namespace Helium
{
    namespace MayaContent
    {
        class MAYA_API ExportPoint : public ExportBase
        {
        public:
            ExportPoint( const MPoint& mayaPoint );

            Content::Point* GetContentPoint() const;
        };
        typedef Helium::SmartPtr< ExportPoint > ExportPointPtr;
    }
}