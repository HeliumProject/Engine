#pragma once

#include "API.h"
#include "Exceptions.h"
#include "BuilderOptions.h"

#include "Asset/AssetClass.h"
#include "Console/Console.h"
#include "Dependencies/Dependencies.h"

namespace AssetBuilder
{
  class IBuilder;
  typedef Nocturnal::SmartPtr< IBuilder > IBuilderPtr;

  namespace JobResults
  {
    enum JobResult
    {
      Unknown,
      Skip,
      Download,
      Clean,
      Dirty,
      Failure,
    };
    static void JobResultEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Unknown, "Unknown");
      info->AddElement(Skip, "Skip");
      info->AddElement(Download, "Download");
      info->AddElement(Clean, "Clean");
      info->AddElement(Dirty, "Dirty");
      info->AddElement(Failure, "Failure");
    }
  }
  typedef JobResults::JobResult JobResult;
  
  namespace JobFlags
  {
    enum JobFlag
    {
      Required =                    1<<0, // is this job required by the builder that allocated it?  ie, should its failure stop execution
      RequiredOnlyInTopLevelBuild = 1<<1, // same as required, but only fatal if the job that required it is the top-level build
    };
  }
  typedef JobFlags::JobFlag JobFlag;

  struct BuildJob;
  typedef Nocturnal::SmartPtr< BuildJob > BuildJobPtr;
  typedef std::vector< BuildJobPtr > V_BuildJob;

  struct ASSETBUILDER_API BuildJob : public Nocturnal::RefCountBase<BuildJob>
  {
    // input
    Asset::AssetClassPtr      m_Asset;
    BuilderOptionsPtr         m_Options;
    u32                       m_Flags;

    // ouput
    JobResult                 m_Result;
    Console::V_Statement      m_ConsoleOutput;
    u32                       m_WarningCount;
    u32                       m_ErrorCount;

    // used internally by AssetBuilder
    IBuilderPtr               m_Builder; // if null, AssetBuilder will allocate a builder based on the assetclass type
    std::string               m_BuildString;
    Dependencies::V_DependencyInfo  m_OutputFiles;
    V_BuildJob                m_DependentJobs;
    V_BuildJob                m_PostJobs; 
    u32                       m_OriginalFlags; // the build process can modify m_Flags, so this stores its original state

    BuildJob();
    BuildJob( const Asset::AssetClassPtr& asset, const BuilderOptionsPtr& options, IBuilder* builder = NULL, u32 flags = 0 );
  };

  class ASSETBUILDER_API IBuilder : public Nocturnal::RefCountBase<IBuilder>
  {
  public:
    virtual ~IBuilder()
    {

    }

    virtual BuilderOptionsPtr ParseOptions( const V_string& options )
    {
      return NULL;
    }

    virtual Asset::AssetClass* GetAssetClass() = 0;
    virtual const std::string GetBuildString() = 0;

    virtual std::string GetOutputDirectory()
    {
      return GetAssetClass()->GetBuiltDir();
    }

    //! Initialize should do as little work as possible
    virtual void Initialize( const Asset::AssetClassPtr& assetClass, BuilderOptionsPtr options = NULL ) = 0;

    //! GatherJobs should add any jobs that must be run regardless of whether this asset is up-to-date
    //! Builds added in here MUST be compatible with running at the same time as the current builder
    //! (ie, builder doesn't depend on the output of the job)
    virtual void GatherJobs( V_BuildJob& jobs ) { };

    //! GatherDependentJobs should add any jobs that are only required if this asset is out-of-date
    //! This means it will only be called if IsUpToDate() returned true
    virtual void GatherDependentJobs( V_BuildJob& jobs , u32 pass) { };

    //! GatherPostJobs should add any jobs that should be run AFTER the current builder.
    //! Post jobs are gathered after Build is called. 
    //! These jobs will run regardless of whether the current job is built or downloaded or even executed
    //! 
    virtual void GatherPostJobs( V_BuildJob& jobs) { }; 

    //! If NeedsPreRegisterInputs returns true, RegisterInputs will be called before IsUpToDate
    //! You should pre-register inputs if your builder may need to run even though none of your input
    //! files have changed. Otherwise the build system uses the cached dependency tree defined by your 
    //! previous build. Typically, the only way to change what input files are in your dependency 
    //! graph is to change one of your inputs (eg. zone content file), which will trigger you to be out 
    //! of date. However, if you depend on environment vars or any external stuff and need to re-evaluate 
    //! your surroundings to decide to build, return true from this function... 
    //!
    virtual bool NeedsPreRegisterInputs() { return false; }
    virtual bool IsUpToDate() = 0;
    virtual void RegisterInputs( Dependencies::V_DependencyInfo& outputFiles ) = 0;

    //! Returns whether or not the build had any non-fatal errors.  AssetBuilder will not call UpdateOutputs
    //! on files from builds with errors, so they will be assumed out-of-date again next build
    virtual bool Build() = 0;

    //! This determines whether multiple builds of this same type can be split between threads
    virtual bool NeedsThreadAffinity() { return false; }

    template< class T >
    void CastOptions( const BuilderOptionsPtr& options, Nocturnal::SmartPtr<T>& builderOptions )
    {
      if ( !options.ReferencesObject() )
      {
        builderOptions = new T();
      }
      else if ( options->HasType( Reflect::GetType<T>() ) )
      {
        builderOptions = Reflect::DangerousCast<T>( options );
      }
      else
      {
        builderOptions = new T();
        options->CopyTo( builderOptions );
      }
    }
  };
  
  typedef std::vector< IBuilderPtr > V_IBuilder;
}

namespace BuilderInterface
{
  void Initialize();
  void Cleanup();

  ASSETBUILDER_API void AllocateBuilders( Asset::EngineType engineType, AssetBuilder::V_IBuilder& builders );
}

// initialziation prototype
typedef void (*AllocateBuildersFunc)( AssetBuilder::V_IBuilder& builders );

#define BUILDER_DECL extern "C" __declspec(dllexport)

#define DECLARE_BUILDER_ENTRY_POINTS(__BuilderName) \
BUILDER_DECL void InitializeBuilderModule()         \
{                                                   \
  AssetBuilder::__BuilderName::Initialize();        \
}                                                   \
                                                    \
BUILDER_DECL void CleanupBuilderModule()            \
{                                                   \
  AssetBuilder::__BuilderName::Cleanup();           \
}