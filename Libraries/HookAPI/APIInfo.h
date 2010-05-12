#pragma once

#include <windows.h>

typedef void (WINAPI *APIFUNC)(void);

#define CALL_BYTES_SIZE		5		// CALL 0xnnnnnnnn		size=5

struct APIINFO
{
	char              module_name[100];
	char              api_name[50];
	char              my_api_name[50];
	char              my_friend_api_name[50];
	int               param_count;
	int               start_pos;
	APIFUNC           old_api, new_api;
	int               f_hooked;
	BYTE              save_bytes[CALL_BYTES_SIZE];
	CRITICAL_SECTION  cs;
	DWORD             old_protection_flags;
	APIINFO*          pnext;
};

class APIInfo
{
public:
	APIInfo();
	virtual ~APIInfo();

  APIINFO* Get()
  {
    return m_pInfo;
  }

	APIINFO* Add( char *module_name,
                char *api_name,
                char *my_api_name,
		            int param_count,
                APIFUNC old_api=NULL,
                APIFUNC new_api =NULL,
		            char *my_friend_api_name =NULL,
                int start_pos=0);

	int DeleteAll();
	
	APIINFO *FindByMyAPI(APIFUNC new_api);
	APIINFO *FindByOldAPI(APIFUNC old_api);
	APIINFO *FindByAPIName(char *api_name);
	APIINFO *FindByMyAPIName(char *my_api_name);

	int Lock(APIINFO *pAPIInfo);
	int Unlock(APIINFO *pAPIInfo);

private:
	int m_count;
	APIINFO *m_pInfo;
};