//----------------------------------------------------------------------------------------------------------------------
// Color.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/Color.h"

#include "Engine/Serializer.h"

namespace Lunar
{
    /// Serialize a Color.
    ///
    /// @param[in]     s       Serializer with which to serialize.
    /// @param[in,out] rValue  Color to serialize.
    ///
    /// @return  Reference to the given serializer.
    Serializer& operator<<( Serializer& s, Color& rValue )
    {
        s.BeginStruct( Serializer::STRUCT_TAG_COLOR );
        s << Serializer::Tag( TXT( "r" ) ) << rValue.m_color.components.r;
        s << Serializer::Tag( TXT( "g" ) ) << rValue.m_color.components.g;
        s << Serializer::Tag( TXT( "b" ) ) << rValue.m_color.components.b;
        s << Serializer::Tag( TXT( "a" ) ) << rValue.m_color.components.a;
        s.EndStruct();

        return s;
    }
}
