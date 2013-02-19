
namespace Helium
{
    Helium::Component *ComponentDefinition::CreateComponent(struct Components::ComponentSet &_target) const
    {
        m_Instance.Set(CreateComponentInternal(_target));
        return m_Instance.Get();
    }
    
    Helium::Component *ComponentDefinition::CreateComponentInternal(struct Components::ComponentSet &_target) const 
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
