#pragma once

#include <map>

#include "Reflect/Object.h"
#include "Inspect/Interpreter.h"

#include "EditorScene/SettingsManager.h"

#include "Editor/API.h"
#include "Editor/Inspect/wxCanvas.h"
#include "Editor/Inspect/TreeCanvas.h"

namespace Helium
{
    namespace Editor
    {
        class SettingInfo : public Helium::RefCountBase< SettingInfo >
        {
        public:
            SettingInfo( Reflect::ObjectPtr& source, Reflect::ObjectPtr& clone, Editor::TreeCanvasPtr& canvas );

            Reflect::ObjectPtr   m_Source;
            Reflect::ObjectPtr   m_Clone;
            Editor::TreeCanvasPtr m_Canvas;
        };

        typedef Helium::SmartPtr< SettingInfo > SettingInfoPtr;
        typedef std::map< int, SettingInfoPtr > M_SettingInfo;

        class SettingsDialog : public wxDialog
        {
        public:
            SettingsDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
            ~SettingsDialog();

			virtual int ShowModal( SettingsManager* settingsManager );

        private:
            void OnRestoreDefaults( wxCommandEvent& args );
            void OnOk( wxCommandEvent& args );
            void OnCancel( wxCommandEvent& args );
            void OnSettingsChanged( wxCommandEvent& args );
            void OnRefreshElements( const Reflect::ObjectChangeArgs& args );

            void SelectCanvas( SettingInfo* settingInfo );

            wxSizer* m_SettingSizer;
            M_SettingInfo m_SettingInfo;
            SettingInfo* m_CurrentSetting;
            std::vector< Inspect::InterpreterPtr > m_Interpreters;
        };
    }
}