
#include "ComponentsPch.h"
#include "Engine/Components.h"
#include "Components/ComponentJobs.h"
#include <limits>
#include <vector>

#include "Foundation/DynamicArray.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RotateComponent.h"

using namespace Helium;

struct FoundComponentList
{
    DynamicArray<Component *> m_Components;
    size_t m_TypeIndex;
    size_t m_Count;
    Components::TypeId m_TypeId;
};

bool SortFoundComponentList(FoundComponentList &lhs, FoundComponentList &rhs)
{
    return lhs.m_Count < rhs.m_Count;
}

typedef void (*ComponentTupleCallback)(DynamicArray<Component *> &tuple);

struct ITupleCallback
{
    virtual void HandleTupleInternal(DynamicArray<Component *> &components) = 0;
};

void EmitTuples(DynamicArray<Component *> &tuple, std::vector<FoundComponentList> &found_components, size_t type_index, ITupleCallback *emit_tuple_callback)
{
    DynamicArray<Component *> &this_type_components = found_components[type_index].m_Components;
    for (DynamicArray<Component *>::Iterator iter = this_type_components.Begin();
        iter != this_type_components.End(); ++iter)
    {
        tuple[type_index] = *iter;
        if (type_index < found_components.size() - 1)
        {
            EmitTuples(tuple, found_components, ++type_index, emit_tuple_callback);
        }
        else
        {
            emit_tuple_callback->HandleTupleInternal(tuple);
        }
    }
}

void QueryComponents(const DynamicArray<Components::TypeId> &types, ITupleCallback *emit_tuple_callback)
{
    // If no types to query, do nothing
    if (types.IsEmpty())
    {
        return;
    }
    
    // Prepare the structure that will help us emit all permutations of found components
    std::vector<FoundComponentList> found_components;
    found_components.resize(types.GetSize());
    
    // Find the component with the least instances
    for (size_t index = 0; index < types.GetSize(); ++index)
    {
        found_components[index].m_TypeIndex = index;
        found_components[index].m_TypeId = types[index];
        found_components[index].m_Count = Components::CountAllComponentsThatImplement(types[index]);
        
        // Bail if any component type doesn't exist
        if (!found_components[index].m_Count)
        {
            return;
        }
    }

    // Sort the types by commonality
    std::sort(found_components.begin(), found_components.end(), SortFoundComponentList);
    
    // Find all of the component type with the least instances (the outer component)
    Components::GetAllComponentsThatImplement(found_components[0].m_TypeId, found_components[0].m_Components);
    HELIUM_ASSERT(found_components[0].m_Count == found_components[0].m_Components.GetSize());
    

    DynamicArray<Component *> &outer_components = found_components[0].m_Components;
    
    // For every component
    for (DynamicArray<Component *>::Iterator outer_component = outer_components.Begin();
        outer_component != outer_components.End(); ++outer_component)
    {
        bool emit_tuples = true;
        for (size_t type_index = 1; type_index < found_components.size(); ++type_index)
        {
            DynamicArray<Component *> &inner_component_array = found_components[type_index].m_Components;
            Components::FindAllComponentsThatImplement(
                *(*outer_component)->m_OwningSet, 
                found_components[type_index].m_TypeId, 
                inner_component_array);
            if (inner_component_array.IsEmpty())
            {
                emit_tuples = false;
                break;
            }
        }
        
        if (emit_tuples)
        {
            DynamicArray<Component *> tuple;
            tuple.Resize(types.GetSize());
            tuple[found_components[0].m_TypeIndex] = *outer_component;
            EmitTuples(tuple, found_components, 1, emit_tuple_callback);
        }
    }
}

template <class A, class B>
class ComponentQuery : public ITupleCallback
{
public:
    void Run()
    {
        DynamicArray<Components::TypeId> types;
        types.Add(Components::GetType<A>());
        types.Add(Components::GetType<B>());

        QueryComponents(types, this);
    }

protected:
    virtual void HandleTuple(A *, B *) = 0;

private:
    void HandleTupleInternal(DynamicArray<Component *> &components)
    {
        HandleTuple(static_cast<A *>(components[0]), static_cast<B *>(components[1]));
    }
};

class UpdateRotateComponents : public ComponentQuery<RotateComponent, TransformComponent>
{
    virtual void HandleTuple(RotateComponent *pRotate, TransformComponent *pTransform)
    {
        pRotate->ApplyRotation(pTransform);
    }
};

class UpdateMeshComponents : public ComponentQuery<TransformComponent, MeshComponent>
{
    virtual void HandleTuple(TransformComponent *pTransform, MeshComponent *pMeshComponent)
    {
        pMeshComponent->Update(0, pTransform);
    }
};

void Helium::DoEverything()
{
    DynamicArray<TransformComponent *> transformComponents;
    Helium::Components::GetAllComponents<TransformComponent>(transformComponents);

    for (DynamicArray<TransformComponent *>::Iterator iter = transformComponents.Begin();
        iter != transformComponents.End(); ++iter)
    {
        (*iter)->ClearDirtyFlag();
    }
    
    UpdateRotateComponents UpdateRotateComponents; 
    UpdateRotateComponents.Run();

    UpdateMeshComponents updateMeshComponents; 
    updateMeshComponents.Run();
}
