
namespace Helium
{

	HasPhysicalContactsComponent *BulletBodyComponent::GetOrCreateHasPhysicalContactsComponent()
	{
		if ( !m_HasPhysicalContactsComponent.IsGood() )
		{
			ComponentCollection *pCollection = GetComponentCollection();
			HELIUM_ASSERT( !pCollection->GetFirst<HasPhysicalContactsComponent>() );
			m_HasPhysicalContactsComponent.Reset( AllocateSiblingComponent<HasPhysicalContactsComponent>() );
		}

		return m_HasPhysicalContactsComponent.Get();
	}

	bool BulletBodyComponent::GetShouldTrackPhysicalContact( BulletBodyComponent *pOther )
	{
		return (m_TrackPhysicalContactGroupMask & pOther->m_AssignedGroups) != 0;
	}
}
