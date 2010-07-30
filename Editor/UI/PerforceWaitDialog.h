#pragma once

#include "PerforceGenerated.h"

#include "Foundation/Profile.h"
#include "Application/RCS/Providers/Perforce/P4Provider.h"

#include <wx/dialog.h>
#include <wx/timer.h>

namespace Helium
{
    namespace Editor
    {
        typedef bool (*CancelWaitFunc)();

        class WaitDialog : public wxDialog
        {
        public:
            static void Enable( bool enable );

            WaitDialog();

            int ShowModal( Perforce::WaitInterface* waitInterface );
            void Throb();

        private:
            PerforceWaitPanelGenerated* m_Panel;
        };

        class PerforceWaitTimer : public wxTimer
        {
        public:
            PerforceWaitTimer( class WaitDialog* dialog, Perforce::WaitInterface* waitInterface );

        protected:
            void Notify();

        private:
            WaitDialog*               m_WaitDialog;
            Perforce::WaitInterface*  m_WaitInterface;
            Profile::Timer            m_WaitTimer;
        };
    }
}