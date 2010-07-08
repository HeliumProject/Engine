#pragma once

#include "Application/API.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectFieldInterpreter.h"

namespace Inspect
{
  // Forwards
  class Button;
  class Action;
  class List;
  struct FilteredDropTargetArgs;

  class APPLICATION_API FileContainerInterpreter : public ReflectFieldInterpreter
  {
  public:
    FileContainerInterpreter (Container* container);

    virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent);

  private:
    // translate array of TUIDs to file path and back
    void TranslateInputTUIDContainer( Reflect::TranslateInputEventArgs& args );
    void TranslateOutputTUID( tstring& path, const u64& fileId );
    void TranslateOutputTUIDContainer( Reflect::TranslateOutputEventArgs& args );

    // callbacks
    void OnAdd( Button* button );
    void OnAddFile( Button* button );
    void OnFindFile( Button* button );
    void OnEdit( Button* button );
    void OnRemove( Button* button );
    void OnMoveUp( Button* button );
    void OnMoveDown( Button* button );

    void OnDrop( const Inspect::FilteredDropTargetArgs& args );

  private:
      tstring m_FileFilter;
    Inspect::List* m_List;
  };

  typedef Nocturnal::SmartPtr<FileContainerInterpreter> FileContainerInterpreterPtr;
  typedef std::vector< FileContainerInterpreterPtr > V_FileContainerInterpreterSmartPtr;
}