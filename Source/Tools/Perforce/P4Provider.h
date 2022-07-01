#pragma once

#include "Application/RCS.h"
#include "Application/RCSProvider.h"
#include "Foundation/Event.h"
#include "Foundation/Profile.h"
#include "Platform/Condition.h"
#include "Platform/Thread.h"
#include "Platform/Locks.h"
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
            std::string m_Message;
            std::string m_Title;

            MessageArgs( const std::string& message, const std::string& title )
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
            virtual int	IsAlive() override;
            virtual bool StopWaiting() override;

        public:
            //
            // RCS::Provider
            //

            virtual bool IsEnabled() override;
            virtual void SetEnabled( bool online ) override;

            virtual const char* GetName() override;

            virtual void Sync( RCS::File& file, const uint64_t timestamp = 0 ) override;

            virtual void GetInfo( RCS::File& file, const RCS::GetInfoFlag flags = RCS::GetInfoFlags::Default ) override;
            virtual void GetInfo( const std::string& folder, RCS::V_File& files, bool recursive = false, uint32_t fileData = RCS::FileData::All, uint32_t actionData = RCS::ActionData::All ) override;

            virtual void Add( RCS::File& file ) override;
            virtual void Edit( RCS::File& file ) override;
            virtual void Delete( RCS::File& file ) override;

            virtual void GetOpenedFiles( RCS::V_File& file ) override;

            virtual void Reopen( RCS::File& file ) override;

            virtual void Rename( RCS::File& source, RCS::File& dest ) override;
            virtual void Integrate( RCS::File& source, RCS::File& dest ) override;

            virtual void Revert( RCS::Changeset& changeset, bool revertUnchangedOnly = false ) override;
            virtual void Revert( RCS::File& file, bool revertUnchangedOnly = false ) override;

            virtual void Commit( RCS::Changeset& changeset ) override;

            virtual void CreateChangeset( RCS::Changeset& changeset ) override;
            virtual void GetChangesets( RCS::V_Changeset& changesets ) override;

        public:
            bool                  m_IsEnabled;
            bool                  m_IsInitialized;
            bool                  m_IsConnected;
            SimpleTimer           m_ConnectTimer;
            uint32_t              m_ConnectionTestTimeout;        // the time we are willing to wait to test if the server is running
            uint32_t              m_ForegroundExecuteTimeout;     // the timeout in the foreground thread before we open the wait dialog
            uint32_t              m_BackgroundExecuteTimeout;     // this is lame and we should consider opening a new connection per-calling thread?

            bool                  m_Abort;
            ClientApi             m_Client;
            std::string               m_UserName;
            std::string               m_ClientName;

        private:
            // transaction thread
            Helium::CallbackThread        m_Thread;     // the thread to run commands in
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