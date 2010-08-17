#pragma once

namespace Helium
{
    namespace Editor
    {
        class InspectClientData : public Reflect::AbstractInheritor< InspectClientData, Inspect::ClientData >
        {
        public:
            InspectClientData( Inspect::Control* control )
                : Inspect::ClientData( control )
                , m_Window( NULL )
                , m_DropTarget( NULL )
            {

            }

            ~InspectClientData()
            {
                if ( m_DropTarget && !m_Window )
                {
                    delete m_DropTarget;
                }
            }

            wxWindow* GetWindow()
            {
                return m_Window;
            }

            const wxWindow* GetWindow() const
            {
                return m_Window;
            }

            void SetWindow( wxWindow* window )
            {
                m_Window = window;

                if ( m_DropTarget )
                {
                    m_Window->SetDropTarget( m_DropTarget );
                    m_DropTarget = NULL;
                }
            }

            wxDropTarget* GetDropTarget()
            {
                return m_DropTarget;
            }

            const wxDropTarget* GetDropTarget() const
            {
                return m_DropTarget;
            }

            void SetDropTarget( wxDropTarget* dropTarget )
            {
                if ( m_Window )
                {
                    m_Window->SetDropTarget( dropTarget );
                }
                else
                {
                    delete m_DropTarget;
                    m_DropTarget = dropTarget;
                }
            }

        protected:
            // a handle to the control
            wxWindow* m_Window;

            // our drop target
            wxDropTarget* m_DropTarget;
        };
    }
}