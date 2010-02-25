#pragma once

#include "Profile.h"
#include "Packets.h"

#define PROFILE_ACCUMULATOR_MAX   (2048)
#define PROFILE_CONTEXTS_MAX      (128)

namespace Profile
{
  struct Context
  {
    Platform::LogFile m_LogFile; 
    u32               m_UniqueID; 
    u32               m_StackDepth; 
    u32               m_PacketBufferOffset; 
    u8                m_PacketBuffer[PROFILE_PACKET_BLOCK_SIZE]; 
    u32               m_AccumStack[PROFILE_ACCUMULATOR_MAX]; 

    Context(); 
    ~Context(); 

    void FlushFile(); 

    template <class T>
    T* AllocPacket(u32 cmd)
    {
      u32 spaceNeeded = sizeof(T) + sizeof(BlockEndPacket) + sizeof(ScopeEnterPacket); 

      if (m_PacketBufferOffset + spaceNeeded >= PROFILE_PACKET_BLOCK_SIZE)
      {
        FlushFile(); 
      }

      T* packet = (T*) (m_PacketBuffer + m_PacketBufferOffset); 
      m_PacketBufferOffset += sizeof(T); 

      //Console::Print("CMD %d OFFSET %d\n", cmd, m_PacketBufferOffset); 

      packet->m_Header.m_Command = cmd; 
      packet->m_Header.m_Size    = sizeof(T); 

      return packet; 
    }
  }; 
}
