//----------------------------------------------------------------------
//	����:		INPUT.CPP
//	��������:	��������� ������ �� ������ ���������� DirectInput
//----------------------------------------------------------------------
#define STRICT

#include <windows.h>
#include <ddraw.h>
#include <dinput.h>

#include "main.h"

LPDIRECTINPUT7 pDI = NULL;
LPDIRECTINPUTDEVICE7 pDIMouse = NULL;
LPDIRECTINPUTDEVICE7 pDIKeyboard = NULL;

int nMouseX=MAX_WIDTH/2;
int nMouseY=MAX_HEIGHT/2;
int nScale=2;

BYTE bKeyBuffer[256];
BYTE bKeyState[256];

BOOL KeyPressed(BYTE key);

bool up=false;
bool down=false;
bool left=false;
bool right=false;
bool fire=false;
bool accel=false;

bool lb=false;

//--------------------------------------------------------
// ����������� ��� ������� ��������� � DirectInput
//--------------------------------------------------------
void RemoveDirectInput()
{
	if (pDIMouse)
	{
		pDIMouse->Unacquire();
		pDIMouse->Release();
		pDIMouse=NULL;
	}

	if (pDIKeyboard)
	{
		pDIKeyboard->Unacquire();
		pDIKeyboard->Release();
		pDIKeyboard=NULL;
	}

	SAFE_RELEASE(pDI);
}

//--------------------------------------------------------
// ������� ������� ������������� DirectInput
//--------------------------------------------------------
BOOL	InitDirectInput ()
{
	HRESULT	hRet;
	HINSTANCE hInst;
	DIPROPDWORD dipropdw; 
	
	ZeroMemory(bKeyBuffer,sizeof(bKeyBuffer));

	hInst=(HINSTANCE)GetWindowLong( hMainWnd, GWL_HINSTANCE );
    
	hRet = DirectInputCreateEx(hInst, DIRECTINPUT_VERSION, 
        IID_IDirectInput7, (void**)&pDI, NULL); 
	if (FAILED(hRet))
	{
		ErrorHandle(hMainWnd,"DirectInputCreate");
		return (FALSE);
	}
	
	hRet = pDI->CreateDeviceEx(GUID_SysMouse, IID_IDirectInputDevice7, (void**)&pDIMouse, NULL); 
	if (FAILED(hRet))
	{
		ErrorHandle(hMainWnd,"Create mouse device");
		return (FALSE);
	}

	hRet = pDI->CreateDeviceEx(GUID_SysKeyboard, IID_IDirectInputDevice7, (void**)&pDIKeyboard, NULL); 
	if (FAILED(hRet))
	{
		ErrorHandle(hMainWnd,"Create keyboard device");
		return (FALSE);
	}

	hRet = pDIMouse->SetCooperativeLevel(hMainWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(hRet))
	{
		ErrorHandle(hMainWnd,"Set mouse cooperative level");
		return (FALSE);
	}

	hRet = pDIKeyboard->SetCooperativeLevel(hMainWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hRet))
	{
		ErrorHandle(hMainWnd,"Set keyboard cooperative level");
		return (FALSE);
	}

	hRet=pDIMouse->SetDataFormat(&c_dfDIMouse); 
	if (FAILED(hRet))
	{
		ErrorHandle(hMainWnd,"Set mouse data format");
		return (FALSE);
	}

	hRet=pDIKeyboard->SetDataFormat(&c_dfDIKeyboard); 
	if (FAILED(hRet))
	{
		ErrorHandle(hMainWnd,"Set keyboard data format");
		return (FALSE);
	}

	dipropdw.diph.dwSize = sizeof(DIPROPDWORD); 
	dipropdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	dipropdw.diph.dwObj = 0; 
	dipropdw.diph.dwHow = DIPH_DEVICE; 
	dipropdw.dwData = 16; 

	hRet = pDIMouse->SetProperty(DIPROP_BUFFERSIZE, &dipropdw.diph);
	if (FAILED(hRet))
	{
		ErrorHandle(hMainWnd,"Set mouse buffer size");
		return (FALSE);
	}

	pDIMouse->Acquire();	
	pDIKeyboard->Acquire();	

	return (TRUE);
}

