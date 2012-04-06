/*
 * Copyright 1995, 2000 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/* 
 * p4tags.h - definition of rpc variable names
 *
 * The P4Tag class contains nothing but static variables containing
 * the strings passed to the Rpc::SetVar() and Invoke() functions, to
 * save string space across files.
 */

struct P4Tag {

	// protocol levels

	static const char l_client[];
	static const char l_xfiles[];
	static const char l_proxy[];
	static const char l_server[];
	static const char l_server2[];

	// client service methods

	static const char c_Ack[];
	static const char c_ActionResolve[];
	static const char c_CheckFile[];
	static const char c_ChmodFile[];
	static const char c_CloseDiff[];
	static const char c_CloseFile[];
	static const char c_CloseMerge[];
	static const char c_Crypto[];
	static const char c_DeleteFile[];
	static const char c_EditData[];
	static const char c_ErrorPause[];
	static const char c_FstatInfo[];
	static const char c_HandleError[];
	static const char c_InputData[];
	static const char c_Message[];
	static const char c_OpenDiff[];
	static const char c_OpenFile[];
	static const char c_OpenMerge2[];
	static const char c_OpenMerge3[];
	static const char c_OutputBinary[];
	static const char c_OutputData[];
	static const char c_OutputError[];
	static const char c_OutputInfo[];
	static const char c_OutputText[];
	static const char c_Ping[];
	static const char c_Prompt[];
	static const char c_MoveFile[];
	static const char c_ScanDir[];
	static const char c_SendFile[];
	static const char c_SetPassword[];
	static const char c_SSO[];
	static const char c_WriteDiff[];
	static const char c_WriteFile[];
	static const char c_WriteMerge[];

	// protocol service methods

	static const char p_compress1[];
	static const char p_compress2[];
	static const char p_echo[];
	static const char p_errorHandler[];
	static const char p_flush1[];
	static const char p_flush2[];
	static const char p_funcHandler[];
	static const char p_protocol[];
	static const char p_release[];
	static const char p_release2[];

	// variables known to the clients

