#pragma once

#include "API.h"
#include "Inspect/FilteredDropTarget.h"
#include "InspectReflect/ReflectFieldInterpreter.h"

namespace Finder
{
  class FinderSpec;
}

namespace Inspect
{
  class INSPECTFILE_API FileInterpreter : public ReflectFieldInterpreter
  {
  public:
    FileInterpreter (Container* container);

    virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent);

  private:
    // translate TUID to file path and back
    void TranslateInputTUID( Reflect::TranslateInputEventArgs& args );
    void TranslateOutputTUID( Reflect::TranslateOutputEventArgs& args );

    // callbacks
    bool DataChanging( DataChangingArgs& args );
    void Edit( Button* button );
    
    void OnDrop( const Inspect::FilteredDropTargetArgs& args );

  protected:
    const Finder::FinderSpec* m_FinderSpec;

  private:
    Inspect::Value* m_Value;
  };

  typedef Nocturnal::SmartPtr<FileInterpreter> FileInterpreterPtr;
  typedef std::vector< FileInterpreterPtr > V_FileInterpreter;
}