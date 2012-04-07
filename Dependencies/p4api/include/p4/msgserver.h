/*
 * Copyright 1995, 2000 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * msgserver.h - definitions of errors for server subsystem.
 */

class MsgServer {

    public:
	static ErrorId LbrCheckout;
	static ErrorId LbrDiff;
	static ErrorId LbrDigest;
	static ErrorId LbrFileSize;
	static ErrorId LbrCheckin;
	static ErrorId LbrMerge;
	static ErrorId LbrNoTrigger;
	static ErrorId BadRoot;
	static ErrorId BadIPAddr;
	static ErrorId GotUnlocked;
	static ErrorId GotLocked;
	static ErrorId NoInteg;
	static ErrorId GotUnresolved;
	static ErrorId CantOpen;
	static ErrorId NoDumpName;
	static ErrorId NoCkptName;
	static ErrorId BadJnlFlag;
	static ErrorId BadExtraFlag;
	static ErrorId MetaDumpFailed;
	static ErrorId Password982;
	static ErrorId BadPassword;
	static ErrorId MustSetPassword;
	static ErrorId WeakPassword;
	static ErrorId TicketOnly;
	static ErrorId Unicode;
	static ErrorId Unicode2;
	static ErrorId OperationFailed;
	static ErrorId OperationDate;
	static ErrorId BadCommand;
	static ErrorId IllegalCommand;
	static ErrorId HandshakeFailed;
	static ErrorId ConnectBroken;
	static ErrorId ClientOpFailed;
	static ErrorId OnlyOneFilter;
	static ErrorId Usage;
	static ErrorId OldDiffClient;
	static ErrorId Jobs982Win;
	static ErrorId No973Wingui;
	static ErrorId JobsDashS;
	static ErrorId AddDelete;
	static ErrorId ZFlagsConflict;
	static ErrorId Password991;
	static ErrorId Password032;
	static ErrorId NoClearText;
	static ErrorId LoginExpired;
	static ErrorId PasswordExpired;
	static ErrorId LoginNotRequired;
	static ErrorId LoginPrintTicket;
	static ErrorId LoginUser;
	static ErrorId LoginGoodTill;
	static ErrorId LoginNoTicket;
	static ErrorId LogoutUser;
	static ErrorId LoggedOut;
	static ErrorId Login032;
	static ErrorId Login042;
	static ErrorId Login072;
	static ErrorId SSOfailed;
	static ErrorId SSONoEnv;
	static ErrorId SSOInvalid;
	static ErrorId CantAuthenticate;
	static ErrorId CantChangeOther;
	static ErrorId NoSuchUser;
	static ErrorId BadPassword0;
	static ErrorId BadPassword1;
	static ErrorId PasswordTooShort;
	static ErrorId PasswordTooLong;
	static ErrorId PasswordTooSimple;
	static ErrorId NoProxyAuth;
	static ErrorId MimAttack;
	static ErrorId NoSuppASflag;
	static ErrorId NoSuppVflag;
	static ErrorId SubmitFailed;
	static ErrorId SubmitIsShelved;
	static ErrorId CounterWarning;
	static ErrorId CouldntLock;
	static ErrorId MergesPending;
	static ErrorId RetypeInvalidTempobj;
	static ErrorId ResolveOrRevert;
	static ErrorId CantRevertToPurged;
	static ErrorId NoSubmit;
	static ErrorId TriggerFailed;
	static ErrorId TriggerOutput;
	static ErrorId TriggersFailed;
	static ErrorId SubmitAborted;
	static ErrorId NoDefaultSubmit;
	static ErrorId BadImport;
	static ErrorId BadTransfers;
	static ErrorId DigestMisMatch;
	static ErrorId SubmitDataChanged;
	static ErrorId SubmitTampered;
	static ErrorId DirsWild;
	static ErrorId HelpSeeRename;
	static ErrorId PurgeReport;
	static ErrorId SnapReport;
	static ErrorId PurgeWarning;
	static ErrorId PurgeOptGone;
	static ErrorId PurgeBadOption;
	static ErrorId LogCommand;
	static ErrorId Unlicensed;
	static ErrorId TrackCommand;
	static ErrorId NoValidLicense;
	static ErrorId MaxLicensedFiles;
	static ErrorId MaxUnLicensedFiles;
	static ErrorId NoCentralLicense;
	static ErrorId NoAuthFileCount;
	static ErrorId BadServicePack;
	static ErrorId Startup;
	static ErrorId Shutdown;
	static ErrorId Restarted;
	static ErrorId Restarting;
	static ErrorId CreatingDb;
	static ErrorId SkippedJnls;

