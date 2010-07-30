#include "JointOrdering.h"
#include "Pipeline/Content/ContentExceptions.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS( JointOrdering )

void JointOrdering::EnumerateClass( Reflect::Compositor<JointOrdering>& comp )
{
    Reflect::Field* fieldJointOrdering = comp.AddField( &JointOrdering::m_JointOrdering, "m_JointOrdering" );
}


JointOrdering::JointOrdering( u32 numRequiredJoints )
: SceneNode()
{
    SetRequiredJointCount( numRequiredJoints );
}
void JointOrdering::SetRequiredJointCount( u32 requiredJointCount )
{
    m_JointOrdering.resize( requiredJointCount );
}

u32 JointOrdering::GetRequiredJointCount()
{
    return (u32)m_JointOrdering.size();
}

void JointOrdering::AddJoint( const Helium::TUID& jointId )
{
    m_JointOrdering.push_back( jointId );
}

void JointOrdering::Clear()
{
    m_JointOrdering.clear();
}

bool JointOrdering::IsRequired( const Helium::TUID& jointId )
{
    return std::find( m_JointOrdering.begin(), m_JointOrdering.end(), jointId ) != m_JointOrdering.end();
}

Helium::TUID JointOrdering::GetMasterJoint( const Helium::TUID& localJoint )
{
    Helium::HM_TUID::iterator jointIt = m_LocalToMasterMap.find( localJoint );

    if ( jointIt == m_LocalToMasterMap.end() )
    {
        tstring localJointGuid;
        localJoint.ToString( localJointGuid );
        throw MissingJointException( localJointGuid, TXT( "could not find master joint for this local joint." ) );
    }  

    return jointIt->second;
}

Helium::TUID JointOrdering::GetLocalJoint( const Helium::TUID& masterJoint )
{
    Helium::HM_TUID::iterator jointIt = m_MasterToLocalMap.find( masterJoint );

    if ( jointIt == m_MasterToLocalMap.end() )
    {
        tstring masterJointGuid;
        masterJoint.ToString( masterJointGuid );
        throw MissingJointException( masterJointGuid, TXT( "could not find local joint for this master joint." ) );
    }  

    return jointIt->second;
}
