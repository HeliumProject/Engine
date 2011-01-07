#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

#include "API.h"
#include "Cache.h"
#include "Class.h"
#include "Exceptions.h"
#include "ArchiveStream.h" 

#include "Platform/Assert.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Log.h" 
#include "Foundation/File/Path.h"

#include "Foundation/Memory/SmartPtr.h"

namespace Helium
{
    namespace Reflect
    {
        //
        // Client processing helper
        //

        class FOUNDATION_API Archive;

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

        struct StatusInfo
        {
            const Archive& m_Archive;
            ArchiveState m_ArchiveState;
            int m_Progress;
            mutable bool m_Abort;
            tstring m_Info;

            StatusInfo( const Archive& archive, const ArchiveState& state )
                : m_Archive( archive )
                , m_ArchiveState( state )
                , m_Progress ( 0 )
                , m_Abort ( false )
            {
            }
        };
        typedef Helium::Signature< const StatusInfo& > StatusSignature;

        namespace ExceptionActions
        {
            enum ExceptionAction
            {
                Unknown,
                Accept,
                Reject,
            };
        }
        typedef ExceptionActions::ExceptionAction ExceptionAction;

        typedef void (Object::*ObjectCallback)();

        struct ExceptionInfo
        {
            const Archive& m_Archive;

            Object* m_Object;
            ObjectCallback m_Callback;
            const Helium::Exception& m_Exception;

            mutable ExceptionAction m_Action;

            ExceptionInfo( const Archive& archive, Object* element, ObjectCallback callback, const Helium::Exception& exception )
                : m_Archive ( archive )
                , m_Object ( element )
                , m_Callback ( callback )
                , m_Exception ( exception )
                , m_Action ( ExceptionActions::Unknown )
            {
            }
        };
        typedef Helium::Signature< const ExceptionInfo& > ExceptionSignature;


        //
        // Types
        //

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
        // Event Delegates
        //

        class FOUNDATION_API ArchiveVisitor : public Helium::AtomicRefCountBase< ArchiveVisitor >
        {
        public:
            virtual void VisitObject(Object* element)
            {
                // called for each element object we serialize to the file
            }

            virtual void VisitField(Object* element, const Field* field)
            {
                // called for each field we serialize to the file (pointer or data...)
            }
        };
        typedef Helium::SmartPtr<ArchiveVisitor> ArchiveVisitorPtr;
        typedef std::vector<ArchiveVisitorPtr> V_ArchiveVisitor;

        namespace FileOperations
        {
            enum FileOperation
            {
                PreRead,
                PostRead,
                PreWrite,
                PostWrite,
            };
        }
        typedef FileOperations::FileOperation FileOperation;

        //
        // Archive Base Class
        //

        namespace ArchiveFlags
        {
            enum ArchiveFlag
            {
                Status  = 1 << 0, // Display status reporting
                Sparse  = 1 << 1, // Allow sparse array populations for failed objects
            };
        }

        class FOUNDATION_API Archive : public Helium::RefCountBase< Archive >
        {
            friend class RefCountBase< Archive >;

        protected:

            // The number of bytes Parsed so far
            unsigned m_Progress;

            // The file we are working with
            Path m_Path;

            // The byte order
            ByteOrder m_ByteOrder;

            // The array of elements that we've found
            std::vector< ObjectPtr > m_Objects;

            // The mode
            ArchiveMode m_Mode;

            // The cache of data objects
            Cache m_Cache;

            // The visitors to use
            V_ArchiveVisitor m_Visitors;

            // The type to serach for
            const Class* m_SearchClass;

            // The abort status
            bool m_Abort;

        protected:
            Archive( const Path& path, ByteOrder byteOrder = Helium::PlatformByteOrder );
            Archive();
            virtual ~Archive();

        public:
            // File access
            const Helium::Path& GetPath() const
            {
                return m_Path;
            }

            // Cache access
            Cache& GetCache()
            {
                return m_Cache;
            }

            ArchiveMode GetMode() const
            {
                return m_Mode;
            }

