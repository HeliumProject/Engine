#pragma once

#include "Maya/Export.h"
#include "Pipeline/Content/ContentTypes.h"

#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>

#include <string>

class ExportInfoCmd : public MPxCommand
{
public:
  static MString CommandName;
  static const char* s_flag_long_geom;
  static const char* s_flag_short_geom;
  static const char* s_flag_long_skel;
  static const char* s_flag_short_skel;
  static const char* s_flag_long_bang;
  static const char* s_flag_short_bang;
  static const char* s_flag_long_water;
  static const char* s_flag_short_water;
  static const char* s_flag_long_water_sim;
  static const char* s_flag_short_water_sim;
  static const char* s_flag_long_hrc;
  static const char* s_flag_short_hrc;
  static const char* s_flag_long_lrc;
  static const char* s_flag_short_lrc;
  static const char* s_flag_long_path;
  static const char* s_flag_short_path;
  static const char* s_flag_long_index;
  static const char* s_flag_short_index;
  static const char* s_flag_long_long;
  static const char* s_flag_short_long;
  static const char* s_flag_long_verbose;
  static const char* s_flag_short_verbose;
  static const char* s_flag_long_aos;
  static const char* s_flag_short_aos;
  static const char* s_flag_long_an;
  static const char* s_flag_short_an;
  static const char* s_flag_long_node;
  static const char* s_flag_short_node;
  static const char* s_flag_long_groupNode;
  static const char* s_flag_short_groupNode;
  static const char* s_flag_long_lightmap;
  static const char* s_flag_short_lightmap;
  static const char* s_flag_long_fragment;
  static const char* s_flag_short_fragment;

  static const MString s_igNodeNotFound;

  static void * creator()               { return new ExportInfoCmd; }
  static MSyntax newSyntax();

  virtual bool isUndoable()             { return false; }
  virtual bool hasSyntax()              { return true; }

  virtual MStatus doIt( const MArgList & args );

protected:
  Content::ContentType m_type;
  int                      m_index;
  int                      m_artNumber;
  bool                     m_longNames;
  bool                     m_verbose;
  bool                     m_allowOldStyle;
  std::vector<tstring> m_selection;
  std::vector<tstring> m_groupNode;

  bool parseArgs( const MArgList & args );
};
