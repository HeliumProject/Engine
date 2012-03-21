//----------------------------------------------------------------------------------------------------------------------
// TestJobsInterface.inl
//
// Copyright (C) 2012 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{

/// Constructor.
FibJob::FibJob()
{
}

/// Destructor.
FibJob::~FibJob()
{
}

/// Get the parameters for this job.
///
/// @return  Reference to the structure containing the job parameters.
///
/// @see SetParameters()
FibJob::Parameters& FibJob::GetParameters()
{
    return m_parameters;
}

/// Get the parameters for this job.
///
/// @return  Constant reference to the structure containing the job parameters.
///
/// @see SetParameters()
const FibJob::Parameters& FibJob::GetParameters() const
{
    return m_parameters;
}

/// Set the job parameters.
///
/// @param[in] rParameters  Structure containing the job parameters.
///
/// @see GetParameters()
void FibJob::SetParameters( const Parameters& rParameters )
{
    m_parameters = rParameters;
}

/// Callback executed to run the job.
///
/// @param[in] pJob      Job to run.
/// @param[in] pContext  Context associated with the running job instance.
void FibJob::RunCallback( void* pJob, JobContext* pContext )
{
    HELIUM_ASSERT( pJob );
    HELIUM_ASSERT( pContext );
    static_cast< FibJob* >( pJob )->Run( pContext );
}

/// Constructor.
FibJob::Parameters::Parameters()
    : n(0)
    , pSum(NULL)
{
}

/// Constructor.
FibContinuation::FibContinuation()
{
}

/// Destructor.
FibContinuation::~FibContinuation()
{
}

/// Get the parameters for this job.
///
/// @return  Reference to the structure containing the job parameters.
///
/// @see SetParameters()
FibContinuation::Parameters& FibContinuation::GetParameters()
{
    return m_parameters;
}

/// Get the parameters for this job.
///
/// @return  Constant reference to the structure containing the job parameters.
///
/// @see SetParameters()
const FibContinuation::Parameters& FibContinuation::GetParameters() const
{
    return m_parameters;
}

/// Set the job parameters.
///
/// @param[in] rParameters  Structure containing the job parameters.
///
/// @see GetParameters()
void FibContinuation::SetParameters( const Parameters& rParameters )
{
    m_parameters = rParameters;
}

/// Callback executed to run the job.
///
/// @param[in] pJob      Job to run.
/// @param[in] pContext  Context associated with the running job instance.
void FibContinuation::RunCallback( void* pJob, JobContext* pContext )
{
    HELIUM_ASSERT( pJob );
    HELIUM_ASSERT( pContext );
    static_cast< FibContinuation* >( pJob )->Run( pContext );
}

/// Constructor.
FibContinuation::Parameters::Parameters()
    : x(0)
    , y(0)
    , pSum(NULL)
{
}

}  // namespace Helium

