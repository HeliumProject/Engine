#pragma once

#include "API.h"
#include "Exceptions.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/File/File.h"

#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"

#include "TUID/TUID.h"

namespace File
{ 
    class Reference;
    typedef Nocturnal::SmartPtr< Reference > ReferencePtr;
    typedef std::vector< ReferencePtr > V_Reference;
    typedef std::set< ReferencePtr > S_Reference; // not sure this will compare properly (Reference::m_Id vs. SmartPtr==)

    class FILE_API Reference : public Reflect::Element
    {
        friend class Resolver;

    private:
        void Set( const Reference& rhs );
        void Set( const std::string& path );

    public:

        Reference( tuid id );
        Reference( const std::string& path = "" );
        Reference( const Reference& rhs )
        {
            Set( rhs );
        }
        virtual ~Reference();

    private:
        static std::string s_GlobalBaseDirectory;    

    protected:

        // serialization helpers
        std::string       m_FullPathForSerialization;

        // reference info
        tuid              m_Id;
        u64               m_CreationTime;
        u64               m_ModifiedTime;

        // file info
        std::string       m_RelativePath;
        std::string       m_LastUsername;

        u64               m_LastFileModifiedTime;
        std::string       m_LastSignature;

    protected:

        Nocturnal::File   m_FileObject;

    public:
        REFLECT_DECLARE_CLASS( Reference, Reflect::Element );

        static void EnumerateClass( Reflect::Compositor< Reference >& comp );
        virtual void PostDeserialize() NOC_OVERRIDE;

    public:

        static void SetGlobalBaseDirectory( const std::string& directory = "" )
        {
            s_GlobalBaseDirectory = directory;
        }

    public:

        tuid GetId() const
        {
            return m_Id;
        }

        std::string GetPath() const
        {
            return m_FileObject.GetPath().Get();
        }
        std::string GetRelativePath() const
        {
            return m_RelativePath;
        }

        u64 GetHash() const;

        std::string GetLastUsername()
        {
            return m_LastUsername;
        }

        std::string AsString( bool verbose = false );

        bool operator<( const Reference& rhs ) const
        {
            return m_FileObject.GetPath() == rhs.m_FileObject.GetPath();
        }

        bool operator==( const Reference& rhs ) const
        {
            return ( m_Id == rhs.m_Id );
        }

        const Nocturnal::File& GetFile() const
        {
            return m_FileObject;
        }

    public:
        bool IsValid();
        void Resolve();
        void Commit();

        void Retarget( const std::string& newPath );

    };
}