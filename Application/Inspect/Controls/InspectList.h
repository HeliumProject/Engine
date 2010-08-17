#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectControl.h"

#include "Foundation/Reflect/Class.h"

namespace Helium
{
    namespace Inspect
    {
        //
        // Listbox control
        //

        const static tchar LIST_ATTR_SORTED[]    = TXT( "sorted" );

        namespace MoveDirections
        {
            enum MoveDirection
            {
                Up,
                Down
            };
        }
        typedef MoveDirections::MoveDirection MoveDirection;

        class APPLICATION_API List : public Reflect::ConcreteInheritor<List, Control>
        {
        public:
            // Delimiter to use for key-value pairs if this list is displaying a map of data
            static const tchar* s_MapKeyValDelim; 

        protected:
            std::vector< tstring > m_Items;
            std::vector< tstring > m_SelectedItems;
            bool m_Sorted;
            bool m_IsMap; // Is the data bound to this control acutally a std::map?

        public:
            List();

            virtual void Realize( Container* parent ) HELIUM_OVERRIDE;
            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;

            void SetSorted( bool sort );

            void SetMap( bool isMap );


            const std::vector< tstring >& GetItems();
            void AddItems( const std::vector< tstring >& items );

            void AddItem( const tstring& item );
            void RemoveItem( const tstring& item );

            const std::vector< tstring >& GetSelectedItems();
            void SetSelectedItems( const std::vector< tstring >& items );

            tstring GetSelectedItems( const tstring delimiter );
            void SetSelectedItems( const tstring& delimitedList, const tstring& delimiter );

            void MoveSelectedItems( MoveDirection direction = MoveDirections::Up );

        protected:
            tstring GetDelimitedList( const std::vector< tstring >& items, const tstring& delimiter );
            void UpdateUI( const std::vector< tstring >& items );
            virtual bool Process(const tstring& key, const tstring& value) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<List> ListPtr;
    }
}