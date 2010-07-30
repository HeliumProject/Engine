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

        class FOUNDATION_API StatusInfo
        {
        public:
            const Archive& m_Archive;

            int m_Action;
            int m_Progress;
            tstring m_DestinationFile;

            bool m_Abort;

            StatusInfo( const Archive& archive, ArchiveState action )
                : m_Archive ( archive )
                , m_Action ( action )
                , m_Progress ( 0 )
                , m_Abort ( false )
            {

            }
        };

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

        class FOUNDATION_API ExceptionInfo
        {
        public:
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

        class FOUNDATION_API StatusHandler
        {
        public:
            virtual void ArchiveStatus( StatusInfo& info );
            virtual void ArchiveException( ExceptionInfo& info );
            virtual void ArchiveWarning( const tstring& warning );
            virtual void ArchiveDebug( const tstring& debug );
        };

        class FOUNDATION_API PrintStatus : public StatusHandler
        {
        public:
            bool m_Start;
            u64  m_Timer;
            int  m_Progress;
            std::auto_ptr<Log::Bullet> m_Bullet;

            PrintStatus()
                : m_Start (true)
                , m_Timer (0)
                , m_Progress (0)
                , m_Bullet (NULL)
            {

            }

            virtual void ArchiveStatus(StatusInfo& info) HELIUM_OVERRIDE;
            virtual void ArchiveWarning(const tstring& warning) HELIUM_OVERRIDE;
            virtual void ArchiveDebug(const tstring& debug) HELIUM_OVERRIDE;
        };


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
            TXT( "nrb" ),    // Binary
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

        struct FileAccessArgs
        {
            const tstring& m_File;
            FileOperation m_Operation;

            FileAccessArgs( const tstring& file, FileOperation operation )
                : m_File (file)
                , m_Operation (operation)
            {

            }
        };
        typedef Helium::Signature<void, const FileAccessArgs&, Helium::AtomicRefCountBase> FileAccessSignature;

        struct SerializeArgs
        {
            V_ArchiveVisitor& m_Visitors;

            SerializeArgs(V_ArchiveVisitor& visitors)
                : m_Visitors (visitors)
            {

            }
        };
        typedef Helium::Signature<void, SerializeArgs&, Helium::AtomicRefCountBase> SerializeSignature;

        struct DeserializeArgs
        {
            V_ArchiveVisitor& m_Visitors;

            DeserializeArgs(V_ArchiveVisitor& visitors)
                : m_Visitors (visitors)
            {

            }
        };
        typedef Helium::Signature<void, DeserializeArgs&, Helium::AtomicRefCountBase> DeserializeSignature;


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

            // The target function pointer for static function processing
            StatusHandler* m_Status;

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
            Archive (StatusHandler* status = NULL);

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
            static bool GetFileType( const tstring& file, ArchiveType& type );

            // Get the type of this archive
            virtual ArchiveType GetType() const = 0;

            // File Open/Close
            virtual void OpenFile(const tstring& file, bool write = false) = 0;
            virtual void Close() = 0; 

            // Begins parsing the InputStream
            virtual void Read() = 0;

            // Write to the OutputStream
            virtual void Write() = 0;

            // Write the file header
            virtual void Start() = 0;

            // Write the file footer
            virtual void Finish() = 0;

            // Emit a warning
            void Warning(const tchar* fmt, ...);

            // Emit an debug
            void Debug(const tchar* fmt, ...);


            //
            // Stream management
            //

            // Opens a file
        protected:
            // Get parser for a file
            static Archive* GetArchive(ArchiveType type, StatusHandler* handler = NULL);
            static Archive* GetArchive(const tstring& file, StatusHandler* handler = NULL);

            //
            // Serialization
            //
        public:
            virtual void Serialize(const ElementPtr& element) = 0;
            virtual void Serialize(const V_Element& elements, u32 flags = 0) = 0;
            virtual void Deserialize(ElementPtr& element) = 0;
            virtual void Deserialize(V_Element& elements, u32 flags = 0) = 0;

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

            //
            // Event API
            //

        private:
            static FileAccessSignature::Event s_FileAccess;
        public:
            static void AddFileAccessListener(FileAccessSignature::Delegate& delegate)
            {
                s_FileAccess.Add( delegate );
            }
            static void RemoveFileAccessListener(FileAccessSignature::Delegate& delegate)
            {
                s_FileAccess.Remove( delegate );
            }

        private:
            static SerializeSignature::Event s_Serialize;
        public:
            static void AddSerializeListener(SerializeSignature::Delegate& delegate)
            {
                s_Serialize.Add( delegate );
            }
            static void RemoveSerializeListener(SerializeSignature::Delegate& delegate)
            {
                s_Serialize.Remove( delegate );
            }

        private:
            static DeserializeSignature::Event s_Deserialize;
        public:
            static void AddDeserializeListener(DeserializeSignature::Delegate& delegate)
            {
                s_Deserialize.Add( delegate );
            }
            static void RemoveDeserializeListener(DeserializeSignature::Delegate& delegate)
            {
                s_Deserialize.Remove( delegate );
            }

            // Archive-level processing (visitor setup and append generation)
            void PreSerialize();
            void PostSerialize(V_Element& append);

            // Archive-level processing (visitor setup and append processing)
            void PreDeserialize();
            void PostDeserialize(V_Element& append);

            // Instance-level processing (visit calls and type tracking)
            void PreSerialize(const ElementPtr& element, const Field* field = NULL);
            void PostDeserialize(const ElementPtr& element, const Field* field = NULL);

            // Shared code for doing per-element pre and post serialize work with exception handling
            bool TryElementCallback( Element* element, ElementCallback callback );


            //
            // Serialize/Deserialize API
            //

            // Reading and writing single element from a file
            static void       ToFile(const ElementPtr& element, const tstring& file);
            static void       ToFile(const ElementPtr& element, const tstring& file, VersionPtr version, StatusHandler* status = NULL);
            static ElementPtr FromFile(const tstring& file, int searchType = Reflect::ReservedTypes::Any, StatusHandler* status = NULL);

            // Reading and writing multiple elements from a file
            static void       ToFile(const V_Element& elements, const tstring& file);
            static void       ToFile(const V_Element& elements, const tstring& file, VersionPtr version, StatusHandler* status = NULL);
            static void       FromFile(const tstring& file, V_Element& elements, StatusHandler* status = NULL);

            // Get all elements of the specified type in the archive ( not optimal if you need to get lots of different types at once )
            template< class T >
            static void FromFile( const tstring& file, std::vector< Helium::SmartPtr<T> >& elements, StatusHandler* status = NULL )
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
            static Helium::SmartPtr<T> FromFile(const tstring& file, StatusHandler* status = NULL)
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