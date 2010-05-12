#include "Precompile.h"

#include "RemoteCharacter.h"
#include "CharacterEditor.h"

#include "Live/RuntimeConnection.h"

#include "rpc/interfaces/rpc_lunaview_host.h"
#include "rpc/interfaces/rpc_lunaview_tool.h"

using namespace Luna;
using namespace RPC;

namespace Luna
{
#pragma TODO( "make a character editor RPC interface" )
#if 0
  struct LunaViewTool : ILunaViewTool
  {
    RemoteCharacter* m_RemoteCharacter;
    CharacterEditor* m_CharacterEditor;

    LOCAL_IMPL(LunaViewTool, ILunaViewTool);

    void Init( RemoteCharacter* remoteCharacter )
    {
      m_RemoteCharacter = remoteCharacter;
      m_CharacterEditor = remoteCharacter->GetCharacterEditor();
    }

    virtual void TransformCamera(TransformCameraParam* param);
    virtual void RequestScene();

    virtual void UpdatePhysicsJoint(RPC::PhysicsJointParam* param); 
    virtual void SelectJoint(RPC::SelectJointParam* param); 
  };

  void LunaViewTool::TransformCamera( TransformCameraParam* param )
  {
  }

  void LunaViewTool::RequestScene()
  {
  }

  void LunaViewTool::UpdatePhysicsJoint(RPC::PhysicsJointParam* param)
  {
    if ( m_RemoteCharacter->IsEnabled() )
    {
      m_CharacterEditor->GetManager()->RemotePhysicsJointUpdate( param );
    }
  }

  void LunaViewTool::SelectJoint(RPC::SelectJointParam* param)
  {
  }
#endif
}

RemoteCharacter::RemoteCharacter( CharacterEditor* editor )
: RemoteEditor( editor )
, m_CharacterEditor( editor )
{
  RPC::Host* host = RuntimeConnection::GetHost();

#if 0
  m_LunaViewTool = new LunaViewTool( host );
  m_LunaViewTool->Init( this );
#endif
}

RemoteCharacter::~RemoteCharacter()
{
#if 0
  delete m_LunaViewTool; 
  m_LunaViewTool = NULL; 
#endif
}

void RemoteCharacter::Enable( bool enable )
{
  __super::Enable( enable );

  if ( enable )
  {
#if 0
    RPC::Host* host = RuntimeConnection::GetHost();
    host->SetLocalInterface( kLunaViewTool, m_LunaViewTool );
#endif
  }
}

void RemoteCharacter::SendScene()
{
  __super::SendScene();

  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  Content::JointTransformPtr jointPtr = m_CharacterEditor->GetManager()->GetSelectedJoint();
  if(jointPtr)
  {
    RPC::SelectJointParam joint; 
    joint.m_ID = jointPtr->m_ID;
    joint.m_Type = RPC::PhysicsJointTypes::Unknown;
    lunaView->SelectJoint(&joint); 
  }
}