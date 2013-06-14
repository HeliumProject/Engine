namespace Helium
{
#if 0
/// Constructor.
template< typename EntityUpdateJobType >
WorldManagerUpdate< EntityUpdateJobType >::WorldManagerUpdate()
{
}

/// Destructor.
template< typename EntityUpdateJobType >
WorldManagerUpdate< EntityUpdateJobType >::~WorldManagerUpdate()
{
}

/// Get the parameters for this job.
///
/// @return  Reference to the structure containing the job parameters.
///
/// @see SetParameters()
template< typename EntityUpdateJobType >
typename WorldManagerUpdate< EntityUpdateJobType >::Parameters& WorldManagerUpdate< EntityUpdateJobType >::GetParameters()
{
    return m_parameters;
}

/// Get the parameters for this job.
///
/// @return  Constant reference to the structure containing the job parameters.
///
/// @see SetParameters()
template< typename EntityUpdateJobType >
const typename WorldManagerUpdate< EntityUpdateJobType >::Parameters& WorldManagerUpdate< EntityUpdateJobType >::GetParameters() const
{
    return m_parameters;
}

/// Set the job parameters.
///
/// @param[in] rParameters  Structure containing the job parameters.
///
/// @see GetParameters()
template< typename EntityUpdateJobType >
void WorldManagerUpdate< EntityUpdateJobType >::SetParameters( const Parameters& rParameters )
{
    m_parameters = rParameters;
}

/// Callback executed to run the job.
///
/// @param[in] pJob      Job to run.
/// @param[in] pContext  Context associated with the running job instance.
template< typename EntityUpdateJobType >
void WorldManagerUpdate< EntityUpdateJobType >::RunCallback( void* pJob, JobContext* pContext )
{
    HELIUM_ASSERT( pJob );
    HELIUM_ASSERT( pContext );
    static_cast< WorldManagerUpdate* >( pJob )->Run( pContext );
}

/// Constructor.
template< typename EntityUpdateJobType >
WorldManagerUpdate< EntityUpdateJobType >::Parameters::Parameters()
    : startSliceIndex(0)
    , startEntityIndex(0)
{
}

/// Constructor.
EntityPreUpdate::EntityPreUpdate()
{
}

/// Destructor.
EntityPreUpdate::~EntityPreUpdate()
{
}

/// Get the parameters for this job.
///
/// @return  Reference to the structure containing the job parameters.
///
/// @see SetParameters()
EntityPreUpdate::Parameters& EntityPreUpdate::GetParameters()
{
    return m_parameters;
}

/// Get the parameters for this job.
///
/// @return  Constant reference to the structure containing the job parameters.
///
/// @see SetParameters()
const EntityPreUpdate::Parameters& EntityPreUpdate::GetParameters() const
{
    return m_parameters;
}

/// Set the job parameters.
///
/// @param[in] rParameters  Structure containing the job parameters.
///
/// @see GetParameters()
void EntityPreUpdate::SetParameters( const Parameters& rParameters )
{
    m_parameters = rParameters;
}

/// Callback executed to run the job.
///
/// @param[in] pJob      Job to run.
/// @param[in] pContext  Context associated with the running job instance.
void EntityPreUpdate::RunCallback( void* pJob, JobContext* pContext )
{
    HELIUM_ASSERT( pJob );
    HELIUM_ASSERT( pContext );
    static_cast< EntityPreUpdate* >( pJob )->Run( pContext );
}

/// Constructor.
EntityPreUpdate::Parameters::Parameters()
{
}


/// Constructor.
EntityPostUpdate::EntityPostUpdate()
{
}

/// Destructor.
EntityPostUpdate::~EntityPostUpdate()
{
}

/// Get the parameters for this job.
///
/// @return  Reference to the structure containing the job parameters.
///
/// @see SetParameters()
EntityPostUpdate::Parameters& EntityPostUpdate::GetParameters()
{
    return m_parameters;
}

/// Get the parameters for this job.
///
/// @return  Constant reference to the structure containing the job parameters.
///
/// @see SetParameters()
const EntityPostUpdate::Parameters& EntityPostUpdate::GetParameters() const
{
    return m_parameters;
}

/// Set the job parameters.
///
/// @param[in] rParameters  Structure containing the job parameters.
///
/// @see GetParameters()
void EntityPostUpdate::SetParameters( const Parameters& rParameters )
{
    m_parameters = rParameters;
}

/// Callback executed to run the job.
///
/// @param[in] pJob      Job to run.
/// @param[in] pContext  Context associated with the running job instance.
void EntityPostUpdate::RunCallback( void* pJob, JobContext* pContext )
{
    HELIUM_ASSERT( pJob );
    HELIUM_ASSERT( pContext );
    static_cast< EntityPostUpdate* >( pJob )->Run( pContext );
}

/// Constructor.
EntityPostUpdate::Parameters::Parameters()
{
}
#endif
}  // namespace Helium

