#pragma once

#include "Platform/Compiler.h"
#include "Foundation/InitializerStack.h"

#include "Editor/DocumentManager.h"
#include "Scene/SceneEditor.h"

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

        virtual void    OnAssertFailure(const wxChar *file, int line, const wxChar *func, const wxChar *cond, const wxChar *msg) NOC_OVERRIDE;

        DocumentManager* GetDocumentManager()
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

    protected:
        Nocturnal::InitializerStack m_InitializerStack;
        DocumentManager* m_DocumentManager;
        SceneEditor* m_SceneEditor;
    };

    DECLARE_APP( LunaApp );
}