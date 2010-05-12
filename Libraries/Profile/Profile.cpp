#include "Profile.h"
#include "Packets.h"
#include "Context.h"

#include "Common/Assert.h"

#define PLATFORM_INCLUDE_SYSTEM
#include "Platform/Platform.h"
#include "Platform/Thread.h"

#ifndef MIN
#define MIN(A,B)        ((A) < (B) ? (A) : (B))
#endif
#ifndef MAX
#define MAX(A,B)        ((A) > (B) ? (A) : (B))
#endif

#ifdef WIN32
#define QSORT qsort
#else
namespace IGG {
typedef i32 (*CompareElementsFunc)(const void*, const void*);
void Qsort(void* elements, u32 element_count, u32 element_size, CompareElementsFunc compare_elements);
}
#define QSORT IGG::Qsort
#endif

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace Profile;

static u32           g_AccumulatorCount = 0;
static Accumulator*  g_Accumulators[ PROFILE_ACCUMULATOR_MAX ];

static u32           g_ContextCount = 0; 
static Context*      g_Contexts[ PROFILE_CONTEXTS_MAX ];

static bool          g_Enabled = false;

bool Profile::Settings::Enabled()
{
  return g_Enabled;
}

void Profile::Initialize()
{
  g_Enabled = true;
  Platform::ProfileInit(); 
}

void Profile::Cleanup()
{
  for(u32 i = 0; i < g_ContextCount; ++i)
  {
    g_Contexts[i]->FlushFile(); 
    delete(g_Contexts[i]); 
  }
  g_ContextCount = 0; 

  Platform::ProfileCleanup(); 
  g_Enabled = false;
}

Accumulator::Accumulator()
: m_Hits (0)
, m_TotalMillis (0.0f)
, m_Index (-1)
{


}

Accumulator::Accumulator(const char* name)
: m_Hits (0)
, m_TotalMillis (0.0f)
, m_Index (-1)
{
  Init(name); 
}

Accumulator::Accumulator(const char* function, const char* name)
: m_Hits (0)
, m_TotalMillis (0.0f)
, m_Index (-1)
{
  size_t temp = MIN( strlen(function), sizeof(m_Name)-1 );
  memcpy( m_Name, function, temp );

  size_t temp2 = MIN( strlen(name), sizeof(m_Name) - 1 - temp );
  memcpy( m_Name + temp, name, temp2 );

  m_Name[ temp + temp2 ] = '\0';

  Init(NULL);
}

void Accumulator::Init(const char* name)
{
  if (name)
  {
    strncpy(m_Name, name, sizeof(m_Name));
    m_Name[ sizeof(m_Name)-1] = '\0'; 
  }
  else
  {
    NOC_ASSERT(m_Name[0] != '\0');
  }

  if (m_Index < 0 && g_AccumulatorCount < PROFILE_ACCUMULATOR_MAX)
  {
    g_Accumulators[ g_AccumulatorCount ] = this;
    m_Index = g_AccumulatorCount++;
  }
}

Accumulator::~Accumulator()
{
  if (m_Index >= 0)
  {
    g_Accumulators[m_Index] = NULL;
  }
}

void Accumulator::Report()
{
  Platform::PrintProfile("[%12.3f] [%8d] %s\n", m_TotalMillis, m_Hits, m_Name);
}

int CompareAccumulatorPtr( const void* ptr1, const void* ptr2 )
{
  const Accumulator* left = *(const Accumulator**)ptr1;
  const Accumulator* right = *(const Accumulator**)ptr2;

  if (left && !right)
  {
    return -1;
  }

  if (!left && right)
  {
    return 1;
  }

  if (left && right)
  {
    if ((left)->m_TotalMillis > (right)->m_TotalMillis)
    {
      return -1;
    }
    else if ((left)->m_TotalMillis < (right)->m_TotalMillis)
    {
      return 1;
    }
  }

  return 0;
}

