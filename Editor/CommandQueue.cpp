#include "EditorPch.h"
#include "CommandQueue.h"

using namespace Helium;
using namespace Helium::Editor;

DEFINE_EVENT_TYPE(wxEVT_FLUSH_COMMANDS)

Editor::CommandQueue::CommandQueue( wxWindow* window )
: m_Window( window )
{
    m_Window->Connect( m_Window->GetId(), wxEVT_FLUSH_COMMANDS, wxCommandEventHandler(CommandQueue::HandleEvent), NULL, this );
}

Editor::CommandQueue::~CommandQueue()
{
    m_Window->Disconnect( m_Window->GetId(), wxEVT_FLUSH_COMMANDS, wxCommandEventHandler(CommandQueue::HandleEvent), NULL, this );
}

void Editor::CommandQueue::EnqueueFlush()
{
    m_Window->GetEventHandler()->AddPendingEvent( wxCommandEvent ( wxEVT_FLUSH_COMMANDS, m_Window->GetId() ) );
}

void Editor::CommandQueue::HandleEvent( wxCommandEvent& event )
{
    Flush();
}