#pragma once

#include "TestJobs/TestJobs.h"
#include "Platform/Assert.h"

namespace Helium
{
    class JobContext;
}

namespace Helium
{

class HELIUM_TEST_JOBS_API FibJob : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
        /// [in] 
        uint32_t n;
        /// [inout] 
        uint32_t* pSum;

        /// @name Construction/Destruction
        //@{
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline FibJob();
    inline ~FibJob();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

class HELIUM_TEST_JOBS_API FibContinuation : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
        /// [in] 
        uint32_t x;
        /// [in] 
        uint32_t y;
        /// [inout] 
        uint32_t* pSum;

        /// @name Construction/Destruction
        //@{
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline FibContinuation();
    inline ~FibContinuation();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

}  // namespace Helium

#include "TestJobs/TestJobsInterface.inl"
