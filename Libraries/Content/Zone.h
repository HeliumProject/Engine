#pragma once

#include "API.h"
#include "SceneNode.h"

#include "Foundation/File/Path.h"

namespace Content
{
    // 
    // A zone is reference to another content file
    //

    class CONTENT_API Zone : public SceneNode
    {
    public:
        Nocturnal::Path m_Path;

        V_string m_Modes;
        V_string m_Regions;

        bool m_Active;
        bool m_Interactive;
        bool m_Global;
        bool m_Background;
        bool m_HasNavData;

        Math::Color3 m_Color;

        Zone()
            : m_Active( true )
            , m_Interactive( true )
            , m_Global( false )
            , m_Background( false )
            , m_Color( 255 )
            , m_HasNavData (false)
        {

        }

        Zone( const Nocturnal::TUID& id )
            : SceneNode( id )
            , m_Active( true )
            , m_Interactive( true )
            , m_Global( false )
            , m_Background( false )
            , m_Color( 255 )
            , m_HasNavData (false)
        {

        }

        REFLECT_DECLARE_CLASS( Zone, SceneNode );

        static void EnumerateClass( Reflect::Compositor<Zone>& comp );
    };

    typedef Nocturnal::SmartPtr< Zone > ZonePtr;
    typedef std::vector< ZonePtr > V_Zone;
    typedef std::set< ZonePtr > S_Zone;
}