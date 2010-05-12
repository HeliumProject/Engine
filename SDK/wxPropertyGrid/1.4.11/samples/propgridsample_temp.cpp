/***************************************************************
 * Name:      wxpropgrid_testMain.h
 * Purpose:   Defines Application Frame
 * Author:    obfuscated ()
 * Created:   2009-08-16
 * Copyright: obfuscated ()
 * License:
 **************************************************************/

#ifndef WXPROPGRID_TESTMAIN_H
#define WXPROPGRID_TESTMAIN_H

//(*Headers(wxpropgrid_testDialog)
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
//*)

class wxPropertyGridManager;

class wxpropgrid_testDialog: public wxDialog
{
    public:

        wxpropgrid_testDialog(wxWindow* parent,wxWindowID id = -1);
        virtual ~wxpropgrid_testDialog();

    private:

        //(*Handlers(wxpropgrid_testDialog)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButtonLabelClick(wxCommandEvent& event);
        //*)

        //(*Identifiers(wxpropgrid_testDialog)
        static const long ID_BUTTON3;
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON1;
        static const long ID_STATICLINE1;
        static const long ID_BUTTON2;
        //*)

        //(*Declarations(wxpropgrid_testDialog)
        wxButton* Button1;
        wxButton* Button2;
        wxButton* Button3;
        wxBoxSizer* BoxSizer2;
        wxStaticLine* StaticLine1;
        wxTextCtrl* TextCtrl1;
        wxBoxSizer* BoxSizer1;
        //*)
        wxPropertyGridManager *pgman;

        DECLARE_EVENT_TABLE()
};

#endif // WXPROPGRID_TESTMAIN_H

#ifndef _TEST_PROP_H_
#define _TEST_PROP_H_


//#include <wx/window.h>

#include <wx/propgrid/propgrid.h>


class TestProperty : public wxPGProperty
{
public:
    TestProperty(wxString const & label = wxPG_LABEL,
                 wxString const & name = wxPG_LABEL,
                 wxString const & value = wxEmptyString);
//    virtual void OnSetValue();
    virtual bool DoSetAttribute(const wxString& name, wxVariant& value);
    virtual wxString GetValueAsString( int argFlags ) const;
//    virtual bool StringToValue( wxVariant& variant, const wxString& text, int argFlags ) const;
private:
};


#endif // _TEST_PROP_H_


/***************************************************************
 * Name:      wxpropgrid_testApp.h
 * Purpose:   Defines Application Class
 * Author:    obfuscated ()
 * Created:   2009-08-16
 * Copyright: obfuscated ()
 * License:
 **************************************************************/

#ifndef WXPROPGRID_TESTAPP_H
#define WXPROPGRID_TESTAPP_H

#include <wx/app.h>

class wxpropgrid_testApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // WXPROPGRID_TESTAPP_H

/***************************************************************
 * Name:      wxpropgrid_testMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    obfuscated ()
 * Created:   2009-08-16
 * Copyright: obfuscated ()
 * License:
 **************************************************************/

#include <wx/msgdlg.h>

//(*InternalHeaders(wxpropgrid_testDialog)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/artprov.h>
#include <wx/dnd.h>

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(wxpropgrid_testDialog)
const long wxpropgrid_testDialog::ID_BUTTON3 = wxNewId();
const long wxpropgrid_testDialog::ID_TEXTCTRL1 = wxNewId();
const long wxpropgrid_testDialog::ID_BUTTON1 = wxNewId();
const long wxpropgrid_testDialog::ID_STATICLINE1 = wxNewId();
const long wxpropgrid_testDialog::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxpropgrid_testDialog,wxDialog)
    //(*EventTable(wxpropgrid_testDialog)
    //*)
END_EVENT_TABLE()


class PropGridDropTarget : public wxTextDropTarget
{
public:
    PropGridDropTarget(wxPropertyGridManager *grid_manager) : m_grid_manager(grid_manager) {}

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text)
    {
        wxPropertyGridPage *page = m_grid_manager->GetCurrentPage();
        wxPGProperty *prop = page->Append(new wxStringProperty(wxT("dropped"), text, text));

        page->SetPropertyAttribute(prop, wxT("Units"), wxT("test"));
        return true;
    }
