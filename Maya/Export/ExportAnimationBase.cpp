#include "Precompile.h"
#include "ExportAnimationBase.h"

#include "MayaContentCmd.h"

using namespace Content;
using namespace MayaContent;


V_ExportAnimationBasePtr ExportAnimationBase::s_AnimationExporters;

//
// convert rate to an integer
//
int ExportAnimationBase::Rate( MTime::Unit units )
{
  switch (units)
  {
  case MTime::kSeconds:
    return 1;
    break;
  case MTime::kGames:
    return 15;
    break;
  case MTime::kFilm:
    return 24;
    break;
  case MTime::kPALFrame:
    return 25;
    break;
  case MTime::kNTSCFrame:
    return 30;
    break;
  case MTime::kShowScan:
    return 48;
    break;
  case MTime::kPALField:
    return 50;
    break;
  case MTime::kNTSCField:
    return 60;
    break;
  case MTime::kMilliseconds:
    return 1000;
    break;
  }
  return 1;
}

void ExportAnimationBase::SampleMayaAnimationData()
{
  EXPORT_SCOPE_TIMER( ("") );

  if( s_AnimationExporters.empty() )
    return;

  V_ExportAnimationBasePtr::iterator begin = s_AnimationExporters.begin();
  V_ExportAnimationBasePtr::iterator end = s_AnimationExporters.end();

  // get the time range values from Maya
  MTime minTime( MAnimControl::minTime() );
  MTime maxTime  ( MAnimControl::maxTime() );
  MTime animationStartTime( MAnimControl::animationStartTime() );
  MTime animationEndTime( MAnimControl::animationEndTime() );

  // use the inner time range unless some exporter requires the outer range
  MTime startTime = minTime;
  MTime endTime = maxTime;
  if ( HasFullTimeRangeExporter() )
  {
    // if any exporter uses the entire time range then use those limits
    // this will happen for cinematics
    startTime = animationStartTime;
    endTime = animationEndTime;
  }

  // loop through each time step
  // purposefully sample one extra frame, so we get proper motion/aiming/phase joint data
  MTime currentTime = startTime; 
  do
  {
    bool innerTimeRange = ( currentTime >= minTime ) || ( currentTime <= maxTime );
    MAnimControl::setCurrentTime( currentTime );

    bool extraFrame = (currentTime > endTime);

    for (V_ExportAnimationBasePtr::iterator itr = begin; itr!=end; ++itr )
    {
      if ( innerTimeRange || (*itr)->IsFullTimeRange() )
      {
        // sample all frames for inner range, and necessary ones for outer range
        (*itr)->SampleOneFrame( currentTime, extraFrame );
      }
    }
    currentTime++;
  }
  while( currentTime <= ( endTime + 1 ) );


  // all of the exporters were processed, so forget them
  s_AnimationExporters.clear();
}

bool ExportAnimationBase::HasFullTimeRangeExporter()
{
  V_ExportAnimationBasePtr::iterator itr = s_AnimationExporters.begin();
  V_ExportAnimationBasePtr::iterator end = s_AnimationExporters.end();
  for ( ; itr!=end; ++itr )
  {
    if ( (*itr)->IsFullTimeRange() )
    {
      return true;
    }
  }
  return false;
}

void ExportAnimationBase::RememberAnimationExporter( ExportAnimationBase * exporter )
{
  V_ExportAnimationBasePtr::iterator itr = s_AnimationExporters.begin();
  V_ExportAnimationBasePtr::iterator end = s_AnimationExporters.end();
  for ( ; itr!=end; ++itr )
  {
    if ( (*itr).Ptr() == exporter )
    {
      // already remembered this exporter
      return;
    }
  }
  // remember a new exporter
  s_AnimationExporters.push_back( exporter );
}

void ExportAnimationBase::ForgetAnimationExporter( ExportAnimationBase * exporter )
{
  V_ExportAnimationBasePtr::iterator itr = s_AnimationExporters.begin();
  V_ExportAnimationBasePtr::iterator end = s_AnimationExporters.end();
  for ( ; itr!=end; ++itr )
  {
    if ( (*itr).Ptr() == exporter )
    {
      // remove the exporter that was found
      s_AnimationExporters.erase( itr );
      return;
    }
  }
}