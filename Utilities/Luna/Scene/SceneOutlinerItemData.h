#pragma once

// Includes
#include "Luna/API.h"

namespace Luna
{
  // Forwards
  class Object;

  /////////////////////////////////////////////////////////////////////////////
  // Class for attaching Objects to items displayed in a tree control.
  // 
  class SceneOutlinerItemData : public wxTreeItemData
  {
  protected:
    Object*    m_Object;
    std::string m_ItemText; 
    int         m_CachedCount; 
    bool        m_Countable; 

  public:
    SceneOutlinerItemData( Object* object );
    virtual ~SceneOutlinerItemData();
    
    Object* GetObject() const;
    void SetObject( Object* object );

    void SetItemText(const std::string& text); 
    const std::string& GetItemText(); 

    int GetCachedCount(); 
    void SetCachedCount(int count); 

    bool GetCountable() { return m_Countable; }
    void SetCountable(bool countable ) { m_Countable = countable; }
  };
}