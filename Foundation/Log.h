#pragma once

#include <string>

#include "Platform/Types.h"
#include "Foundation/API.h"
#include "Foundation/Atomic.h"
#include "Foundation/Automation/Event.h"

namespace Helium
{
    namespace Log
    {
        const static u32 MAX_PRINT_SIZE = 8192;


        //
        // Color coding
        //

        namespace Colors
        {
            enum Color
            {
                Auto   = 0x0,
                None   = 0xffffffff,

                // from wincon.h
                Red    = 0x0004,
                Green  = 0x0002,
                Blue   = 0x0001,

                Yellow = Red | Green,
                Aqua   = Green | Blue,
                Purple = Blue | Red,

                White  = Red | Green | Blue,
            };
        }
        typedef Colors::Color Color;


        //
        // Output streams, these speak to the qualitative value of the print:
        //  - normal is any print that updates the status of a process to a normal user
        //  - debug is any print that is only really meaningful to developers, and not indicative of a problem
        //  - warning is notification that there is a minor problem happening, but things will turn out OK
        //  - error is notification that there is a serious problem and something needs attention to work as expected
        //

        namespace Streams
        {
            enum Stream
            {
                Normal   = 1 << 0,
                Debug    = 1 << 1,
                Profile  = 1 << 2,
                Warning  = 1 << 3,
                Error    = 1 << 4,

                Count    = 5, // careful, this needs to be updated based on the number of streams
                All      = 0xFFFFFFFF,
            };
        }

        typedef u32 Stream; 

        //
        // Verbosity levels, these speak to the quantitative value of the print:
        //  - basic is any print that denotes what is happening on a large scale within the program
        //  - advanced is any print that provides additional detail within a large scale section of code within the program, and is optional
        //  - gratuitous is any print that is so dense in occurance that both users and developers will very rarely need to read it
        //

        namespace Levels
        {
            enum Level
            {
                Default  = 1 << 0, // EG. 'Processing texture...' or 'Copying data...'
                Verbose  = 1 << 1, // EG. 'Adding object ratchet_clone to level data...'
                Extreme  = 1 << 2, // EG. 'Calculating binormal for polygon 3554...'
            };
        }
        typedef Levels::Level Level;


        //
        // Printing event API allows for in-process APIs to handle print events themselves
        //

        struct FOUNDATION_API Statement
        {
            tstring     m_String;
            Stream      m_Stream;
            Level       m_Level;
            int         m_Indent;

            Statement( const tstring& string, Stream stream = Streams::Normal, Level level = Levels::Default, int indent = 0 )
                : m_String( string )
                , m_Stream( stream )
                , m_Level( level )
                , m_Indent( indent )
            {

            }

            void ApplyIndent()
            {
                tstring indented;
                ApplyIndent( m_String.c_str(), indented );
                m_String = indented;
            }

            void ApplyIndent( const tchar* string, tstring& output );
        };

        typedef std::vector< Statement > V_Statement;


        //
        // Printing event
        //

        struct FOUNDATION_API PrintingArgs
        {
            const Statement&  m_Statement;
            bool              m_Skip;

            PrintingArgs( const Statement& statement )
                : m_Statement ( statement )
                , m_Skip( false )
            {

            }
        };

        typedef Helium::Signature<void, PrintingArgs&, Helium::AtomicRefCountBase> PrintingSignature;

        FOUNDATION_API void AddPrintingListener(const PrintingSignature::Delegate& listener);
        FOUNDATION_API void RemovePrintingListener(const PrintingSignature::Delegate& listener);


        //
        // Printed event
        //

        struct FOUNDATION_API PrintedArgs
        {
            const Statement&  m_Statement;

            PrintedArgs( const Statement& statement )
                : m_Statement ( statement )
            {

            }
        };

        typedef Helium::Signature<void, PrintedArgs&, Helium::AtomicRefCountBase> PrintedSignature;

        FOUNDATION_API void AddPrintedListener(const PrintedSignature::Delegate& listener);
        FOUNDATION_API void RemovePrintedListener(const PrintedSignature::Delegate& listener);


        //
        // Tracing API handles echoing all output to the trace text file associated with the process
        //

        // the trace file gets everything Console delivers to the console and more
        FOUNDATION_API bool AddTraceFile( const tstring& fileName, Stream stream, u32 threadId = -1, bool append = false );
        FOUNDATION_API void RemoveTraceFile( const tstring& fileName );

        template <bool (*AddFunc)(const tstring& fileName, Stream stream, u32 threadId, bool append), void (*RemoveFunc)(const tstring& fileName)>
        class FileHandle
        {
        private:
            tstring m_File;

        public:
            FileHandle(const tstring& file, Stream stream, u32 threadId = -1, bool append = false )
                : m_File (file)
            {
                if (!m_File.empty())
                {
                    AddFunc(m_File, stream, threadId, append);
                }
            }

