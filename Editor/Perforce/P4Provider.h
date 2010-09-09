#pragma once

#include "Foundation/RCS/RCS.h"
#include "Foundation/RCS/Provider.h"
#include "Platform/Compiler.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Profile.h"
#include "Platform/Condition.h"
#include "Platform/Thread.h"
#include "Platform/Mutex.h"
#include "P4API.h"

namespace Helium
{
    namespace Perforce
    {
        //
        // Command transaction thread object
        //

        namespace CommandPhases
        {
            enum CommandPhase
            {
                Unknown,
                Executing,
                Connecting,
                Complete,
            };
        };
        typedef CommandPhases::CommandPhase CommandPhase;

        class WaitInterface
        {
        public:
            virtual ~WaitInterface();
            virtual bool StopWaiting() = 0;
        };

        struct WaitArgs
        {
            WaitArgs( WaitInterface* waitInterface )
                : m_WaitInterface( waitInterface )
                , m_Cancel( false )
            {

            }

            WaitInterface*  m_WaitInterface;
            bool            m_Cancel;
        };
        typedef Helium::Signature< WaitArgs& > WaitSignature;

        extern WaitSignature::Delegate g_ShowWaitDialog;

        struct MessageArgs
        {
            tstring m_Message;
            tstring m_Title;

            MessageArgs( const tstring& message, const tstring& title )
                : m_Message( message )
                , m_Title( title )
            {

            }
        };
        typedef Helium::Signature< const MessageArgs&> MessageSignature;
        extern MessageSignature::Delegate g_ShowWarningDialog;

#pragma warning (disable : 4275)
        class Provider : public RCS::Provider, public KeepAlive, public WaitInterface
#pragma warning (default : 4275)
        {
        public:
            Provider();
            ~Provider();

            void Initialize();
            void Cleanup();

        private:
            void ThreadEntry();

        public:
            void RunCommand( class Command* command );
            bool Connect();
            virtual int	IsAlive() HELIUM_OVERRIDE;
            virtual bool StopWaiting() HELIUM_OVERRIDE;

        public:
            //
            // RCS::Provider
            //

            virtual bool IsEnabled() HELIUM_OVERRIDE;
            virtual void SetEnabled( bool online ) HELIUM_OVERRIDE;

            virtual const char* GetName() HELIUM_OVERRIDE;

            virtual void Sync( RCS::File& file, const u64 timestamp = 0 ) HELIUM_OVERRIDE;

            virtual void GetInfo( RCS::File& file, const RCS::GetInfoFlag flags = RCS::GetInfoFlags::Default ) HELIUM_OVERRIDE;
            virtual void GetInfo( const tstring& folder, RCS::V_File& files, bool recursive = false, u32 fileData = RCS::FileData::All, u32 actionData = RCS::ActionData::All ) HELIUM_OVERRIDE;

            virtual void Add( RCS::File& file ) HELIUM_OVERRIDE;
            virtual void Edit( RCS::File& file ) HELIUM_OVERRIDE;
            virtual void Delete( RCS::File& file ) HELIUM_OVERRIDE;

            virtual void GetOpenedFiles( RCS::V_File& file ) HELIUM_OVERRIDE;

            virtual void Reopen( RCS::File& file ) HELIUM_OVERRIDE;

            virtual void Rename( RCS::File& source, RCS::File& dest ) HELIUM_OVERRIDE;
            virtual void Integrate( RCS::File& source, RCS::File& dest ) HELIUM_OVERRIDE;

            virtual void Revert( RCS::Changeset& changeset, bool revertUnchangedOnly = false ) HELIUM_OVERRIDE;
            virtual void Revert( RCS::File& file, bool revertUnchangedOnly = false ) HELIUM_OVERRIDE;

            virtual void Commit( RCS::Changeset& changeset ) HELIUM_OVERRIDE;

            virtual void CreateChangeset( RCS::Changeset& changeset ) HELIUM_OVERRIDE;
            virtual void GetChangesets( RCS::V_Changeset& changesets ) HELIUM_OVERRIDE;

        public:
            bool                  m_IsEnabled;
            bool                  m_IsConnected;
            Timer                 m_ConnectTimer;
            u32                   m_ConnectionTestTimeout;        // the time we are willing to wait to test if the server is running
            u32                   m_ForegroundExecuteTimeout;     // the timeout in the foreground thread before we open the wait dialog
            u32                   m_BackgroundExecuteTimeout;     // this is lame and we should consider opening a new connection per-calling thread?

            bool                  m_Abort;
            ClientApi             m_Client;
            tstring               m_UserName;
            tstring               m_ClientName;

        private:
            // transaction thread
            Helium::Thread        m_Thread;     // the thread to run commands in
            bool                  m_Shutdown;   // the shutdown signal
            Helium::Mutex         m_Mutex;      // to ensure thread safety
            Helium::Condition     m_Execute;    // to wakeup the command thread
            Helium::Condition     m_Completed;  // to wakeup the calling thread

            // command to execute
            class Command*        m_Command;    // the command to run
            CommandPhase          m_Phase;
        };
    }
}