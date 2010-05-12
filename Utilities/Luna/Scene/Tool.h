#pragma once

#include "API.h"
#include "View.h"

#include "Core/Selection.h"

namespace Luna
{
  class View;

  class Tool NOC_ABSTRACT : public Object
  {
    //
    // Members
    //

  protected:
    // The attribute enumerator to use
    Enumerator* m_Enumerator;

    // The 3d view
    Luna::View* m_View;

    bool m_AllowSelection;

    //
    // RTTI
    //

  public:
    LUNA_DECLARE_TYPE(Luna::Tool, Object);
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    Tool( Luna::View* view, Enumerator* enumerator )
      : m_Enumerator (enumerator)
      , m_View ( view )
      , m_AllowSelection( true )
    {

    }

    virtual ~Tool()
    {

    }

    // Create and Initialize
    virtual bool Initialize()
    {
      return true;
    }

    // Cleanup
    virtual void Cleanup()
    {

    }

    // Reset scaling
    virtual void ResetSize()
    {

    }

    // Scale to a target factor (camera was zoomed)
    virtual void ScaleTo( float factor )
    {

    }

    // Route interface callbacks to me
    virtual bool AllowSelection()
    {
      return m_AllowSelection;
    }

    // Validate any selection if not in InterfaceOverride
    virtual bool ValidateSelection( OS_SelectableDumbPtr& items )
    {
      return true;
    }

    // Create attribute UI
    virtual void CreateProperties()
    {

    }

    // Eval Nodes
    virtual void Evaluate()
    {

    }

    // Render UI
    virtual void Draw( DrawArgs* args )
    {
      return;
    }

    // Pick
    virtual bool Pick( PickVisitor* pick )
    {
      return false;
    }

    //
    // Handle Mouse Input
    //

    // this returns whether the tool is taking the drag
    virtual bool MouseDown( wxMouseEvent& e )
    {
      return !AllowSelection();
    }

    virtual void MouseUp( wxMouseEvent& e )
    {

    }

    virtual void MouseMove( wxMouseEvent& e )
    {

    }

    virtual void KeyPress( wxKeyEvent& e )
    {
      e.Skip();
    }

    virtual void KeyDown( wxKeyEvent& e )
    {
      e.Skip();
    }

    virtual void KeyUp( wxKeyEvent& e )
    {
      e.Skip();
    }
  };

  typedef Nocturnal::SmartPtr< Luna::Tool > LToolPtr;
}
