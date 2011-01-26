#pragma once

#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API StlVectorData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( StlVectorData, ContainerData );

            virtual void SetSize(size_t size) = 0;

            virtual const Class* GetItemClass() const = 0;
            virtual DataPtr GetItem(size_t at) = 0;
            virtual void SetItem(size_t at, Data* value) = 0;
            virtual void Insert( size_t at, Data* value ) = 0;
            virtual void Remove( size_t at ) = 0;
            virtual void MoveUp( std::set< size_t >& selectedIndices ) = 0;
            virtual void MoveDown( std::set< size_t >& selectedIndices ) = 0;
        };

        template <class T>
        class FOUNDATION_API SimpleStlVectorData : public StlVectorData
        {
        public:
            typedef std::vector<T> DataType;
            DataPointer<DataType> m_Data;

            typedef SimpleStlVectorData<T> StlVectorDataT;
            REFLECT_DECLARE_OBJECT( StlVectorDataT, StlVectorData )

                SimpleStlVectorData();
            virtual ~SimpleStlVectorData();

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void SetSize(size_t size) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetItemClass() const HELIUM_OVERRIDE;
            virtual DataPtr GetItem(size_t at) HELIUM_OVERRIDE;
            virtual void SetItem(size_t at, Data* value) HELIUM_OVERRIDE;
            virtual void Insert( size_t at, Data* value ) HELIUM_OVERRIDE;
            virtual void Remove( size_t at ) HELIUM_OVERRIDE;
            virtual void MoveUp( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;
            virtual void MoveDown( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;

            virtual bool Set(Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
        };

        typedef SimpleStlVectorData<tstring> StlStringStlVectorData;
        typedef SimpleStlVectorData<bool> BoolStlVectorData;
        typedef SimpleStlVectorData<uint8_t> UInt8StlVectorData;
        typedef SimpleStlVectorData<int8_t> Int8StlVectorData;
        typedef SimpleStlVectorData<uint16_t> UInt16StlVectorData;
        typedef SimpleStlVectorData<int16_t> Int16StlVectorData;
        typedef SimpleStlVectorData<uint32_t> UInt32StlVectorData;
        typedef SimpleStlVectorData<int32_t> Int32StlVectorData;
        typedef SimpleStlVectorData<uint64_t> UInt64StlVectorData;
        typedef SimpleStlVectorData<int64_t> Int64StlVectorData;
        typedef SimpleStlVectorData<float32_t> Float32StlVectorData;
        typedef SimpleStlVectorData<float64_t> Float64StlVectorData;
        typedef SimpleStlVectorData<Helium::GUID> GUIDStlVectorData;
        typedef SimpleStlVectorData<Helium::TUID> TUIDStlVectorData;
        typedef SimpleStlVectorData<Helium::Path> PathStlVectorData;

        typedef SimpleStlVectorData<Vector2> Vector2StlVectorData;
        typedef SimpleStlVectorData<Vector3> Vector3StlVectorData;
        typedef SimpleStlVectorData<Vector4> Vector4StlVectorData;
        typedef SimpleStlVectorData<Matrix3> Matrix3StlVectorData;
        typedef SimpleStlVectorData<Matrix4> Matrix4StlVectorData;

        typedef SimpleStlVectorData<Color3> Color3StlVectorData;
        typedef SimpleStlVectorData<Color4> Color4StlVectorData;
        typedef SimpleStlVectorData<HDRColor3> HDRColor3StlVectorData;
        typedef SimpleStlVectorData<HDRColor4> HDRColor4StlVectorData;
    }
}