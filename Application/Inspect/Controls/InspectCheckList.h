#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectControl.h"

#include "Foundation/Reflect/Class.h"

class CheckListBox;

namespace Helium
{
    namespace Inspect
    {
        //
        // Listbox control (list with check boxes next to each item).
        // Underlying data is a map of string to bool (item label to check box state).
        //

        // Not supported for now (data is an std::map< string, bool > so it is sorted already)
        //const static tchar CHECK_LIST_ATTR_SORTED[]    = TXT( "sorted" );

        class APPLICATION_API CheckList : public Reflect::ConcreteInheritor<CheckList, Control>
        {
        public:
            CheckList();

            virtual void Realize( Container* parent ) HELIUM_OVERRIDE;
            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;
            virtual bool IsDefault() const;

        protected:
        };

        typedef Helium::SmartPtr<CheckList> CheckListPtr;
    }
}