#pragma once

#include "Foundation/FileDialog.h"

namespace Helium
{
    namespace Editor
    {
        class FileDialogDisplayer
        {
        public:
            FileDialogDisplayer( wxWindow* parent = NULL )
                : m_Parent( parent )
            {

            }

            void SetParent( wxWindow* parent )
            {
                m_Parent = parent;
            }

            Helium::Path DisplayFileDialog( const FileDialogArgs& args );

        private:
            wxWindow* m_Parent;
        };
    }
}