namespace Helium
{
	bool StateTransition::operator==( const StateTransition& _rhs ) const
	{
		return ( 
			m_NextStateName == _rhs.m_NextStateName &&
			m_MinimumTimeInState == _rhs.m_MinimumTimeInState
			);
	}

	bool StateTransition::operator!=( const StateTransition& _rhs ) const
	{
		return !( *this == _rhs );
	}

	bool State::operator==( const State& _rhs ) const
	{
		return ( 
			m_Transitions == _rhs.m_Transitions &&
			m_StateName == _rhs.m_StateName &&
			m_StateFlags == _rhs.m_StateFlags
			);
	}

	bool State::operator!=( const State& _rhs ) const
	{
		return !( *this == _rhs );
	}
}