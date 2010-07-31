#include "Element.h"
#include "Archive.h"
#include "Registry.h"
#include "Version.h"
#include "Serializers.h"

#include "ArchiveXML.h"
#include "ArchiveBinary.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( Element );

void Element::EnumerateClass( Reflect::Compositor<Element>& comp )
{

}

Element::Element()
{

}

bool Element::ProcessComponent(ElementPtr element, const tstring& fieldName)
{
    return false; // incurs data loss
}

void Element::ToXML(tstring& xml) const
{
    ArchiveXML::ToString(this, xml);
}

void Element::ToBinary(std::iostream& stream) const
{
    ArchiveBinary::ToStream(this, stream);
}

void Element::ToFile(const tstring& file, const VersionPtr& version) const
{
    Archive::ToFile(this, file, version);
}

void Element::Host(Visitor& visitor)
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
