#pragma once

#ifndef P4CLIENTAPI_H
# define P4CLIENTAPI_H
# pragma warning (disable : 4267 4244)
# include "p4/clientapi.h"
# pragma warning (default : 4267 4244)
#endif

namespace Perforce
{
  static const StrRef g_ActionTag( "action" );
  static const StrRef g_DigestTag( "digest" );
  static const StrRef g_FileSizeTag( "fileSize" );
  static const StrRef g_DepotFileTag( "depotFile" );
  static const StrRef g_ClientFileTag( "clientFile" );
  static const StrRef g_HeadRevTag( "headRev" );
  static const StrRef g_HaveRevTag( "haveRev" );
  static const StrRef g_HeadModTimeTag( "headModTime" );
  static const StrRef g_HeadTimeTag( "headTime" );
  static const StrRef g_HeadActionTag( "headAction" );
  static const StrRef g_HeadTypeTag( "headType" );
  static const StrRef g_OtherOpenTag( "otherOpen" );
  static const StrRef g_OtherChangeTag( "otherChange" );
  static const StrRef g_OtherActionTag( "otherAction" );
  static const StrRef g_ChangeTag( "change" );
  static const StrRef g_ActionOwnerTag( "actionOwner" );
  static const StrRef g_RevisionTag( "rev" );
  static const StrRef g_TypeTag( "type" );
  static const StrRef g_TimeTag( "time" );
  static const StrRef g_UserTag( "user" );
  static const StrRef g_ClientTag( "client" );
  static const StrRef g_DescriptionTag( "desc" );
  static const StrRef g_PathTag( "path" );
  static const StrRef g_DirTag( "dir" );
  static const StrRef g_SubmittedChangeTag( "submittedChange" );
  static const StrRef g_OldActionTag( "oldAction" );
  static const StrRef g_WorkRevTag( "workRev" );
  static const StrRef g_OpenFilesTag( "openFiles" );
  static const StrRef g_HowTag( "how" );
  static const StrRef g_FileTag( "file" );
}