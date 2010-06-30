///////////////////////////////////////////////////
// Text.cpp -  Definitions for the CViewText, 
//              and CDockText classes

#include "Text.h"
#include "resource.h"


///////////////////////////////////////////////
// CViewText functions
CViewText::CViewText()
{
	m_hRichEdit = ::LoadLibrary(_T("Riched20.dll")); // RichEdit ver 2.0
    if (!m_hRichEdit)
    {
		::MessageBox(NULL,_T("CRichView::CRichView  Failed to load Riched20.dll"), _T(""), MB_ICONWARNING);
    }
}

CViewText::~CViewText(void)
{
	if (m_hRichEdit)
		::FreeLibrary(m_hRichEdit);
}

void CViewText::OnInitialUpdate()
{
	SetWindowText(_T("Text Edit Window\r\n\r\n You can type some text here ..."));
}

void CViewText::PreCreate(CREATESTRUCT &cs)
{
	cs.style = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CHILD | 
				WS_CLIPCHILDREN | WS_HSCROLL | WS_VISIBLE;

	cs.lpszClass = RICHEDIT_CLASS; // RichEdit ver 2.0
}


///////////////////////////////////////////////
// CContainText functions
CContainText::CContainText()
{	 
	SetDockCaption (_T("Text View - Docking container"));
	SetTabText(_T("Text"));
	SetTabIcon(IDI_VIEW /*IDI_TEXT*/);
	SetView(m_ViewText);
} 


//////////////////////////////////////////////
//  Definitions for the CDockText class
CDockText::CDockText()
{
	// Set the view window to our edit control
	SetView(m_View);
}

void CDockText::OnInitialUpdate()
{
	// Set the width of the splitter bar
	SetBarWidth(8);
//  SetDockStyle(DS_CONTROL);
}


