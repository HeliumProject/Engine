#pragma once

#include "Core/API.h"

#include "Foundation/File/Path.h"
#include "Foundation/Reflect/Element.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API MRUData : public Reflect::ConcreteInheritor< MRUData, Reflect::Element >
        {
        public:
            const std::vector< Helium::Path >& GetPaths() const;
            void SetPaths( const std::vector< Helium::Path >& paths );

        private:
            std::vector< Helium::Path > m_Paths;

        public:
            static void EnumerateClass( Reflect::Compositor< MRUData >& comp )
            {
                comp.AddField( &MRUData::m_Paths, "m_Paths" );
            }
        };

        typedef Helium::SmartPtr< MRUData > MRUDataPtr;
    }
}