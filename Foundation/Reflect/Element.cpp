#include "Element.h"
#include "Archive.h"
#include "Registry.h"
#include "Version.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

#include "ArchiveXML.h"
#include "ArchiveBinary.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( Element );

Element::Element()
{

}

void Element::EnumerateClass( Reflect::Compositor<Element>& comp )
{

}

const tstring& Element::GetTitle() const
{
    return GetClass()->m_UIName;
}

bool Element::IsCompact() const
{
    return false;
}

bool Element::ProcessComponent(ElementPtr element, const tstring& fieldName)
{
    return false; // incurs data loss
}

void Element::ToXML(tstring& xml) const
{
#pragma TODO( "Fix const correctness." )
    ArchiveXML::ToString( const_cast< Element* >( this ), xml );
}

void Element::ToBinary(std::iostream& stream) const
{
#pragma TODO( "Fix const correctness." )
    ArchiveBinary::ToStream( const_cast< Element* >( this ), stream );
}

void Element::ToFile( const Path& path ) const
{
    ArchivePtr archive = GetArchive( path );
#pragma TODO( "Fix const correctness." )
    archive->Put( const_cast< Element* >( this ) );
    archive->Close();
}

void Element::Accept(Visitor& visitor)
{
    Composite::Visit(this, visitor);
}

bool Element::Equals(const ElementPtr& rhs) const
{
    return Composite::Equals(this, rhs);
}

void Element::CopyTo(const ElementPtr& destination)
{
    Composite::Copy( this, destination );
}

ElementPtr Element::Clone()
{
    ElementPtr clone;

    clone = Class::Clone( this );

    return clone;
}
