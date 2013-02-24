
namespace Helium
{
    Helium::Component *ComponentDefinition::CreateComponent(struct Components::ComponentSet &target) const
    {
        m_Instance.Set(CreateComponentInternal(target));
        return m_Instance.Get();
    }
    
    Helium::Component *ComponentDefinition::CreateComponentInternal(struct Components::ComponentSet &target) const 
    { 
        HELIUM_ASSERT(0); 
        return 0; 
    }
    
    void ComponentDefinition::FinalizeComponent(Entity *pEntity) const 
    { 
    }
        
    Helium::Component *ComponentDefinition::GetCreatedComponent() const 
    { 
        return m_Instance.Get(); 
    }
}