	static ErrorId ConfigureSet;
	static ErrorId ConfigureUnSet;
	static ErrorId NotThisConfigVar;
	static ErrorId InvalidConfigValue;
	static ErrorId ConfigureNone;
	static ErrorId ConfigureServerNone;

	static ErrorId CounterDelete;
	static ErrorId CounterSet;
	static ErrorId CounterGet;
	static ErrorId CounterNotNumeric;

	static ErrorId DescribeFixed;
	static ErrorId DescribeAffected;
	static ErrorId DescribeMovedFiles;
	static ErrorId DescribeDifferences;
	static ErrorId DescribeEmpty;
	static ErrorId DescribeShelved;

	static ErrorId Diff2Differ;
	static ErrorId Diff2BadArgs;

	static ErrorId GrepIllegalContext;
	static ErrorId GrepContextTooLarge;

	static ErrorId IndexOutput;

	static ErrorId InfoUser;
	static ErrorId InfoBadUser;
	static ErrorId InfoClient;
	static ErrorId InfoBadClient;
	static ErrorId InfoStream;
	static ErrorId InfoHost;
	static ErrorId InfoDirectory;
	static ErrorId InfoDiskSpace;
	static ErrorId InfoClientAddress;
	static ErrorId InfoServerAddress;
	static ErrorId InfoServerRoot;
	static ErrorId InfoServerDate;
	static ErrorId InfoServerVersion;
	static ErrorId InfoServerLicense;
	static ErrorId InfoServerLicenseIp;
	static ErrorId InfoServerUptime;
	static ErrorId InfoUnknownClient;
	static ErrorId InfoClientRoot;
	static ErrorId InfoProxyVersion;
	static ErrorId InfoProxyAddress;
	static ErrorId InfoAuthServer;
	static ErrorId InfoChangeServer;
	static ErrorId InfoCaseHandling;
	static ErrorId InfoMinClient;

	static ErrorId PasswordSave;
	static ErrorId PasswordDelete;
	static ErrorId PasswordNoChange;

	static ErrorId ShelveBegin;
	static ErrorId NoDefaultShelve;
	static ErrorId UnshelveNotOwner;
	static ErrorId ShelveAborted;
	static ErrorId NoShelve;
	static ErrorId NoShelveDelete;
	static ErrorId ShelveComplete;
	static ErrorId UnshelveFileChanged;
	static ErrorId ShelveDelete;
	static ErrorId ShelveMaxSize;
	static ErrorId ShelveTriggersFailed;
	static ErrorId ShelveXOpen;

	static ErrorId SpecNotCorrect;
	static ErrorId ErrorInSpec;
	static ErrorId SpecArchiveWarning;
	static ErrorId SpecCheckTriggers;

	static ErrorId SubmitLocking;
	static ErrorId SubmitComplete;
	static ErrorId SubmitBegin;
	static ErrorId SubmitRenamed;

	static ErrorId ResolveOptAuto;
	static ErrorId ResolveOptHelp;
	static ErrorId ResolveOptMerge;
	static ErrorId ResolveOptSkip;
	static ErrorId ResolveOptTheirs;
	static ErrorId ResolveOptYours;
	static ErrorId ResolvePromptMerge;
	static ErrorId ResolvePromptTheirs;
	static ErrorId ResolvePromptType;
	static ErrorId ResolvePromptYours;
	static ErrorId ResolveUserError;
	static ErrorId ResolveUserPrompt;
	static ErrorId ResolveUserPrompt2;

	static ErrorId ResolvedFile;
	static ErrorId ResolvedSkipped;
	static ErrorId ResolveTampered;

	static ErrorId JobRebuilt;

	static ErrorId SearchResult;

	static ErrorId DiffCmp;
	static ErrorId DiffList;

	static ErrorId DeltaLine1;
	static ErrorId DeltaLine2;

	static ErrorId MonitorDisabled;
	static ErrorId MonitorBadId;

	static ErrorId IstatInvalid;

