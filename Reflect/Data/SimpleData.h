#pragma once

#include "Reflect/Data/Data.h"
#include "Foundation/GUID.h"
#include "Foundation/TUID.h"
#include "Foundation/Name.h"

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
        class HELIUM_REFLECT_API SimpleData : public Data
        {
        public:
            typedef T DataType;
            DataPointer<DataType> m_Data;

            typedef SimpleData<DataType> SimpleDataT;
            REFLECT_DECLARE_OBJECT( SimpleDataT, Data );

            SimpleData ();
            ~SimpleData();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual bool Set(Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveBinary& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveBinary& archive) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveXML& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveXML& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>>(tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<<(tistream& stream);
        };

        typedef SimpleData<Name> NameData;
        typedef SimpleData<tstring> StlStringData;
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
    }
}