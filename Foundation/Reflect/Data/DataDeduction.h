#pragma once

#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/Data.h"
#include "Foundation/Reflect/Data/PointerData.h"
#include "Foundation/Reflect/Data/EnumerationData.h"
#include "Foundation/Reflect/Data/BitfieldData.h"

#define REFLECT_SPECIALIZE_DATA(Name) \
template<> \
static inline const Helium::Reflect::Class* Helium::Reflect::GetDataClass<Name::DataType>() \
{ \
    return Helium::Reflect::GetClass<Name>(); \
} \
template<> \
static inline Name::DataType* Helium::Reflect::Data::GetData<Name::DataType>( Data* data ) \
{ \
    return data && data->GetClass() == Helium::Reflect::GetDataClass<Name::DataType>() ? static_cast<Name*>( data )->m_Data.Ptr() : NULL; \
} \
template<> \
static inline const Name::DataType* Helium::Reflect::Data::GetData<Name::DataType>( const Data* data ) \
{ \
    return data && data->GetClass() == Helium::Reflect::GetDataClass<Name::DataType>() ? static_cast<const Name*>( data )->m_Data.Ptr() : NULL; \
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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::BoolData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt8Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int8Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt16Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int16Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int64Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Float32Data );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Float64Data );
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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringStlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::BoolStlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt8StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int8StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt16StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int16StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int64StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Float32StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Float64StlVectorData );
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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringStlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32StlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64StlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDStlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDStlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathStlSetData );

#include "Foundation/Reflect/Data/StlMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringStlStringStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringBoolStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringUInt32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringInt32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32StringStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32UInt32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32Int32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32UInt64StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32StringStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32UInt32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32Int32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32UInt64StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64StringStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64UInt32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64UInt64StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64Matrix4StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDUInt32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDMatrix4StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDUInt32StlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDMatrix4StlMapData );

#include "Foundation/Reflect/Data/ObjectStlVectorData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectStlVectorData );

#include "Foundation/Reflect/Data/ObjectStlSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectStlSetData );

#include "Foundation/Reflect/Data/ObjectStlMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32ObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32ObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64ObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int64ObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDObjectStlMapData );
