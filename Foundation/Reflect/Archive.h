#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

#include "Platform/Assert.h"

#include "Foundation/Log.h" 
#include "Foundation/File/Path.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Foundation/Reflect/API.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Exceptions.h"
#include "Foundation/Reflect/ArchiveStream.h" 

namespace Helium
{
    namespace Reflect
    {
        class Archive;

        namespace ArchiveFlags
        {
            enum ArchiveFlag
            {
                Status  = 1 << 0, // Display status reporting
                Sparse  = 1 << 1, // Allow sparse array populations for failed objects
            };
        }

        namespace ArchiveTypes
        {
            enum ArchiveType
            {
                Auto,
                Binary,
                XML,
                Base
            };
        }
        typedef ArchiveTypes::ArchiveType ArchiveType;

        namespace ArchiveModes
        {
            enum ArchiveMode
            {
                Read,
                Write,
            };
        }

        typedef ArchiveModes::ArchiveMode ArchiveMode;

        //
        // Status reporting
        //

        namespace ArchiveStates
        {
            enum ArchiveState
            {
                Starting,
                PreProcessing,
                ArchiveStarting,
                ObjectProcessed,
                ArchiveComplete,
                PostProcessing,
                Complete,
                Publishing,
            };
        }
        typedef ArchiveStates::ArchiveState ArchiveState;

        struct ArchiveStatus
        {
            ArchiveStatus( const Archive& archive, const ArchiveState& state )
                : m_Archive( archive )
                , m_State( state )
                , m_Progress ( 0 )
                , m_Abort ( false )
            {
            }

            const Archive&  m_Archive;
            ArchiveState    m_State;
            int             m_Progress;
            tstring         m_Info;

            // flag this if you want to give up
            mutable bool    m_Abort;
        };
        typedef Helium::Signature< const ArchiveStatus& > ArchiveStatusSignature;

        //
        // Archive base class
        //

        class FOUNDATION_API Archive : public Helium::RefCountBase< Archive >
        {
        protected:
            friend class RefCountBase< Archive >;

            Archive( const Path& path, ByteOrder byteOrder = Helium::PlatformByteOrder );
            Archive();
            virtual ~Archive();

        public:
            const Helium::Path& GetPath() const
            {
                return m_Path;
            }

            ArchiveMode GetMode() const
            {
                return m_Mode;
            }

            virtual ArchiveType GetType() const
            {
                return ArchiveTypes::Base;
            }

            //
            // Virutal functionality, meant to be overridden by Binary/XML/etc. archives
            //

            // File Open/Close
            virtual void Open( bool write = false ) = 0;
            virtual void Close() = 0;

            // Begins parsing the InputStream
            virtual void Read() = 0;

            // Write to the OutputStream
            virtual void Write() = 0;

            //
            // Serialization
            //

            virtual void Serialize( Object* object ) = 0;
            virtual void Serialize( void* structure, const Structure* type ) = 0;
            virtual void Serialize( const std::vector< ObjectPtr >& objects, uint32_t flags = 0 ) = 0;
            virtual void Serialize( const DynArray< ObjectPtr >& objects, uint32_t flags = 0 ) = 0;
            virtual void Deserialize( ObjectPtr& object ) = 0;
            virtual void Deserialize( void* structure, const Structure* type ) = 0;
            virtual void Deserialize( std::vector< ObjectPtr >& objects, uint32_t flags = 0 ) = 0;
            virtual void Deserialize( DynArray< ObjectPtr >& objects, uint32_t flags = 0 ) = 0;

            //
            // Event API
            //

            ArchiveStatusSignature::Event e_Status;

            //
            // Get objects from the file
            //

            void Put( Object* object );
            void Put( const std::vector< ObjectPtr >& objects );

            ObjectPtr Get( const Class* searchClass = NULL );
            void Get( std::vector< ObjectPtr >& objects );

            // Get a single object of the specified type in the archive
            template <class T>
            Helium::StrongPtr<T> Get()
            {
                ObjectPtr found = Get( Reflect::GetClass<T>() );

                if (found.ReferencesObject())
                {
                    return SafeCast<T>( found );
                }
                else
                {
                    return NULL;
                }
            }

            // Get all objects of the specified type in the archive
            template< class T >
            void Get( std::vector< Helium::StrongPtr<T> >& objects )
            {
                std::vector< ObjectPtr > archiveObjects;
                Get( archiveObjects );

                std::vector< ObjectPtr >::iterator itr = archiveObjects.begin();
                std::vector< ObjectPtr >::iterator end = archiveObjects.end();
                for( ; itr != end; ++itr )
                {
                    T* casted = Reflect::SafeCast< T >( *itr )
                    if( casted ) 
                    {
                        objects.push_back( casted );
                    }
                }
            }

        protected:
            // The number of bytes Parsed so far
            unsigned m_Progress;

            // The abort status
            bool m_Abort;

            // The file we are working with
            Path m_Path;

            // The mode
            ArchiveMode m_Mode;

            // The byte order
            ByteOrder m_ByteOrder;

            // The type to serach for
            const Class* m_SearchClass;

            // The array of objects that we've found
            std::vector< ObjectPtr > m_Objects;
        };

        typedef Helium::SmartPtr< Archive > ArchivePtr;

        // Get parser for a file
        FOUNDATION_API ArchivePtr GetArchive( const Path& path, ArchiveType archiveType = ArchiveTypes::Auto, ByteOrder byteOrder = Helium::PlatformByteOrder );

        FOUNDATION_API bool ToArchive( const Path& path, ObjectPtr object, ArchiveType archiveType = ArchiveTypes::Auto, tstring* error = NULL, ByteOrder byteOrder = Helium::PlatformByteOrder );
        FOUNDATION_API bool ToArchive( const Path& path, const std::vector< ObjectPtr >& objects, ArchiveType archiveType = ArchiveTypes::Auto, tstring* error = NULL, ByteOrder byteOrder = Helium::PlatformByteOrder );

        template <class T>
        Helium::StrongPtr<T> FromArchive( const Path& path, ArchiveType archiveType = ArchiveTypes::Auto, ByteOrder byteOrder = Helium::PlatformByteOrder )
        {
            ArchivePtr archive = GetArchive( path, archiveType, byteOrder );

            if ( archive.ReferencesObject() )
            {
                return archive->Get< T >();
            }

            return NULL;
        }

        template< class T >
        void FromArchive( const Path& path, std::vector< Helium::StrongPtr<T> >& objects, ArchiveType archiveType = ArchiveTypes::Auto, ByteOrder byteOrder = Helium::PlatformByteOrder )
        {
            ArchivePtr archive = GetArchive( path, archiveType, byteOrder );
            archive->Get< T >( objects );
        }
    }
}