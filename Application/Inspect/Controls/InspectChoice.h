#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectItems.h"

#ifdef INSPECT_REFACTOR

namespace Helium
{
    namespace Inspect
    {
        //
        // Event delegates
        //

        class APPLICATION_API Choice;

        // the delegate for populating items
        struct ChoiceArgs
        {
            Choice* m_Choice;

            ChoiceArgs(Choice* choice)
                : m_Choice (choice)
            {

            }
        };
        typedef Helium::Signature<void, const ChoiceArgs&> ChoiceSignature;

        // the delegate for connecting an enumerated type's values
        struct ChoiceEnumerateArgs : public ChoiceArgs
        {
            tstring m_Enumeration;

            ChoiceEnumerateArgs(Choice* choice, const tstring& enumeration)
                : ChoiceArgs (choice)
                , m_Enumeration (enumeration)
            {

            }
        };
        typedef Helium::Signature<void, const ChoiceEnumerateArgs&> ChoiceEnumerateSignature;


        //
        // Choice control (base class for comboboxes and listboxes)
        //

        const static tchar CHOICE_ATTR_ENUM[]      = TXT( "enum" );
        const static tchar CHOICE_ATTR_SORTED[]    = TXT( "sorted" );
        const static tchar CHOICE_ATTR_DROPDOWN[]  = TXT( "dropdown" );

        class APPLICATION_API Choice : public Reflect::ConcreteInheritor<Choice, Items>
        {
        protected:
            tstring m_Enum;
            bool m_Sorted;
            bool m_DropDown;
            bool m_EnableAdds; 

        public:
            Choice();

        protected:
            virtual bool Process(const tstring& key, const tstring& value) HELIUM_OVERRIDE;

            virtual void SetOverride(bool isOverride);

            virtual void SetDefaultAppearance(bool def) HELIUM_OVERRIDE;

            void SetToDefault(const ContextMenuEventArgs& event);

        public:
            virtual void SetSorted(bool sorted);
            virtual void SetDropDown(bool dropDown);
            virtual void SetEnableAdds(bool enabled); 

            virtual tstring GetValue() HELIUM_OVERRIDE;
            virtual void SetValue(const tstring& data) HELIUM_OVERRIDE;

            virtual void Realize(Container* parent) HELIUM_OVERRIDE;

            virtual void Populate() HELIUM_OVERRIDE;

            virtual void Read() HELIUM_OVERRIDE;

            virtual bool Write() HELIUM_OVERRIDE;

        protected:
            ChoiceSignature::Event m_Populate;
        public:
            void AddPopulateListener(const ChoiceSignature::Delegate& listener)
            {
                m_Populate.Add(listener);
            }
            void RemovePopulateListener(const ChoiceSignature::Delegate& listener)
            {
                m_Populate.Remove(listener);
            }

        protected:
            ChoiceEnumerateSignature::Event m_Enumerate;
        public:
            void AddEnumerateListener(const ChoiceEnumerateSignature::Delegate& listener)
            {
                m_Enumerate.Add(listener);
            }
            void RemoveEnumerateListener(const ChoiceEnumerateSignature::Delegate& listener)
            {
                m_Enumerate.Remove(listener);
            }
        };

        typedef Helium::SmartPtr<Choice> ChoicePtr;
    }
}

#endif