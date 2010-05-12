#pragma once

#include "Common/Types.h"

#define PROFILE_PROTOCOL_VERSION        (0x00)
#define PROFILE_SIGNATURE               (0x12345678)

#define PROFILE_CMD_INIT                (0x00)
#define PROFILE_CMD_SCOPE_ENTER         (0x01)
#define PROFILE_CMD_SCOPE_EXIT          (0x02)
#define PROFILE_CMD_BLOCK_END           (0x03)

#define PROFILE_PACKET_STRING_BUFSIZE   (64)
#define PROFILE_CYCLES_FOR_CONVERSION   (100000)
#define PROFILE_PACKET_BLOCK_SIZE       (16 * 1024)

namespace Profile
{
  struct Header
  {
    u16 m_Command; 
    u16 m_Size; 
  }; 

  struct InitPacket 
  {
    Header m_Header; 
    u32    m_Version; 
    u32    m_Signature; 
    f32    m_Conversion; // PROFILE_CYCLES_FOR_CONVERSION cycles -> how many millis?
  }; 

  struct ScopeEnterPacket
  {
    Header m_Header; 
    u32    m_UniqueID; 
    u32    m_StackDepth; 
    u32    m_Line; 
    u64    m_StartTicks; 
    char   m_Description[PROFILE_PACKET_STRING_BUFSIZE]; 
    char   m_Function[PROFILE_PACKET_STRING_BUFSIZE]; 
  }; 

  struct ScopeExitPacket 
  {
    Header m_Header;
    u32    m_UniqueID;   
    u32    m_StackDepth; 
    u64    m_Duration; 
  }; 

  struct BlockEndPacket
  {
    Header m_Header; 
  };

  union UberPacket
  {
    Header           m_Header; 
    InitPacket       m_Init; 
    ScopeEnterPacket m_ScopeEnter; 
    ScopeExitPacket  m_ScopeExitPacket; 
  };

}

