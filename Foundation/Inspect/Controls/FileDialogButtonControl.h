#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Controls/ButtonControl.h"

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
            mutable Path      m_Result;
        };
        typedef Helium::Signature< const FileDialogButtonClickedArgs& > FileDialogButtonClickedSignature;

        ///////////////////////////////////////////////////////////////////////////
        // Button control that opens a file browser dialog.
        // 
        class FOUNDATION_API FileDialogButton : public Reflect::ConcreteInheritor< FileDialogButton, Inspect::Button >
        {
        public:
            FileDialogButton( const FileDialogType& type = FileDialogTypes::OpenFile, const tstring& caption = TXT( "Open" ), const tstring& filter = TXT( "All files (*.*)|*.*" ) )
            {
                a_Type.Set( type );
                a_Caption.Set( caption );
                a_Filter.Set( filter );
            }

            virtual bool Write() HELIUM_OVERRIDE
            {
                tstring path;
                ReadStringData( path );
                Path startPath( path );

                FileDialogButtonClickedArgs args ( this, a_Type.Get(), a_Caption.Get(), startPath, a_Filter.Get() );
                d_Clicked.Invoke( args );
                WriteStringData( args.m_Result.Get() );
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