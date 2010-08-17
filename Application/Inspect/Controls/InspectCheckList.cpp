#include "Application/Inspect/Controls/InspectCheckList.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Application/Inspect/InspectData.h"

#include "Foundation/String/Tokenize.h"

using namespace Helium::Reflect;
using namespace Helium::Inspect;

typedef std::map< tstring, bool > M_strbool;

///////////////////////////////////////////////////////////////////////////////
// Local class wrapping the wxCheckListBox (a list box with check marks next
// to each item).
// 
class CheckListBox : public wxCheckListBox
{
public:
    CheckList* m_CheckList;

    CheckListBox (wxWindow* parent, CheckList* checkList, long style )
        : wxCheckListBox (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, style)
        , m_CheckList (checkList)
    {

    }

    void OnCheck( wxCommandEvent& args )
    {
        args.Skip();
        m_CheckList->Write();
    }


    DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(CheckListBox, wxCheckListBox)
EVT_CHECKLISTBOX( wxID_ANY, OnCheck )
END_EVENT_TABLE();


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
CheckList::CheckList()
{
    m_IsFixedHeight = true;
}

///////////////////////////////////////////////////////////////////////////////
// Called when it's time to create the UI.
// 
void CheckList::Realize(Container* parent)
{
    PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

    if (m_Window != NULL)
        return;

    m_Window = new CheckListBox(parent->GetWindow(), this, wxLB_SINGLE | wxLB_HSCROLL);

    wxSize size( -1, m_Canvas->GetStdSize(Math::SingleAxes::Y) * 5);
    m_Window->SetSize( size );
    m_Window->SetMinSize( size );
    m_Window->SetMaxSize( size );

    __super::Realize(parent);
}

///////////////////////////////////////////////////////////////////////////////
// Reads from the bound data into the UI.
// 
void CheckList::Read()
{
    // from data into ui
    if ( IsRealized() && IsBound() )
    {
        SerializerData< M_strbool >* data = CastData< SerializerData< M_strbool >, DataTypes::Serializer >( GetData() );
        M_strbool value;
        data->Get( value );

        CheckListBox* list = Control::Cast< CheckListBox >( this );
        list->Freeze();

        M_strbool::const_iterator itr = value.begin();
        M_strbool::const_iterator end = value.end();
        for ( i32 index = 0; itr != end; ++itr, ++index )
        {
            if ( index < (i32)( list->GetCount() ) )
            {
                if ( itr->first != list->GetString( index ).c_str() )
                {
                    list->Delete( index );
                    index = list->Append( itr->first.c_str() );
                }
            }
            else
            {
                index = list->Append( itr->first.c_str() );
            }

            if ( index >= 0 && itr->second )
            {
                list->Check( static_cast< u32 >( index ) );
            }
        }

        list->Thaw();
    }

    __super::Read();
}

///////////////////////////////////////////////////////////////////////////////
// Writes from the UI back into the data bound to this control.
// 
bool CheckList::Write()
{
    bool result = false;

    if ( IsRealized() && IsBound() )
    {
        M_strbool value;
        CheckListBox* list = Control::Cast< CheckListBox >( this );
        u32 numItems = list->GetCount();
        for ( u32 index = 0; index < numItems; ++index )
        {
            const wxChar* str =list->GetString( index ).c_str();
            value.insert( M_strbool::value_type( str, list->IsChecked( index ) ) );
        }

        SerializerData< M_strbool >* data = CastData< SerializerData< M_strbool >, DataTypes::Serializer >( GetData() );
        if ( WriteTypedData( value, data ) && __super::Write() )
        {
            result = true;
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden because this control manipulates custom (non-string) data.
// Return value should indicate if this control is currently at its default state
// or not.  Currently not supported for this type of control, so it always returns
// false.
// 
bool CheckList::IsDefault() const
{
    // If this control needs to show a default state, some custom code will have to
    // go here.
    return false;
}
