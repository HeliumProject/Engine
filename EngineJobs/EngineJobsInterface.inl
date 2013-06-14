namespace Helium
{

	/// Constructor.
	template< typename T, typename CompareFunction >
	SortJob< T, CompareFunction >::SortJob()
	{
	}

	/// Destructor.
	template< typename T, typename CompareFunction >
	SortJob< T, CompareFunction >::~SortJob()
	{
	}

	/// Get the parameters for this job.
	///
	/// @return  Reference to the structure containing the job parameters.
	///
	/// @see SetParameters()
	template< typename T, typename CompareFunction >
	typename SortJob< T, CompareFunction >::Parameters& SortJob< T, CompareFunction >::GetParameters()
	{
		return m_parameters;
	}

	/// Get the parameters for this job.
	///
	/// @return  Constant reference to the structure containing the job parameters.
	///
	/// @see SetParameters()
	template< typename T, typename CompareFunction >
	const typename SortJob< T, CompareFunction >::Parameters& SortJob< T, CompareFunction >::GetParameters() const
	{
		return m_parameters;
	}

	/// Set the job parameters.
	///
	/// @param[in] rParameters  Structure containing the job parameters.
	///
	/// @see GetParameters()
	template< typename T, typename CompareFunction >
	void SortJob< T, CompareFunction >::SetParameters( const Parameters& rParameters )
	{
		m_parameters = rParameters;
	}

	/// Callback executed to run the job.
	///
	/// @param[in] pJob      Job to run.
	/// @param[in] pContext  Context associated with the running job instance.
	template< typename T, typename CompareFunction >
	void SortJob< T, CompareFunction >::RunCallback( void* pJob, JobContext* pContext )
	{
		HELIUM_ASSERT( pJob );
		HELIUM_ASSERT( pContext );
		static_cast< SortJob* >( pJob )->Run( pContext );
	}

	/// Constructor.
	template< typename T, typename CompareFunction >
	SortJob< T, CompareFunction >::Parameters::Parameters()
		: singleJobCount(24)
	{
	}

}  // namespace Helium

