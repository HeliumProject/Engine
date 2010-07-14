#pragma once

#include "Platform/Compiler.h"
#include "Foundation/InitializerStack.h"

#include "Editor/DocumentManager.h"
#include "Scene/SceneEditor.h"
#include "Vault/Vault.h"

#include <wx/app.h>
#include <wx/xrc/xmlres.h>

namespace Luna
{
    class LunaApp : public wxApp
    {
    public:
        LunaApp();
        ~LunaApp();

        virtual bool    OnInit() NOC_OVERRIDE;
        virtual int     OnRun() NOC_OVERRIDE;
        virtual int     OnExit() NOC_OVERRIDE;

        virtual void    OnNew( wxCommandEvent& event );

        virtual void    OnAssertFailure(const wxChar *file, int line, const wxChar *func, const wxChar *cond, const wxChar *msg) NOC_OVERRIDE;

        DocumentManager& GetDocumentManager()
        {
            return m_DocumentManager;
        }

        SceneEditor* GetSceneEditor()
        {
            if ( !m_SceneEditor )
            {
                m_SceneEditor = new SceneEditor();
            }
            return m_SceneEditor;
        }

        Vault& GetVault()
        {
            if ( !m_Vault )
            {
                m_Vault = new Vault();
            }
            return *m_Vault;
        }

    protected:
        Nocturnal::InitializerStack m_InitializerStack;
        SceneEditor* m_SceneEditor;
        DocumentManager m_DocumentManager;
        Vault* m_Vault;
    };

    DECLARE_APP( LunaApp );
}