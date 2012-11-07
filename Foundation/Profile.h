#pragma once 

#include "Platform/Types.h"
#include "Platform/Profile.h"

#include "Foundation/API.h"
#include "Foundation/Memory.h"

#define PROFILE_STRINGIFY(x) #x
#define PROFILE_TOSTRING(x) PROFILE_STRINGIFY(x)

#define PROFILE_ACCUMULATOR_MAX         (2048)
#define PROFILE_CONTEXTS_MAX            (128)

#define PROFILE_PROTOCOL_VERSION        (0x00)
#define PROFILE_SIGNATURE               (0x12345678)

#define PROFILE_CMD_INIT                (0x00)
#define PROFILE_CMD_SCOPE_ENTER         (0x01)
#define PROFILE_CMD_SCOPE_EXIT          (0x02)
#define PROFILE_CMD_BLOCK_END           (0x03)

#define PROFILE_PACKET_STRING_BUFSIZE   (64)
#define PROFILE_CYCLES_FOR_CONVERSION   (100000)
#define PROFILE_PACKET_BLOCK_SIZE       (16 * 1024)

//
// Profile code API, for the most part almost all of this code always gets compiled in
// In general its beneficial to leave the accumulation API on all the time.
//

namespace Helium
{
    namespace Profile
    {
        namespace Settings
        {
            HELIUM_FOUNDATION_API bool Enabled();
            HELIUM_FOUNDATION_API bool MemoryProfilingEnabled();
        }

        const static int MAX_DESCRIPTION = 256;

        HELIUM_FOUNDATION_API void Initialize(); 
        HELIUM_FOUNDATION_API void Cleanup(); 

        //
        // Accumulates information over multiple calls
        //

        class HELIUM_FOUNDATION_API Accumulator
        {
        public:
            uint32_t   m_Hits; 
            float m_TotalMillis; 

            int32_t   m_Index;
            char  m_Name[MAX_DESCRIPTION];

            Accumulator(); 
            Accumulator(const char* name);
            Accumulator (const char* function, const char* name);
            ~Accumulator();

            void Init(const char* name);
            void Report();

            static void ReportAll();

        private: 
        };

        //
        // Scope timer prints or logs information
        //

        class HELIUM_FOUNDATION_API ScopeTimer
        {
        public: 
            ScopeTimer(Accumulator* accum, const char* func, uint32_t line, const tchar_t* desc = NULL); 
            ~ScopeTimer(); 

            tchar_t      m_Description[MAX_DESCRIPTION]; 
            uint64_t     m_StartTicks; 
            Accumulator* m_Accum; 
            uint32_t     m_UniqueID; 
            bool         m_Print; 

        private: 
            ScopeTimer(const ScopeTimer& rhs);  // no implementation

        };

        struct Header
        {
            uint16_t m_Command; 
            uint16_t m_Size; 
        }; 

        struct InitPacket 
        {
            Header m_Header; 
            uint32_t    m_Version; 
            uint32_t    m_Signature; 
            float32_t   m_Conversion; // PROFILE_CYCLES_FOR_CONVERSION cycles -> how many millis?
        }; 

        struct ScopeEnterPacket
        {
            Header      m_Header; 
            uint32_t    m_UniqueID; 
            uint32_t    m_StackDepth; 
            uint32_t    m_Line; 
            uint64_t    m_StartTicks; 
            char        m_Description[PROFILE_PACKET_STRING_BUFSIZE]; 
            char        m_Function[PROFILE_PACKET_STRING_BUFSIZE]; 
        }; 

        struct ScopeExitPacket 
        {
            Header      m_Header;
            uint32_t    m_UniqueID;   
            uint32_t    m_StackDepth; 
            uint64_t    m_Duration; 
        }; 

        struct BlockEndPacket
        {
            Header      m_Header; 
        };

        union UberPacket
        {
            Header           m_Header; 
            InitPacket       m_Init; 
            ScopeEnterPacket m_ScopeEnter; 
            ScopeExitPacket  m_ScopeExitPacket; 
        };