	static const char v_actionOwner[];
	static const char v_action[];
	static const char v_api[];
	static const char v_app[];
	static const char v_attack[];
	static const char v_attr[];
	static const char v_authServer[];
	static const char v_baseName[];
	static const char v_bits[];
	static const char v_blockCount[];
	static const char v_archiveFile[];
	static const char v_caddr[];
	static const char v_caseHandling[];
	static const char v_change[];
	static const char v_changeServer[];
	static const char v_changeType[];
	static const char v_clientAddress[];
	static const char v_clientCwd[];
	static const char v_clientFile[];
	static const char v_clientHost[];
	static const char v_clientName[];
	static const char v_clientRoot[];
	static const char v_clientStream[];
	static const char v_client[];
	static const char v_cmpfile[];
	static const char v_code[];
	static const char v_commit[];
	static const char v_compare[];
	static const char v_confirm[];
	static const char v_counter[];
	static const char v_cwd[];
	static const char v_daddr[];
	static const char v_data[];
	static const char v_data2[];		// p4 passwd
	static const char v_dbstat[];
	static const char v_decline[];
	static const char v_desc[];
	static const char v_descKey[];		// original CL#
	static const char v_dhash[];
	static const char v_diffFlags[];
	static const char v_digest[];
	static const char v_dir[];
	static const char v_enableStreams[];
	static const char v_endFromRev[];
	static const char v_endToRev[];
	static const char v_erev[];
	static const char v_externalAuth[];
	static const char v_extraTag[];
	static const char v_extraTagType[];
	static const char v_fatal[];
	static const char v_fileCount[];
	static const char v_fileSize[];
	static const char v_file[];
	static const char v_fmt[];
	static const char v_forceType[];
	static const char v_fromFile[];
	static const char v_fromRev[];
	static const char v_fseq[];
	static const char v_func[];
	static const char v_func2[];
	static const char v_handle[];
	static const char v_haveRev[];
	static const char v_headAction[];
	static const char v_headChange[];
	static const char v_headModTime[];
	static const char v_headRev[];
	static const char v_headTime[];
	static const char v_headType[];
	static const char v_headContent[];
	static const char v_himark[];
	static const char v_host[];
	static const char v_how[];
	static const char v_isgroup[];
	static const char v_job[];
	static const char v_jobstat[];
	static const char v_key[];
	static const char v_language[];
	static const char v_lbrFile[];		// also remote depot
	static const char v_lbrRev[];		// also remote depot
	static const char v_lbrType[];		// also remote depot
	static const char v_level[];
	static const char v_lfmt[];
	static const char v_line[];
	static const char v_lower[];
	static const char v_mangle[];
	static const char v_matchedLine[];
	static const char v_matchBegin[];
	static const char v_matchEnd[];
	static const char v_maxLockTime[];
	static const char v_maxResults[];
	static const char v_maxScanRows[];
	static const char v_mergeAuto[];
	static const char v_mergeConfirm[];
	static const char v_mergeDecline[];
	static const char v_mergeHow[];
	static const char v_mergePerms[];
	static const char v_minClient[];
	static const char v_monitor[];
	static const char v_noBase[];
	static const char v_nocase[];
	static const char v_noclobber[];
	static const char v_noecho[];
	static const char v_op[];
	static const char v_offset[];
	static const char v_open[];
	static const char v_os[];
	static const char v_otherAction[];
	static const char v_otherChange[];
	static const char v_otherLock[];
	static const char v_otherOpen[];
	static const char v_ourLock[];
	static const char v_password[];
	static const char v_path[];
	static const char v_path2[];
	static const char v_perm[];
	static const char v_permmax[];
	static const char v_perms[];
	static const char v_preview[];
	static const char v_prog[];
	static const char v_proxy[];
	static const char v_proxyVersion[];
	static const char v_rActionType[];
	static const char v_rActionMerge[];
	static const char v_rActionTheirs[];
	static const char v_rActionYours[];
	static const char v_rAutoResult[];
	static const char v_rOptAuto[];
	static const char v_rOptHelp[];
	static const char v_rOptMerge[];
	static const char v_rOptSkip[];
	static const char v_rOptTheirs[];
	static const char v_rOptYours[];
	static const char v_rPromptMerge[];
	static const char v_rPromptTheirs[];
	static const char v_rPromptType[];
	static const char v_rPromptYours[];
	static const char v_rUserError[];
	static const char v_rUserHelp[];
	static const char v_rUserPrompt[];
	static const char v_rUserResult[];
	static const char v_rcvbuf[];
	static const char v_reason[];
	static const char v_remap[];
	static const char v_remoteFunc[];
	static const char v_remoteMap[];
	static const char v_remoteRange[];
	static const char v_reresolvable[];
	static const char v_resolved[];
	static const char v_resolveType[];
	static const char v_rev[];
	static const char v_rev2[];
	static const char v_rmdir[];
	static const char v_rseq[];
	static const char v_security[];
	static const char v_sndbuf[];
	static const char v_sendspec[];
	static const char v_sequence[];
	static const char v_server[];
	static const char v_server2[];
	static const char v_serverAddress[];
	static const char v_serverDescription[];
	static const char v_serverDate[];
	static const char v_serverName[];
	static const char v_serverRoot[];
	static const char v_serverUptime[];
	static const char v_serverLicense[];
	static const char v_serverLicenseIp[];
	static const char v_serverVersion[];
	static const char v_showAll[];
	static const char v_specdef[];
	static const char v_specstring[];
	static const char v_specFormatted[];
	static const char v_srev[];
	static const char v_sso[];
	static const char v_startFromRev[];
	static const char v_startToRev[];
	static const char v_stat[];
	static const char v_status[];
	static const char v_svrname[];
	static const char v_tag[];
	static const char v_tagJnl[];
	static const char v_theirName[];
	static const char v_time[];
	static const char v_toFile[];
	static const char v_token[];
	static const char v_token2[];
	static const char v_totalFileCount[];
	static const char v_totalFileSize[];
	static const char v_track[];
	static const char v_trans[];
	static const char v_truncate[];
	static const char v_type[];
	static const char v_type2[];
	static const char v_type3[];
	static const char v_type4[];
	static const char v_unicode[];
	static const char v_unmap[];
	static const char v_unresolved[];
	static const char v_upper[];
	static const char v_user[];
	static const char v_userName[];
	static const char v_version[];
	static const char v_warning[];
	static const char v_wingui[];
	static const char v_workRev[];
	static const char v_write[];
	static const char v_xfiles[];
	static const char v_yourName[];

