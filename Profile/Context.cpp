#include "Context.h"

#include <string.h>

using namespace Profile;

Context::Context()
: m_UniqueID(0)
, m_StackDepth(0)
, m_PacketBufferOffset(0)
{
  m_LogFile.Open( Platform::GetOutputFile() ); 
  memset(m_AccumStack, 0, sizeof(m_AccumStack)); 
}

Context::~Context()
{
  m_LogFile.Close(); 
}

void Context::FlushFile()
{
  u64 startTicks = Platform::TimerGetClock(); 

  // make a scope enter packet for flushing the file
  ScopeEnterPacket* enter = (ScopeEnterPacket*) (m_PacketBuffer + m_PacketBufferOffset); 
  m_PacketBufferOffset += sizeof(ScopeEnterPacket); 

  enter->m_Header.m_Command = PROFILE_CMD_SCOPE_ENTER; 
  enter->m_Header.m_Size    = sizeof(ScopeEnterPacket); 
  enter->m_UniqueID         = 0; 
  enter->m_StackDepth       = 0; 
  enter->m_Line             = __LINE__;
  enter->m_StartTicks       = startTicks; 
  strcpy(enter->m_Function, "Context::FlushFile"); 
  enter->m_Description[0]   = 0; 

  // make a block end packet for end of packet
  BlockEndPacket* blockEnd = (BlockEndPacket*) (m_PacketBuffer + m_PacketBufferOffset); 
  m_PacketBufferOffset += sizeof(BlockEndPacket); 

  blockEnd->m_Header.m_Command = PROFILE_CMD_BLOCK_END; 
  blockEnd->m_Header.m_Size    = sizeof(BlockEndPacket); 

  // we write the whole buffer, in large blocks
  m_LogFile.Write( (const char*) m_PacketBuffer, PROFILE_PACKET_BLOCK_SIZE); 

  // reset the packet buffer
  m_PacketBufferOffset = 0; 


  // make a scope exit packet for being done flushing the file
  ScopeExitPacket* exit = (ScopeExitPacket*) (m_PacketBuffer + m_PacketBufferOffset); 
  m_PacketBufferOffset += sizeof(ScopeExitPacket); 

  exit->m_Header.m_Command = PROFILE_CMD_SCOPE_EXIT; 
  exit->m_Header.m_Size    = sizeof(ScopeExitPacket); 

  exit->m_UniqueID   = 0; 
  exit->m_StackDepth = 0; 
  exit->m_Duration   = Platform::TimerGetClock() - startTicks; 

  // return to filling out the packet buffer
}
