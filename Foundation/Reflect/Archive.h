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
#include "Stream.h" 

#include "Platform/Assert.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Log.h" 
#include "Foundation/File/Path.h"

#include "Foundation/Atomic.h"

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

        namespace StatusTypes
        {
            enum StatusType
            {
                General,
                Debug,
                Warning,
            };
        }
        typedef StatusTypes::StatusType StatusType;

        struct StatusInfo
        {
            StatusType m_Type;
            const Archive& m_Archive;
            ArchiveState m_ArchiveState;
            int m_Progress;
            bool m_Abort;
            tstring m_Info;

            StatusInfo( const Archive& archive, const ArchiveState& state, const StatusType& type = StatusTypes::General )
                : m_Archive( archive )
                , m_ArchiveState( state )
                , m_Type( type )
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

            ExceptionAction m_Action;

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
                Binary,
                XML,
            };
        }
        typedef ArchiveTypes::ArchiveType ArchiveType;

        // must line up with enum above
        const static tchar* s_ArchiveExtensions[] =
        {
            TXT( "hrb" ),   // Binary
            TXT( "xml" )    // XML
        };

        // must line up with archive type enum
        const static tchar* s_ArchiveDescriptions[] =
        {
            TXT( "Binary Reflect File" ),
            TXT( "XML Reflect File" )
        };

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

        class FOUNDATION_API ArchiveVisitor : public Helium::AtomicRefCountBase
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

            virtual void CreateAppendElements(V_Element& append)
            {
                // Called after the main spool is serialized and is a call to the visitor for meta data
            }

            virtual void ProcessAppendElements(V_Element& append)
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

        class FOUNDATION_API Archive
        {
        protected:
            // The number of bytes Parsed so far
            unsigned m_Progress;

            // The file we are working with
            Helium::Path m_Path;

            // The array of elements that we've found
            V_Element m_Spool;

            // The mode
            ArchiveMode m_Mode;

            // The cache of serializers
            Cache m_Cache;

            // The types used
            std::set< i32 > m_Types;

            // The visitors to use
            V_ArchiveVisitor m_Visitors;

            // The type to serach for
            i32 m_SearchType;

            // The abort status
            bool m_Abort;

        protected:
            Archive();

        public:
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

            // Peek the type of file
            static bool GetFileType( const Path& path, ArchiveType& type );

            // Get the type of this archive
            virtual ArchiveType GetType() const = 0;

            // File Open/Close
            virtual void OpenFile( const Path& path, bool write = false ) = 0;
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
            // Stream management
            //

            // Opens a file
        protected:
            // Get parser for a file
            static Archive* GetArchive( ArchiveType type );
            static Archive* GetArchive( const Path& path );

            //
            // Serialization
            //
        public:
            virtual void Serialize( const ElementPtr& element ) = 0;
            virtual void Serialize( const V_Element& elements, u32 flags = 0 ) = 0;
            virtual void Deserialize( ElementPtr& element ) = 0;
            virtual void Deserialize( V_Element& elements, u32 flags = 0 ) = 0;

        public:
            static const tchar* GetExtension( ArchiveType t )
            {
                HELIUM_ASSERT( t < sizeof( s_ArchiveExtensions ) / sizeof( tchar* ) );
                return s_ArchiveExtensions[ t ];
            }

            static void GetExtensions( std::set< tstring >& extensions )
            {
                for ( int i = 0; i < sizeof( s_ArchiveExtensions ) / sizeof( tchar* ); ++i )
                {
                    extensions.insert( s_ArchiveExtensions[ i ] );
                }
            }

            static void GetFileFilters( std::set< tstring > filters )
            {
                for ( int i = 0; i < sizeof( s_ArchiveExtensions ) / sizeof( tchar* ); ++i )
                {
                    tstring filter = tstring( s_ArchiveDescriptions[ i ] ) + TXT( " (*." ) + s_ArchiveExtensions[ i ] + TXT( ")|*." ) + s_ArchiveExtensions[ i ];
                    filters.insert( filter );
                }
            }

            static void GetFileFilters( tstring& filters )
            {
                filters.clear();
                for ( int i = 0; i < sizeof( s_ArchiveExtensions ) / sizeof( tchar* ); ++i )
                {
                    if ( i != 0 )
                    {
                        filters.push_back( TXT('|') );
                    }

                    filters += tstring( s_ArchiveDescriptions[ i ] ) + TXT( " (*." ) + s_ArchiveExtensions[ i ] + TXT( ")|*." ) + s_ArchiveExtensions[ i ];
                }
            }


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
            void PostSerialize( V_Element& append );

            // Archive-level processing (visitor setup and append processing)
            void PreDeserialize();
            void PostDeserialize(V_Element& append);

            // Instance-level processing (visit calls and type tracking)
            void PreSerialize( const ElementPtr& element, const Field* field = NULL );
            void PostDeserialize( const ElementPtr& element, const Field* field = NULL );

            // Shared code for doing per-element pre and post serialize work with exception handling
            bool TryElementCallback( Element* element, ElementCallback callback );


            //
            // Serialize/Deserialize API
            //

            // Reading and writing from/to a file
            void Save();
            ElementPtr FromFile( const tstring& file, int searchType = Reflect::ReservedTypes::Any );

            // Reading and writing multiple elements from a file
            static void       FromFile( const tstring& file, V_Element& elements );

            // Get all elements of the specified type in the archive ( not optimal if you need to get lots of different types at once )
            template< class T >
            static void FromFile( const tstring& file, std::vector< Helium::SmartPtr<T> >& elements )
            {
                V_Element archiveElements;
                FromFile( file, archiveElements, status );

                V_Element::iterator itor = archiveElements.begin();
                V_Element::iterator end = archiveElements.end();

                for( ; itor != end; ++itor )
                {
                    if( (*itor)->HasType( Reflect::GetType<T>() ) ) 
                    {


                        elements.push_back( Reflect::DangerousCast< T >( *itor )  );
                    }

                }
            }

            // Fancy template version
            template <class T>
            static Helium::SmartPtr<T> FromFile( const tstring& file )
            {
                ElementPtr found = FromFile(file, Reflect::GetType<T>(), status);

                if (found.ReferencesObject())
                {
                    return ObjectCast<T>( found );
                }
                else
                {
                    return NULL;
                }
            }
        };
    }
}