//--------------------------------------------------------
// �������� ������ �� ���� �� ������
//--------------------------------------------------------
void ProcessMouse(BOOL isButton)
{
	HRESULT hRet;
	DIDEVICEOBJECTDATA dod;
	DWORD dwElements = 1;

	lb=false;

	while (TRUE) {        

		//�������� ���� ������� �� ������
		hRet=pDIMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &dod,&dwElements, 0);
		if (hRet == DIERR_INPUTLOST)
		{
			pDIMouse->Acquire();
			hRet=pDIMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &dod,&dwElements, 0);
		}

		//���� �������, ���� ��������� ���, �� �������
		if (FAILED(hRet) || dwElements == 0) 
			break;

		switch (dod.dwOfs) 
		{ 
		case DIMOFS_X://����������� �� ��� X 
			nMouseX+=(dod.dwData*nScale);

			if (nMouseX>(MAX_WIDTH-MOUSE_WIDTH))
				nMouseX=MAX_WIDTH-MOUSE_WIDTH;
			if (nMouseX<0)
				nMouseX=0;

			break; 
        
		case DIMOFS_Y://����������� �� ��� Y
			nMouseY+=(dod.dwData*nScale);

			if (nMouseY>(MAX_HEIGHT-MOUSE_HEIGHT))
				nMouseY=MAX_HEIGHT-MOUSE_HEIGHT;
			if (nMouseY<0)
				nMouseY=0;

			break; 
        
		case DIMOFS_BUTTON0://����� ������ ���� 
			if (isButton)
			{
				lb=true;
			}
			break;
		}
	}

	MoveMouse(nMouseX,nMouseY,NULL,lb, false,false);
}

//--------------------------------------------------------
// ����������� ��� ����������� ����
//--------------------------------------------------------
void AccessMouse(BOOL isActive)
{
	if (pDIMouse) 
	{
		if (isActive)
			pDIMouse->Acquire();	
		else
			pDIMouse->Unacquire();	
	}
}

//--------------------------------------------------------
// �������� ���������������� ������ �� ����������
//--------------------------------------------------------
void ProcessKeyboard(BOOL canShuffle)
{
	HRESULT hRet;

	up=false;
	down=false;
	left=false;
	right=false;
	fire=false;
	accel=false;

	hRet=pDIKeyboard->GetDeviceState(sizeof(bKeyState), bKeyState);
	if (hRet==DIERR_INPUTLOST)
	{
		pDIKeyboard->Acquire();
		hRet=pDIKeyboard->GetDeviceState(sizeof(bKeyState), bKeyState);
	}
	if (FAILED(hRet))
		return ;

	if (KEYDOWN(bKeyState,DIK_UP))			up=true;
	if (KEYDOWN(bKeyState,DIK_DOWN))		down=true;
	if (KEYDOWN(bKeyState,DIK_LEFT))		left=true;
	if (KEYDOWN(bKeyState,DIK_RIGHT))		right=true;
	if (KEYDOWN(bKeyState,DIK_LCONTROL))	fire=true;
	if (KEYDOWN(bKeyState,DIK_LSHIFT))		accel=true;

	MoveShip(up,down,left,right,fire,accel);

	if (KEYDOWN(bKeyState,DIK_ESCAPE))
	if(!bLogo)
	{
		bLogo=true;
		HoleInit();
		PlayMusic(MENU_MUSIC);
	}
	else
		if (fasa==3) 
		{
			MouseLock=true;
			bExit=true;
		}
}

//--------------------------------------------------------
// ����������� ��� ����������� ����
//--------------------------------------------------------
void AccessKeyboard(BOOL isActive)
{
	if (pDIKeyboard) 
	{
		if (isActive)
			pDIKeyboard->Acquire();	
		else
			pDIKeyboard->Unacquire();	
	}
}


//--------------------------------------------------------
// ���������, ������ �� ��������� ������� 
//--------------------------------------------------------
BOOL KeyPressed(BYTE key)
{
	if(KEYDOWN(bKeyState,key) && !bKeyBuffer[key]) 
	{
		bKeyBuffer[key]=TRUE;
		return (TRUE);
	}	
	else if(!(KEYDOWN(bKeyState,key)) && bKeyBuffer[key]) 
	{
		bKeyBuffer[key]=FALSE;
		return(FALSE);
	}
	return(FALSE);
}

//--------------------------------------------------------
// � ������� ���������� �������� ��������� ���� ������
//--------------------------------------------------------
void SleepGame(DWORD dwMilliseconds)
{
	DWORD dwNeedTime;
	DWORD dwCurTime;
	
	dwNeedTime=dwCurTime=timeGetTime();

	while ((dwNeedTime-dwCurTime)<dwMilliseconds)
	{
		dwNeedTime=timeGetTime();
		DrawFrame();
		ProcessMouse(FALSE);
		ProcessKeyboard(FALSE);
	}
}