	static ErrorId UseAdmin;
	static ErrorId UseAdminCheckpoint;
	static ErrorId UseAdminJournal;
	static ErrorId UseAdminSpecDepot;
	static ErrorId UseAdminDBSigs;
	static ErrorId UseAdminImport;
	static ErrorId UseAnnotate;
	static ErrorId UseArchive;
	static ErrorId UseBranch;
	static ErrorId UseBrancho;
	static ErrorId UseBranchd;
	static ErrorId UseBranchi;
	static ErrorId UseChange;
	static ErrorId UseChanged;
	static ErrorId UseChangeo;
	static ErrorId UseChangei;
	static ErrorId UseChanges;
	static ErrorId UseChanget;
	static ErrorId UseClient;
	static ErrorId UseCliento;
	static ErrorId UseClientd;
	static ErrorId UseClienti;
	static ErrorId UseClientS;
	static ErrorId UseClients;
	static ErrorId UseConfigure;
	static ErrorId UseCopy;
	static ErrorId UseCopyb;
	static ErrorId UseCopyS;
	static ErrorId UseCounter;
	static ErrorId UseCounteri;
	static ErrorId UseCounters;
	static ErrorId UseCstat;
	static ErrorId UseDbpack;
	static ErrorId UseDbstat;
	static ErrorId UseDepot;
	static ErrorId UseDepoto;
	static ErrorId UseDepotd;
	static ErrorId UseDepoti;
	static ErrorId UseDepots;
	static ErrorId UseDescribe;
	static ErrorId UseDiff;
	static ErrorId UseDiff2;
	static ErrorId UseDiff2b;
	static ErrorId UseDiff2n;
	static ErrorId UseDiff2S;
	static ErrorId UseDirs;
	static ErrorId UseDiskspace;
	static ErrorId UseBranches;
	static ErrorId UseLabels;
	static ErrorId UseDomainClients;
	static ErrorId UseDup;
	static ErrorId UseExport;
	static ErrorId UseFilelog;
	static ErrorId UseFiles;
	static ErrorId UseFix;
	static ErrorId UseFixes;
	static ErrorId UseFstat;
	static ErrorId UseGrep;
	static ErrorId UseGroup;
	static ErrorId UseGroupo;
	static ErrorId UseGroupd;
	static ErrorId UseGroupi;
	static ErrorId UseGroups;
	static ErrorId UseHave;
	static ErrorId UseHelp;
	static ErrorId UseIndex;
	static ErrorId UseInfo;
	static ErrorId UseInteg;
	static ErrorId UseIntegb;
	static ErrorId UseIntegS;
	static ErrorId UseInteged;
	static ErrorId UseInterChanges;
	static ErrorId UseInterChangesb;
	static ErrorId UseInterChangesS;
	static ErrorId UseIstat;
	static ErrorId UseJob;
	static ErrorId UseJobd;
	static ErrorId UseJobo;
	static ErrorId UseJobi;
	static ErrorId UseJobs;
	static ErrorId UseJobSpec;
	static ErrorId UseLabel;
	static ErrorId UseLabelo;
	static ErrorId UseLabeld;
	static ErrorId UseLabeli;
	static ErrorId UseLabelSync;
	static ErrorId UseLicense;
	static ErrorId UseLock;
	static ErrorId UseLogin;
	static ErrorId UseLogout;
	static ErrorId UseLogger;
	static ErrorId UseLogtail;
	static ErrorId UseMain;
	static ErrorId UseMerge;
	static ErrorId UseMergeb;
	static ErrorId UseMergeS;
	static ErrorId UseMonitor;
	static ErrorId UseMonitorc;
	static ErrorId UseMonitorf;
	static ErrorId UseMonitors;
	static ErrorId UseMonitorP;
	static ErrorId UseMonitorR;
	static ErrorId UseOpen;
	static ErrorId UseOpen2;
	static ErrorId UseOpened;
	static ErrorId UsePasswd;
	static ErrorId UseProtect;
	static ErrorId UseProtects;
	static ErrorId UsePull;
	static ErrorId UsePurge;
	static ErrorId UseRelease;
	static ErrorId UseReopen;
	static ErrorId UseResolve;
	static ErrorId UseResolved;
	static ErrorId UseRestore;
	static ErrorId UseRetype;
	static ErrorId UseReview;
	static ErrorId UseReviews;
	static ErrorId UseSearch;
	static ErrorId UseSizes;
	static ErrorId UseShelve;
	static ErrorId UseShelvec;
	static ErrorId UseShelvei;
	static ErrorId UseShelver;
	static ErrorId UseShelveNoOpts;
	static ErrorId UseSnap;
	static ErrorId UseSpec;
	static ErrorId UseStream;
	static ErrorId UseStreamc;
	static ErrorId UseStreamd;
	static ErrorId UseStreami;
	static ErrorId UseStreamo;
	static ErrorId UseStreams;
	static ErrorId UseSubmit;
	static ErrorId UseSubmitc;
	static ErrorId UseSubmitd;
	static ErrorId UseSync;
	static ErrorId UseSyncp;
	static ErrorId UseSyncs;
	static ErrorId UseTag;
	static ErrorId UseTrait;
	static ErrorId UseTraiti;
	static ErrorId UseTriggers;
	static ErrorId UseTypeMap;
	static ErrorId UseUnlock;
	static ErrorId UseUnshelve;
	static ErrorId UseUser;
	static ErrorId UseUsero;
	static ErrorId UseUserd;
	static ErrorId UseUseri;
	static ErrorId UseUsers;
	static ErrorId UseVerify;
	static ErrorId UseWhere;
	static ErrorId NotAsService;
	static ErrorId UseProxy;
	static ErrorId UsePing;
	static ErrorId UseMove;

