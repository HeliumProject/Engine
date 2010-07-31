#pragma once

//
// SmartBuffer supports the following
//  - reference counted auto growing buffers
//  - adding basic types to buffers
//  - treating a SmartBuffer as a pointer to an object and/or an array of that object type
//  - intelligent pointer fixups
//  - intelligent endian fixups
//  - nested, named, SmartBuffers 
//  - complete and partial serialization to and from std::streams
//

#include "API.h"
#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Memory/Endian.h"
#include "Foundation/Container/OrderedSet.h"

#include "Foundation/Profile.h"

#include <vector>
#include <map>
#include <set>

namespace Helium
{
    class SmartBuffer;
    typedef Helium::SmartPtr<SmartBuffer>               SmartBufferPtr;
    typedef Helium::OrderedSet< SmartBufferPtr >        S_SmartBufferPtr;
    typedef std::vector< SmartBufferPtr >               V_SmartBufferPtr;

    typedef std::pair< u32, SmartBufferPtr >            BufferLocation;
    typedef std::pair< u32, SmartBuffer* >              DumbBufferLocation;
    typedef Helium::OrderedSet< DumbBufferLocation >    S_DumbBufferLocation;

    namespace ByteOrders
    {
        enum ByteOrder
        {
            LittleEndian = 0,   // little endian
            BigEndian,          // big endian
            Count
        };
    }
    typedef ByteOrders::ByteOrder ByteOrder;

    const ByteOrder DEFAULT_BYTE_ORDER = ByteOrders::LittleEndian;

    typedef Helium::SmartPtr<class Fixup> FixupPtr;

    namespace FixupTypes
    {
        enum FixupType
        {
            Unknown = 0,
            Pointer,
            Offset,
            Endian,
            VTable,
        };
    }
    typedef FixupTypes::FixupType FixupType;

    class Fixup : public Helium::RefCountBase<Fixup>
    {
    protected:
        Fixup();
        virtual ~Fixup();

    public:
        virtual FixupType GetType() const
        {
            return FixupTypes::Unknown;
        }

        virtual bool GetDestination( BufferLocation& destination ) const
        {
            return false;
        }

        virtual void EraseIncoming( const DumbBufferLocation& source_location ) const
        {

        }

        virtual void ChangeDestination( const BufferLocation& new_destination )
        {

        }

        virtual bool DoFixup( const DumbBufferLocation& source_location )
        {
            return true;
        }
    };

    class PointerFixup : public Fixup
    {
    protected:
        u32 m_Size;
        bool m_HasReference;
        DumbBufferLocation m_Destination;

    public:
        PointerFixup(u32 size);
        ~PointerFixup();

        u32 GetSize()
        {
            return m_Size;
        }

        virtual FixupType GetType() const
        {
            return FixupTypes::Pointer;
        }

        virtual bool GetDestination( BufferLocation& destination ) const
        {
            destination = m_Destination;
            return true;
        }

        virtual void EraseIncoming( const DumbBufferLocation& source_location ) const;
        virtual void ChangeDestination( const BufferLocation& new_destination );
        virtual bool DoFixup( const DumbBufferLocation& source_location );
    };

    class OffsetFixup : public Fixup
    {
    protected:
        bool m_Absolute;
        bool m_HasReference;
        DumbBufferLocation m_Destination;

    public:
        OffsetFixup( bool absolute );
        ~OffsetFixup();

        bool IsAbsolute()
        {
            return m_Absolute;
        }

        virtual FixupType GetType() const
        {
            return FixupTypes::Offset;
        }

        virtual bool GetDestination( BufferLocation& destination ) const
        {
            destination = m_Destination; return true;
        }

        virtual void EraseIncoming( const DumbBufferLocation& source_location ) const;
        virtual void ChangeDestination( const BufferLocation& new_destination );
        virtual bool DoFixup( const DumbBufferLocation& source_location );    
    };

    class VTableFixup : public Fixup
    {
    protected:
        u32 m_Size;
        u32 m_ClassIndex;

    public:
        VTableFixup( u32 class_index, u32 size );

        virtual FixupType GetType() const
        {
            return FixupTypes::VTable;
        }

        virtual bool DoFixup( const DumbBufferLocation& source_location );
    };

    class FOUNDATION_API SmartBuffer : public Helium::RefCountBase<SmartBuffer>
    {
    public:
        typedef std::map< u32, FixupPtr >           M_OffsetToFixup;

        static const u32 s_PointerSizes[ ByteOrders::Count ];
        static const bool s_BigEndian[ ByteOrders::Count ];

        static Profile::MemoryPoolHandle s_ObjectPool;
        static Profile::MemoryPoolHandle s_DataPool;

    protected:
        tstring         m_Name;
        u32             m_Type;
        u8*             m_Data;
        u32             m_Size;
        u32             m_MaxSize;
        u32             m_Capacity;
        bool            m_OwnsData;
        ByteOrder       m_ByteOrder;
        bool            m_Virtual;
        M_OffsetToFixup m_OutgoingFixups;
        S_DumbBufferLocation  m_IncomingFixups;

    public:
        SmartBuffer();
        virtual ~SmartBuffer();

        void* operator new (size_t bytes);
        void operator delete (void *ptr, size_t bytes);

