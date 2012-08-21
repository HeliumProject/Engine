////----------------------------------------------------------------------------------------------------------------------
//// XmlTemplateSerializer.h
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
//#pragma once
//#ifndef HELIUM_PC_SUPPORT_XML_TEMPLATE_SERIALIZER_H
//#define HELIUM_PC_SUPPORT_XML_TEMPLATE_SERIALIZER_H
//
//#include "PcSupport/XmlSerializerBase.h"
//
//namespace Helium
//{
//    /// Serializer for tracking of template object properties for comparison during general object serialization.
//    class HELIUM_PC_SUPPORT_API XmlTemplateSerializer : public XmlSerializerBase
//    {
//    public:
//        /// Property data.
//        class HELIUM_PC_SUPPORT_API PropertyData
//        {
//        public:
//            /// @name Construction/Destruction
//            //@{
//            PropertyData( Name name, const void* pSourceData, size_t size );
//            PropertyData( const PropertyData& rSource );
//            ~PropertyData();
//            //@}
//
//            /// @name Data Access
//            //@{
//            inline Name GetName() const;
//            inline const void* GetData() const;
//            inline size_t GetSize() const;
//            //@}
//
//            /// @name Overloaded Operators
//            //@{
//            PropertyData& operator=( const PropertyData& rSource );
//            //@}
//
//        private:
//            /// Property name.
//            Name m_name;
//            /// Binary data stream.
//            void* m_pData;
//            /// Size of the data stream, in bytes.
//            size_t m_size;
//        };
//
//        /// @name Construction/Destruction
//        //@{
//        XmlTemplateSerializer();
//        virtual ~XmlTemplateSerializer();
//        //@}
//
//        /// @name Property Access
//        //@{
//        inline size_t GetPropertyCount() const;
//        inline const PropertyData& GetPropertyData( size_t index ) const;
//
//        const PropertyData* FindProperty( Name propertyName ) const;
//
//        void ClearProperties();
//        //@}
//
//    protected:
//        /// @name Serialization Implementation Support
//        //@{
//        virtual void PreSerialize( GameObject* pObject );
//        virtual void PostSerialize( GameObject* pObject );
//
//        virtual void PushPropertyData( Name name, const void* pData, size_t size );
//        //@}
//
//    private:
//        /// Serialized properties.
//        DynArray< PropertyData > m_properties;
//    };
//}
//
//#include "PcSupport/XmlTemplateSerializer.inl"
//
//#endif  // HELIUM_PC_SUPPORT_XML_TEMPLATE_SERIALIZER_H
