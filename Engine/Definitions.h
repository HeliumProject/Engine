
#pragma once

namespace Helium
{
  // TODO: Turn this from a string to something like "names" where the names are interned strings.
  //       The names might be case insensitive and hashed to be a lot faster than just a naive
  //       map lookup. But I'm not expecting this system to be used many times a frame.. really
  //       maybe just when we're loading the game or creating a new object.
  namespace Definitions
  {
    typedef std::vector<Reflect::ObjectPtr> V_Object;

    extern const tstring NULL_NAME;
    extern const tstring NULL_GROUP_NAME;

    namespace Private
    {
      namespace BindModes
      {
        enum BindMode
        {
          PrepareToWrite, // Bind with this param before we write out definitions
          Update          // Any time we add/update/delete other definitions, rebind all objects with this param
        };
      }
      typedef BindModes::BindMode BindMode;
    }

    ////////////////////////////////////////////////////////////////////////
    // Base definition class
    ////////////////////////////////////////////////////////////////////////
    class Definition : public Reflect::Object
    {
    public:
      REFLECT_DECLARE_ABSTRACT(Definitions::Definition, Reflect::Object);
      static void AcceptCompositeVisitor( Reflect::Composite& comp );

      /// Called to force the definition to update string/pointer sets that are
      /// bound together
      virtual void BindDefinitionReferences(Private::BindMode) { }

      // Every definition has a unique name
      tstring m_Name;
    };

    typedef Helium::StrongPtr<Definitions::Definition> DefinitionPtr;

    ////////////////////////////////////////////////////////////////////////
    // Base API to manipulate the definitions system, or groups of definitions
    ////////////////////////////////////////////////////////////////////////

    //! Must be called before creating any systems
    void Initialize();

    //! Call to tear down the component system
    void Cleanup();

    //! Unregisters all definitions within a group
    void Clear(const tstring &_group_name = NULL_GROUP_NAME);
    void ClearAll();

    //! Load all definitions in a file to a group
    bool Load(const tstring &_filename, const tstring &_group_name = NULL_GROUP_NAME);

    //! Save all definitions belonging to the given group into a particular file
    bool Save(const tstring &_filename, const tstring &_group_name = NULL_GROUP_NAME);

    ////////////////////////////////////////////////////////////////////////
    // Base API to find/manipulate individual definitions
    ////////////////////////////////////////////////////////////////////////

    // Asserts if name is not unique
    DefinitionPtr Create(const Reflect::Class *_class, const tstring &_name, const tstring &_group_name = NULL_GROUP_NAME);

    // Asserts if definition is null, or definition name is not unique
    void Set(const DefinitionPtr _definition, const tstring &_group_name = NULL_GROUP_NAME);

    DefinitionPtr Find(const tstring &_name);

    ////////////////////////////////////////////////////////////////////////
    // Template helper functions
    ////////////////////////////////////////////////////////////////////////
    template <class T>
    Helium::SmartPtr<T> Create(const tstring &_name)
    {
      DefinitionPtr definition_ptr = Create(Reflect::GetClass<T>(), _name);
      return Helium::SmartPtr<T>(Reflect::AssertCast<T>(definition_ptr.Ptr()));
    }

    template <class T>
    Helium::SmartPtr<T> Find(const tstring &_name)
    {
      DefinitionPtr definition_ptr = Find(_name);
      return Helium::SmartPtr<T>(Reflect::AssertCast<T>(definition_ptr.Ptr()));
    }

    ////////////////////////////////////////////////////////////////////////
    // Private API
    ////////////////////////////////////////////////////////////////////////
    namespace Private
    {
      // Called from the binding #defines
      template <class T>
      void BindDefinitionReference(tstring &_name, Helium::SmartPtr<T> &_reference, BindMode _bind_mode);

      void FindAllInGroup(const tstring &_group_name, V_Object &_objects_list);

      void BindGroup(const tstring &_group_name, BindMode _bind_mode);
      void BindAll(BindMode _bind_mode);
    }
  }

  template <class T>
  void Definitions::Private::BindDefinitionReference(tstring &_name, Helium::SmartPtr<T> &_reference, BindMode _bind_mode)
  {
    switch (_bind_mode)
    {
    case BindModes::PrepareToWrite:
      {
        // Before we write, update the name with what we actually point to
        if (_reference.Ptr())
        {
          // We're pointing to something, include the name
          _name = _reference->m_Name;
        }
        else
        {
          // Not pointing at something so update the string with the null value
          _name = NULL_NAME;
        }
      }
      break;

    case BindModes::Update:
      {
        // Any time we add/update/delete definitions, or when we create this definition, 
        // we need to fix the cached reference
        if (_name != NULL_NAME)
        {
          _reference = Find<T>(_name);
        }
      }
      break;
    }
  }
}


// Lets users set up string->pointer binding easily
#define BEGIN_DEFINITION_REFERENCES(__Class__, __ParentClass__)           \
  virtual void BindDefinitionReferences(Definitions::Private::BindMode _bind_mode)              \
  {                                                                       \
    __ParentClass__::BindDefinitionReferences(_bind_mode);                          

#define ADD_REFERENCE(__StringName__, __ReferenceName__)                  \
  Definitions::Private::BindDefinitionReference(&__StringName__, &__Reference__, _bind_mode);

#define END_DEFINITION_REFERENCES()                                       \
  }
