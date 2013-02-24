
namespace Helium
{
    Helium::Component *ComponentDefinition::CreateComponent(struct Components::IHasComponents &rHasComponents) const
    {
        m_Instance.Set(CreateComponentInternal(rHasComponents));
        return m_Instance.Get();
    }
    
    Helium::Component *ComponentDefinition::CreateComponentInternal(struct Components::IHasComponents &rHasComponents) const 
    { 
        HELIUM_ASSERT(0); 
        return 0; 
    }
    
    void ComponentDefinition::FinalizeComponent() const 
    { 
    }
        
    Helium::Component *ComponentDefinition::GetCreatedComponent() const 
    { 
        return m_Instance.Get(); 
    }
}
