#pragma once

#include "API.h"
#include "Control.h"

namespace Inspect
{
  typedef std::vector< wxSizer* > V_Sizer;

  //
  // Contains other controls and distributes layout logic
  //

  class INSPECT_API Container : public Reflect::ConcreteInheritor<Container, Control>
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
    virtual void Bind(const DataPtr& data) NOC_OVERRIDE;

    // sets default data
    virtual void SetDefault(const std::string& def) NOC_OVERRIDE;

    // realize control
    virtual void Realize(Container* parent) NOC_OVERRIDE;

    // populate
    virtual void Populate() NOC_OVERRIDE;

    // refreshes the UI state from data
    virtual void Read() NOC_OVERRIDE;

    // updates the data based on the state of the UI
    virtual bool Write() NOC_OVERRIDE;

    // freeze the display of the controls
    virtual void Freeze() NOC_OVERRIDE;

    // thaw the display of the controls
    virtual void Thaw() NOC_OVERRIDE;

    // toggle whether the controls are shown
    virtual void Show() NOC_OVERRIDE;

    // toggle whether the controls are shown
    virtual void Hide() NOC_OVERRIDE;

    // enable/disable child controls
    virtual void SetEnabled(bool enabled) NOC_OVERRIDE;

    // set read only state on child controls
    virtual void SetReadOnly(bool readOnly) NOC_OVERRIDE;
    
#ifdef INSPECT_DEBUG_LAYOUT_LOGIC
    // print recursively to the console
    virtual void PrintLayout();
#endif
  };

  typedef Nocturnal::SmartPtr<Container> ContainerPtr;
  typedef std::vector<ContainerPtr> V_Container;
}