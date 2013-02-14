
#include "FrameworkPch.h"
#include "ComponentSet.h"

HELIUM_IMPLEMENT_OBJECT(Helium::ComponentSet, Framework, 0);

struct NewComponent
{
    Helium::Name m_Name;
    Helium::Component *m_Component;
    Helium::StrongPtr<Helium::ComponentDescriptor> m_Descriptor;
};

void Helium::ComponentFactory::DeployComponents( ComponentSet &set, Helium::Components::ComponentSet &_components )
{
    // 1. Clone all component descriptors
    DynamicArray<NewComponent> components;

    // For each descriptor
    for (size_t i = 0; i < set.m_Descriptors.GetSize(); ++i)
    {
        // Clone it
        Reflect::ObjectPtr object_ptr = set.m_Descriptors[i].m_ComponentDescriptor->Clone();
        Helium::ComponentDescriptorPtr descriptor_ptr = Reflect::AssertCast<Helium::ComponentDescriptor>(object_ptr.Get());

        // Add it to the list
        NewComponent component;
        component.m_Name = set.m_Descriptors[i].m_ComponentName;
        component.m_Descriptor = descriptor_ptr;
        components.Add(component);

        Log::Print("%s cloned to %x\n", component.m_Name.Get(), component.m_Descriptor.Get());
    }

    //2. Link the descriptors
    for (size_t i = 0; i < components.GetSize(); ++i)
    {
        for (size_t linkage_index = 0; linkage_index < set.m_Linkages.GetSize(); ++linkage_index)
        {
            // If there is a linkage that references this component
            if (set.m_Linkages[linkage_index].m_OtherComponentName == components[i].m_Name)
            {
                // Find the component that needs the link
                for (size_t j = 0; j < components.GetSize(); j++)
                {
                    if (components[j].m_Name == set.m_Linkages[linkage_index].m_ComponentName)
                    {
                        // And link it
                        uint32_t fieldNameCrc = Crc32( set.m_Linkages[linkage_index].m_ComponentFieldName.Get() );

                        const Helium::Reflect::Field *field = components[j].m_Descriptor->GetClass()->FindFieldByName(fieldNameCrc);

                        if (field)
                        {
                            Helium::Reflect::DataPtr data = field->CreateData(components[j].m_Descriptor.Get());
                            Helium::GameObjectPointerData *gopd = Reflect::SafeCast<Helium::GameObjectPointerData>(data.Get());

                            if (gopd)
                            {
                                Helium::GameObjectPointerData my_gopd;
                                my_gopd.ConnectData(&components[i].m_Descriptor);
                                gopd->Set(&my_gopd, Reflect::DataFlags::Shallow);
                            }
                        }
                    }
                }
            }
        }
    }

    // 3. For each named parameter Apply override parameters to descriptors
    // 4. Create components and populate component table
    // 5. Destroy descriptor clones
    // 6. Make each component point back to the original descriptor that made it? Don't destroy the clones in TOOLS builds?
}

void Helium::ComponentSet::AddDescriptor( Helium::Name _name, Helium::StrongPtr<Helium::ComponentDescriptor> _descriptor )
{
    DescriptorListEntry entry;
    entry.m_ComponentName = _name;
    entry.m_ComponentDescriptor = _descriptor;
    m_Descriptors.Add(entry);
}

void Helium::ComponentSet::AddLinkage( Helium::Name _to, Helium::Name _from, Helium::Name _field_name )
{
    Linkage l;
    l.m_OtherComponentName = _to;
    l.m_ComponentName = _from;
    l.m_ComponentFieldName = _field_name;
    m_Linkages.Add(l);
}