        class HELIUM_FOUNDATION_API Context
        {
        public:
            Helium::TraceFile   m_TraceFile; 
            uint32_t            m_UniqueID; 
            uint32_t            m_StackDepth; 
            uint32_t            m_PacketBufferOffset; 
            uint8_t             m_PacketBuffer[PROFILE_PACKET_BLOCK_SIZE]; 
            uint32_t            m_AccumStack[PROFILE_ACCUMULATOR_MAX]; 

            Context(); 
            ~Context(); 

            void FlushFile(); 

            template <class T>
            T* AllocPacket(uint32_t cmd)
            {
                uint32_t spaceNeeded = sizeof(T) + sizeof(BlockEndPacket) + sizeof(ScopeEnterPacket); 

                if (m_PacketBufferOffset + spaceNeeded >= PROFILE_PACKET_BLOCK_SIZE)
                {
                    FlushFile(); 
                }

                T* packet = (T*) (m_PacketBuffer + m_PacketBufferOffset); 
                m_PacketBufferOffset += sizeof(T); 

                //Log::Print("CMD %d OFFSET %d\n", cmd, m_PacketBufferOffset); 

                packet->m_Header.m_Command = cmd; 
                packet->m_Header.m_Size    = sizeof(T); 

                return packet; 
            }
        }; 
    }
}

// profile flag check
#ifdef PROFILE_ENABLE
# error PROFILE_ENABLE flag used by another module
#endif

// master profile enable
#define PROFILE_ENABLE

//
// Accumulation API stashes time taken in each profile tag over the course of the entire profile interval
//

// profile flag check
#ifdef PROFILE_ACCUMULATION
# error PROFILE_ACCUMULATION flag used by another module
#endif

// accumulation api enable
#ifdef PROFILE_ENABLE
# define PROFILE_ACCUMULATION
#endif

#define PROFILE_ACCUMULATION


// accumulation macros
#ifdef PROFILE_ACCUMULATION
# define PROFILE_SCOPE_ACCUM(__Accum) \
    Profile::ScopeTimer __ScopeAccum ( &__Accum, __FUNCTION__, __LINE__); 
# define PROFILE_SCOPE_ACCUM_VERBOSE(__Accum, __Str) \
    Profile::ScopeTimer __ScopeAccum ( &__Accum, __FUNCTION__, __LINE__, Profile::Settings::Enabled() ? __Str : NULL );
#else
# define PROFILE_SCOPE_ACCUM(__Accum)
# define PROFILE_SCOPE_ACCUM_VERBOSE(__Accum, __Str)
#endif


//
// Instrumentation API pervades more code blocks and provides fine-grain profile data to a log file
//

// profile flag check
#ifdef PROFILE_INSTRUMENTATION
# error PROFILE_INSTRUMENTATION flag used by another module
#endif

// instrumentation api enable
#ifdef PROFILE_ENABLE
// uncomment this to enable instrumentation
//# define PROFILE_INSTRUMENTATION
#endif

// flag to instrument all code possible
#ifdef PROFILE_INSTRUMENTATION
// uncomment this to enable instrumentation everywhere
//# define PROFILE_INSTRUMENT_ALL
#endif

#if defined(PROFILE_INSTRUMENTATION) && defined(WIN32)
# pragma message (" Profile Instrumentation Enabled")
#endif

// instrumentation macros
#ifdef PROFILE_INSTRUMENTATION

# define PROFILE_FUNCTION_TIMER() \
    static Profile::Accumulator __Accumulator ( __FUNCTION__ ); \
    Profile::ScopeTimer __ScopeTimer ( &__Accumulator, __FUNCTION__, 0, __FILE__ );

# define PROFILE_SCOPE_TIMER(__Description) \
    static Profile::Accumulator __Accumulator ( __FUNCTION__, ":" PROFILE_TOSTRING(__LINE__) ); \
    Profile::ScopeTimer __ScopeTimer ( &__Accumulator, __FUNCTION__, __LINE__, __Description );

#else

# define PROFILE_FUNCTION_TIMER()
# define PROFILE_SCOPE_TIMER(__Description)

#endif
