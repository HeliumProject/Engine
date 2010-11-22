#pragma once

#include "Registry.h"
#include "SimpleSerializer.h"
#include "ContainerSerializer.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API ArraySerializer : public ContainerSerializer
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ArraySerializer, ContainerSerializer );

            virtual void SetSize(size_t size) = 0;

            virtual int32_t GetItemType() const = 0;
            virtual SerializerPtr GetItem(size_t at) = 0;
            virtual ConstSerializerPtr GetItem(size_t at) const = 0;
            virtual void SetItem(size_t at, const Serializer* value) = 0;
            virtual void Insert( size_t at, const Serializer* value ) = 0;
            virtual void Remove( size_t at ) = 0;
            virtual void MoveUp( std::set< size_t >& selectedIndices ) = 0;
            virtual void MoveDown( std::set< size_t >& selectedIndices ) = 0;
        };

        template <class T>
        class FOUNDATION_API SimpleArraySerializer : public ArraySerializer
        {
        public:
            typedef std::vector<T> DataType;
            Serializer::DataPtr<DataType> m_Data;

            typedef SimpleArraySerializer<T> ArraySerializerT;
            REFLECT_DECLARE_CLASS( ArraySerializerT, ArraySerializer )

                SimpleArraySerializer();
            virtual ~SimpleArraySerializer();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void SetSize(size_t size) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual int32_t GetItemType() const HELIUM_OVERRIDE;
            virtual SerializerPtr GetItem(size_t at) HELIUM_OVERRIDE;
            virtual ConstSerializerPtr GetItem(size_t at) const HELIUM_OVERRIDE;
            virtual void SetItem(size_t at, const Serializer* value) HELIUM_OVERRIDE;
            virtual void Insert( size_t at, const Serializer* value ) HELIUM_OVERRIDE;
            virtual void Remove( size_t at ) HELIUM_OVERRIDE;
            virtual void MoveUp( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;
            virtual void MoveDown( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;

            virtual bool Set(const Serializer* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Serializer* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
        };

        typedef SimpleArraySerializer<tstring> StringArraySerializer;
        typedef SimpleArraySerializer<bool> BoolArraySerializer;
        typedef SimpleArraySerializer<uint8_t> U8ArraySerializer;
        typedef SimpleArraySerializer<int8_t> I8ArraySerializer;
        typedef SimpleArraySerializer<uint16_t> U16ArraySerializer;
        typedef SimpleArraySerializer<int16_t> I16ArraySerializer;
        typedef SimpleArraySerializer<uint32_t> U32ArraySerializer;
        typedef SimpleArraySerializer<int32_t> I32ArraySerializer;
        typedef SimpleArraySerializer<uint64_t> U64ArraySerializer;
        typedef SimpleArraySerializer<int64_t> I64ArraySerializer;
        typedef SimpleArraySerializer<float32_t> F32ArraySerializer;
        typedef SimpleArraySerializer<float64_t> F64ArraySerializer;
        typedef SimpleArraySerializer<Helium::GUID> GUIDArraySerializer;
        typedef SimpleArraySerializer<Helium::TUID> TUIDArraySerializer;
        typedef SimpleArraySerializer<Helium::Path> PathArraySerializer;

        typedef SimpleArraySerializer<Vector2> Vector2ArraySerializer;
        typedef SimpleArraySerializer<Vector3> Vector3ArraySerializer;
        typedef SimpleArraySerializer<Vector4> Vector4ArraySerializer;
        typedef SimpleArraySerializer<Matrix3> Matrix3ArraySerializer;
        typedef SimpleArraySerializer<Matrix4> Matrix4ArraySerializer;

        typedef SimpleArraySerializer<Color3> Color3ArraySerializer;
        typedef SimpleArraySerializer<Color4> Color4ArraySerializer;
        typedef SimpleArraySerializer<HDRColor3> HDRColor3ArraySerializer;
        typedef SimpleArraySerializer<HDRColor4> HDRColor4ArraySerializer;
    }
}