#pragma once

#include "Foundation/File/Path.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Serializers.h"

#include "Pipeline/API.h"

namespace Helium
{
    class PIPELINE_API MRUData : public Reflect::Element
    {
    public:
        REFLECT_DECLARE_CLASS( MRUData, Reflect::Element );

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

    typedef Helium::StrongPtr< MRUData > MRUDataPtr;
}