	// server-to-server or server-to-proxy variables
	// clients should not look at these

	static const char v_allTamperCheck[];	// p4 submit
	static const char v_altArg[];		// p4 jobspec
	static const char v_altArg2[];		// p4 change
	static const char v_altArg3[];		// p4 stream
	static const char v_arg[];
	static const char v_asBinary[];		// p4 resolve
	static const char v_attrib[];		// p4 index
	static const char v_author[];		// proxy keyword handling
	static const char v_baseDepotRec[];	// p4 resolve
	static const char v_changeNo[];		// p4 submit 
	static const char v_checkSum[];		// p4 submit 
	static const char v_confirm2[];
	static const char v_delete[];		// p4 index
	static const char v_depotFile[];
	static const char v_depotFile2[];
	static const char v_depotName[];	// proxy
	static const char v_depotRec[];
	static const char v_do[];		// p4 diff
	static const char v_doForce[];		// p4 shelve
	static const char v_fixStatus[];	// p4 change
	static const char v_force[];		// p4 submit
	static const char v_getFlag[];
	static const char v_haveRec[];		// p4 sync
	static const char v_ignoreIsEdit[];	// p4 resolve
	static const char v_index[];
	static const char v_integRec[];
	static const char v_integRec2[];
	static const char v_ipaddr[];
	static const char v_keyVal[];		// p4 index
	static const char v_leaveUnchanged[];	// submit
	static const char v_lockAll[];		// admin
	static const char v_message[];		// p4 diff
	static const char v_message2[];		// p4 release
	static const char v_movedFile[];
	static const char v_movedRev[];
	static const char v_noretry[];		// p4 specs
	static const char v_peer[];
	static const char v_propigate[];	// proxy chaining test
	static const char v_reopen[];  		// submit
	static const char v_replace[];		// shelve
	static const char v_revertUnchanged[];  // submit
	static const char v_revRec[];		// proxy
	static const char v_revtime[];		// proxy invalidation
	static const char v_revver[];		// proxy rev table version
	static const char v_save[];		// revert
	static const char v_shelved[];          // shelve (2009.2)
	static const char v_state[];
	static const char v_table[];		// remote depot
	static const char v_traitCount[];	// submit
	static const char v_tzoffset[];		// server tz offset for proxy
	static const char v_output[];		// proxy print kind
	static const char v_value[];		// p4 index
	static const char v_workRec[];
	static const char v_workRec2[];
	static const char v_yourDepotRec[];	// p4 resolve

	// server user commands
	
	static const char u_add[];
	static const char u_admin[];
	static const char u_branch[];
	static const char u_branches[];
	static const char u_change[];
	static const char u_changes[];
	static const char u_client[];
	static const char u_clients[];
	static const char u_counter[];
	static const char u_counters[];
	static const char u_delete[];
	static const char u_depot[];
	static const char u_depots[];
	static const char u_describe[];
	static const char u_diff[];
	static const char u_diff2[];
	static const char u_dirs[];
	static const char u_edit[];
	static const char u_filelog[];
	static const char u_files[];
	static const char u_fix[];
	static const char u_fixes[];
	static const char u_flush[];
	static const char u_fstat[];
	static const char u_group[];
	static const char u_groups[];
	static const char u_have[];
	static const char u_help[];
	static const char u_info[];
	static const char u_integrate[];
	static const char u_integrated[];
	static const char u_job[];
	static const char u_jobs[];
	static const char u_jobspec[];
	static const char u_label[];
	static const char u_labels[];
	static const char u_labelsync[];
	static const char u_lock[];
	static const char u_obliterate[];
	static const char u_opened[];
	static const char u_passwd[];
	static const char u_print[];
	static const char u_protect[];
	static const char u_rename[];
	static const char u_reopen[];
	static const char u_resolve[];
	static const char u_resolved[];
	static const char u_revert[];
	static const char u_review[];
	static const char u_reviews[];
	static const char u_set[];
	static const char u_stream[];
	static const char u_streams[];
	static const char u_submit[];
	static const char u_sync[];
	static const char u_triggers[];
	static const char u_typemap[];
	static const char u_unlock[];
	static const char u_user[];
	static const char u_users[];
	static const char u_verify[];
	static const char u_where[];

} ;

