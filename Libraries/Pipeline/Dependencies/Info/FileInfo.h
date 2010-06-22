#pragma once

#include "DependencyInfo.h"
#include "Pipeline/API.h"
#include "Pipeline/Dependencies/DependenciesExceptions.h"

#include <string>
#include <vector>
#include <set>

#include "Foundation/Container/OrderedSet.h"
#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Dependencies
{ 
    class DependencyGraph;

    class PIPELINE_API FileInfo : public DependencyInfo
    {
    public:

        FileInfo();
        FileInfo( DependencyGraph* owner, const std::string& path, const std::string& typeName, const GraphConfigs graphConfigs = ConfigFlags::Default );

        ~FileInfo();

        virtual void SetInfo(); 

        virtual bool IsMD5Valid();
        virtual void GenerateMD5();
        virtual bool WasModified();
        virtual bool AppendToSignature( CryptoPP::HashFilter* hashFilter, std::vector< std::string >& trace );
        virtual void CacheCopy( DependencyInfo& rhs );
        virtual bool Exists();


        virtual void CopyFrom( const DependencyInfo &rhs );
        virtual bool IsEqual( const DependencyInfo &rhs ) const;


    public:

        std::string m_AlternateSignatureGenerationPath;

    };
    typedef Nocturnal::SmartPtr< FileInfo > FileInfoPtr;
    typedef Nocturnal::OrderedSet< FileInfoPtr > OS_FileInfo;
}