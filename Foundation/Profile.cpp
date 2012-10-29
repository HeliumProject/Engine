#include "FoundationPch.h"
#include "Profile.h"
#include "Log.h"

#include "Platform/Assert.h"
#include "Platform/Thread.h"
#include "Platform/Platform.h"
#include "Platform/Types.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef MIN
#define MIN(A,B)        ((A) < (B) ? (A) : (B))
#endif
#ifndef MAX
#define MAX(A,B)        ((A) > (B) ? (A) : (B))
#endif

using namespace Helium;
using namespace Helium::Profile;

static uint32_t           g_AccumulatorCount = 0;
static Accumulator*  g_Accumulators[ PROFILE_ACCUMULATOR_MAX ];

static uint32_t           g_ContextCount = 0; 
static Context*      g_Contexts[ PROFILE_CONTEXTS_MAX ];

static bool          g_Enabled = false;

bool Profile::Settings::Enabled()
{
    return g_Enabled;
}

void Profile::Initialize()
{
    g_Enabled = true;
}

void Profile::Cleanup()
{
    for(uint32_t i = 0; i < g_ContextCount; ++i)
    {
        g_Contexts[i]->FlushFile(); 
        delete(g_Contexts[i]); 
    }
    g_ContextCount = 0; 
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
    size_t temp = MIN( strlen( function ), sizeof(m_Name)-1 );
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
        HELIUM_ASSERT(m_Name[0] != '\0');
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
    Log::Profile( TXT( "[%12.3f] [%8d] %s\n" ), m_TotalMillis, m_Hits, m_Name);
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
    for ( uint32_t i = 0; i < g_AccumulatorCount; i++ )
    {
        if (g_Accumulators[i])
        {
            totalTime += g_Accumulators[i]->m_TotalMillis;
        }
    }

    if (totalTime > 0.f)
    {
        Log::Profile( TXT( "\nProfile Report:\n" ) );

        qsort( g_Accumulators, g_AccumulatorCount, sizeof(Accumulator*), &CompareAccumulatorPtr );

        for ( uint32_t i = 0; i < g_AccumulatorCount; i++ )
        {
            if (g_Accumulators[i] && g_Accumulators[i]->m_TotalMillis > 0.f)
            {
                g_Accumulators[i]->Report();
            }
        }
    }
}

Helium::ThreadLocalPointer g_ProfileContext;

ScopeTimer::ScopeTimer(Accumulator* accum, const char* func, uint32_t line, const char* desc)
{
    HELIUM_ASSERT(func); 
    m_Description[0] = '\0'; 
    if(desc)
    {
        strncpy(m_Description, desc, sizeof(m_Description)); 
        m_Description[ sizeof(m_Description)-1 ] = '\0'; 
    }

    m_Accum       = accum; 
    m_StartTicks  = Helium::TimerGetClock(); 
    m_Print       = desc != NULL; 

#if defined(PROFILE_INSTRUMENTATION)

    Context* context = (Context*)g_ProfileContext.GetPointer(); 

    if(context == NULL)
    {
        context = new Context; 
        g_ProfileContext.SetPointer( context ); 

        // save it off. this should probably be locked
        g_Contexts[ g_ContextCount ] = context; 
        g_ContextCount++; 

        InitPacket* init = context->AllocPacket<InitPacket>(PROFILE_CMD_INIT); 

        init->m_Version    = PROFILE_PROTOCOL_VERSION;
        init->m_Signature  = PROFILE_SIGNATURE; 
        init->m_Conversion = Helium::CyclesToMillis(PROFILE_CYCLES_FOR_CONVERSION); 
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
    uint64_t stopTicks = Helium::TimerGetClock();  

    uint64_t   taken  = stopTicks - m_StartTicks; 
    float millis = Helium::CyclesToMillis(taken); 

    if ( m_Print && m_Description[0] != '\0' )
    {
        Log::Profile( TXT( "[%12.3f] %s\n" ), millis, m_Description);
    }

#if defined(PROFILE_INSTRUMENTATION)

    Context* context = (Context*)g_ProfileContext.GetPointer(); 
    HELIUM_ASSERT(context); 

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

Context::Context()
: m_UniqueID(0)
, m_StackDepth(0)
, m_PacketBufferOffset(0)
{
    m_TraceFile.Open( TraceFile::GetFilePath() ); 
    memset(m_AccumStack, 0, sizeof(m_AccumStack)); 
}

Context::~Context()
{
    m_TraceFile.Close(); 
}

void Context::FlushFile()
{
    uint64_t startTicks = Helium::TimerGetClock(); 

    // make a scope enter packet for flushing the file
    ScopeEnterPacket* enter = (ScopeEnterPacket*) (m_PacketBuffer + m_PacketBufferOffset); 
    m_PacketBufferOffset += sizeof(ScopeEnterPacket); 

    enter->m_Header.m_Command = PROFILE_CMD_SCOPE_ENTER; 
    enter->m_Header.m_Size    = sizeof(ScopeEnterPacket); 
    enter->m_UniqueID         = 0; 
    enter->m_StackDepth       = 0; 
    enter->m_Line             = __LINE__;
    enter->m_StartTicks       = startTicks; 
    strcpy( enter->m_Function, "Context::FlushFile" ); 
    enter->m_Description[0]   = 0; 

    // make a block end packet for end of packet
    BlockEndPacket* blockEnd = (BlockEndPacket*) (m_PacketBuffer + m_PacketBufferOffset); 
    m_PacketBufferOffset += sizeof(BlockEndPacket); 

    blockEnd->m_Header.m_Command = PROFILE_CMD_BLOCK_END; 
    blockEnd->m_Header.m_Size    = sizeof(BlockEndPacket); 

    // we write the whole buffer, in large blocks
    m_TraceFile.Write( (const tchar_t*) m_PacketBuffer, PROFILE_PACKET_BLOCK_SIZE); 

    // reset the packet buffer
    m_PacketBufferOffset = 0; 

    // make a scope exit packet for being done flushing the file
    ScopeExitPacket* exit = (ScopeExitPacket*) (m_PacketBuffer + m_PacketBufferOffset); 
    m_PacketBufferOffset += sizeof(ScopeExitPacket); 

    exit->m_Header.m_Command = PROFILE_CMD_SCOPE_EXIT; 
    exit->m_Header.m_Size    = sizeof(ScopeExitPacket); 

    exit->m_UniqueID   = 0; 
    exit->m_StackDepth = 0; 
    exit->m_Duration   = Helium::TimerGetClock() - startTicks; 

    // return to filling out the packet buffer
}
