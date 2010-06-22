#include "Element.h"
#include "Archive.h"
#include "Registry.h"
#include "Version.h"
#include "Serializers.h"

using namespace Reflect;

REFLECT_DEFINE_ABSTRACT( Element );

void Element::EnumerateClass( Reflect::Compositor<Element>& comp )
{

}

Element::Element()
{

}

bool Element::ProcessComponent(ElementPtr element, const std::string& fieldName)
{
    return false; // incurs data loss
}

void Element::ToXML(std::string& xml) const
{
    Archive::ToXML(this, xml);
}

void Element::ToStream(std::iostream& stream, ArchiveType type) const
{
    Archive::ToStream(this, stream, type);
}

void Element::ToFile(const std::string& file, const VersionPtr& version) const
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
