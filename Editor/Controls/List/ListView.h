#pragma once

#include <wx/listctrl.h>

#include "Platform/Types.h"
#include "Foundation/Automation/Event.h"

namespace Helium
{
    namespace Editor
    {
        // Forwards
        class ListView;

        /////////////////////////////////////////////////////////////////////////////
        // Event arguments and delegate for events that refer to selection/deselection
        // in the list.  Provides some missing features to the list view, such as the
        // ability to veto when a selection change is about to happen.
        // 
        struct ListSelectionChangeArgs
        {
            ListView* m_List; // List that fired the event
            int32_t m_Index;      // Index/id of the item whose selection changed
            int32_t m_Data;       // Data attached to the list item whose selection changed
            bool m_Select;    // True if a select event, otherwise a deselect event
            bool m_Veto;      // Callbacks should set this to true if the want to veto a selection changing event

            ListSelectionChangeArgs( ListView* list, bool select, int32_t index, int32_t data, bool veto = false )
                : m_List( list )
                , m_Index( index )
                , m_Data( data )
                , m_Select( select )
                , m_Veto( veto )
            {
            }
        };

        typedef Helium::Signature< ListSelectionChangeArgs& > ListSelectionChangingSignature;

        /////////////////////////////////////////////////////////////////////////////
        // Event arguments and delegate for events that indicate a change in a list
        // control.
        // 
        struct ListChangeArgs
        {
            ListView* m_List;

            ListChangeArgs( ListView* list )
                : m_List( list )
            {
            }
        };

        typedef Helium::Signature< const ListChangeArgs& > ListSelectionChangedSignature;

        /////////////////////////////////////////////////////////////////////////////
        // Extends the wxListView to provide additional functionality including:
        // - The ability to veto selection changing events (for selection and 
        //   deselection of objects.
        // - An event to batch together all of the selection updates that happen
        //   due to one mouse-down event.  This way, consumers of this class do not
        //   have to HELIUM_OVERRIDE the normal wx-selection changed events.  They can
        //   subscribe to the SelectionChanged event provided here and just get one
        //   event at the end of a selection range.
        // 
        class ListView : public wxListView
        {
        private:
            bool m_SelectionDirty;
            ListSelectionChangingSignature::Event m_SelectionChanging;
            ListSelectionChangedSignature::Event m_SelectionChanged;

        public:
            ListView();
            ListView( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT( "ListView" ) );
            virtual ~ListView();
            virtual WXLRESULT MSWWindowProc( WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam );
            virtual void DeselectAll();

        private:
            void OnItemSelected( wxListEvent& args );
            void OnItemDeselected( wxListEvent& args );

        public:
            void AddSelectionChangingListener( const ListSelectionChangingSignature::Delegate& listener );
            void RemoveSelectionChangingListener( const ListSelectionChangingSignature::Delegate& listener );
            void AddSelectionChangedListener( const ListSelectionChangedSignature::Delegate& listener );
            void RemoveSelectionChangedListener( const ListSelectionChangedSignature::Delegate& listener );

        private:
            DECLARE_DYNAMIC_CLASS( ListView )

        };

        // Enumeration of different sorting methods useable by the tree control
        namespace ListViewSortMethods
        {
            enum ListViewSortMethod
            {
                Normal, // Regular, alphabetical sorting. Ex: Item1, Item10, Item2
                Natural // Natrual string ordering, keeping numeric values in order. Ex: Item1, Item2, Item10
            };
        };
        typedef ListViewSortMethods::ListViewSortMethod ListViewSortMethod;

        /////////////////////////////////////////////////////////////////////////////
        // Extends the normal list view to provide automatic sorting of items by the
        // text value in various columns (meaningful mainly in report mode of a list).
        // Calling SortItems will sort the entire list, based upon the text values
        // in the specified column.  In order for sorting to work, each item in the
        // list must have a unique identifier located in its item data (see
        // wxListItem::SetData).  This is a limitation of wxWidgets, in that the sort
        // callback only returns the item data of the items to be compared.
        // 
        class SortableListView : public ListView
        {
        private:
            bool m_IsSortingEnabled;
            ListViewSortMethod m_SortMethod;

        public:
            SortableListView();
            SortableListView( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT( "SortableListView" ) );
            virtual ~SortableListView();
            bool IsSortingEnabled() const;
            void EnableSorting( bool enable = true );
            void DisableSorting() { EnableSorting( false ); }
            void SetSortMethod( ListViewSortMethod method );
            ListViewSortMethod GetSortMethod() const;
            bool SortItems( long whichColumn = 0 );

        private:
            DECLARE_DYNAMIC_CLASS( SortableListView )
        };
    }
}