	static ErrorId ServerTooOld;
	static ErrorId ProxyChain;
	static ErrorId ProxyDelivered;
	static ErrorId RmtAuthFailed;
	static ErrorId ReplicaRestricted;
	static ErrorId ReplicaNoUpgrade;
	static ErrorId ReplicaBadOption;
	static ErrorId UnknownReplicationMode;
	static ErrorId MissingReplicationMode;
	static ErrorId UnknownReplicationTarget;
	static ErrorId ReplicaXferFailed;
	static ErrorId BadPCache;
	static ErrorId ProxyNoRemote;
	static ErrorId ProxyUpdateFail;
	static ErrorId RemoteInvalidCmd;
	static ErrorId InvalidNesting;
	static ErrorId ClientTooOld;
	static ErrorId NoTicketSupport;
	static ErrorId CommandCancelled;
	static ErrorId AdminNoSpecDepot;
	static ErrorId ImportReport;
	static ErrorId AdminReplicaCkp;
	static ErrorId AdminNothingLocked;
	static ErrorId AdminNothingLogged;
	static ErrorId AdminSizeData;
	static ErrorId Move091;
	static ErrorId Move101;
	static ErrorId ActionResolve111;
	static ErrorId BadJournalNum;
	static ErrorId BadCheckpointNum;
	static ErrorId JournalorCheckpointRequired;
	static ErrorId CurJournalButNotJournaling;
	static ErrorId LogtailNoLog;
	static ErrorId PullNotReplica;
	static ErrorId PullTransferSummary;
	static ErrorId PullJournalSummary;
	static ErrorId JournalCounterMismatch;

	static ErrorId TransferNotReplica;
	static ErrorId UsersCRNotReplica;
	static ErrorId UsersCRNotBoth;
	static ErrorId TZMismatch;
	static ErrorId NeedFilePath;
	static ErrorId NoSuchField;
	static ErrorId EmptyTypeList;
	static ErrorId NotStreamReady;
	static ErrorId NotStreamOwner;

	static ErrorId BadSortOption;
	static ErrorId TooManySortTraits;

	static ErrorId InvalidStartupCommand;
	static ErrorId StartupCommandError;
	static ErrorId InvalidServerChain;

        static ErrorId InfoPingTime;
        static ErrorId InfoPingTimeB;
        static ErrorId InfoPingCount;
        static ErrorId InfoPingCountB;
        static ErrorId ErrorPingProtocol;
        static ErrorId ErrorPingParam;

	static ErrorId NoCustomSpec;

	static ErrorId CopyWrongDirection;
	static ErrorId CopyDoNothing;
	static ErrorId CopyNeedsMergeFirst;
	static ErrorId MergeWrongDirection;

	// Retired ErrorIds. We need to keep these so that clients 
	// built with newer apis can commnunicate with older servers 
	// still sending these.

	static ErrorId UseAdminCopyin; // DEPRECATED
	static ErrorId UseAdminCopyout; // DEPRECATED
	static ErrorId UseTunables; // DEPRECATED
	static ErrorId UseDomains; // Used in 2009.1 through 2010.2
	static ErrorId PullTransferPending; // Was used in 2010.2 BETA only.
};
