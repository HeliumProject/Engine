#pragma once

#include "Application/API.h"
#include "Control.h"

#include <wx/sizer.h>

namespace Helium
{
    namespace Inspect
    {
        typedef std::vector< wxSizer* > V_Sizer;

        //
        // Contains other controls and distributes layout logic
        //

        class APPLICATION_API Container : public Reflect::ConcreteInheritor<Container, Control>
        {
        protected:
            V_Control m_Controls;

        public:
            // constructor
            Container();

            // destructor cleans up contained controls
            virtual ~Container();

            // get control count
            virtual const V_Control& GetControls() const;

            // set all the controls, will NOT free existing controls (could leak)
            virtual void SetControls(const V_Control& controls);

            // add a single control
            virtual void AddControl(Control* control);

            // insert a single control
            virtual void InsertControl(int index, Control* control);

            // remove a single control
            virtual void RemoveControl(Control* control);

            // clear everything
            virtual void Clear();

            // get indent
            virtual int GetIndent();

            // recusively binds contained controls to data
            virtual void Bind(const DataPtr& data) HELIUM_OVERRIDE;

            // sets default data
            virtual void SetDefault(const tstring& def) HELIUM_OVERRIDE;

            // realize control
            virtual void Realize(Container* parent) HELIUM_OVERRIDE;

            // populate
            virtual void Populate() HELIUM_OVERRIDE;

            // refreshes the UI state from data
            virtual void Read() HELIUM_OVERRIDE;

            // updates the data based on the state of the UI
            virtual bool Write() HELIUM_OVERRIDE;

            // freeze the display of the controls
            virtual void Freeze() HELIUM_OVERRIDE;

            // thaw the display of the controls
            virtual void Thaw() HELIUM_OVERRIDE;

            // toggle whether the controls are shown
            virtual void Show() HELIUM_OVERRIDE;

            // toggle whether the controls are shown
            virtual void Hide() HELIUM_OVERRIDE;

            // enable/disable child controls
            virtual void SetEnabled(bool enabled) HELIUM_OVERRIDE;

            // set read only state on child controls
            virtual void SetReadOnly(bool readOnly) HELIUM_OVERRIDE;

#ifdef INSPECT_DEBUG_LAYOUT_LOGIC
            // print recursively to the console
            virtual void PrintLayout();
#endif
        };

        typedef Helium::SmartPtr<Container> ContainerPtr;
        typedef std::vector<ContainerPtr> V_Container;
    }
}