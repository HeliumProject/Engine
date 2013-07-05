namespace Helium
{
	bool SequenceStepTransition::operator==( const SequenceStepTransition& _rhs ) const
	{
		return ( 
			m_NextStepName == _rhs.m_NextStepName &&
			m_MinimumTimeInState == _rhs.m_MinimumTimeInState
			);
	}

	bool SequenceStepTransition::operator!=( const SequenceStepTransition& _rhs ) const
	{
		return !( *this == _rhs );
	}

	bool SequenceStep::operator==( const SequenceStep& _rhs ) const
	{
		return ( 
			m_Transitions == _rhs.m_Transitions &&
			m_StepName == _rhs.m_StepName &&
			m_StateFlags == _rhs.m_StateFlags
			);
	}

	bool SequenceStep::operator!=( const SequenceStep& _rhs ) const
	{
		return !( *this == _rhs );
	}
}