///////////////////////////////////////////////////////
// Text.h - Declaration of the CViewText, CContainText, 
//          and CDockText classes

#ifndef TEXT_H
#define TEXT_H

#include "docking.h"

#include <richedit.h>

using namespace Win32xx;

// Declaration of the CViewText class
class CViewText : public CWnd
{
public:
	CViewText();
	virtual ~CViewText();

protected:
	virtual void PreCreate(CREATESTRUCT &cs); 
	virtual void OnInitialUpdate(); 

private:
	HMODULE m_hRichEdit;

};

// Declaration of the CContainText class
class CContainText : public CContainer
{
public:
	CContainText();
	~CContainText() {}

private:
	CViewText m_ViewText;
};

// Declaration of the CDockText class
class CDockText : public CDocker
{
public:
	CDockText();
	virtual ~CDockText() {}
	virtual void OnInitialUpdate();

private:
	CContainText m_View;

};


#endif // TEXT_H

