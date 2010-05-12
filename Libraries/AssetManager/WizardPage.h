#pragma once

#include "Wizard.h"
#include "UIToolKit/WizardPageTemplate.h"

namespace AssetManager
{
  class StyledText
  {
  public:
    StyledText( wxTextAttr* style, std::string text )
      : m_Style( style )
      , m_Text( text )
    {
    }

    wxTextAttr* m_Style;
    std::string m_Text;
  };
  typedef std::vector< StyledText > V_StyledText;


  ///////////////////////////////////////////////////////////////////////////
  // Templated base class for generating wizard pages and associating them
  // with wxFormBuilder constructed panels.
  // 
  template < class TPanel >
  class WizardPage NOC_ABSTRACT : public UIToolKit::WizardPageTemplate< TPanel >
  {
  protected:
    // Protected constructor - use a derived class's static creator function.
    WizardPage( Wizard* wizard )
      : UIToolKit::WizardPageTemplate< TPanel >( wizard )
    {
      const wxFont& defaultFont = m_Panel->GetFont();
      m_DefaultTextAttr.SetFont( defaultFont );

      wxFont titleFont( defaultFont );
      titleFont.SetWeight( wxFONTWEIGHT_BOLD );
      m_TitleTextAttr.SetFlags( wxTEXT_ATTR_FONT_FACE );
      m_TitleTextAttr.SetFont( titleFont );

      wxFont msgFont( defaultFont );
      msgFont.SetStyle( wxFONTSTYLE_ITALIC );
      m_MsgTextAttr.SetFlags( wxTEXT_ATTR_FONT_FACE );
      m_MsgTextAttr.SetFont( msgFont );
    }

  public:
    // Destructor
    virtual ~WizardPage()
    {
    }

    // Returns properly typed wizard object.
    Wizard* GetWizard() const
    {
      return wxStaticCast( __super::GetWizard(), Wizard );
    }

    virtual const V_StyledText& GetPageSummary() const 
    { 
      return m_PageSummary; 
    }

    virtual const V_string& GetFileList() const 
    { 
      return m_P4FileList; 
    }

  protected:
    V_StyledText              m_PageSummary;
    V_string                  m_P4FileList; //used by the PerforcePage to display the list of files

    wxTextAttr                m_DefaultTextAttr;
    wxTextAttr                m_TitleTextAttr;
    wxTextAttr                m_MsgTextAttr;
  };
}
