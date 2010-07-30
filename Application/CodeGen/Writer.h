#pragma once

#include "Application/API.h"

#include "Namespace.h"

namespace Helium
{
    namespace CodeGen
    {
        // Enumeration of different types of header guards
        namespace HeaderGuards
        {
            enum HeaderGuard
            {
                PragmaOnce, // #pragma once
                Define,     // #ifndef _INCLUDED_NAME_ ... #endif
            };
        }
        typedef HeaderGuards::HeaderGuard HeaderGuard;

        /////////////////////////////////////////////////////////////////////////////
        // Class for writing a C++ header from a collection of namespace elements.
        // 
        class APPLICATION_API Writer
        {
        private:
            std::string m_Name;
            HeaderGuard m_Guard;
            std::string m_Comment;
            std::string m_LastError;
            std::string m_Indent;
            bool        m_Headerparsed;
            bool        m_ToLua;

        public:
            Writer();
            virtual ~Writer();

            void SetName( const std::string& name );
            void SetHeaderGuard( HeaderGuards::HeaderGuard guard );
            void SetComment( const std::string& comment );
            void SetHeaderparsed( bool headerparsed );
            void SetToLua( bool tolua );
            bool Write( std::ostream& stream, const NamespacePtr& namespacePtr );
            const std::string& GetLastError() const;

        protected:
            bool WriteHeader( std::ostream& stream );
            bool WriteFooter( std::ostream& stream );
            bool WriteNamespace( std::ostream& stream, const NamespacePtr& namespacePtr );
            bool WriteEnum( std::ostream& stream, const EnumPtr& enumPtr );
        };
    }
}