            // Get the type of this archive
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
        public:
            virtual void Serialize( const ObjectPtr& element ) = 0;
            virtual void Serialize( const std::vector< ObjectPtr >& elements, uint32_t flags = 0 ) = 0;
            virtual void Deserialize( ObjectPtr& element ) = 0;
            virtual void Deserialize( std::vector< ObjectPtr >& elements, uint32_t flags = 0 ) = 0;

            //
            // Event API
            //
        public:
            StatusSignature::Event e_Status;
            ExceptionSignature::Delegate d_Exception;

            //
            // Serialization
            //
        public:

            // Archive-level processing (visitor setup and append generation)
            void PreSerialize();
            void PostSerialize( std::vector< ObjectPtr >& append );

            // Archive-level processing (visitor setup and append processing)
            void PreDeserialize();
            void PostDeserialize( std::vector< ObjectPtr >& append );

            // Instance-level processing (visit calls and type tracking)
            void PreSerialize( const ObjectPtr& element, const Field* field = NULL );
            void PostDeserialize( const ObjectPtr& element, const Field* field = NULL );

            // Shared code for doing per-element pre and post serialize work with exception handling
            bool TryObjectCallback( Object* element, ObjectCallback callback );

            //
            // Get elements from the file
            //

            void Put( const ObjectPtr& element );
            void Put( const std::vector< ObjectPtr >& elements );

            ObjectPtr Get( const Class* searchClass = NULL );
            void Get( std::vector< ObjectPtr >& elements );

            template <class T>
            Helium::StrongPtr<T> Get()
            {
                ObjectPtr found = Get( Reflect::GetClass<T>() );

                if (found.ReferencesObject())
                {
                    return ObjectCast<T>( found );
                }
                else
                {
                    return NULL;
                }
            }

            // Get all elements of the specified type in the archive ( not optimal if you need to get lots of different types at once )
            template< class T >
            void Get( std::vector< Helium::StrongPtr<T> >& elements )
            {
                std::vector< ObjectPtr > archiveObjects;
                Get( archiveObjects );

                std::vector< ObjectPtr >::iterator itor = archiveObjects.begin();
                std::vector< ObjectPtr >::iterator end = archiveObjects.end();

                for( ; itor != end; ++itor )
                {
                    if( (*itor)->HasType( Reflect::GetType<T>() ) ) 
                    {
                        elements.push_back( Reflect::DangerousCast< T >( *itor )  );
                    }
                }
            }
        };

        typedef Helium::SmartPtr< Archive > ArchivePtr;

        // Get parser for a file
        FOUNDATION_API ArchivePtr GetArchive( const Path& path, ArchiveType archiveType = ArchiveTypes::Auto, ByteOrder byteOrder = Helium::PlatformByteOrder );

        FOUNDATION_API bool ToArchive( const Path& path, ObjectPtr element, ArchiveType archiveType = ArchiveTypes::Auto, tstring* error = NULL, ByteOrder byteOrder = Helium::PlatformByteOrder );
        FOUNDATION_API bool ToArchive( const Path& path, const std::vector< ObjectPtr >& elements, ArchiveType archiveType = ArchiveTypes::Auto, tstring* error = NULL, ByteOrder byteOrder = Helium::PlatformByteOrder );

        template <class T>
        Helium::StrongPtr<T> FromArchive( const Path& path, ArchiveType archiveType = ArchiveTypes::Auto, ByteOrder byteOrder = Helium::PlatformByteOrder )
        {
            ArchivePtr archive = GetArchive( path, archiveType, byteOrder );
            return archive->Get< T >();
        }

        template< class T >
        void FromArchive( const Path& path, std::vector< Helium::StrongPtr<T> >& elements, ArchiveType archiveType = ArchiveTypes::Auto, ByteOrder byteOrder = Helium::PlatformByteOrder )
        {
            ArchivePtr archive = GetArchive( path, archiveType, byteOrder );
            archive->Get< T >( elements );
        }
    }
}