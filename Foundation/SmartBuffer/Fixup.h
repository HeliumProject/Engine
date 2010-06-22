#pragma once

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

#include <map>
#include <set>

#include "SmartBuffer.h"

namespace Nocturnal
{
    typedef Nocturnal::SmartPtr<class Fixup> FixupPtr;

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

    class Fixup : public Nocturnal::RefCountBase<Fixup>
    {
    protected:
        Fixup();
        virtual ~Fixup();

    public:
        virtual FixupType GetType() const
        {
            return FixupTypes::Unknown;
        }

        virtual bool GetDestination( SmartBuffer::Location& destination ) const
        {
            return false;
        }

        virtual void EraseIncoming( const SmartBuffer::DumbLocation& source_location ) const
        {

        }

        virtual void ChangeDestination( const SmartBuffer::Location& new_destination )
        {

        }

        virtual bool DoFixup( const SmartBuffer::DumbLocation& source_location )
        {
            return true;
        }
    };

    class PointerFixup : public Fixup
    {
    protected:
        u32 m_Size;
        bool m_HasReference;
        SmartBuffer::DumbLocation m_Destination;

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

        virtual bool GetDestination( SmartBuffer::Location& destination ) const
        {
            destination = m_Destination;
            return true;
        }

        virtual void EraseIncoming( const SmartBuffer::DumbLocation& source_location ) const;
        virtual void ChangeDestination( const SmartBuffer::Location& new_destination );
        virtual bool DoFixup( const SmartBuffer::DumbLocation& source_location );
    };


    class OffsetFixup : public Fixup
    {
    protected:
        bool m_Absolute;
        bool m_HasReference;
        SmartBuffer::DumbLocation m_Destination;

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

        virtual bool GetDestination( SmartBuffer::Location& destination ) const
        {
            destination = m_Destination; return true;
        }

        virtual void EraseIncoming( const SmartBuffer::DumbLocation& source_location ) const;
        virtual void ChangeDestination( const SmartBuffer::Location& new_destination );
        virtual bool DoFixup( const SmartBuffer::DumbLocation& source_location );    
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

        virtual bool DoFixup( const SmartBuffer::DumbLocation& source_location );
    };
}