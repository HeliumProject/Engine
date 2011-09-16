//----------------------------------------------------------------------------------------------------------------------
// Color.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/Color.h"
//#include "Foundation/Reflect/Data/SimpleData.impl"

#include "Engine/Serializer.h"

//C:\Helium\Foundation\Reflect\Registry.cpp(114):        g_Registry->m_InitializerStack.Push( Reflect::RegisterClassType<Float32Data>( TXT( "Float32" ) ) );

//REFLECT_DEFINE_OBJECT(RendererColorData);


using namespace Helium;

/// Serialize a Color.
///
/// @param[in]     s       Serializer with which to serialize.
/// @param[in,out] rValue  Color to serialize.
///
/// @return  Reference to the given serializer.
// Serializer& Helium::operator<<( Serializer& s, Color& rValue )
// {
//     s.BeginStruct( Serializer::STRUCT_TAG_COLOR );
//     s << Serializer::Tag( TXT( "r" ) ) << rValue.m_color.components.r;
//     s << Serializer::Tag( TXT( "g" ) ) << rValue.m_color.components.g;
//     s << Serializer::Tag( TXT( "b" ) ) << rValue.m_color.components.b;
//     s << Serializer::Tag( TXT( "a" ) ) << rValue.m_color.components.a;
//     s.EndStruct();
// 
//     return s;
// }

//////////////template SimpleData<Helium::Color>;
//////////////REFLECT_DEFINE_OBJECT(RendererColorData);


//
// THIS FILE IS UNUSUAL
//
// Include it only within a .cpp where you are defining a new type of simple data!


//#include "Foundation/Reflect/Data/SimpleData.h"

#include "Foundation/Memory/Endian.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Reflect;


#define API_DEFINE HELIUM_FOUNDATION_API
#define TEMPLATE_NAME RenderingSimpleData
#include "Foundation/Reflect/Data/SimpleDataTemplate.cpp.inl"
#undef API_DEFINE
#undef TEMPLATE_NAME


//template RenderingSimpleData<Helium::Color>;
//REFLECT_DEFINE_OBJECT(RenderingColorData);