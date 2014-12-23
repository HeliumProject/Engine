#include "PipelinePch.h"
#include "MenuItem.h"

using namespace Helium;

MenuItem::MenuItem()
	: m_Disable( false )
{
}

MenuItem::~MenuItem()
{
}

bool MenuItem::Execute()
{
// #ifdef _DEBUG
// 	if ( !EShellMenu::ExecuteCommand( m_Command ) ) 
// #else
// 	if ( !EShellMenu::ExecuteCommand( m_Command, false ) ) 
// #endif
// 	{
// 		tstring error = wxT("Unable to create eshell, with command:\n  ") + m_Command;
// 		wxMessageDialog dialog( NULL, error.c_str(), wxT("Error"), wxOK | wxICON_INFORMATION );
// 		dialog.ShowModal();
// 		return false;
// 	}

	return true;
}
