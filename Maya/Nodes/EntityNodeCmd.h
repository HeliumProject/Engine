#pragma once

#include "API.h"

#include <maya/MPxCommand.h>

namespace Helium
{

    class MAYA_API EntityNodeCmd : public MPxCommand
    {
    public:
        static MString CommandName;

        static void * creator()
        {
            return new EntityNodeCmd;
        }

        static MSyntax newSyntax();

        virtual bool isUndoable()
        {
            return false;
        }

        virtual bool hasSyntax()
        {
            return true;
        }

        virtual MStatus doIt( const MArgList & args );
    };

}