            ~FileHandle()
            {
                if (!m_File.empty())
                {
                    RemoveFunc(m_File);
                }
            }

            const tstring& GetFile()
            {
                return m_File;
            }
        };

        typedef FileHandle<&AddTraceFile, &RemoveTraceFile> TraceFileHandle;

        //
        // Indenting API causes all output to be offset by whitespace
        //

        // indent all output
        FOUNDATION_API void Indent(int col = -1);

        // unindent all output
        FOUNDATION_API void UnIndent(int col = -1);


        //
        // Tracking APIs configure what streams and levels to use, and allows access to warning/error counters
        //

        // verbosity setting
        FOUNDATION_API Level GetLevel();
        FOUNDATION_API void SetLevel(Level level);

        // enable stream calls
        FOUNDATION_API bool IsStreamEnabled( Stream stream );
        FOUNDATION_API void EnableStream( Stream stream, bool enable );

        // get the print color for the given stream
        FOUNDATION_API Color GetStreamColor(Stream stream);

        // get the current counts
        FOUNDATION_API int GetErrorCount();
        FOUNDATION_API int GetWarningCount();

        // reset the counts to zero
        FOUNDATION_API void ResetErrorCount();
        FOUNDATION_API void ResetWarningCount();

        // enter/leave this library's section
        FOUNDATION_API void LockMutex();
        FOUNDATION_API void UnlockMutex();


        //
        // Printing APIs are the heart of Console
        //

        // main printing function used by all prototypes
        FOUNDATION_API void PrintString(const tchar* string,                // the string to print
            Stream stream = Streams::Normal,   // the stream its going into
            Level level = Levels::Default,     // the verbosity level
            Color color = Colors::Auto,        // the color to use (None for auto)
            int indent = -1,                   // the amount to indent
            tchar* output = NULL,               // the buffer to copy the result string to
            u32 outputSize = 0);               // the size of the output buffer

        // print a persisted print statment
        FOUNDATION_API void PrintStatement(const Statement& statement);

        // print several statements
        FOUNDATION_API void PrintStatements(const V_Statement& statements, u32 streams = Streams::All);

        // simple way to print a particular color
        FOUNDATION_API void PrintColor(Color color, const tchar* fmt, ...);

        // make a print statement
        FOUNDATION_API void Print(const tchar *fmt,...);
        FOUNDATION_API void Print(Level level, const tchar *fmt,...);

        // make a debug-only statement
        FOUNDATION_API void Debug(const tchar *fmt,...);
        FOUNDATION_API void Debug(Level level, const tchar *fmt,...);

        // make a profile-only statement
        FOUNDATION_API void Profile(const tchar *fmt,...);
        FOUNDATION_API void Profile(Level level, const tchar *fmt,...);

        // warn the user, increments warning count
        FOUNDATION_API void Warning(const tchar *fmt,...);
        FOUNDATION_API void Warning(Level level, const tchar *fmt,...);

        // give an error, increments error count
        FOUNDATION_API void Error(const tchar *fmt,...);
        FOUNDATION_API void Error(Level level, const tchar *fmt,...);

        // stack-based indention helper object indents all output while on the stack
        class FOUNDATION_API Indentation
        {
        public:
            Indentation()
            {
                Indent();
            }

            ~Indentation()
            {
                UnIndent();
            }
        };

        // like an indentation, but prints to the basic output stream the name of the heading
        class FOUNDATION_API Heading
        {
        public:
            Heading(const tchar *fmt, ...);
            ~Heading();
        };

        // like a heading but preceeded with a delimiter (o, *, -, etc...)
        class FOUNDATION_API Bullet
        {
        private:
            Stream m_Stream;
            Level  m_Level;
            bool   m_Valid;

        public:
            Bullet(const tchar *fmt, ...);
            Bullet(Stream stream, Level level, const tchar *fmt, ...);

            ~Bullet();

        private:
            void CreateBullet(const tchar *fmt, va_list args );
        };

        // grab the path to the current bullet
        FOUNDATION_API tstring GetOutlineState();

        class FOUNDATION_API Listener
        {
        public:
            Listener( u32 throttle = Log::Streams::All, u32* errorCount = NULL, u32* warningCount = NULL, Log::V_Statement* consoleOutput = NULL );
            ~Listener();

            void Start();
            void Stop();
            void Dump(bool stop = true);

            u32 GetWarningCount();
            u32 GetErrorCount();

        private:
            void Print( Log::PrintingArgs& args );

        private:
            u32                   m_Thread;
            u32                   m_Throttle;
            u32*                  m_ErrorCount;
            u32*                  m_WarningCount;
            Log::V_Statement*     m_LogOutput;
        };
    }
}