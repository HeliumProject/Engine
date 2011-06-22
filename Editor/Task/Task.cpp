#include "EditorPch.h"
#include "Task.h"

using namespace Helium;
using namespace Helium::Editor;

TaskStartedSignature::Event g_Started;
void Editor::AddTaskStartedListener( const TaskStartedSignature::Delegate& listener )
{
  g_Started.Add( listener );
}
void Editor::RemoveTaskStartedListener( const TaskStartedSignature::Delegate& listener )
{
  g_Started.Remove( listener );
}
void Editor::RaiseTaskStarted( const TaskStartedArgs& args )
{
  g_Started.Raise( args );
}

TaskFinishedSignature::Event g_Finished;
void Editor::AddTaskFinishedListener( const TaskFinishedSignature::Delegate& listener )
{
  g_Finished.Add( listener );
}
void Editor::RemoveTaskFinishedListener( const TaskFinishedSignature::Delegate& listener )
{
  g_Finished.Remove( listener );
}
void Editor::RaiseTaskFinished( const TaskFinishedArgs& args )
{
  g_Finished.Raise( args );
}