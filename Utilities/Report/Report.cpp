#include "AppUtils/AppUtils.h"
#include "Common/Version.h"
#include "Console/Console.h"

#include "Reflect/Archive.h"
#include "Reflect/Version.h"

#include "Symbol/Symbolinit.h"
#include "Symbol/SymbolBuilder.h"
#include "Symbol/SymbolTypes.h"
using namespace Symbol;

#include "Content/ContentInit.h"

#include "File/Manager.h"
#include "Asset/Entity.h"
#include "Asset/EntityAsset.h"
#include "Asset/AssetInit.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/WorldFileAttribute.h"
#include "Asset/UpdateClassAttribute.h"
#include "Asset/MetaInfoAttribute.h"
#include "Asset/AnimationAttribute.h"
#include "Asset/AnimationSetAsset.h"
#include "Asset/LevelAsset.h"
#include "Common/InitializerStack.h"
#include "Content/Scene.h"
#include "Content/SceneNode.h"
#include "Content/Zone.h"
#include "ContentProcess/Processor.h"
#include "Asset/DependenciesAttribute.h"

using namespace Reflect;
using namespace Asset;
using namespace Attribute;

typedef void (*UsageFunc)( const struct ReportStruct& );
typedef void (*ReportFunc)( std::ofstream&, const std::string& );

struct ReportStruct
{
  const char* m_ReportType;
  UsageFunc   m_Usage;
  ReportFunc  m_Report;
};

enum ReportType
{
  REPORT_MOBY_DUMP,
  REPORT_LEVEL_MOBIES,
  REPORT_SYMBOL_SIZE,

  REPORT_COUNT,
  REPORT_UNKNOWN,
};

struct ReportTask
{
  ReportType   type;
  std::string  asset;
};

typedef std::vector<ReportTask> V_ReportTask;


// forward declares
void AddTask( const std::string& type, const std::string& asset );
void AddBatch( const std::string& file );
void PrintTypeUsage();

void MobyDumpUsage( const ReportStruct& );
void LevelMobiesUsage( const ReportStruct& );
void SymbolSizeUsage( const ReportStruct& );

void MobyDumpReport( std::ofstream&, const std::string& );
void LevelMobiesReport( std::ofstream&, const std::string& );
void SymbolSizeReport( std::ofstream&, const std::string& );

V_ReportTask g_ReportTasks;

ReportStruct g_ReportStructs[REPORT_COUNT] = 
{
  { "moby_dump",    MobyDumpUsage,    MobyDumpReport   },
  { "level_mobies", LevelMobiesUsage, LevelMobiesReport },
  { "symbol_size", SymbolSizeUsage, SymbolSizeReport },
};

std::string g_Batch;
std::string g_Asset;
std::string g_Type;

int Main(int argc, const char** argv)
{
  for ( int i=1; i<argc; ++i )
  {
    if ( !stricmp(argv[i], "-batch") && i+1 < argc )
    {
      g_Batch = argv[++i];
    }
    else if ( !stricmp(argv[i], "-asset") && i+1 < argc )
    {
      g_Asset = argv[++i];
    }
    else if ( !stricmp(argv[i], "-type") && i+1 < argc )
    {
      g_Type = argv[++i];
    }
  }

  Nocturnal::InitializerStack initializerStack( true );
  initializerStack.Push( Asset::Initialize,      Asset::Cleanup );
  initializerStack.Push( Content::Initialize,    Content::Cleanup );

  if ( !g_Batch.empty() )
  {
    AddBatch( g_Batch );
  }
  else if ( !g_Type.empty() && !g_Asset.empty() )
  {
    AddTask( g_Type, g_Asset );
  }

  if ( g_ReportTasks.empty() )
  {
    Console::Error( "Invalid arguments, please specify -type and -asset\nValid types are:\n" );
    PrintTypeUsage();

    return 1;
  }

  V_ReportTask::const_iterator taskItr = g_ReportTasks.begin();
  V_ReportTask::const_iterator taskEnd = g_ReportTasks.end();

  for ( ; taskItr != taskEnd; ++taskItr )
  {
    const ReportTask& task = (*taskItr);

    char reportFileName[260];
    tuid AssetID=File::GlobalManager().GetID( task.asset );
    sprintf( reportFileName, "%s/%s_%I64u.txt", Finder::ProjectTemp().c_str(), g_ReportStructs[ task.type ].m_ReportType, AssetID );
    std::ofstream reportFile( reportFileName );
    
    (*g_ReportStructs[ task.type ].m_Report)( reportFile, task.asset );
  }

  return 0;
}

int main(int argc, const char** argv)
{
  return AppUtils::StandardMain( &Main, argc, argv );
}

