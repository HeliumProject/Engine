#include "APIInfo.h"

#include <stdio.h>

APIInfo::APIInfo()
{
	m_count =0;
	m_pInfo =NULL;
}

APIInfo::~APIInfo()
{
	DeleteAll();
}

APIINFO *APIInfo::Add(char *module_name, char *api_name, char *my_api_name, int param_count, APIFUNC old_api, APIFUNC new_api, char *my_friend_api_name, int start_pos)
{
	APIINFO *pAPIInfo =(APIINFO *)malloc(sizeof(APIINFO));

	strcpy(pAPIInfo->module_name, module_name);
	strcpy(pAPIInfo->api_name, api_name);
	strcpy(pAPIInfo->my_api_name, my_api_name);
	pAPIInfo->param_count =param_count;

	pAPIInfo->old_api =old_api;
	pAPIInfo->new_api =new_api;

	if(my_friend_api_name)
		strcpy(pAPIInfo->my_friend_api_name, my_friend_api_name);
	else pAPIInfo->my_friend_api_name[0] =0;

	pAPIInfo->start_pos =start_pos;
	pAPIInfo->f_hooked =false;

	InitializeCriticalSection(&pAPIInfo->cs);

	pAPIInfo->pnext =NULL;
	APIINFO *pinfoitr =m_pInfo;
	while(pinfoitr !=NULL && pinfoitr->pnext !=NULL)
		pinfoitr =pinfoitr->pnext;
	if(pinfoitr ==NULL) m_pInfo =pAPIInfo;
	else pinfoitr->pnext =pAPIInfo;

	m_count++;

	return pAPIInfo;
}

int APIInfo::DeleteAll()
{
	APIINFO *pAPIInfo =m_pInfo, *pinfoitr;
	while(pAPIInfo)
	{
		pinfoitr =pAPIInfo;
		pAPIInfo =pAPIInfo->pnext;
		DeleteCriticalSection(&pinfoitr->cs);
		free(pinfoitr);
	}

	m_pInfo =NULL;
	m_count =0;

	return 0;
}

APIINFO *APIInfo::FindByMyAPI(APIFUNC new_api)
{
	APIINFO *pAPIInfo =m_pInfo;

	while(pAPIInfo)
	{
		if(pAPIInfo->new_api ==new_api)
			break;
		pAPIInfo =pAPIInfo->pnext;
	}

	return pAPIInfo;
}

APIINFO *APIInfo::FindByOldAPI(APIFUNC old_api)
{
	APIINFO *pAPIInfo =m_pInfo;

	while(pAPIInfo)
	{
		if((BYTE *)pAPIInfo->old_api+pAPIInfo->start_pos ==(BYTE *)old_api)
			break;
		pAPIInfo =pAPIInfo->pnext;
	}

	return pAPIInfo;
}

APIINFO *APIInfo::FindByAPIName(char *api_name)
{
	APIINFO *pAPIInfo =m_pInfo;

	while(pAPIInfo)
	{
		if(!strcmp(pAPIInfo->api_name, api_name))
			break;
		pAPIInfo =pAPIInfo->pnext;
	}

	return pAPIInfo;
}

APIINFO *APIInfo::FindByMyAPIName(char *my_api_name)
{
	APIINFO *pAPIInfo =m_pInfo;

	while(pAPIInfo)
	{
		if(!strcmp(pAPIInfo->my_api_name, my_api_name))
			break;
		pAPIInfo =pAPIInfo->pnext;
	}

	return pAPIInfo;
}

int APIInfo::Lock(APIINFO *pAPIInfo)
{
	EnterCriticalSection(&pAPIInfo->cs);

	return 0;
}

int APIInfo::Unlock(APIINFO *pAPIInfo)
{
	LeaveCriticalSection(&pAPIInfo->cs);

	return 0;
}
