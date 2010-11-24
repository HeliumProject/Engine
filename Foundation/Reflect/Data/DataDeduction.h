#pragma once

#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/Data.h"
#include "Foundation/Reflect/Data/PointerData.h"
#include "Foundation/Reflect/Data/EnumerationData.h"
#include "Foundation/Reflect/Data/BitfieldData.h"

#define REFLECT_SPECIALIZE_DATA(Name) \
template<> \
static inline int32_t Helium::Reflect::GetData<Name::DataType>() \
{ \
    return Helium::Reflect::GetClass<Name>()->m_TypeID; \
} \
template<> \
static inline Name::DataType* Helium::Reflect::Data::GetData<Name::DataType>( Data* serializer ) \
{ \
    return serializer && serializer->GetType() == Helium::Reflect::GetData<Name::DataType>() ? static_cast<Name*>( serializer )->m_Data.Ptr() : NULL; \
} \
template<> \
static inline const Name::DataType* Helium::Reflect::Data::GetData<Name::DataType>( const Data* serializer ) \
{ \
    return serializer && serializer->GetType() == Helium::Reflect::GetData<Name::DataType>() ? static_cast<const Name*>( serializer )->m_Data.Ptr() : NULL; \
}

#include "Foundation/Reflect/Data/TypeIDData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDData );

#include "Foundation/Reflect/Data/PathData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathData );

//
// Container Datas
//

#include "Foundation/Reflect/Data/ContainerData.h"

#include "Foundation/Reflect/Data/SimpleData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::BoolData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U8Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I8Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U16Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I16Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U32Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I32Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U64Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I64Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::F32Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::F64Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Vector2Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Vector3Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Vector4Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Matrix3Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Matrix4Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Color3Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Color4Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::HDRColor3Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::HDRColor4Data );

#include "Foundation/Reflect/Data/StlVectorData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringStlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::BoolStlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U8StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I8StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U16StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I16StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U32StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I32StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U64StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I64StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::F32StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::F64StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDStlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDStlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathStlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Vector2StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Vector3StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Vector4StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Matrix3StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Matrix4StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Color3StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Color4StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::HDRColor3StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::HDRColor4StlVectorData );

#include "Foundation/Reflect/Data/StlSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringStlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U32StlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U64StlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::F32StlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDStlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDStlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathStlSetData );

#include "Foundation/Reflect/Data/StlMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringStringStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringBoolStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringU32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringI32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U32StringStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U32U32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U32I32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U32U64StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I32StringStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I32U32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I32I32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I32U64StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U64StringStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U64U32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U64U64StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U64Matrix4StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDU32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDMatrix4StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDU32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDMatrix4StlMapData );

#include "Foundation/Reflect/Data/ElementStlVectorData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ElementStlVectorData );

#include "Foundation/Reflect/Data/ElementStlSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ElementStlSetData );

#include "Foundation/Reflect/Data/ElementStlMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDElementStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringElementStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U32ElementStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I32ElementStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::U64ElementStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::I64ElementStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDElementStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDElementStlMapData );
