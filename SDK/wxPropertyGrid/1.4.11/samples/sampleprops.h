/////////////////////////////////////////////////////////////////////////////
// Name:        sampleprops.h
// Purpose:     wxPropertyGrid Sample Properties Header
// Author:      Jaakko Salli
// Modified by:
// Created:     Mar-05-2006
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_PROPGRID_SAMPLEPROPS_H
#define WX_PROPGRID_SAMPLEPROPS_H

#ifndef __WXPYTHON__
    // This decl is what you would use in a normal app
    #define wxPG_EXTRAS_DECL
#else
    #define wxPG_EXTRAS_DECL     WXDLLIMPEXP_PG
#endif

//
// Additional properties that can be useful as more than just a demonstration.
//

#ifndef SWIG

WX_PG_DECLARE_WXOBJECT_VARIANT_DATA(wxPGVariantDataFontData, wxFontData, wxPG_EXTRAS_DECL)

#endif

class wxPG_EXTRAS_DECL wxFontDataProperty : public wxFontProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxFontDataProperty)
public:

    wxFontDataProperty( const wxString& label = wxPG_LABEL,
                        const wxString& name = wxPG_LABEL,
                        const wxFontData& value = wxFontData() );
    virtual ~wxFontDataProperty ();

    void OnSetValue();

    // Inorder to have different value type in a derived property
    // class, we will override GetValue to return custom variant,
    // instead of changing the base m_value. This allows the methods
    // in base class to function properly.
    virtual wxVariant DoGetValue() const;

    WX_PG_DECLARE_PARENTAL_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()

protected:
    // Value must be stored as variant - otherwise it will be
    // decreffed to oblivion on GetValue().
    wxVariant  m_value_wxFontData;
};

// -----------------------------------------------------------------------

class wxPG_EXTRAS_DECL wxSizeProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxSizeProperty)
public:

    wxSizeProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                    const wxSize& value = wxSize() );
    virtual ~wxSizeProperty();

    WX_PG_DECLARE_PARENTAL_METHODS()

protected:

    // I stands for internal
    void SetValueI( const wxSize& value )
    {
        m_value = wxSizeToVariant(value);
    }
};

// -----------------------------------------------------------------------

class wxPG_EXTRAS_DECL wxPointProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxPointProperty)
public:

    wxPointProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                     const wxPoint& value = wxPoint() );
    virtual ~wxPointProperty();

    WX_PG_DECLARE_PARENTAL_METHODS()

protected:

    // I stands for internal
    void SetValueI( const wxPoint& value )
    {
        m_value = wxPointToVariant(value);
    }
};

// -----------------------------------------------------------------------

WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_VALIDATOR_WITH_DECL(wxDirsProperty, class wxPG_EXTRAS_DECL)

// -----------------------------------------------------------------------

#ifndef SWIG

WX_PG_DECLARE_VARIANT_DATA(wxPGVariantDataArrayDouble, wxArrayDouble, wxPG_EXTRAS_DECL)

#endif

class wxPG_EXTRAS_DECL wxArrayDoubleProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxArrayDoubleProperty)
public:

    wxArrayDoubleProperty( const wxString& label = wxPG_LABEL,
                           const wxString& name = wxPG_LABEL,
                           const wxArrayDouble& value = wxArrayDouble() );

    virtual ~wxArrayDoubleProperty ();

    virtual void OnSetValue();
    WX_PG_DECLARE_BASIC_TYPE_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()
    WX_PG_DECLARE_ATTRIBUTE_METHODS()

    // Generates cache for displayed text
    virtual void GenerateValueAsString ( wxString& target, int prec, bool removeZeroes ) const;

protected:
    wxString        m_display; // Stores cache for displayed text
    int             m_precision; // Used when formatting displayed string.
    wxChar          m_delimiter; // Delimiter between array entries.
};

// -----------------------------------------------------------------------

#endif // WX_PROPGRID_SAMPLEPROPS_H