void AddTask( const std::string& type, const std::string& asset )
{
  size_t curIndex = g_ReportTasks.size();
  g_ReportTasks.resize( curIndex + 1 );
  ReportTask& task = g_ReportTasks[ curIndex ];
  task.asset = asset;
  task.type  = REPORT_UNKNOWN;
  for ( u32 index = 0; index < REPORT_COUNT; ++index )
  {
    if ( strcmp( type.c_str(), g_ReportStructs[ index ].m_ReportType ) == 0 )
    {
      task.type = (ReportType)index;
      break;
    }
  }

  if ( task.type == REPORT_UNKNOWN )
  {
    PrintTypeUsage();
    throw Nocturnal::Exception ( "Unknown report type: %s\nValid types are:\n", type.c_str() );
  }
}

void AddBatch( const std::string& file )
{
  std::ifstream strm( file.c_str() );

  if ( !strm.is_open() )
  {
    throw Nocturnal::Exception ( "Invalid batch file name\n" );
  }

  std::string type;
  std::string asset;

  while ( !strm.eof() )
  {
    strm >> type;
    strm >> asset;
    
    if ( strm.fail() )
    {
      break;
    }

    AddTask( type, asset );
  }
}

void MobyDumpUsage( const ReportStruct& reportStruct )
{
  Console::Error( " Type: %s, Dumps the places moby class numbers and a count for each moby class\n", reportStruct.m_ReportType );
}

void MobyDumpReport( std::ofstream& outFile, const std::string& entityName )
{
  //Start JB Code

  FILE * ptrF; 
  ptrF = fopen( entityName.c_str() , "r" ); 
  if(ptrF == NULL)
  {
    throw Nocturnal::Exception ( "I cant find the file: %s", entityName.c_str() );
  }

  tuid entityID = File::GlobalManager().GetID( entityName );



  AssetClassPtr assetClass = AssetClass::FindAssetClass( entityID );

  //An attempt to grab Description, but not sure this actually exists
  //AttributeViewer< MetaInfoAttribute > metaPtr( assetClass );
  //std::string description = metaPtr->m_Description;

  AttributeViewer< ArtFileAttribute > artFileHandle( assetClass );
  if ( artFileHandle.Valid() )
  {
    std::string artFile = artFileHandle->GetFilePath();
    outFile << "ArtFile : " << artFile << std::endl;
  }

  AttributeViewer< UpdateClassAttribute > runTimeClassPtr( assetClass );
  if ( runTimeClassPtr.Valid() )
  {
    std::string runTimeClass = runTimeClassPtr->GetClassNames().front(); 
    outFile << "RunTimeClass : " << runTimeClass << std::endl;
  }

  AttributeViewer< DependenciesAttribute > dependencyPtr( assetClass );
  if ( dependencyPtr.Valid() )
  {
    V_tuid v_Dependencies = dependencyPtr->m_AssetIds;
    if ( v_Dependencies.size() > 0 )
    {
      outFile << "Reqs : " << v_Dependencies.size() << std::endl;
      for( V_tuid::iterator dependencyItr = v_Dependencies.begin();
                            dependencyItr != v_Dependencies.end();
                            ++dependencyItr)
      {
        AssetClassPtr dependencyClass = AssetClass::FindAssetClass( *dependencyItr );
        std::string dependencyFile = dependencyClass->GetFilePath();
        outFile << *dependencyItr << " : " << dependencyFile << std::endl;
      }
    }
  }

  AttributeViewer< AnimationAttribute > animationSet( assetClass );
  if ( animationSet.Valid() && animationSet->m_AnimationSetId != TUID::Null )
  {
    AnimationSetAssetPtr animSetClass = AssetClass::GetAssetClass<AnimationSetAsset>( animationSet->m_AnimationSetId );
    
    V_AnimationClipData animationClips;
    animSetClass->GetAnimationClips( animationClips );
    outFile << "Animations : " << animationClips.size() << std::endl;
    for each ( const AnimationClipDataPtr& clip in animationClips )
    {
      std::string artFilePath = File::GlobalManager().GetPath( clip->m_ArtFile );
      std::string animClipFile = artFilePath;
      outFile << animClipFile << std::endl;
    }
  }

  //End JB Code

  //MobyDefsPtr defsPtr = Reflect::DangerousCast<MobyDefs>(AssetDefs::LoadDefs(AssetDefs::kMoby, asset));
  //if ( !defsPtr.ReferencesObject() )
  //{
  //  Console::Error( "Unable to load moby %d\n", asset );
  //  return;
  //}

  //outFile << "Reqs - " << defsPtr->m_RequiredMobys.size() << std::endl;
  //V_i32::const_iterator reqItr = defsPtr->m_RequiredMobys.begin();
  //V_i32::const_iterator reqEnd = defsPtr->m_RequiredMobys.end();

  //for ( ; reqItr != reqEnd; ++reqItr )
  //{
  //  outFile << (*reqItr) << std::endl;
  //}

  //outFile << "Shadow Of : " << defsPtr->m_ShadowOf << std::endl;

  //if ( !defsPtr->m_ValidUpdateClasses.empty() )
  //{
  //  outFile << "Update Class : " << defsPtr->m_ValidUpdateClasses[0] << std::endl;
  //}

  //outFile << "Wobbly : " << defsPtr->m_EnableWobblePhysics << std::endl;
}

