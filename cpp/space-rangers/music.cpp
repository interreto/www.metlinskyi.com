//--------------------------------------------------------------------
//	����:		MUSIC.CPP
//	��������:	��������������� ������� ���������� DirectMusic
//--------------------------------------------------------------------
#define STRICT

#include <windows.h>
#include <dmusici.h>
#include "main.h"

IDirectMusic*				pDMusic=NULL;
IDirectMusicPort*			pDMPort=NULL;
IDirectMusicLoader*			pDMLoader=NULL;
IDirectMusicPerformance*	pDMPerformance=NULL;

IDirectMusicSegment*		pMenu=NULL;
IDirectMusicSegment*		pTitle=NULL;
IDirectMusicSegment*		pLevel=NULL;
IDirectMusicSegment*		pBoss=NULL;

WCHAR wszHomeDir[MAX_PATH];
WCHAR wszLevelDir[MAX_PATH];

CHAR *szFileNames[8]={"title.mid","menu.mid","level.mid","boss.mid"};

MusicType fMusic=DM_FULL;

extern LPDIRECTSOUND       pDS;//�������� � SOUND.CPP

BOOL ConfigureSynthesizer();
BOOL LoadMIDI(IDirectMusicSegment**, CHAR *);
BOOL CreatePort();
BOOL CreateLoader(WCHAR*);
BOOL fUseMusic=TRUE;

//--------------------------------------------------------
// ����������� ��� ������� ��������� � DirectMusic
//--------------------------------------------------------
void RemoveDirectMusic()
{

	if (fMusic==MCI)
		return;

	if(pDMusic)
	{
		pDMusic->Activate(FALSE);
	}

	SAFE_RELEASE(pBoss);
	SAFE_RELEASE(pLevel);
	SAFE_RELEASE(pMenu);
	SAFE_RELEASE(pTitle);
	SAFE_RELEASE(pDMPort);

	if(pDMPerformance)
	{
		pDMPerformance->Stop( NULL, NULL, 0, 0 );
        
		pDMPerformance->CloseDown();
		pDMPerformance->Release();
		pDMPerformance = NULL;
	}

	SAFE_RELEASE(pDMLoader);
	SAFE_RELEASE(pDMusic);

	CoUninitialize();
}

//--------------------------------------------------------
// ������� ������� ������������� DirectMusic
//--------------------------------------------------------
BOOL	InitDirectMusic ()
{
	if (fMusic==MCI)
	{
		mciSendString("close all",NULL,0,0);
		mciSendString("close MUSIC",NULL,0,0);
		mciSendString("open music.mid type sequencer alias MUSIC",NULL,0,0);
		mciSendString("play MUSIC from 0 notify",NULL,0,hMainWnd);
		
		return (TRUE);
	}

	HRESULT	hRet;
	
	hRet = CoInitialize(NULL);
	if (FAILED(hRet))
	{
		if(DEBUG)  ErrorHandle(hMainWnd,"COM ������������������!!!");
		return (FALSE);
	}

	hRet=CoCreateInstance( CLSID_DirectMusic, NULL, CLSCTX_INPROC_SERVER, IID_IDirectMusic,(VOID**)&pDMusic );
	if( FAILED(hRet))
	{
		if(DEBUG) ErrorHandle(hMainWnd,"� � ��� DirectMusic �� ����� ����������!");
		return (FALSE);
	}

	hRet=pDMusic->SetDirectSound( pDS, hMainWnd );
	if (FAILED(hRet))
	{
		if(DEBUG) ErrorHandle(hMainWnd,"������������ ��������� DirectSound �� DirectMusic");
		return (FALSE);
	}
	
	hRet = CoCreateInstance( CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance, (void**)&pDMPerformance );
	if (FAILED(hRet))
	{
		if(DEBUG) ErrorHandle(hMainWnd," CoCreateInstance  ���-���-��� � ��� �����!");
		return (FALSE);
	}

	if (!CreateLoader(wszHomeDir))
	{
		if(DEBUG) ErrorHandle(hMainWnd,"��������� �������� - �� ��������!");
		return (FALSE);
	}

	if (!CreatePort())
	{
		if(DEBUG) 		ErrorHandle(hMainWnd,"�������� ���� ����!");
		return (FALSE);
	}
	
	if (!ConfigureSynthesizer())
	{
		if(DEBUG) ErrorHandle(hMainWnd,"�������� ���������� !");
		return (FALSE);
	}
/*
	if (!LoadMIDI(&pTitle,szFileNames[0]))
	{
		if(DEBUG) ErrorHandle(hMainWnd,"�� ������� ���� title.mid - ��� ��������� ���������� ������������� ������� ��� �� �����");
		return (FALSE);
	}
*/
	if (!LoadMIDI(&pMenu,szFileNames[1]))
	{
		if(DEBUG) ErrorHandle(hMainWnd,"�� ������� ���� menu.mid - ��� ��������� ���������� ������������� ������� ��� �� �����");
		return (FALSE);
	}

	if (!LoadMIDI(&pLevel,szFileNames[2]))
	{
		if(DEBUG) ErrorHandle(hMainWnd,"�� ������� ���� level.mid - ��� ��������� ���������� ������������� ������� ��� �� �����");
		return (FALSE);
	}
/*
	if (!LoadMIDI(&pBoss,szFileNames[3]))
	{
		if(DEBUG) ErrorHandle(hMainWnd,"�� ������� ���� boss.mid - ��� ��������� ���������� ������������� ������� ��� �� �����");
		return (FALSE);
	}

*/
//	pTitle->SetRepeats(DWORD(-1));
	pMenu->SetRepeats(DWORD(-1));
	pLevel->SetRepeats(DWORD(-1));
//	pBoss->SetRepeats(DWORD(-1));

	hRet=pDMPort->Activate( TRUE );
	if (FAILED(hRet))
	{
		if(DEBUG) ErrorHandle(hMainWnd,"�������������� ���. ����!");
		return (FALSE);
	}


//	pTitle->SetParam(GUID_Download, 0xFFFFFFFF, 0, 0, (void*)pDMPerformance);	
	pMenu->SetParam(GUID_Download, 0xFFFFFFFF, 0, 0, (void*)pDMPerformance);
	pLevel->SetParam(GUID_Download, 0xFFFFFFFF, 0, 0, (void*)pDMPerformance);		
//	pBoss->SetParam(GUID_Download, 0xFFFFFFFF, 0, 0, (void*)pDMPerformance);	

	return (TRUE);
}