private:
    wxPropertyGridManager *m_grid_manager;
};

wxpropgrid_testDialog::wxpropgrid_testDialog(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(wxpropgrid_testDialog)
    wxBoxSizer* m_grid_sizer;

    Create(parent, id, _("wxWidgets app"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxSYSTEM_MENU|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("id"));
    SetClientSize(wxSize(198,258));
    SetMinSize(wxSize(200,200));
    SetMaxSize(wxSize(-1,-1));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    m_grid_sizer = new wxBoxSizer(wxVERTICAL);
    Button3 = new wxButton(this, ID_BUTTON3, _("Label"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    m_grid_sizer->Add(Button3, 0, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    m_grid_sizer->Add(TextCtrl1, 0, wxALL|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer1->Add(m_grid_sizer, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    Button1 = new wxButton(this, ID_BUTTON1, _("About"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer2->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
    StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
    BoxSizer2->Add(StaticLine1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
    Button2 = new wxButton(this, ID_BUTTON2, _("&Quit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer2->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
    BoxSizer1->Add(BoxSizer2, 0, wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 4);
    SetSizer(BoxSizer1);
    BoxSizer1->SetSizeHints(this);
    Center();

    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxpropgrid_testDialog::OnButtonLabelClick);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxpropgrid_testDialog::OnAbout);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxpropgrid_testDialog::OnQuit);
    //*)

    pgman = new wxPropertyGridManager(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                      wxPG_SPLITTER_AUTO_CENTER /*| wxPG_BOLD_MODIFIED*/
                                      | wxPG_TOOLBAR /*|wxPG_DESCRIPTION*/);
    wxPropertyGridPage *page = pgman->GetPage(pgman->AddPage(wxT("Main Page")));
    wxPropertyGrid *grid = pgman->GetGrid();
//    grid->SetColumnCount(3);

    grid->SetColumnCount(3);
    grid->SetColumnProportion(0, 40);
    grid->SetColumnProportion(1, 40);
    grid->SetColumnProportion(2, 20);
    grid->MakeColumnEditable(0);

//    grid->SetDropTarget(new PropGridDropTarget(pgman));

//    pgman->SetExtraStyle(wxPG_EX_MODE_BUTTONS | wxPG_EX_UNFOCUS_ON_ENTER);

    m_grid_sizer->Add(pgman, 1, wxALL | wxEXPAND, 1);

    page->Append( new wxPropertyCategory(wxT("Main")) );
    page->Append( new wxStringProperty(wxT("Label"),wxT("Name"),wxT("Initial Value")) );
    page->Append( new wxStringProperty(wxT("Label"),wxT("Name"),wxT("Initial Value")) );
    page->Append( new wxPropertyCategory(wxT("Local")) );
    page->Append( new wxStringProperty(wxT("Label"),wxT("Name"),wxT("Initial Value")) );
    page->Append( new wxStringProperty(wxT("Label"),wxT("Name"),wxT("Initial Value")) );
    wxPGId pgid = page->Append( new wxStringProperty(wxT("Tree"), wxT("Name"),wxT("Initial Value")) );
    wxPGId pgid2 = page->AppendIn(pgid, new wxStringProperty(wxT("SubTree1"), wxT("Name"), wxT("Initial Value")) );
    page->AppendIn(pgid, new wxStringProperty(wxT("SubTree2"), wxT("Name"), wxT("Initial Value")) );
    page->AppendIn(pgid, new wxStringProperty(wxT("SubTree3"), wxT("Name"), wxT("Initial Value")) );

    page->AppendIn(pgid2, new wxStringProperty(wxT("SubTree11"), wxT("Name"), wxT("Initial Value")) );

    page->Append( new wxIntProperty(wxT("Height"), wxT("Height"), 480) );
    page->SetPropertyAttribute(wxT("Height"), wxT("Min"), (long)10 );
    page->SetPropertyAttribute(wxT("Height"), wxT("Max"), (long)2048 );
    page->SetPropertyAttribute(wxT("Height"), wxT("Units"), wxT("Pixels") );

    page->Collapse(pgid);
    page->Collapse(pgid2);

    for(int ii = 0; ii < 200; ++ii)
    {
        wxPGProperty *prop;
        prop = new wxStringProperty(wxString::Format(wxT("Label %d"), ii),
                                wxString::Format(wxT("Name %d"), ii),
                                wxString::Format(wxT("Initial Value %d"), ii));
        page->AppendIn(pgid2, prop);
    }

    page = pgman->GetPage(pgman->AddPage(wxT("Secondary Page")));

    wxBitmap bmp = wxArtProvider::GetBitmap(wxART_FOLDER);

//    for (wxPropertyGridIterator it = page->GetGrid()->GetIterator();
//         !it.AtEnd();
//         it++ )
//    {
//        wxPGProperty* p = *it;
//        if ( p->IsCategory() )
//            continue;
//
//        page->SetPropertyCell( p, 3, wxT("Cell 3"), bmp );
//        page->SetPropertyCell( p, 4, wxT("Cell 4"), wxNullBitmap, *wxWHITE, *wxBLACK );
//    }
    SetSize(400, 600);
}

wxpropgrid_testDialog::~wxpropgrid_testDialog()
{
    //(*Destroy(wxpropgrid_testDialog)
    //*)
}

void wxpropgrid_testDialog::OnQuit(wxCommandEvent& event)
{
    Close();
}

void wxpropgrid_testDialog::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void wxpropgrid_testDialog::OnButtonLabelClick(wxCommandEvent& event)
{
/*
    wxPropertyGridPage *page = pgman->GetPage(wxT("Main Page"));
    wxPGProperty *prop = page->GetPropertyByNameA(wxT("Height"));
    if(!prop)
        prop = page->GetPropertyByLabel(wxT("Height"));
    if(prop)
    {
        page->SetPropertyCell(prop, 0, wxT("new height"));
        page->RefreshProperty(prop);
    }
*/
    int b = 5;
    int &a = b;
    wxString s = wxT("test");
    wxString const &rs = wxT("test");
    wxString const *ps = &rs;

    wxString res = s + wxT(" ") + rs;

}

TestProperty::TestProperty(wxString const & label,
                           wxString const & name,
                           wxString const & value) :
    wxPGProperty(label, name)
{
    wxVariant variant(value);
    SetValue(variant);
}
//void TestProperty::OnSetValue()
//{
//}

wxString TestProperty::GetValueAsString( int argFlags ) const
{
    wxString s = m_value.GetString();
//
//    if ( GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE) )
//    {
//        // Value stored in m_value is non-editable, non-full value
//        if ( (argFlags & wxPG_FULL_VALUE) || (argFlags & wxPG_EDITABLE_VALUE) )
//            GenerateComposedValue(s, argFlags);
//
//        return s;
//    }
//
//    // If string is password and value is for visual purposes,
//    // then return asterisks instead the actual string.
//    if ( (m_flags & wxPG_PROP_PASSWORD) && !(argFlags & (wxPG_FULL_VALUE|wxPG_EDITABLE_VALUE)) )
//        return wxString(wxChar('*'), s.Length());

    return s;
}

//bool TestProperty::StringToValue( wxVariant& variant, const wxString& text, int argFlags ) const
//{
//    if ( GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE) )
//        return wxPGProperty::StringToValue(variant, text, argFlags);
//
//    if ( m_value.IsNull() || m_value.GetString() != text )
//    {
//        variant = text;
//        return true;
//    }
//
//    return false;
//}

bool TestProperty::DoSetAttribute(const wxString& name, wxVariant& value)
{
    return true;
}

/***************************************************************
 * Name:      wxpropgrid_testApp.cpp
 * Purpose:   Code for Application Class
 * Author:    obfuscated ()
 * Created:   2009-08-16
 * Copyright: obfuscated ()
 * License:
 **************************************************************/

//(*AppHeaders
#include <wx/image.h>
//*)

IMPLEMENT_APP(wxpropgrid_testApp);

bool wxpropgrid_testApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	wxpropgrid_testDialog Dlg(0);
    	SetTopWindow(&Dlg);
    	Dlg.ShowModal();
    	wxsOK = false;
    }
    //*)
    return wxsOK;

}

