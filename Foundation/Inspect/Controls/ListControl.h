#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar_t LIST_ATTR_SORTED[]    = TXT( "sorted" );

        namespace MoveDirections
        {
            enum MoveDirection
            {
                Up,
                Down
            };
        }
        typedef MoveDirections::MoveDirection MoveDirection;

        struct AddItemArgs
        {
            AddItemArgs()
            {
            }
        };
        typedef Helium::Signature< const AddItemArgs& > AddItemSignature;

        class FOUNDATION_API List : public Control
        {
        public:
            REFLECT_DECLARE_OBJECT( List, Control );

            List();

            virtual bool Process(const tstring& key, const tstring& value) HELIUM_OVERRIDE;

            AddItemSignature::Event             e_AddItem;
            Attribute< bool >                   a_IsSorted;
            Attribute< std::set< size_t > >     a_SelectedItemIndices;
        };

        typedef Helium::StrongPtr<List> ListPtr;
    }
}
