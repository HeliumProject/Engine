#pragma once

#include "Foundation/Reflect/Object.h"
#include "Foundation/Container/OrderedSet.h"

#include "Core/API.h"

namespace Helium
{
    namespace Core
    {
        class PropertiesGenerator;
        struct EnumerateElementArgs;

        class CORE_API Selectable : public Reflect::Object
        {
        protected:
            bool m_Selected;

        public:
            REFLECT_DECLARE_ABSTRACT( Selectable, Reflect::Object );

            Selectable();
            virtual ~Selectable();

            // Is this object currently selectable?
            //  Sometimes objects can on a per-instance or per-type basis decided to NOT be selectable
            //  This prototype exposes the capability to HELIUM_OVERRIDE the selection of an object
            virtual bool IsSelectable() const;

            // Get/Set selected state
            virtual bool IsSelected() const;
            virtual void SetSelected(bool);

            // do enumeration of applicable attributes on this object
            virtual void ConnectProperties(EnumerateElementArgs& args);

            // validate a named panel as usable
            virtual bool ValidatePanel(const tstring& name);
        };

        // Smart pointer to an Selectable
        typedef Helium::SmartPtr< Selectable > SelectablePtr;

        // vector of selectable objects
        typedef Helium::OrderedSet<Selectable*> OS_SelectableDumbPtr;
    }
}