#pragma once

#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/Data.h"
#include "Foundation/Reflect/Data/PointerData.h"
#include "Foundation/Reflect/Data/StructureData.h"
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
    return data && data->GetClass() == Helium::Reflect::GetDataClass<Name::DataType>() ? static_cast<Name*>( data )->m_Data.operator->() : NULL; \
}

#include "Foundation/Reflect/Data/TypeIDData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDData );

#include "Foundation/Reflect/Data/PathData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathData );

//
// Container Datas
//

#include "Foundation/Reflect/Data/ContainerData.h"
#include "Foundation/Reflect/Data/StructureDynArrayData.h"

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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::NameData );

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

#include "Foundation/Reflect/Data/DynArrayData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringDynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::BoolDynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt8DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int8DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt16DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int16DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int64DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Float32DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Float64DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDDynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDDynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathDynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Vector2DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Vector3DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Vector4DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Matrix3DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Matrix4DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Color3DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Color4DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::HDRColor3DynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::HDRColor4DynArrayData );

#include "Foundation/Reflect/Data/SetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32SetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64SetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathSetData );

#include "Foundation/Reflect/Data/MapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringStringMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringBoolMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringUInt32MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringInt32MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32StringMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32UInt32MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32Int32MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32UInt64MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32StringMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32UInt32MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32Int32MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32UInt64MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64StringMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64UInt32MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64UInt64MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64Matrix4MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDUInt32MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDMatrix4MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDUInt32MapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDMatrix4MapData );

#include "Foundation/Reflect/Data/SortedSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringSortedSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32SortedSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64SortedSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDSortedSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDSortedSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathSortedSetData );

#include "Foundation/Reflect/Data/SortedMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringStringSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringBoolSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringUInt32SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringInt32SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32StringSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32UInt32SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32Int32SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32UInt64SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32StringSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32UInt32SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32Int32SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32UInt64SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64StringSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64UInt32SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64UInt64SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64Matrix4SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDUInt32SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDMatrix4SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDUInt32SortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDMatrix4SortedMapData );

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

#include "Foundation/Reflect/Data/ObjectDynArrayData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectDynArrayData );

#include "Foundation/Reflect/Data/ObjectSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectSetData );

#include "Foundation/Reflect/Data/ObjectMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32ObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32ObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64ObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int64ObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDObjectMapData );

#include "Foundation/Reflect/Data/ObjectSortedSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectSortedSetData );

#include "Foundation/Reflect/Data/ObjectSortedMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32ObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32ObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64ObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int64ObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::GUIDObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDObjectSortedMapData );
