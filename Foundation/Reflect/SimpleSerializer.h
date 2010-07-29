#pragma once

#include "Serializer.h"
#include "Foundation/Math/Vector2.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/Vector4.h"
#include "Foundation/Math/Matrix3.h"
#include "Foundation/Math/Matrix4.h"
#include "Foundation/Math/Quaternion.h"
#include "Foundation/Math/Color3.h"
#include "Foundation/Math/Color4.h"
#include "Foundation/Math/HDRColor3.h"
#include "Foundation/Math/HDRColor4.h"
#include "Foundation/GUID.h"
#include "Foundation/TUID.h"

namespace Reflect
{
    //
    // SimpleSerializer wraps data and read/writes it from the archive
    //  This template class can be extended to wrap any primitive type that
    //  has constant size and support for iostream insertion and extraction
    //

    template <class T>
    class FOUNDATION_API SimpleSerializer : public Serializer
    {
    public:
        typedef T DataType;
        Serializer::DataPtr<DataType> m_Data;

        typedef SimpleSerializer<DataType> SimpleSerializerT;
        REFLECT_DECLARE_CLASS( SimpleSerializerT, Serializer );

        SimpleSerializer ();
        virtual ~SimpleSerializer();

        virtual bool IsCompact() const HELIUM_OVERRIDE;

        virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

        virtual bool Set(const Serializer* src, u32 flags = 0) HELIUM_OVERRIDE;
        virtual bool Equals(const Serializer* s) const HELIUM_OVERRIDE;

        virtual void Serialize(const Helium::BasicBufferPtr& buffer, const tchar* debugStr) const HELIUM_OVERRIDE;
        virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
        virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

        virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
        virtual tistream& operator<< (tistream& stream);
    };

    typedef SimpleSerializer<tstring> StringSerializer;                       REFLECT_SPECIALIZE_SERIALIZER( StringSerializer );
    typedef SimpleSerializer<bool> BoolSerializer;                            REFLECT_SPECIALIZE_SERIALIZER( BoolSerializer );
    typedef SimpleSerializer<u8> U8Serializer;                                REFLECT_SPECIALIZE_SERIALIZER( U8Serializer );
    typedef SimpleSerializer<i8> I8Serializer;                                REFLECT_SPECIALIZE_SERIALIZER( I8Serializer );
    typedef SimpleSerializer<u16> U16Serializer;                              REFLECT_SPECIALIZE_SERIALIZER( U16Serializer );
    typedef SimpleSerializer<i16> I16Serializer;                              REFLECT_SPECIALIZE_SERIALIZER( I16Serializer );
    typedef SimpleSerializer<u32> U32Serializer;                              REFLECT_SPECIALIZE_SERIALIZER( U32Serializer );
    typedef SimpleSerializer<i32> I32Serializer;                              REFLECT_SPECIALIZE_SERIALIZER( I32Serializer );
    typedef SimpleSerializer<u64> U64Serializer;                              REFLECT_SPECIALIZE_SERIALIZER( U64Serializer );
    typedef SimpleSerializer<i64> I64Serializer;                              REFLECT_SPECIALIZE_SERIALIZER( I64Serializer );
    typedef SimpleSerializer<f32> F32Serializer;                              REFLECT_SPECIALIZE_SERIALIZER( F32Serializer );
    typedef SimpleSerializer<f64> F64Serializer;                              REFLECT_SPECIALIZE_SERIALIZER( F64Serializer );
    typedef SimpleSerializer<Helium::GUID> GUIDSerializer;                 REFLECT_SPECIALIZE_SERIALIZER( GUIDSerializer );
    typedef SimpleSerializer<Helium::TUID> TUIDSerializer;                 REFLECT_SPECIALIZE_SERIALIZER( TUIDSerializer );

    typedef SimpleSerializer<Math::Vector2> Vector2Serializer;              REFLECT_SPECIALIZE_SERIALIZER( Vector2Serializer );
    typedef SimpleSerializer<Math::Vector3> Vector3Serializer;              REFLECT_SPECIALIZE_SERIALIZER( Vector3Serializer );
    typedef SimpleSerializer<Math::Vector4> Vector4Serializer;              REFLECT_SPECIALIZE_SERIALIZER( Vector4Serializer );
    typedef SimpleSerializer<Math::Matrix3> Matrix3Serializer;              REFLECT_SPECIALIZE_SERIALIZER( Matrix3Serializer );
    typedef SimpleSerializer<Math::Matrix4> Matrix4Serializer;              REFLECT_SPECIALIZE_SERIALIZER( Matrix4Serializer );
    typedef SimpleSerializer<Math::Quaternion> QuaternionSerializer;        REFLECT_SPECIALIZE_SERIALIZER( QuaternionSerializer );

    typedef SimpleSerializer<Math::Color3> Color3Serializer;                REFLECT_SPECIALIZE_SERIALIZER( Color3Serializer );
    typedef SimpleSerializer<Math::Color4> Color4Serializer;                REFLECT_SPECIALIZE_SERIALIZER( Color4Serializer );
    typedef SimpleSerializer<Math::HDRColor3> HDRColor3Serializer;          REFLECT_SPECIALIZE_SERIALIZER( HDRColor3Serializer );
    typedef SimpleSerializer<Math::HDRColor4> HDRColor4Serializer;          REFLECT_SPECIALIZE_SERIALIZER( HDRColor4Serializer );
}
