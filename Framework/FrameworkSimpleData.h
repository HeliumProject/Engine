#pragma once

#include "Framework.h"
#include "Reflect/Data/Data.h"
#include "Reflect/Data/StlVectorData.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Color3.h"
#include "Math/Color4.h"
#include "Math/HDRColor3.h"
#include "Math/HDRColor4.h"

// Everything in this file is a big ol' temporary hack to give us
// SimpleData reflection capability for types that are downstream
// from Reflect. We just duplicate SimpleData and SimpleStlVectorData
// here and instantiate the additional specializations we need.
//
// Also, aclysma made me do it.

namespace Helium
{
    namespace Framework
    {
        template <class T>
        class HELIUM_FRAMEWORK_API FrameworkSimpleData : public Reflect::Data
        {
        public:
            typedef T DataType;
            Reflect::DataPointer<DataType> m_Data;

            typedef FrameworkSimpleData<DataType> SimpleDataT;
            REFLECT_DECLARE_OBJECT( SimpleDataT, Data );

            FrameworkSimpleData();
            ~FrameworkSimpleData();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual bool Set(Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;

            virtual void Serialize(Reflect::ArchiveBinary& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(Reflect::ArchiveBinary& archive) HELIUM_OVERRIDE;

            virtual void Serialize(Reflect::ArchiveXML& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(Reflect::ArchiveXML& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>>(tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<<(tistream& stream);
        };

        typedef FrameworkSimpleData<Vector2> Vector2Data;
        typedef FrameworkSimpleData<Vector3> Vector3Data;
        typedef FrameworkSimpleData<Vector4> Vector4Data;

        typedef FrameworkSimpleData<Matrix3> Matrix3Data;
        typedef FrameworkSimpleData<Matrix4> Matrix4Data;

        typedef FrameworkSimpleData<Color3> Color3Data;
        typedef FrameworkSimpleData<Color4> Color4Data;

        typedef FrameworkSimpleData<HDRColor3> HDRColor3Data;
        typedef FrameworkSimpleData<HDRColor4> HDRColor4Data;




        template <class T>
        class HELIUM_FRAMEWORK_API FrameworkSimpleStlVectorData : public Reflect::StlVectorData
        {
        public:
            typedef std::vector<T> DataType;
            Reflect::DataPointer<DataType> m_Data;

            typedef FrameworkSimpleStlVectorData<T> StlVectorDataT;
            REFLECT_DECLARE_OBJECT( StlVectorDataT, StlVectorData );

            FrameworkSimpleStlVectorData();
            ~FrameworkSimpleStlVectorData();

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void SetSize(size_t size) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Reflect::Class* GetItemClass() const HELIUM_OVERRIDE;
            virtual Reflect::DataPtr GetItem(size_t at) HELIUM_OVERRIDE;
            virtual void SetItem(size_t at, Reflect::Data* value) HELIUM_OVERRIDE;
            virtual void Insert( size_t at, Reflect::Data* value ) HELIUM_OVERRIDE;
            virtual void Remove( size_t at ) HELIUM_OVERRIDE;
            virtual void MoveUp( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;
            virtual void MoveDown( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;

            virtual bool Set(Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;

            virtual void Serialize( Reflect::ArchiveBinary& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( Reflect::ArchiveBinary& archive ) HELIUM_OVERRIDE;

            virtual void Serialize( Reflect::ArchiveXML& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( Reflect::ArchiveXML& archive ) HELIUM_OVERRIDE;

            virtual tostream& operator>>(tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<<(tistream& stream) HELIUM_OVERRIDE;
        };

        typedef FrameworkSimpleStlVectorData<Vector2> Vector2StlVectorData;
        typedef FrameworkSimpleStlVectorData<Vector3> Vector3StlVectorData;
        typedef FrameworkSimpleStlVectorData<Vector4> Vector4StlVectorData;

        typedef FrameworkSimpleStlVectorData<Matrix3> Matrix3StlVectorData;
        typedef FrameworkSimpleStlVectorData<Matrix4> Matrix4StlVectorData;

        typedef FrameworkSimpleStlVectorData<Color3> Color3StlVectorData;
        typedef FrameworkSimpleStlVectorData<Color4> Color4StlVectorData;

        typedef FrameworkSimpleStlVectorData<HDRColor3> HDRColor3StlVectorData;
        typedef FrameworkSimpleStlVectorData<HDRColor4> HDRColor4StlVectorData;
    }
}