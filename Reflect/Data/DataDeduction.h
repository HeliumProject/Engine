#pragma once

#include "Reflect/Class.h"
#include "Reflect/Registry.h"
#include "Reflect/Data/Data.h"
#include "Reflect/Data/PointerData.h"
#include "Reflect/Data/StructureData.h"
#include "Reflect/Data/EnumerationData.h"
#include "Reflect/Data/BitfieldData.h"

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

#include "Reflect/Data/TypeIDData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDData );

#include "Reflect/Data/PathData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathData );

//
// Container Datas
//

#include "Reflect/Data/ContainerData.h"
#include "Reflect/Data/StructureDynArrayData.h"

#include "Reflect/Data/SimpleData.h"
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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::NameData );

#include "Reflect/Data/StlVectorData.h"
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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDStlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathStlVectorData );

#include "Reflect/Data/StlSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringStlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32StlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64StlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDStlSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathStlSetData );

#include "Reflect/Data/StlMapData.h"
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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDUInt32StlMapData );

#include "Reflect/Data/DynArrayData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::NameDynArrayData );
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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDDynArrayData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathDynArrayData );

#include "Reflect/Data/SetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32SetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64SetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathSetData );

#include "Reflect/Data/MapData.h"
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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDUInt32MapData );

#include "Reflect/Data/SortedSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringSortedSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32SortedSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64SortedSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDSortedSetData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::PathSortedSetData );

#include "Reflect/Data/SortedMapData.h"
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
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDUInt32SortedMapData );

#include "Reflect/Data/ObjectStlVectorData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectStlVectorData );

#include "Reflect/Data/ObjectStlSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectStlSetData );

#include "Reflect/Data/ObjectStlMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StlStringObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32ObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32ObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64ObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int64ObjectStlMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDObjectStlMapData );

#include "Reflect/Data/ObjectDynArrayData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectDynArrayData );

#include "Reflect/Data/ObjectSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectSetData );

#include "Reflect/Data/ObjectMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32ObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32ObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64ObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int64ObjectMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDObjectMapData );

#include "Reflect/Data/ObjectSortedSetData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::ObjectSortedSetData );

#include "Reflect/Data/ObjectSortedMapData.h"
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TypeIDObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::StringObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt32ObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int32ObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::UInt64ObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::Int64ObjectSortedMapData );
REFLECT_SPECIALIZE_DATA( Helium::Reflect::TUIDObjectSortedMapData );
