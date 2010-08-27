#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectButton.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar BUTTON_FILEDIALOG_ATTR_FILTER[] = TXT( "filter" );
        const static tchar BUTTON_FILEDIALOG_ATTR_TITLE[] = TXT( "caption" );

        namespace FileDialogTypes
        {
            enum FileDialogType
            {
                OpenFile,
                SaveFile
            };
        }
        typedef FileDialogTypes::FileDialogType FileDialogType;

        class FileDialogButton;

        struct FileDialogButtonClickedArgs
        {
            FileDialogButtonClickedArgs( FileDialogButton* control, const FileDialogType& type, const tstring& caption, const Path& startPath, const tstring& filter )
                : m_Control( control )
                , m_Type( type )
                , m_Caption( caption )
                , m_StartPath( startPath )
                , m_Filter( filter )
            {
            }

            FileDialogButton* m_Control;
            FileDialogType    m_Type;
            tstring           m_Caption;
            Path              m_StartPath;
            tstring           m_Filter;
        };
        typedef Helium::Signature< Path, const FileDialogButtonClickedArgs& > FileDialogButtonClickedSignature;

        ///////////////////////////////////////////////////////////////////////////
        // Button control that opens a file browser dialog.
        // 
        class APPLICATION_API FileDialogButton : public Reflect::ConcreteInheritor< FileDialogButton, Inspect::Button >
        {
        public:
            FileDialogButton( const FileDialogType& type = FileDialogTypes::OpenFile, const tstring& caption = TXT( "Open" ), const Path& startPath = TXT( "" ), const tstring& filter = TXT( "All files (*.*)|*.*" ) )
            {
                 WriteStringData( startPath.Get() );
            }

            virtual bool Write() HELIUM_OVERRIDE
            {
                tstring path;
                ReadStringData( path );
                Path startPath( path );
                Path result = d_Clicked.Invoke( FileDialogButtonClickedArgs( this, a_Type.Get(), a_Caption.Get(), startPath, a_Filter.Get() ) );
                WriteStringData( result.Get() );
                return true;
            }

            FileDialogButtonClickedSignature::Delegate d_Clicked;

            Attribute< FileDialogType >  a_Type;
            Attribute< tstring >         a_Caption;
            Attribute< tstring >         a_Filter;

        protected:
            virtual bool  Process( const tstring& key, const tstring& value ) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr< FileDialogButton > FileDialogButtonPtr;
    }
}