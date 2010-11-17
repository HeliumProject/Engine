#pragma once

#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Registry.h"

//
// Non-Templated Serializers
//

#include "Foundation/Reflect/Serializer.h"
#include "Foundation/Reflect/PointerSerializer.h"
#include "Foundation/Reflect/EnumerationSerializer.h"
#include "Foundation/Reflect/BitfieldSerializer.h"

#include "Foundation/Reflect/TypeIDSerializer.h"
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::TypeIDSerializer );

#include "Foundation/Reflect/PathSerializer.h"
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::PathSerializer );

//
// Container Serializers
//

#include "Foundation/Reflect/ContainerSerializer.h"

#include "Foundation/Reflect/SimpleSerializer.h"
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::StringSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::BoolSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U8Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I8Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U16Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I16Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U32Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I32Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U64Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I64Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::F32Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::F64Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::GUIDSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::TUIDSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Vector2Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Vector3Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Vector4Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Matrix3Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Matrix4Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Color3Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Color4Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::HDRColor3Serializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::HDRColor4Serializer );

#include "Foundation/Reflect/ArraySerializer.h"
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::StringArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::BoolArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U8ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I8ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U16ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I16ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U32ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I32ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U64ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I64ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::F32ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::F64ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::GUIDArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::TUIDArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::PathArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Vector2ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Vector3ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Vector4ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Matrix3ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Matrix4ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Color3ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::Color4ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::HDRColor3ArraySerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::HDRColor4ArraySerializer );

#include "Foundation/Reflect/SetSerializer.h"
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::StringSetSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U32SetSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U64SetSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::F32SetSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::GUIDSetSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::TUIDSetSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::PathSetSerializer );

#include "Foundation/Reflect/MapSerializer.h"
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::StringStringMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::StringBoolMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::StringU32MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::StringI32MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U32StringMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U32U32MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U32I32MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U32U64MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I32StringMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I32U32MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I32I32MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I32U64MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U64StringMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U64U32MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U64U64MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U64Matrix4MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::GUIDU32MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::GUIDMatrix4MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::TUIDU32MapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::TUIDMatrix4MapSerializer );

#include "Foundation/Reflect/ElementArraySerializer.h"
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::ElementArraySerializer );

#include "Foundation/Reflect/ElementSetSerializer.h"
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::ElementSetSerializer );

#include "Foundation/Reflect/ElementMapSerializer.h"
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::TypeIDElementMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::StringElementMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U32ElementMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I32ElementMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::U64ElementMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::I64ElementMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::GUIDElementMapSerializer );
REFLECT_SPECIALIZE_SERIALIZER( Helium::Reflect::TUIDElementMapSerializer );