        u32 GetType() const
        {
            return m_Type;
        }
        void SetType( u32 type )
        {
            m_Type = type;
        }

        ByteOrder GetByteOrder() const
        {
            return m_ByteOrder;
        }
        void SetByteOrder(ByteOrder platform)
        {
            HELIUM_ASSERT( m_ByteOrder >= 0 && m_ByteOrder < ByteOrders::Count );
            m_ByteOrder = platform;
        }

        u32 GetPlatformPtrSize() const
        {
            return s_PointerSizes[ m_ByteOrder ];
        }
        bool IsPlatformBigEndian() const
        {
            return s_BigEndian[ m_ByteOrder ];
        }

        // Returns a BufferLocation object denoting the offset at the front of the data
        BufferLocation GetHeadLocation()
        {
            return BufferLocation( 0, this );
        }

        // Returns a BufferLocation object denoting current offset (the end of the data)
        BufferLocation GetCurrentLocation()
        {
            return BufferLocation( m_Size, this );
        }

        // Returns a BufferLocation object at the specified offset
        BufferLocation GetOffsetLocation( u32 offset )
        {
            HELIUM_ASSERT( offset <= m_Size );
            return BufferLocation( offset, this );
        }

        // Return a BufferLocation object based on the specified address
        BufferLocation GetAddressLocation( void* address )
        {
            HELIUM_ASSERT( (u8*)address >= m_Data && (u8*)address < ( m_Data + m_Size ) );
            return BufferLocation( (u32)(uintptr)((u8*)address - m_Data), this );
        }

        // Outgoing fixup access
        M_OffsetToFixup& GetOutgoingFixups()
        {
            return m_OutgoingFixups;
        }
        const M_OffsetToFixup& GetOutgoingFixups() const
        {
            return m_OutgoingFixups;
        }

        // Incoming fixup access
        S_DumbBufferLocation& GetIncomingFixups()
        {
            return m_IncomingFixups;
        }
        const S_DumbBufferLocation& GetIncomingFixups() const
        {
            return m_IncomingFixups;
        }

        // The currently allocated size
        u32 GetSize() const
        {
            return m_Size;
        }

        // Resets the buffer to zero memory allocated and removed all the fixups
        void Reset();

        // Get a pointer to the data
        const u8* GetData() const
        {
            return m_Data;
        }

        // Take ownership of the allocated data
        void TakeData( u32& size, u8*& );

        // Get the max size
        u32 GetMaxSize() const
        {
            return m_MaxSize;
        }

        // Set the max size of a smart buffer, must be called when the buffer is empty
        void SetMaxSize(u32 size);

        // Switch the smart buffer to use virtual memory, the size specified is the maximum size.
        //  This must be called when the buffer is empty
        void SetVirtual(u32 size);

        // Grow the buffer to at least the specified size
        void GrowBy(u32 size);

        // Resize the buffer to at least the specified size
        void Resize(u32 size);

        // Pre-allocate the data to at least the specified size
        void Reserve(u32 size);

        // Take ownership of the specified buffer
        bool AdoptBuffer( const SmartBufferPtr& buffer );

        // Collect buffers this buffer points to
        void CollectChildren( S_SmartBufferPtr& buffers );

        // Inherit the fixups, this needs more documentation
        void InheritFixups( const SmartBufferPtr& buffer, u32 offset );

        // Dump debug info, fixups etc
        void Dump();

        // Fixup API
        static bool AddFixup( const DumbBufferLocation& source, const FixupPtr& fixup );

        /// Explicitly link the source location to the destination location with an offset
        /// If absolute is true the offset is intended to be a absolute offset from some
        /// known base address, if false the offset is relative from the current address.
        /// The absolute flag does not alter how the offset is stored, it is mainly used
        /// by writers when writing the final data.
        ///  returns true if the source was successfully linked to the destination
        ///  returns false if the destination isn't valid
        ///  asserts on invalid input
        static bool AddOffsetFixup( const BufferLocation& source, const BufferLocation& destination, bool absolute = false ); 

        /// Explicitly link the source location to the destination location with a pointer
        ///  returns true if the source was successfully linked to the destination
        ///  returns false if the destination isn't valid
        ///  asserts on invalid input
        static bool AddPointerFixup( const BufferLocation& source, const BufferLocation& destination, u32 size = 0 ); 

        /// This fixup writes a class index to the virtual function pointer location of a class
        /// which the runtime loader can then use to assign an actual pointer value
        static bool AddVTableFixup( const BufferLocation& source, u32 class_index, u32 size = 0 );

        // Functions to explictily write a type to a given location
        static void Write(const BufferLocation& pointer,const void* val,u32 size);
        static void WriteI8(const BufferLocation& pointer,i8 val);
        static void WriteU8(const BufferLocation& pointer,u8 val);
        static void WriteI16(const BufferLocation& pointer,i16 val);
        static void WriteU16(const BufferLocation& pointer,u16 val);
        static void WriteI32(const BufferLocation& pointer,i32 val);
        static void WriteU32(const BufferLocation& pointer,u32 val);
        static void WriteI64(const BufferLocation& pointer,i64 val);
        static void WriteU64(const BufferLocation& pointer,u64 val);
        static void WriteF32(const BufferLocation& pointer,f32 val);
        static void WriteF64(const BufferLocation& pointer,f64 val);
    };
}