void LevelMobiesUsage( const ReportStruct& reportStruct )
{
  Console::Error( " Type: %s, Dumps the places moby class numbers and a count for each moby class\n", reportStruct.m_ReportType );
}

void LevelMobiesReport( std::ofstream& outFile, const std::string& asset )
{
  //AssetClassPtr assetClass = Reflect::Archive::FromFile< AssetClass >( asset );

  FILE * ptrF; 
  ptrF = fopen( asset.c_str() , "r" ); 
  if(ptrF == NULL)
  {
    throw Nocturnal::Exception ( "I cant find the file: %s", asset.c_str() );
  }

  tuid entityID = File::GlobalManager().GetID( asset );
  LevelAssetPtr levelClass = AssetClass::GetAssetClass<LevelAsset>(entityID);
  AttributeViewer< WorldFileAttribute > levelContentFilePtr( levelClass );
  std::string levelContentFile = levelContentFilePtr->GetFilePath();

  tuid levelContentID = File::GlobalManager().GetID( levelContentFile );

  //Content::Scene levelScene = Content::Scene(levelContentID);
  //Content::M_DependencyNode levelDependencyNodes = levelScene.m_DependencyNodes;
  //AttributeViewer< WorldFileAttribute > levelNodes( levelScene );


  AttributeViewer<WorldFileAttribute> model( levelClass );
  V_Element elements;
  Archive::FromFile( model->GetFilePath(), elements );

  Content::V_Zone m_zones;
  // gather data and do stuff with it post-load
  V_Element::iterator itr = elements.begin();
  V_Element::iterator end= elements.end();
  Content::Processor   m_Processor;
  for( ; itr != end; ++itr )
  {
    if ( (*itr)->HasType( Reflect::GetType<Content::Zone>() ) )
    {
      Content::ZonePtr zone = DangerousCast< Content::Zone >( (*itr) );

      std::string zoneContentFile = zone->GetFilePath();

      m_Processor.LoadContent( zoneContentFile );
      typedef Nocturnal::SmartPtr<Asset::Entity> EntityPtr;
      typedef std::vector<EntityPtr> V_Entity;
      V_Entity entities;
      m_Processor.GetContentScene().GetAll<Asset::Entity>(entities);

      V_Entity::const_iterator entItr = entities.begin();
      V_Entity::const_iterator entEnd = entities.end();
      outFile << zone->m_FileID << " : " << entities.size() << std::endl;
      for( ; entItr !=entEnd; ++entItr )
      {
        const EntityPtr& entityInstance = (*entItr);
        tuid entityClassID = entityInstance->GetEntityAssetID(); 
        outFile << entityClassID << std::endl;
      }
      //m_zones.push_back( zone );
    }
    }
  }


void SymbolSizeUsage( const ReportStruct& report)
{
}

void SymbolSizeReport( std::ofstream& outFile, const std::string& symbol )
{
  Reflect::Initialize();
  Symbol::Initialize();

  SymbolBuilderPtr symbolBuilder = SymbolBuilder::GetInstance();
  symbolBuilder->Load();

  UDTPtr udtSymbol = symbolBuilder->FindUDT(symbol);
  if (!udtSymbol)
  {
    throw( Nocturnal::Exception( "Unable to find symbol '%s'", symbol.c_str() ) );
  }

  udtSymbol->ComputeSize(Nocturnal::BufferPlatforms::Power32, symbolBuilder);
  u32 size = udtSymbol->GetSize(Nocturnal::BufferPlatforms::Power32);

  outFile 
      << symbol     
      << ":\t"
      << size
      << std::endl;

  Console::Print("\n");
  Console::Print("%s:\t%d\n", symbol.c_str(), size);
  Console::Print("\n");

  Symbol::Cleanup();
  Reflect::Cleanup();
}

void PrintTypeUsage()
{
  for ( u32 index = 0; index < REPORT_COUNT; ++index )
  {
    (*g_ReportStructs[ index ].m_Usage)( g_ReportStructs[ index ] );
  }
}