//--------------------------------------------------------
// ����������� ��������� ����� ����������
//--------------------------------------------------------
BOOL ConfigureSynthesizer()
{
	HRESULT hRet;

	hRet=pDMPerformance->Init( &pDMusic, NULL, NULL );
	if (FAILED(hRet))
		return (FALSE);

	hRet=pDMPerformance->AddPort( pDMPort );
	if (FAILED(hRet))
		return (FALSE);

	hRet=pDMPerformance->AssignPChannelBlock( 0, pDMPort, 1 ) ;
	if (FAILED(hRet))
        return (FALSE);

	return (TRUE);
}

//--------------------------------------------------------
// ��������� ��������� ���� � ��������� �������
//--------------------------------------------------------
BOOL LoadMIDI(IDirectMusicSegment **pDMSegment, CHAR* szFilename)
{
	HRESULT hRet;
	DMUS_OBJECTDESC dmod;
	WCHAR	wszFilename[MAX_PATH];

	mbstowcs(wszFilename, szFilename, MAX_PATH );

	dmod.guidClass = CLSID_DirectMusicSegment;
	dmod.dwSize = sizeof(DMUS_OBJECTDESC);
	wcscpy( dmod.wszFileName, wszFilename);
	dmod.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;

	hRet = pDMLoader->GetObject( &dmod, IID_IDirectMusicSegment, (void**)pDMSegment );
	if(FAILED(hRet))
		return (FALSE);

	return (TRUE);
}

//--------------------------------------------------------
// ������ ���� � ���������� �����������
//--------------------------------------------------------
BOOL CreatePort()
{
	HRESULT hRet;
	GUID guidPortGUID;
	DMUS_PORTPARAMS dmos;

	hRet=pDMusic->GetDefaultPort( &guidPortGUID );
  	if (FAILED(hRet))
		return (FALSE);

	ZeroMemory( &dmos, sizeof(dmos) );
	dmos.dwSize          = sizeof(DMUS_PORTPARAMS);
	dmos.dwChannelGroups = 1;
	
	if (fMusic==DM_FULL)
		dmos.dwEffectFlags = DMUS_EFFECT_REVERB;
	else
		dmos.dwEffectFlags = DMUS_EFFECT_NONE;

	dmos.dwValidParams   = DMUS_PORTPARAMS_CHANNELGROUPS|DMUS_PORTPARAMS_EFFECTS;

	hRet=pDMusic->CreatePort( guidPortGUID, &dmos, &pDMPort,NULL );
	if (FAILED(hRet))
		return (FALSE);

	return (TRUE);
}

//--------------------------------------------------------
// ������ ��������� ����������
// � ��������� ��� ���� ������� ������
//--------------------------------------------------------
BOOL CreateLoader(WCHAR* wszDirectory)
{
	HRESULT hRet;

	hRet = CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC,IID_IDirectMusicLoader, (void**)&pDMLoader );
	if (FAILED(hRet))
        return (FALSE);

	hRet=pDMLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, wszHomeDir, FALSE );
   	if (FAILED(hRet))
        return (FALSE);

	return (TRUE);
}

//--------------------------------------------------------
// ������������� ��������� ����
//--------------------------------------------------------
void PlayMusic(int iMusic)
{
	if(fUseMusic)
	{
		if (iMusic==TITLE_MUSIC)
		{
			pDMPerformance->Stop(NULL,NULL,0,0);
			pDMPerformance->PlaySegment(pTitle, DMUS_SEGF_BEAT, 0, NULL );
		}

		if (iMusic==MENU_MUSIC)
		{
			pDMPerformance->Stop(NULL,NULL,0,0);
			pDMPerformance->PlaySegment(pMenu, DMUS_SEGF_BEAT, 0, NULL );
		}

		if (iMusic==LEVEL_MUSIC)
		{
			pDMPerformance->Stop(NULL,NULL,0,0);
			pDMPerformance->PlaySegment(pLevel, DMUS_SEGF_BEAT, 0, NULL );
		}

		if (iMusic==BOSS_MUSIC)
		{
			pDMPerformance->Stop(NULL,NULL,0,0);
			pDMPerformance->PlaySegment(pBoss, DMUS_SEGF_BEAT, 0, NULL );
		}
	}
}
