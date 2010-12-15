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
#include "StringPool.h"
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
                ElementProcessed,
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

        typedef void (Element::*ElementCallback)();

        struct ExceptionInfo
        {
            const Archive& m_Archive;

            Element* m_Element;
            ElementCallback m_Callback;
            const Helium::Exception& m_Exception;

            mutable ExceptionAction m_Action;

            ExceptionInfo( const Archive& archive, Element* element, ElementCallback callback, const Helium::Exception& exception )
                : m_Archive ( archive )
                , m_Element ( element )
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
            virtual void VisitElement(Element* element)
            {
                // called for each element object we serialize to the file
            }

            virtual void VisitField(Element* element, const Field* field)
            {
                // called for each field we serialize to the file (pointer or data...)
            }

            virtual void CreateAppendElements(std::vector< ElementPtr >& append)
            {
                // Called after the main spool is serialized and is a call to the visitor for meta data
            }

            virtual void ProcessAppendElements(std::vector< ElementPtr >& append)
            {
                // Called after the append spool is deserialized and is a call to the visitor to process the meta data
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

            ByteOrder m_ByteOrder;

            // The array of elements that we've found
            std::vector< ElementPtr > m_Spool;

            // The mode
            ArchiveMode m_Mode;

            // The cache of serializers
            Cache m_Cache;

            // The classes used
            std::set< const Class* > m_Classes;

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

            // Write the file header
            virtual void Start() = 0;

            // Write the file footer
            virtual void Finish() = 0;

            //
            // Serialization
            //
        public:
            virtual void Serialize( const ElementPtr& element ) = 0;
            virtual void Serialize( const std::vector< ElementPtr >& elements, uint32_t flags = 0 ) = 0;
            virtual void Deserialize( ElementPtr& element ) = 0;
            virtual void Deserialize( std::vector< ElementPtr >& elements, uint32_t flags = 0 ) = 0;

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
            void PostSerialize( std::vector< ElementPtr >& append );

            // Archive-level processing (visitor setup and append processing)
            void PreDeserialize();
            void PostDeserialize( std::vector< ElementPtr >& append );

            // Instance-level processing (visit calls and type tracking)
            void PreSerialize( const ElementPtr& element, const Field* field = NULL );
            void PostDeserialize( const ElementPtr& element, const Field* field = NULL );

            // Shared code for doing per-element pre and post serialize work with exception handling
            bool TryElementCallback( Element* element, ElementCallback callback );

            //
            // Get elements from the file
            //

            void Put( const ElementPtr& element );
            void Put( const std::vector< ElementPtr >& elements );

            ElementPtr Get( const Class* searchClass = NULL );
            void Get( std::vector< ElementPtr >& elements );

            template <class T>
            Helium::StrongPtr<T> Get()
            {
                ElementPtr found = Get( Reflect::GetClass<T>() );

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
                std::vector< ElementPtr > archiveElements;
                Get( archiveElements );

                std::vector< ElementPtr >::iterator itor = archiveElements.begin();
                std::vector< ElementPtr >::iterator end = archiveElements.end();

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

        FOUNDATION_API bool ToArchive( const Path& path, ElementPtr element, ArchiveType archiveType = ArchiveTypes::Auto, tstring* error = NULL, ByteOrder byteOrder = Helium::PlatformByteOrder );
        FOUNDATION_API bool ToArchive( const Path& path, const std::vector< ElementPtr >& elements, ArchiveType archiveType = ArchiveTypes::Auto, tstring* error = NULL, ByteOrder byteOrder = Helium::PlatformByteOrder );

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