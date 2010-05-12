#pragma once

#include "DependencyInfo.h"
#include "API.h"
#include "Exceptions.h"

#include <string>
#include <vector>
#include <set>

#include "Finder/Finder.h"
#include "Common/Container/OrderedSet.h"
#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

typedef Nocturnal::OrderedSet< std::string > OS_string;

namespace Dependencies
{ 
  

  /////////////////////////////////////////////////////////////////////////////

  class DEPENDENCIES_API FileInfo : public DependencyInfo
  {
  public:

    FileInfo();
    FileInfo( const std::string &path, const Finder::FileSpec &fileSpec, const GraphConfigs graphConfigs = ConfigFlags::Default );
    
    ~FileInfo();

    virtual void SetInfo(); 
    
    virtual bool IsMD5Valid();
    virtual void GenerateMD5();
    virtual bool WasModified();
    virtual bool AppendToSignature( CryptoPP::HashFilter* hashFilter, V_string& trace );
    virtual void CacheCopy( DependencyInfo& rhs );
    virtual bool Exists();

   
    virtual void CopyFrom( const DependencyInfo &rhs );
    virtual bool IsEqual( const DependencyInfo &rhs ) const;

      
  public:
    //
    // Members
    //

    std::string m_AlternateSignatureGenerationPath;

  };
  typedef Nocturnal::SmartPtr< FileInfo > FileInfoPtr;
  //typedef std::vector< FileInfoPtr > V_FileInfo;
  //typedef V_DependencyInfo V_FileInfo;
  //typedef std::vector< V_FileInfo > VV_FileInfo;
  typedef Nocturnal::OrderedSet< FileInfoPtr > OS_FileInfo;
}