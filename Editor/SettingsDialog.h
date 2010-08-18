#pragma once

#include <map>

#include "Foundation/Reflect/Element.h"
#include "Application/Inspect/Controls/Canvas.h"

#include "Editor/API.h"
#include "Editor/Settings.h"

namespace Helium
{
    namespace Editor
    {
        class SettingInfo : public Helium::RefCountBase< SettingInfo >
        {
        public:
            SettingInfo( Reflect::ElementPtr& source, Reflect::ElementPtr& clone, Inspect::CanvasPtr& canvas );

            Reflect::ElementPtr m_Source;
            Reflect::ElementPtr m_Clone;
            Inspect::CanvasPtr m_Canvas;
        };

        typedef Helium::SmartPtr< SettingInfo > SettingInfoPtr;
        typedef std::map< int, SettingInfoPtr > M_SettingInfo;

        class SettingsDialog : public wxDialog
        {
        public:
            SettingsDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 644,375 ), long style = wxDEFAULT_DIALOG_STYLE );
            ~SettingsDialog();

            virtual int ShowModal( Editor::Settings* prefs );

        private:
            void OnRestoreDefaults( wxCommandEvent& args );
            void OnApply( wxCommandEvent& args );
            void OnOk( wxCommandEvent& args );
            void OnCancel( wxCommandEvent& args );
            void OnSettingsChanged( wxCommandEvent& args );
            void OnRefreshElements( const Reflect::ElementChangeArgs& args );

            void SelectCanvas( SettingInfo* settingInfo );

            wxSizer* m_SettingSizer;
            M_SettingInfo m_SettingInfo;

            SettingInfo* m_CurrentSetting;
        };
    }
}