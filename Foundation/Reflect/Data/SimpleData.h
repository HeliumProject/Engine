#pragma once

#include "Foundation/Reflect/Data/Data.h"
#include "Foundation/Math/Vector2.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/Vector4.h"
#include "Foundation/Math/Matrix3.h"
#include "Foundation/Math/Matrix4.h"
#include "Foundation/Math/Color3.h"
#include "Foundation/Math/Color4.h"
#include "Foundation/Math/HDRColor3.h"
#include "Foundation/Math/HDRColor4.h"
#include "Foundation/GUID.h"
#include "Foundation/TUID.h"

namespace Helium
{
    namespace Reflect
    {
        //
        // SimpleData wraps data and read/writes it from the archive
        //  This template class can be extended to wrap any primitive type that
        //  has constant size and support for iostream insertion and extraction
        //

        template <class T>
        class FOUNDATION_API SimpleData : public Data
        {
        public:
            typedef T DataType;
            Data::Pointer<DataType> m_Data;

            typedef SimpleData<DataType> SimpleDataT;
            REFLECT_DECLARE_CLASS( SimpleDataT, Data );

            SimpleData ();
            ~SimpleData();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual bool Set(const Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Data* s) const HELIUM_OVERRIDE;

            virtual void Serialize(const Helium::BasicBufferPtr& buffer, const tchar_t* debugStr) const HELIUM_OVERRIDE;
            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream);
        };

        typedef SimpleData<tstring> StringData;
        typedef SimpleData<bool> BoolData;
        typedef SimpleData<uint8_t> UInt8Data;
        typedef SimpleData<int8_t> Int8Data;
        typedef SimpleData<uint16_t> UInt16Data;
        typedef SimpleData<int16_t> Int16Data;
        typedef SimpleData<uint32_t> UInt32Data;
        typedef SimpleData<int32_t> Int32Data;
        typedef SimpleData<uint64_t> UInt64Data;
        typedef SimpleData<int64_t> Int64Data;
        typedef SimpleData<float32_t> Float32Data;
        typedef SimpleData<float64_t> Float64Data;
        typedef SimpleData<Helium::GUID> GUIDData;
        typedef SimpleData<Helium::TUID> TUIDData;

        typedef SimpleData<Vector2> Vector2Data;
        typedef SimpleData<Vector3> Vector3Data;
        typedef SimpleData<Vector4> Vector4Data;
        typedef SimpleData<Matrix3> Matrix3Data;
        typedef SimpleData<Matrix4> Matrix4Data;

        typedef SimpleData<Color3> Color3Data;
        typedef SimpleData<Color4> Color4Data;
        typedef SimpleData<HDRColor3> HDRColor3Data;
        typedef SimpleData<HDRColor4> HDRColor4Data;
    }
}