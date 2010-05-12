#include "stdafx.h"
#include "ConstructionTool.h"
#include "ConstructionCmd.h"

#include "IPC/TCP.h"
#include "IPC/Pipe.h"

#include "EntityAssetNode.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"

using namespace Attribute;
using namespace Construction;
using namespace Asset;
using namespace RPC;

RPC::IConstructionTool*   Construction::g_ConstructionTool = NULL;
RPC::IConstructionHost*   Construction::g_ConstructionHost = NULL;
tuid                      Construction::g_ClassID = TUID::Null;

IPC::PipeConnection       g_Connection;
RPC::Host                 g_Host;
HANDLE                    g_DispatchComplete = NULL;
HANDLE                    g_Thread = NULL;

bool g_Initialized = false;

struct ConstructionTool : RPC::IConstructionTool
{
  LOCAL_IMPL(ConstructionTool, RPC::IConstructionTool)

  virtual void OpenArtFile( RPC::EntityEditParam *param ) NOC_OVERRIDE;
 
  virtual void BeginScene() NOC_OVERRIDE;
  virtual void CreateProxy( RPC::CreateInstanceParam* param ) NOC_OVERRIDE;
  virtual void EndScene() NOC_OVERRIDE;
};

void ConstructionTool::OpenArtFile( RPC::EntityEditParam *param )
{
  Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( *reinterpret_cast<tuid*>( &param->m_EntityAsset ) );
  AttributeViewer< ArtFileAttribute > artFile( assetClass );

  if( artFile.Valid() )
  {
    MStatus status = MFileIO::open( artFile->GetFilePath().c_str(), NULL, true );
    if( status )
    {
      Construction::g_ClassID = assetClass->m_AssetClassID;
      EntityNode::s_RelativeTransform = *reinterpret_cast< Math::Matrix4* >( &param->m_Transform );
    }
    else
    {
      Construction::g_ClassID = TUID::Null;
    }
  }
  else
  {
    Construction::g_ClassID = TUID::Null;
  }
}

void ConstructionTool::BeginScene()
{

}

void ConstructionTool::CreateProxy(RPC::CreateInstanceParam *param)
{
  MGlobal::displayInfo(MString ("Creating ") + param->m_Name.Characters + "...");

  Asset::EntityPtr entity = new Asset::Entity( param->m_ID, param->m_EntityAsset );
  entity->m_DefaultName = param->m_Name.Characters;
  entity->m_GlobalTransform = *reinterpret_cast< Math::Matrix4* >( &param->m_Transform );

  EntityAssetNode::CreateInstance( entity );
}

void ConstructionTool::EndScene()
{

}

namespace Construction
{
  DWORD Thread(void*);
}

bool Construction::Initialize()
{
  if ( g_Initialized )
  {
    return true;
  }

  g_Initialized = true;

  g_Connection.Initialize(false, "Construction Connection", "construction");
  g_Host.SetConnection(&g_Connection);

  // create a local repeater to send messages to our viewer
  g_ConstructionHost = new IConstructionHostRemote (&g_Host);

  // create a local implementation of our functionality
  g_ConstructionTool = new ConstructionTool (&g_Host);
  g_Host.SetLocalInterface(kConstructionTool, g_ConstructionTool);

  g_DispatchComplete = ::CreateEvent(0, TRUE, FALSE, "DispatchComplete");
  g_Thread = ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Construction::Thread, 0, 0, 0);

  return true;
}

void Construction::Cleanup()
{
  if ( !g_Initialized )
  {
    return;
  }

  g_Initialized = false;

  g_Connection.Cleanup();
  g_Host.Reset();

  delete g_ConstructionHost;
  g_ConstructionHost = NULL;

  delete g_ConstructionTool;
  g_ConstructionTool = NULL;

  ::SetEvent(g_DispatchComplete);
  ::WaitForSingleObject(g_Thread, INFINITE);
}

void Construction::Dispatch()
{
  if (g_Initialized)
  {
    g_Host.Dispatch();
  }

  ::SetEvent(g_DispatchComplete);
}

DWORD Construction::Thread(void*)
{
  MStatus stat;

  while ( true )
  {
    g_Connection.Wait();

    if ( !g_Initialized )
    {
      break;
    }

    ::ResetEvent(g_DispatchComplete);

    const MString command = ConstructionCmd::CommandName + " " + ConstructionCmd::DispatchFlagLong;

    if (MS::kSuccess != MGlobal::executeCommandOnIdle( command ))
    {
      std::cerr << "Failed idle command: '" << command.asChar() << "'" << std::endl;
    }

    ::WaitForSingleObject(g_DispatchComplete, INFINITE);
  }

  return 0;
}

bool Construction::EstablishConnection()
{
  Construction::Initialize();

  u32 ticks = 10;
  while( ticks-- > 0 && !g_Host.Connected() )
  {
    Sleep( 100 );
  }

  return g_Host.Connected();
}

bool Construction::Connected()
{
  return g_Host.Connected();
}