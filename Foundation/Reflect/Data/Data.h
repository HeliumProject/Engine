#pragma once

#include <sstream>
#include <iostream>

#include "Foundation/Automation/Event.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Archive.h"
#include "Foundation/SmartBuffer/BasicBuffer.h"

namespace Helium
{
    namespace Reflect
    {
        class ArchiveBinary;
        class ArchiveXML;

        namespace DataFlags
        {
            enum DataFlag
            {
                Shallow   = 1 << 0,
            };
        }

        typedef DataFlags::DataFlag DataFlag;

        //
        // A pointer to some typed data (owned by the object itself or someone else
        //

        template<class T>
        class DataPointer
        {
        public:
            inline DataPointer();
            inline ~DataPointer();

            inline void Allocate() const;
            inline void Deallocate() const;

            inline void Connect(void* pointer);
            inline void Disconnect();

            inline const T* operator->() const;
            inline T* operator->();

            inline operator const T*() const;
            inline operator T*();

        private:
            mutable T*      m_Target;
            mutable bool    m_Owned;
        };

        //
        // Like DataPointer, but typeless
        //

        class VoidDataPointer
        {
        public:
            inline VoidDataPointer();
            inline ~VoidDataPointer();

            inline void Allocate( uint32_t size ) const;
            inline void Deallocate() const;

            inline void Connect(void* pointer, uint32_t size);
            inline void Disconnect();

            inline const void* Get(uint32_t size) const;
            inline void* Get(uint32_t size);

        private:
            mutable void*       m_Target;
            mutable bool        m_Owned;
#ifdef REFLECT_CHECK_MEMORY
            mutable uint32_t    m_Size;
#endif
        };
        
        //
        // A Data is an Object that knows how to read/write data
        //  from any kind of support Archive type (XML and Binary), given
        //  an address in memory to serialize/deserialize data to/from
        //

        class HELIUM_FOUNDATION_API Data : public Object
        {
        protected:
            // the instance we are processing, if any
            void* m_Instance;

            // the field we are processing, if any
            const Field* m_Field;

        public:
            REFLECT_DECLARE_ABSTRACT( Data, Object );

            // instance init and cleanup
            Data();
            ~Data();

            // static init and cleanup
            static void Initialize();
            static void Cleanup();

            //
            // Connection
            //

            // set the address to interface with
            virtual void ConnectData(void* data) = 0;

            // connect to a field of an object
            void ConnectField(void* instance, const Field* field, uintptr_t offsetInField = 0);

            // reset all pointers
            void Disconnect();

            //
            // Specializations
            //

            template<class T>
            inline static T* GetData(Data*);

            //
            // Creation templates
            //

            template <class T>
            inline static DataPtr Create();

            template <class T>
            inline static DataPtr Create(const T& value);

            template <class T>
            inline static DataPtr Bind(T& value, void* instance, const Field* field);
            
            // Not inline because this requires StructureData, so must be in .cpp
            static DataPtr BindStructure(void* value, void* instance, const Field* field);

            //
            // Value templates
            //

            template <typename T>
            inline static bool GetValue(Data* ser, T& value);

            template <typename T>
            inline static bool SetValue(Data* ser, T value, bool raiseEvents = true);

            //
            // Data Management
            //

            // check to see if a cast is supported
            static bool CastSupported(const Class* srcType, const Class* destType);

            // convert value data from one data to another
            static bool CastValue(Data* src, Data* dest, uint32_t flags = 0);

            // copies value data from one data to another
            virtual bool Set(Data* src, uint32_t flags = 0) = 0;

            // assign
            inline Data& operator=(Data* rhs);
            inline Data& operator=(Data& rhs);

            // equality
            inline bool operator==(Data* rhs);
            inline bool operator== (Data& rhs);

            // inequality
            inline bool operator!=(Data* rhs);
            inline bool operator!=(Data& rhs);

            //
            // Serialization
            //

            // is this data worth serializing? (perhaps its an empty container?)
            virtual bool ShouldSerialize();

            // data serialization (extract to smart buffer)
            virtual void Serialize(const Helium::BasicBufferPtr& buffer, const tchar_t* debugStr) const;

            // binary serialization
            virtual void Serialize(ArchiveBinary& archive) = 0;
            virtual void Deserialize(ArchiveBinary& archive) = 0;

            // xml serialization
            virtual void Serialize(ArchiveXML& archive) = 0;
            virtual void Deserialize(ArchiveXML& archive) = 0;

            // text serialization (extract to text stream)
            virtual tostream& operator>>(tostream& stream) const;

            // text deserialization (insert from text stream)
            virtual tistream& operator<<(tistream& stream);

            //
            // Visitor
            //

            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;
        };

        //
        // These are the global entry points for our virtual insert an extract functions
        //

        inline tostream& operator<<(tostream& stream, const Data& s);
        inline tistream& operator>>(tistream& stream, Data& s);

        //
        // Data class deduction function
        //

        template<class T>
        inline static const Class* GetDataClass();
    }
}

#include "Foundation/Reflect/Data/Data.inl"