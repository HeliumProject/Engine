//----------------------------------------------------------------------------------------------------------------------
// XmlSerializer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_PC_SUPPORT_XML_SERIALIZER_H
#define LUNAR_PC_SUPPORT_XML_SERIALIZER_H

#include "PcSupport/XmlSerializerBase.h"

#include "PcSupport/XmlTemplateSerializer.h"

namespace Helium
{
    class FileStream;
    class BufferedStream;
}

namespace Lunar
{
    /// XML file object serializer.
    class LUNAR_PC_SUPPORT_API XmlSerializer : public XmlSerializerBase
    {
    public:
        /// @name Construction/Destruction
        //@{
        XmlSerializer();
        virtual ~XmlSerializer();
        //@}

        /// @name Initialization
        //@{
        bool Initialize( const tchar_t* pFileName );
        void Shutdown();
        //@}

    protected:
        /// @name Serialization Implementation Support
        //@{
        virtual void PreSerialize( GameObject* pObject );
        virtual void PostSerialize( GameObject* pObject );

        virtual void PushPropertyData( Name name, const void* pData, size_t size );
        //@}

    private:
        /// Output file stream.
        FileStream* m_pFileStream;
        /// Buffered output stream.
        BufferedStream* m_pStream;

        /// Serialized template object data.
        XmlTemplateSerializer m_templateSerializer;
    };
}

#endif  // LUNAR_PC_SUPPORT_XML_SERIALIZER_H
