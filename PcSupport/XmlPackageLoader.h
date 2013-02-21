////----------------------------------------------------------------------------------------------------------------------
//// XmlPackageLoader.h
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
//#pragma once
//#ifndef HELIUM_PC_SUPPORT_XML_PACKAGE_LOADER_H
//#define HELIUM_PC_SUPPORT_XML_PACKAGE_LOADER_H
//
//#include "PcSupport/PcSupport.h"
//#include "Engine/PackageLoader.h"
//#include "Engine/Serializer.h"
//
//#include "Foundation/FilePath.h"
//
///// XML package file extension string.
//#define HELIUM_XML_PACKAGE_FILE_EXTENSION TXT( ".xml" )
///// Directory-based XML package file name string.
//#define HELIUM_XML_PACKAGE_FILE_NAME TXT( "!package" ) HELIUM_XML_PACKAGE_FILE_EXTENSION
//
//namespace Helium
//{
//    class HELIUM_PC_SUPPORT_API XmlPackageLoader : public PackageLoader
//    {
//        struct LinkEntry;
//
//    public:
//        /// Load request pool block size.
//        static const size_t LOAD_REQUEST_POOL_BLOCK_SIZE = 4;
//
//        /// Maximum number of bytes to parse at a time.
//        static const size_t PARSE_CHUNK_SIZE = 4 * 1024;
//
//        /// Serialized object data.
//        struct SerializedObjectData
//        {
//            /// Asset path.
//            AssetPath objectPath;
//
//            /// Type name.
//            Name typeName;
//            /// Template path.
//            AssetPath templatePath;
//
//            /// Serialized properties.
//            ConcurrentHashMap< String, String > properties;
//            /// Cached array sizes.
//            ConcurrentHashMap< String, uint32_t > arraySizes;
//        };
//
//        /// Asset deserializer.
//        class Deserializer : public Serializer
//        {
//        public:
//            /// @name Construction/Destruction
//            //@{
//            Deserializer();
//            virtual ~Deserializer();
//            //@}
//
//            /// @name Serialization Control
//            //@{
//            void Prepare( SerializedObjectData* pObjectData, DynamicArray< LinkEntry >* pLinkTable );
//            //@}
//
//            /// @name Serialization Interface
//            //@{
//            virtual bool Serialize( Asset* pObject );
//            virtual EMode GetMode() const;
//
//            virtual void SerializeTag( const Tag& rTag );
//            virtual bool CanResolveTags() const;
//
//            virtual void GetPropertyTagNames( DynamicArray< String >& rTagNames ) const;
//
//            virtual void SerializeBool( bool& rValue );
//            virtual void SerializeInt8( int8_t& rValue );
//            virtual void SerializeUint8( uint8_t& rValue );
//            virtual void SerializeInt16( int16_t& rValue );
//            virtual void SerializeUint16( uint16_t& rValue );
//            virtual void SerializeInt32( int32_t& rValue );
//            virtual void SerializeUint32( uint32_t& rValue );
//            virtual void SerializeInt64( int64_t& rValue );
//            virtual void SerializeUint64( uint64_t& rValue );
//            virtual void SerializeFloat32( float32_t& rValue );
//            virtual void SerializeFloat64( float64_t& rValue );
//            virtual void SerializeBuffer( void* pBuffer, size_t elementSize, size_t count );
//            virtual void SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* ppNames );
//            virtual void SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* pEnumeration );
//            virtual void SerializeName( CharName& rValue );
//            virtual void SerializeString( String& rValue );
//            virtual void SerializeObjectReference( const AssetType* pType, AssetPtr& rspObject );
//
//            virtual void BeginStruct( EStructTag tag );
//            virtual void EndStruct();
//
//            virtual void BeginArray( uint32_t size );
//            virtual void EndArray();
//
//            virtual void BeginDynamicArray();
//            virtual void EndDynamicArray();
//            //@}
//
//        private:
//            /// Default "scanf" value parser.
//            template< typename T >
//            class ScanfParser
//            {
//            public:
//                /// @name Construction/Destruction
//                //@{
//                ScanfParser( const tchar_t* pFormat );
//                //@}
//
//                /// @name Overloaded Operators
//                //@{
//                bool operator()( const String& rText, T& rValue ) const;
//                //@}
//
//            private:
//                /// Format string.
//                const tchar_t* m_pFormat;
//            };
//
//            /// "scanf" value parser for floating-point types.
//            template< typename T >
//            class ScanfFloatParser
//            {
//            public:
//                /// @name Construction/Destruction
//                //@{
//                ScanfFloatParser( const tchar_t* pDecFormat );
//                //@}
//
//                /// @name Overloaded Operators
//                //@{
//                bool operator()( const String& rText, T& rValue ) const;
//                //@}
//
//            private:
//                /// Decimal format string.
//                const tchar_t* m_pDecFormat;
//
//                /// @name Hexidecimal Parsing Support
//                //@{
//                static bool ParseHex( const String& rText, T& rValue );
//                //@}
//            };
//
//            /// Enumeration value parser.
//            class EnumParser
//            {
//            public:
//                /// @name Construction/Destruction
//                //@{
//                EnumParser( uint32_t nameCount, const tchar_t* const* ppNames );
//                //@}
//
//                /// @name Overloaded Operators
//                //@{
//                bool operator()( const String& rText, int32_t& rValue ) const;
//                //@}
//
//            private:
//                /// Number of enumeration value names.
//                uint32_t m_nameCount;
//                /// Array of enumeration name strings.
//                const tchar_t* const* m_ppNames;
//            };
//
//            /// Single-byte character string value parser.
//            class CharStringParser
//            {
//            public:
//                /// @name Overloaded Operators
//                //@{
//                bool operator()( const String& rText, String& rValue ) const;
//                //@}
//            };
//
//            /// Wide-character string value parser.
//            class WideStringParser
//            {
//            public:
//                /// @name Overloaded Operators
//                //@{
//                bool operator()( const String& rText, WideString& rValue ) const;
//                //@}
//            };
//
//            /// Single-byte character name value parser.
//            class CharNameParser
//            {
//            public:
//                /// @name Overloaded Operators
//                //@{
//                bool operator()( const String& rText, CharName& rValue ) const;
//                //@}
//
//#if HELIUM_WCHAR_T
//            private:
//                /// String parser.
//                CharStringParser m_stringParser;
//#endif
//            };
//
//            /// Asset reference parser.
//            class ObjectParser
//            {
//            public:
//                /// @name Construction/Destruction
//                //@{
//                ObjectParser( DynamicArray< LinkEntry >* pLinkTable );
//                //@}
//
//                /// @name Overloaded Operators
//                //@{
//                bool operator()( const String& rText, AssetPtr& rspValue ) const;
//                //@}
//
//            private:
//                /// Asset link table.
//                DynamicArray< LinkEntry >* m_pLinkTable;
//            };
//
//            /// Null handler for properties not found in the XML property map.
//            template< typename T >
//            class NullDefaultHandler
//            {
//            public:
//                /// @name Overloaded Operators
//                //@{
//                void operator()( T& rValue ) const;
//                //@}
//            };
//
//            /// Handler for when an object reference property is not found in the XML property map.
//            class ObjectDefaultHandler
//            {
//            public:
//                /// @name Construction/Destruction
//                //@{
//                ObjectDefaultHandler( DynamicArray< LinkEntry >* pLinkTable );
//                //@}
//
//                /// @name Overloaded Operators
//                //@{
//                void operator()( AssetPtr& rspValue ) const;
//                //@}
//
//            private:
//                /// Asset link table.
//                DynamicArray< LinkEntry >* m_pLinkTable;
//            };
//
//            /// Property tag stack entry.
//            struct TagStackElement
//            {
//                /// Tag value.
//                Tag tag;
//                /// Index (for array properties; invalid if not deserializing an array, invalid - 1 if waiting for a
//                /// dynamic array size).
//                uint32_t index;
//
//                /// @name Construction/Destruction
//                //@{
//                TagStackElement();
//                //@}
//            };
//
//            /// Package loader instance.
//            XmlPackageLoader* m_pPackageLoader;
//            /// Data for the object being deserialized.
//            SerializedObjectData* m_pObjectData;
//            /// Link table for the object being deserialized.
//            DynamicArray< LinkEntry >* m_pLinkTable;
//
//            /// Property tag stack.
//            DynamicArray< TagStackElement > m_tagStack;
//
//            /// @name Private Utility Functions
//            //@{
//            template< typename T, typename Parser, typename DefaultHandler > void ReadValue(
//                T& rValue, const tchar_t* pTypeString, const Parser& rParser, const DefaultHandler& rDefaultHandler );
//
//            bool BuildCurrentPropertyName( String& rPropertyName ) const;
//
//            void ReadUtf8String( String& rString );
//            void ReadUtf8String( WideString& rString );
//            //@}
//        };
//
//        /// @name Construction/Destruction
//        //@{
//        XmlPackageLoader();
//        virtual ~XmlPackageLoader();
//        //@}
//
//        /// @name Initialization
//        //@{
//        bool Initialize( AssetPath packagePath );
//        void Shutdown();
//        //@}
//
//        /// @name Loading
//        //@{
//        bool BeginPreload();
//        virtual bool TryFinishPreload();
//
//        virtual size_t BeginLoadObject( AssetPath path );
//        virtual bool TryFinishLoadObject(
//            size_t requestId, AssetPtr& rspObject, DynamicArray< AssetLoader::LinkEntry >& rLinkTable );
//
//        virtual void Tick();
//        //@}
//
//        /// @name Data Access
//        //@{
//        virtual size_t GetObjectCount() const;
//        virtual AssetPath GetObjectPath( size_t index ) const;
//
//        Package* GetPackage() const;
//        AssetPath GetPackagePath() const;
//
//        inline const FilePath& GetPackageFileSystemPath() const;
//        //@}
//
//        /// @name Package File Information
//        //@{
//        virtual bool IsSourcePackageFile() const;
//        virtual int64_t GetFileTimestamp() const;
//        //@}
//
//    private:
//        /// Load request flags.
//        enum ELoadFlag
//        {
//            /// Set once property preloading has completed.
//            LOAD_FLAG_PROPERTY_PRELOADED            = 1 << 0,
//            /// Set once persistent resource data loading has completed.
//            LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED = 1 << 1,
//
//            /// Set once all preloading has completed.
//            LOAD_FLAG_PRELOADED = LOAD_FLAG_PROPERTY_PRELOADED | LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED,
//
//            /// Set when an error has occurred in the load process.
//            LOAD_FLAG_ERROR = 1 << 2
//        };
//
//        /// Link table entry.
//        struct LinkEntry
//        {
//            /// Asset path.
//            AssetPath path;
//            /// Load request ID.
//            size_t loadRequestId;
//        };
//
//        /// Asset load request data.
//        struct LoadRequest
//        {
//            /// Temporary object reference (hold while loading is in progress).
//            AssetPtr spObject;
//            /// Asset index.
//            size_t index;
//
//            /// Link table.
//            DynamicArray< LinkEntry > linkTable;
//
//            /// Cached type reference.
//            AssetTypePtr spType;
//            /// Cached template reference.
//            AssetPtr spTemplate;
//            /// Cached owner reference.
//            AssetPtr spOwner;
//            /// Template object load request ID.
//            size_t templateLoadId;
//            /// Owner object load request ID.
//            size_t ownerLoadId;
//
//            /// Async load ID for persistent resource data.
//            size_t persistentResourceDataLoadId;
//            /// Buffer for loading cached object data (for pre-loading the persistent resource data).
//            uint8_t* pCachedObjectDataBuffer;
//            /// Size of the cached object data buffer.
//            uint32_t cachedObjectDataBufferSize;
//
//            /// Load flags.
//            uint32_t flags;
//        };
//
//        /// Package reference.
//        PackagePtr m_spPackage;
//        /// Package path.
//        AssetPath m_packagePath;
//
//        /// Non-zero if the preload process has started.
//        volatile int32_t m_startPreloadCounter;
//        /// Non-zero if the package has been preloaded.
//        volatile int32_t m_preloadedCounter;
//
//        /// Serialized object data parsed from the XML package.
//        DynamicArray< SerializedObjectData > m_objects;
//
//        /// Pending load requests.
//        SparseArray< LoadRequest* > m_loadRequests;
//        /// Load request pool.
//        ObjectPool< LoadRequest > m_loadRequestPool;
//
//        /// Package file path name.
//        FilePath m_packageFilePath;
//        /// Size of the package data file.
//        size_t m_packageFileSize;
//
//        /// Destination buffer for async loading.
//        void* m_pLoadBuffer;
//        /// Async loading ID.
//        size_t m_asyncLoadId;
//
//        /// Parent package load request ID.
//        size_t m_parentPackageLoadId;
//
//        /// Mutex for synchronizing access between threads.
//        Mutex m_accessLock;
//
//        /// @name Private Utility Functions
//        //@{
//        void TickPreload();
//
//        void TickLoadRequests();
//        bool TickDeserialize( LoadRequest* pRequest );
//        bool TickPersistentResourcePreload( LoadRequest* pRequest );
//        //@}
//    };
//}
//
//#include "PcSupport/XmlPackageLoader.inl"
//
//#endif  // HELIUM_PC_SUPPORT_XML_PACKAGE_LOADER_H
