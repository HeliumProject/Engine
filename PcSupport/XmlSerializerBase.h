////----------------------------------------------------------------------------------------------------------------------
//// XmlSerializerBase.h
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
//#pragma once
//#ifndef HELIUM_PC_SUPPORT_XML_SERIALIZER_BASE_H
//#define HELIUM_PC_SUPPORT_XML_SERIALIZER_BASE_H
//
//#include "Foundation/MemoryStream.h"
//#include "Engine/Serializer.h"
//#include "PcSupport/PcSupport.h"
//
//// Output newline character.
//#if HELIUM_OS_WIN
//#define HELIUM_XML_NEWLINE "\r\n"
//#else
//#define HELIUM_XML_NEWLINE "\n"
//#endif
//
//namespace Helium
//{
//    /// Base support for XML serialization routines.
//    class HELIUM_PC_SUPPORT_API XmlSerializerBase : public Serializer
//    {
//    public:
//        /// @name Construction/Destruction
//        //@{
//        XmlSerializerBase();
//        virtual ~XmlSerializerBase() = 0;
//        //@}
//
//        /// @name Serialization Interface
//        //@{
//        virtual bool Serialize( Asset* pObject );
//        virtual EMode GetMode() const;
//
//        virtual void SerializeTag( const Tag& rTag );
//        virtual bool CanResolveTags() const;
//
//        virtual void SerializeBool( bool& rValue );
//        virtual void SerializeInt8( int8_t& rValue );
//        virtual void SerializeUint8( uint8_t& rValue );
//        virtual void SerializeInt16( int16_t& rValue );
//        virtual void SerializeUint16( uint16_t& rValue );
//        virtual void SerializeInt32( int32_t& rValue );
//        virtual void SerializeUint32( uint32_t& rValue );
//        virtual void SerializeInt64( int64_t& rValue );
//        virtual void SerializeUint64( uint64_t& rValue );
//        virtual void SerializeFloat32( float32_t& rValue );
//        virtual void SerializeFloat64( float64_t& rValue );
//        virtual void SerializeBuffer( void* pBuffer, size_t elementSize, size_t count );
//        virtual void SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* ppNames );
//        virtual void SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* pEnumeration );
//        virtual void SerializeName( CharName& rValue );
//        virtual void SerializeString( String& rValue );
//        virtual void SerializeObjectReference( const AssetType* pType, AssetPtr& rspObject );
//
//        virtual void BeginStruct( EStructTag tag );
//        virtual void EndStruct();
//
//        virtual void BeginArray( uint32_t size );
//        virtual void EndArray();
//
//        virtual void BeginDynamicArray();
//        virtual void EndDynamicArray();
//        //@}
//
//    protected:
//        /// @name Serialization Implementation Support
//        //@{
//        virtual void PreSerialize( Asset* pObject ) = 0;
//        virtual void PostSerialize( Asset* pObject ) = 0;
//
//        virtual void PushPropertyData( Name name, const void* pData, size_t size ) = 0;
//        //@}
//
//        /// @name UTF-8 String Writing Support
//        //@{
//        static void WriteStringAsUtf8(
//            Stream* pStream, const char* pString, size_t length, bool bCharacterData = false );
//        static void WriteStringAsUtf8(
//            Stream* pStream, const wchar_t* pString, size_t length, bool bCharacterData = false );
//        //@}
//
//    private:
//        /// Output level identifiers.
//        enum EOutputLevel
//        {
//            OUTPUT_LEVEL_FIRST   =  0,
//            OUTPUT_LEVEL_INVALID = -1,
//
//            /// Property tag.
//            OUTPUT_LEVEL_PROPERTY,
//            /// Array tag.
//            OUTPUT_LEVEL_ARRAY,
//            /// Struct tag.
//            OUTPUT_LEVEL_STRUCT,
//
//            OUTPUT_LEVEL_MAX,
//            OUTPUT_LEVEL_LAST = OUTPUT_LEVEL_MAX - 1
//        };
//
//        /// Default "printf" value writer.
//        template< typename T >
//        class PrintfFormatter
//        {
//        public:
//            /// @name Construction/Destruction
//            //@{
//            PrintfFormatter( const tchar_t* pFormat );
//            //@}
//
//            /// @name Overloaded Operators
//            //@{
//            void operator()( XmlSerializerBase& rSerializer, const T& rValue ) const;
//            //@}
//
//        private:
//            /// Format string.
//            const tchar_t* m_pFormat;
//        };
//
//        /// "printf" value writer for floating-point types.
//        template< typename T >
//        class PrintfFloatFormatter
//        {
//        public:
//            /// @name Construction/Destruction
//            //@{
//            PrintfFloatFormatter( const tchar_t* pFormat );
//            //@}
//
//            /// @name Overloaded Operators
//            //@{
//            void operator()( XmlSerializerBase& rSerializer, const T& rValue ) const;
//            //@}
//
//        private:
//            /// Format string (value is written out twice).
//            const tchar_t* m_pFormat;
//        };
//
//        /// Enumeration value writer.
//        class EnumFormatter
//        {
//        public:
//            /// @name Construction/Destruction
//            //@{
//            EnumFormatter( uint32_t nameCount, const tchar_t* const* ppNames );
//            //@}
//
//            /// @name Overloaded Operators
//            //@{
//            void operator()( XmlSerializerBase& rSerializer, int32_t value ) const;
//            //@}
//
//        private:
//            /// Number of enumeration value names.
//            uint32_t m_nameCount;
//            /// Array of enumeration name strings.
//            const tchar_t* const* m_ppNames;
//        };
//
//        /// Enumeration value writer.
//        class ReflectEnumFormatter
//        {
//        public:
//            /// @name Construction/Destruction
//            //@{
//            ReflectEnumFormatter( const Helium::Reflect::Enumeration* pEnumeration );
//            //@}
//
//            /// @name Overloaded Operators
//            //@{
//            void operator()( XmlSerializerBase& rSerializer, int32_t value ) const;
//            //@}
//
//        private:
//            /// Enumeration reflection information
//            const Helium::Reflect::Enumeration* m_enumeration;
//        };
//
//        /// Name value writer.
//        template< typename TableType >
//        class NameFormatter
//        {
//        public:
//            /// @name Overloaded Operators
//            //@{
//            void operator()( XmlSerializerBase& rSerializer, const NameBase< TableType >& rValue ) const;
//            //@}
//        };
//
//        /// String value formatter.
//        template< typename CharType >
//        class StringFormatter
//        {
//        public:
//            /// @name Overloaded Operators
//            //@{
//            void operator()(
//                XmlSerializerBase& rSerializer, const StringBase< CharType, DefaultAllocator >& rValue ) const;
//            //@}
//        };
//
//        /// Asset reference formatter.
//        class ObjectFormatter
//        {
//        public:
//            /// @name Overloaded Operators
//            //@{
//            void operator()( XmlSerializerBase& rSerializer, Asset* pValue ) const;
//            //@}
//        };
//
//        /// Stream for the serialized data associated with the property currently being serialized.
//        DynamicMemoryStream m_propertyStream;
//        /// Buffer used by the property stream.
//        DynamicArray< uint8_t > m_propertyStreamBuffer;
//
//        /// Tag for the top-most property being serialized.
//        Tag m_propertyTag;
//
//        /// Current property tag (set to null after the corresponding value has been read).
//        Tag m_currentTag;
//        /// True if tabs should be written out when closing the top-most property tag.
//        bool m_bClosePropertyWithTabs;
//
//        /// True to ignore the next value (for ignoring the size value for a dynamic array).
//        bool m_bIgnoreNextValue;
//
//        /// Current output level stack.
//        DynamicArray< EOutputLevel > m_outputLevelStack;
//
//        /// @name Private Utility Functions
//        //@{
//        bool ConditionalOpenPropertyTag();
//        void ConditionalClosePropertyTag();
//
//        template< typename T, typename Formatter > void WriteValue(
//            const T& rValue, const tchar_t* pTypeString, const Formatter& rFormatter );
//        //@}
//
//        /// @name Static Private Utility Functions
//        //@{
//        static void WriteTabs( Stream* pStream, size_t count = 1 );
//        //@}
//    };
//}
//
//#endif  // HELIUM_PC_SUPPORT_XML_SERIALIZER_BASE_H