void Accumulator::ReportAll()
{
  float totalTime = 0.f;
  for ( u32 i = 0; i < g_AccumulatorCount; i++ )
  {
    if (g_Accumulators[i])
    {
      totalTime += g_Accumulators[i]->m_TotalMillis;
    }
  }

  if (totalTime > 0.f)
  {
    Platform::PrintProfile("\nProfile Report:\n");

    QSORT( g_Accumulators, g_AccumulatorCount, sizeof(Accumulator*), &CompareAccumulatorPtr );

    for ( u32 i = 0; i < g_AccumulatorCount; i++ )
    {
      if (g_Accumulators[i] && g_Accumulators[i]->m_TotalMillis > 0.f)
      {
        g_Accumulators[i]->Report();
      }
    }
  }
}

ScopeTimer::ScopeTimer(Accumulator* accum, const char* func, u32 line, const char* desc)
{
  NOC_ASSERT(func); 
  m_Description[0] = '\0'; 
  if(desc)
  {
    strncpy(m_Description, desc, sizeof(m_Description)); 
    m_Description[ sizeof(m_Description)-1 ] = '\0'; 
  }

  m_Accum       = accum; 
  m_StartTicks  = Platform::TimerGetClock(); 
  m_Print       = desc != NULL; 

#if defined(PROFILE_INSTRUMENTATION)

  Context* context = Platform::GetThreadContext(); 

  if(context == NULL)
  {
    context = new Context; 
    Platform::SetThreadContext( context ); 

    // save it off. this should probably be locked
    g_Contexts[ g_ContextCount ] = context; 
    g_ContextCount++; 

    InitPacket* init = context->AllocPacket<InitPacket>(PROFILE_CMD_INIT); 

    init->m_Version    = PROFILE_PROTOCOL_VERSION;
    init->m_Signature  = PROFILE_SIGNATURE; 
    init->m_Conversion = Platform::CyclesToMillis(PROFILE_CYCLES_FOR_CONVERSION); 
  }

  ScopeEnterPacket* enter = context->AllocPacket<ScopeEnterPacket>(PROFILE_CMD_SCOPE_ENTER); 

  enter->m_UniqueID   = context->m_UniqueID++; 
  enter->m_StackDepth = context->m_StackDepth; 
  enter->m_Line       = line; 
  enter->m_StartTicks = m_StartTicks; 

  strncpy(enter->m_Description, m_Description, sizeof(enter->m_Description)); 
  enter->m_Description[ sizeof(enter->m_Description)-1] = 0; 

  strncpy(enter->m_Function, func, sizeof(enter->m_Function)); 
  enter->m_Function[ sizeof(enter->m_Function)-1] = 0; 

  context->m_StackDepth++; 
  if(m_Accum && m_Accum->m_Index != -1)
  {
    context->m_AccumStack[ m_Accum->m_Index ]++; 
  }

#endif
}

ScopeTimer::~ScopeTimer()
{
  u64 stopTicks = Platform::TimerGetClock();  

  u64   taken  = stopTicks - m_StartTicks; 
  float millis = Platform::CyclesToMillis(taken); 

  if ( m_Print && m_Description[0] != '\0' )
  {
    Platform::PrintProfile("[%12.3f] %s\n", millis, m_Description);
  }

#if defined(PROFILE_INSTRUMENTATION)

  Context* context = Platform::GetThreadContext(); 
  NOC_ASSERT(context); 

  ScopeExitPacket* packet = context->AllocPacket<ScopeExitPacket>(PROFILE_CMD_SCOPE_EXIT); 

  packet->m_UniqueID   = context->m_UniqueID++; 
  packet->m_StackDepth = --context->m_StackDepth;
  packet->m_Duration   = taken; 
  
  if(m_Accum && m_Accum->m_Index != -1)
  {
    int stack = --context->m_AccumStack[ m_Accum->m_Index ]; 

    if(stack == 0)
    {
      m_Accum->m_TotalMillis += millis; 
    }

    m_Accum->m_Hits++; 
  }

#elif defined(PROFILE_ACCUMULATION)

  if(m_Accum && m_Accum->m_Index != -1)
  {
    m_Accum->m_TotalMillis += millis; 
    m_Accum->m_Hits++; 
  }

#endif
}
