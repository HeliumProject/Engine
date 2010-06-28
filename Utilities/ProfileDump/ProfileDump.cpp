#include <stdio.h>
#include <stdlib.h>
#include "Foundation/Log.h"
#include "Foundation/Profile.h"

using namespace Profile; 

void ParseAndPrintBlock(char* buffer); 

int main(int argc, const char** argv)
{
  if(argc != 2)
  {
    Log::Print( TXT( "Usage:\n" ) ); 
    Log::Print( TXT( "ProfileDump.exe filename.bin\n" ) ); 
    exit(1); 
  }
  
  const char* filename = argv[1]; 


  FILE* file = fopen(filename, "rb"); 
  if(!file)
  {
    Log::Print( TXT( "Unable to open %s for reading!\n" ), filename); 
    exit(1); 
  }

  fseek(file, 0, SEEK_END); 
  long filesize = ftell(file); 
  fseek(file, 0, SEEK_SET); 

  u32 blockCount = filesize / PROFILE_PACKET_BLOCK_SIZE; 

  if( filesize % PROFILE_PACKET_BLOCK_SIZE != 0)
  {
    Log::Print( TXT( "File %s is of the wrong size (not a multiple of PROFILE_PACKET_BLOCK_SIZE %d)\n" ), 
                   filename, PROFILE_PACKET_BLOCK_SIZE); 
  }
  else
  {
    Log::Print( TXT( "File %s has %d blocks\n" ), filename, blockCount); 
  }

  char* buffer = new char[PROFILE_PACKET_BLOCK_SIZE]; 

  for(u32 i = 0; i < blockCount; ++i)
  {
    size_t bytesRead = fread(buffer, 1, PROFILE_PACKET_BLOCK_SIZE, file); 

    if(bytesRead != PROFILE_PACKET_BLOCK_SIZE)
    {
      Log::Print( TXT( "Error reading block %d from file\n" ), i); 
      exit(1); 
    }

    Log::Print( TXT( "Block %d\n" ), i); 
    ParseAndPrintBlock(buffer); 
  }

  delete[] buffer; 

  fclose(file); 
}

u32 g_Indent = 0; 

void PrintIndent()
{
  for(u32 i = 0; i < g_Indent; ++i)
  {
    Log::Print( TXT( " " ) ); 
  }
}


void ParseAndPrintBlock(char* buffer)
{
  char* currentByte = buffer; 

  while(currentByte < buffer + PROFILE_PACKET_BLOCK_SIZE)
  {
    UberPacket* uberPacket = (UberPacket*) currentByte; 

    u16 cmd = uberPacket->m_Header.m_Command; 
    u16 size = uberPacket->m_Header.m_Size; 

    const char* packet_names[] = { "INIT       ", 
                                   "SCOPE_ENTER", 
                                   "SCOPE_EXIT ", 
                                   "BLOCK_END  " }; 

    if(cmd <= PROFILE_CMD_BLOCK_END)
    {
      Log::Print( TXT( "%s:" ), packet_names[cmd]); 
    }
    else
    {
      Log::Error( TXT( "UNKNOWN PROFILE TAG: 0x%.2x\n" ), cmd); 
      exit(1); 
    }

    switch(cmd)
    {
    case PROFILE_CMD_SCOPE_ENTER: 
      g_Indent++; 
      PrintIndent(); 
      Log::Print( TXT( "%s %d\n" ), 
                     uberPacket->m_ScopeEnter.m_Function, 
                     uberPacket->m_ScopeEnter.m_Line); 
      
      break; 
    case PROFILE_CMD_SCOPE_EXIT:
      PrintIndent(); 
      Log::Print( TXT( "%lld\n" ), uberPacket->m_ScopeExitPacket.m_Duration); 
      g_Indent--; 
      break; 

    default:
      Log::Print( TXT( "\n" ) );
    }

    if(cmd == PROFILE_CMD_BLOCK_END)
      break; 

    currentByte += size; 
  }
}