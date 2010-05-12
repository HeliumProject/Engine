#include "Precompile.h"
#include "Task.h"

using namespace Luna;

TaskStartedSignature::Event g_Started;
void Luna::AddTaskStartedListener( const TaskStartedSignature::Delegate& listener )
{
  g_Started.Add( listener );
}
void Luna::RemoveTaskStartedListener( const TaskStartedSignature::Delegate& listener )
{
  g_Started.Remove( listener );
}
void Luna::RaiseTaskStarted( const TaskStartedArgs& args )
{
  g_Started.Raise( args );
}

TaskFinishedSignature::Event g_Finished;
void Luna::AddTaskFinishedListener( const TaskFinishedSignature::Delegate& listener )
{
  g_Finished.Add( listener );
}
void Luna::RemoveTaskFinishedListener( const TaskFinishedSignature::Delegate& listener )
{
  g_Finished.Remove( listener );
}
void Luna::RaiseTaskFinished( const TaskFinishedArgs& args )
{
  g_Finished.Raise( args );
}