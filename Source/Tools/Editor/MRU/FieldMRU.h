#pragma once

#include <map>

#include "Platform/Assert.h"
#include "Foundation/SmartPtr.h"
#include "Application/OrderedSet.h"
#include "Editor/API.h"

#include "MRU.h"

//
// Forwards
//
class wxControlWithItems;

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // System for managing the global MRU FieldMap 
        //
        class FieldMRU : public Helium::RefCountBase<FieldMRU>
        {
        public:
            FieldMRU()
            {
                Initialize();
            }

            ~FieldMRU()
            {
                Cleanup();
            }

            void Initialize();
            void Cleanup();

            ManagedStringSetPtr GetFieldItems( const std::string& fieldKey, const std::string& defaultValue = "", const bool autoInit = true );
            bool AddItem( wxControlWithItems* control, const std::string& fieldKey, const std::string& value );
            void PopulateControl( wxControlWithItems* control, const std::string& fieldKey, const std::string& defaultValue = "", const bool autoInit = true );  

        private:
            // Mapping of String field name to MostRecentlyUsed
            typedef std::map< std::string, ManagedStringSetPtr > M_ManagedStringSet;

            // Mapping of the most recently used field values
            M_ManagedStringSet  m_Fields;

        };

        typedef Helium::SmartPtr< FieldMRU > FieldMRUPtr;
    }
}