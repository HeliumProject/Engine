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

            virtual int32_t GetItemType() const = 0;
            virtual DataPtr GetItem(size_t at) = 0;
            virtual ConstDataPtr GetItem(size_t at) const = 0;
            virtual void SetItem(size_t at, const Data* value) = 0;
            virtual void Insert( size_t at, const Data* value ) = 0;
            virtual void Remove( size_t at ) = 0;
            virtual void MoveUp( std::set< size_t >& selectedIndices ) = 0;
            virtual void MoveDown( std::set< size_t >& selectedIndices ) = 0;
        };

        template <class T>
        class FOUNDATION_API SimpleStlVectorData : public StlVectorData
        {
        public:
            typedef std::vector<T> DataType;
            Data::Pointer<DataType> m_Data;

            typedef SimpleStlVectorData<T> StlVectorDataT;
            REFLECT_DECLARE_CLASS( StlVectorDataT, StlVectorData )

                SimpleStlVectorData();
            virtual ~SimpleStlVectorData();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void SetSize(size_t size) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual int32_t GetItemType() const HELIUM_OVERRIDE;
            virtual DataPtr GetItem(size_t at) HELIUM_OVERRIDE;
            virtual ConstDataPtr GetItem(size_t at) const HELIUM_OVERRIDE;
            virtual void SetItem(size_t at, const Data* value) HELIUM_OVERRIDE;
            virtual void Insert( size_t at, const Data* value ) HELIUM_OVERRIDE;
            virtual void Remove( size_t at ) HELIUM_OVERRIDE;
            virtual void MoveUp( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;
            virtual void MoveDown( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;

            virtual bool Set(const Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Data* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
        };

        typedef SimpleStlVectorData<tstring> StringStlVectorData;
        typedef SimpleStlVectorData<bool> BoolStlVectorData;
        typedef SimpleStlVectorData<uint8_t> U8StlVectorData;
        typedef SimpleStlVectorData<int8_t> I8StlVectorData;
        typedef SimpleStlVectorData<uint16_t> U16StlVectorData;
        typedef SimpleStlVectorData<int16_t> I16StlVectorData;
        typedef SimpleStlVectorData<uint32_t> U32StlVectorData;
        typedef SimpleStlVectorData<int32_t> I32StlVectorData;
        typedef SimpleStlVectorData<uint64_t> U64StlVectorData;
        typedef SimpleStlVectorData<int64_t> I64StlVectorData;
        typedef SimpleStlVectorData<float32_t> F32StlVectorData;
        typedef SimpleStlVectorData<float64_t> F64StlVectorData;
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