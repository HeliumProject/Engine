#pragma once

#include "Workspace.h"
#include "FileDropTarget.h"
#include "PropToolGenerated.h"
#include "Platform/Compiler.h"
#include "Foundation/File/FileWatcher.h"

namespace igDXRender
{
  class Render;
  class Scene;
}

class FileWatchTimer : public wxTimer
{
public:
  FileWatchTimer( class PropToolFrame* frame )
    : m_Frame ( frame )
  {

  }

  void Notify();

  class PropToolFrame* m_Frame;
};

class PropToolFrame : public PropToolFrameBase
{
public:
	PropToolFrame( wxWindow* parent );
  virtual ~PropToolFrame();

private:
  static unsigned long __stdcall FileWatcherThread( void* pvoid );

	void OnOpen( wxCommandEvent& event ) NOC_OVERRIDE;
	void OnExit( wxCommandEvent& event ) NOC_OVERRIDE;
	void OnAbout( wxCommandEvent& event ) NOC_OVERRIDE;

  void OBJChanged( const FileDroppedArgs& args );
  void EnvChanged( const FileDroppedArgs& args );
  void WorkspaceFileDropped( const FileDroppedArgs& args );
  void OnShaderSelected( wxCommandEvent& event ) NOC_OVERRIDE;
  void OnAlphaModeChanged( wxCommandEvent& event ) NOC_OVERRIDE;

  void ColorMapChanged( const FileDroppedArgs& args );
  void OnColorMapFileChanged( wxFileDirPickerEvent& event ) NOC_OVERRIDE;
  void OnColorMapFileModification( const Nocturnal::FileChangedArgs& args );

  void NormalMapChanged( const FileDroppedArgs& args );
  void OnNormalMapFileChanged( wxFileDirPickerEvent& event ) NOC_OVERRIDE;
  void OnNormalMapFileModification( const Nocturnal::FileChangedArgs& args );

  void GPIMapChanged( const FileDroppedArgs& args );
  void OnGPIMapFileModification( const Nocturnal::FileChangedArgs& args );
  void OnGPIMapFileChanged( wxFileDirPickerEvent& event ) NOC_OVERRIDE;

  void Open(const std::string& file);

private:
  friend class FileWatchTimer; // Food for thought: is this bad form? -Geoff
  FileWatchTimer m_FileWatchTimer;
  Nocturnal::FileWatcher m_FileWatcher;
};
