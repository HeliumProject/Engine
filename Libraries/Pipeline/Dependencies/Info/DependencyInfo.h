#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Dependencies/DependenciesExceptions.h"

#include <string>
#include <vector>
#include <set>

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"

typedef Nocturnal::OrderedSet< std::string > OS_string;

namespace CryptoPP
{
    class HashFilter;
}

namespace Dependencies
{ 

    class DependencyGraph;

    /////////////////////////////////////////////////////////////////////////////
    // cache output file path and the 

    class GraphInfo;

    // M_GraphInfo is a maping of an outfile path to a GraphInfo struct containing configs for this infile FileInfo
    typedef PIPELINE_API std::map< std::string, GraphInfo > M_GraphInfo;

    class PIPELINE_API GraphInfo
    {
    public:
        i64   m_LastModified;     // last time the given outfile was built, this was the last modified time of this infile
        bool  m_IsOptional;     // for the given outfile, this infile can be missing; check if status has changed via m_FileExistedLastBuild
        bool  m_ExistedLastBuild; // true if the infile existed the last time the given outfile was built

        GraphInfo( const i64 lastModified = 0, const bool isOptional = false, const bool existedLastBuild = true )
            : m_LastModified( lastModified )
            , m_IsOptional( isOptional )
            , m_ExistedLastBuild( existedLastBuild )
        {
        }
    };


    /////////////////////////////////////////////////////////////////////////////
    typedef PIPELINE_API u32 GraphConfigs;
    namespace ConfigFlags
    {
        enum ConfigFlag
        {
            LeafInput           = 1 << 0,      // true if this DependencyInfo represents a leaf input dependency
            InputOrderMatters   = 1 << 1,      // true if, for this outfile, the order that infiles are read matters (used for shaders in LevelBuild, etc?
        };

        static const GraphConfigs Default = 0;
    }

    /////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////

    class PIPELINE_API DependencyInfo : public Nocturnal::RefCountBase<DependencyInfo>
    {
    public:

        DependencyInfo();
        DependencyInfo( DependencyGraph* owner, const std::string& name, const std::string& typeName, const GraphConfigs graphConfigs = ConfigFlags::Default );

        virtual void Initialize();

        ~DependencyInfo();

        virtual void SetInfo(); 

        virtual bool IsMD5Valid() = 0;
        virtual void GenerateMD5() = 0;
        virtual bool WasModified() = 0;
        virtual bool AppendToSignature( CryptoPP::HashFilter* hashFilter, V_string& trace ) = 0;
        virtual void CacheCopy( DependencyInfo& rhs );
        virtual bool Exists() = 0;

        bool IsLeaf() const;
        void IsLeaf( const bool isLeaf );

        bool InputOrderMatters() const;
        void InputOrderMatters( const bool InputOrderMatters );

        virtual void CopyFrom( const DependencyInfo &rhs );
        virtual bool IsEqual( const DependencyInfo &rhs ) const;

        DependencyInfo& operator=( const DependencyInfo &rhs );
        bool      operator==( const DependencyInfo &rhs ) const;
        bool      operator!=( const DependencyInfo &rhs ) const;
        bool      operator<( const DependencyInfo &rhs ) const;

    public:

        DependencyGraph*          m_OwnerGraph;

        Nocturnal::Path           m_Path;
        std::string               m_MD5;
        std::string               m_Signature;
        V_string                  m_SignatureTrace;

        std::string               m_TypeName;
        std::string               m_FormatVersion;

        GraphConfigs              m_GraphConfigs;

        i64                       m_LastModified;
        i64                       m_Size;

        OS_string                 m_Dependencies;
        bool                      m_WasGraphSelected;

        bool                      m_IsUpToDate;
        bool                      m_IsUpToDateCached;    

        // used by the dependencies system to store the graph DB row ids
        i64                       m_RowID;
        i64                       m_VersionRowID;

        bool                      m_IsMD5Valid;

        M_GraphInfo               m_GraphInfo;

        bool                      m_Downloaded;

    };
    typedef Nocturnal::SmartPtr< DependencyInfo > DependencyInfoPtr;
    typedef std::vector< DependencyInfoPtr > V_DependencyInfo;
    typedef std::vector< V_DependencyInfo > VV_DependencyInfo;
    typedef Nocturnal::OrderedSet< DependencyInfoPtr > OS_DependencyInfo;
}