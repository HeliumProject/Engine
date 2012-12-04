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
#include "Foundation/SmartPtr.h"
#include "Foundation/Endian.h"
#include "Foundation/Profile.h"

#include <vector>
#include <map>
#include <set>

namespace Helium
{
    class SmartBuffer;
    typedef Helium::SmartPtr< SmartBuffer >             SmartBufferPtr;
    typedef std::pair< uint32_t, SmartBufferPtr >       StrongBufferLocation;
    typedef std::pair< uint32_t, SmartBuffer* >         WeakBufferLocation;

	class Fixup;
    typedef Helium::SmartPtr< Fixup > FixupPtr;

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
        friend class RefCountBase< Fixup >;

    protected:
        Fixup();
        virtual ~Fixup();

    public:
        virtual FixupType GetType() const
        {
            return FixupTypes::Unknown;
        }

        virtual bool GetDestination( StrongBufferLocation& destination ) const
        {
            return false;
        }

        virtual void EraseIncoming( const WeakBufferLocation& source_location ) const
        {

        }

        virtual void ChangeDestination( const StrongBufferLocation& new_destination )
        {

        }

        virtual bool DoFixup( const WeakBufferLocation& source_location )
        {
            return true;
        }
    };

    class PointerFixup : public Fixup
    {
    protected:
        uint32_t m_Size;
        bool m_HasReference;
        WeakBufferLocation m_Destination;

    public:
        PointerFixup(uint32_t size);
        ~PointerFixup();

        uint32_t GetSize()
        {
            return m_Size;
        }

        virtual FixupType GetType() const
        {
            return FixupTypes::Pointer;
        }

        virtual bool GetDestination( StrongBufferLocation& destination ) const
        {
            destination = m_Destination;
            return true;
        }

        virtual void EraseIncoming( const WeakBufferLocation& source_location ) const;
        virtual void ChangeDestination( const StrongBufferLocation& new_destination );
        virtual bool DoFixup( const WeakBufferLocation& source_location );
    };

    class OffsetFixup : public Fixup
    {
    protected:
        bool m_Absolute;
        bool m_HasReference;
        WeakBufferLocation m_Destination;

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

        virtual bool GetDestination( StrongBufferLocation& destination ) const
        {
            destination = m_Destination; return true;
        }

        virtual void EraseIncoming( const WeakBufferLocation& source_location ) const;
        virtual void ChangeDestination( const StrongBufferLocation& new_destination );
        virtual bool DoFixup( const WeakBufferLocation& source_location );    
    };

    class VTableFixup : public Fixup
    {
    protected:
        uint32_t m_Size;
        uint32_t m_ClassIndex;

    public:
        VTableFixup( uint32_t class_index, uint32_t size );

        virtual FixupType GetType() const
        {
            return FixupTypes::VTable;
        }

        virtual bool DoFixup( const WeakBufferLocation& source_location );
    };

    class HELIUM_BUFFERS_API SmartBuffer : public Helium::RefCountBase<SmartBuffer>
    {
    public:
        static const uint32_t s_PointerSizes[ 2 ]; // big and little endian
        static const bool s_BigEndian[ 2 ];

    protected:
        tstring								m_Name;
        uint32_t							m_Type;
        uint8_t*							m_Data;
        uint32_t							m_Size;
        uint32_t							m_MaxSize;
        uint32_t							m_Capacity;
        bool								m_OwnsData;
        ByteOrder							m_ByteOrder;
        bool								m_Virtual;
        std::map< uint32_t, FixupPtr >		m_OutgoingFixups;
        std::vector< WeakBufferLocation >	m_IncomingFixups;

    public:
        SmartBuffer();
        virtual ~SmartBuffer();

        void* operator new (size_t bytes);
        void operator delete (void *ptr, size_t bytes);

        uint32_t GetType() const
        {
            return m_Type;
        }
        void SetType( uint32_t type )
        {
            m_Type = type;
        }

        ByteOrder GetByteOrder() const
        {
            return m_ByteOrder;
        }
        void SetByteOrder( ByteOrder platform )
        {
            HELIUM_ASSERT( platform == ByteOrders::LittleEndian || platform == ByteOrders::BigEndian );
            m_ByteOrder = platform;
        }

        uint32_t GetPlatformPtrSize() const
        {
            return s_PointerSizes[ m_ByteOrder ];
        }
        bool IsPlatformBigEndian() const
        {
            return s_BigEndian[ m_ByteOrder ];
        }

        // Returns a StrongBufferLocation object denoting the offset at the front of the data
        StrongBufferLocation GetHeadLocation()
        {
            return StrongBufferLocation( 0, this );
        }

        // Returns a StrongBufferLocation object denoting current offset (the end of the data)
        StrongBufferLocation GetCurrentLocation()
        {
            return StrongBufferLocation( m_Size, this );
        }

        // Returns a StrongBufferLocation object at the specified offset
        StrongBufferLocation GetOffsetLocation( uint32_t offset )
        {
            HELIUM_ASSERT( offset <= m_Size );
            return StrongBufferLocation( offset, this );
        }

        // Return a StrongBufferLocation object based on the specified address
        StrongBufferLocation GetAddressLocation( void* address )
        {
            HELIUM_ASSERT( (uint8_t*)address >= m_Data && (uint8_t*)address < ( m_Data + m_Size ) );
            return StrongBufferLocation( (uint32_t)(uintptr_t)((uint8_t*)address - m_Data), this );
        }

        // Outgoing fixup access
        std::map< uint32_t, FixupPtr >& GetOutgoingFixups()
        {
            return m_OutgoingFixups;
        }
        const std::map< uint32_t, FixupPtr >& GetOutgoingFixups() const
        {
            return m_OutgoingFixups;
        }

        // Incoming fixup access
        std::vector< WeakBufferLocation >& GetIncomingFixups()
        {
            return m_IncomingFixups;
        }
        const std::vector< WeakBufferLocation >& GetIncomingFixups() const
        {
            return m_IncomingFixups;
        }

        // The currently allocated size
        uint32_t GetSize() const
        {
            return m_Size;
        }

        // Resets the buffer to zero memory allocated and removed all the fixups
        void Reset();

        // Get a pointer to the data
        const uint8_t* GetData() const
        {
            return m_Data;
        }

        // Take ownership of the allocated data
        void TakeData( uint32_t& size, uint8_t*& );

        // Get the max size
        uint32_t GetMaxSize() const
        {
            return m_MaxSize;
        }

        // Set the max size of a smart buffer, must be called when the buffer is empty
        void SetMaxSize(uint32_t size);

        // Switch the smart buffer to use virtual memory, the size specified is the maximum size.
        //  This must be called when the buffer is empty
        void SetVirtual(uint32_t size);

        // Grow the buffer to at least the specified size
        void GrowBy(uint32_t size);

        // Resize the buffer to at least the specified size
        void Resize(uint32_t size);

        // Pre-allocate the data to at least the specified size
        void Reserve(uint32_t size);

        // Take ownership of the specified buffer
        bool AdoptBuffer( const SmartBufferPtr& buffer );

        // Collect buffers this buffer points to
        void CollectChildren( std::vector< SmartBufferPtr >& buffers );

        // Inherit the fixups, this needs more documentation
        void InheritFixups( const SmartBufferPtr& buffer, uint32_t offset );

        // Dump debug info, fixups etc
        void Dump();

        // Fixup API
        static bool AddFixup( const WeakBufferLocation& source, const FixupPtr& fixup );

        /// Explicitly link the source location to the destination location with an offset
        /// If absolute is true the offset is intended to be a absolute offset from some
        /// known base address, if false the offset is relative from the current address.
        /// The absolute flag does not alter how the offset is stored, it is mainly used
        /// by writers when writing the final data.
        ///  returns true if the source was successfully linked to the destination
        ///  returns false if the destination isn't valid
        ///  asserts on invalid input
        static bool AddOffsetFixup( const StrongBufferLocation& source, const StrongBufferLocation& destination, bool absolute = false ); 

        /// Explicitly link the source location to the destination location with a pointer
        ///  returns true if the source was successfully linked to the destination
        ///  returns false if the destination isn't valid
        ///  asserts on invalid input
        static bool AddPointerFixup( const StrongBufferLocation& source, const StrongBufferLocation& destination, uint32_t size = 0 ); 

        /// This fixup writes a class index to the virtual function pointer location of a class
        /// which the runtime loader can then use to assign an actual pointer value
        static bool AddVTableFixup( const StrongBufferLocation& source, uint32_t class_index, uint32_t size = 0 );

        // Functions to explictily write a type to a given location
        static void Write(const StrongBufferLocation& pointer,const void* val,uint32_t size);
        static void WriteI8(const StrongBufferLocation& pointer,int8_t val);
        static void WriteU8(const StrongBufferLocation& pointer,uint8_t val);
        static void WriteI16(const StrongBufferLocation& pointer,int16_t val);
        static void WriteU16(const StrongBufferLocation& pointer,uint16_t val);
        static void WriteI32(const StrongBufferLocation& pointer,int32_t val);
        static void WriteU32(const StrongBufferLocation& pointer,uint32_t val);
        static void WriteI64(const StrongBufferLocation& pointer,int64_t val);
        static void WriteU64(const StrongBufferLocation& pointer,uint64_t val);
        static void WriteF32(const StrongBufferLocation& pointer,float32_t val);
        static void WriteF64(const StrongBufferLocation& pointer,float64_t val);
    };
}