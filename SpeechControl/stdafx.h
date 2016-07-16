
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



#include <mmsystem.h>
#include <Windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <io.h>
#include <conio.h>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <locale>
#include <map>


using namespace std;


#define WM_MSG_DLG_SEND_INFO		(WM_USER + 10)

#include "my_head.h"


//	��Ϣ���ͣ���Ϣ������Ϣ����
#define PostMessageToMainDialog(TTT, XXX, YYY)								\
		do																	\
		{																	\
			CSpeechControlDlg *p = (CSpeechControlDlg*)m_pMainDlg;			\
			DWORD dwResultLen = sizeof(char) * (YYY) + 10;					\
			char *pR = (char *)malloc(dwResultLen);							\
			if (pR)															\
			{																\
				ZeroMemory(pR, dwResultLen);								\
				memcpy_s(pR , dwResultLen, (XXX), (YYY));					\
				p->PostMessage(WM_MSG_DLG_SEND_INFO, (TTT), (LPARAM)pR);	\
			}																\
		} while (0)

#define PostMessageToMainDialogS(MMM, NNN)									\
		do																	\
		{																	\
			PostMessageToMainDialog(MMM, NNN, strlen(NNN));					\
		} while (0)

#define PostMessageToMainDialogSE(NNN)										\
		do																	\
		{																	\
			PostMessageToMainDialog(MSG_TYPE_ERROR_MSG, NNN, strlen(NNN));	\
		} while (0)

#define PostMessageToMainDialogSI(NNN)										\
		do																	\
		{																	\
			PostMessageToMainDialog(MSG_TYPE_INFO_MSG, NNN, strlen(NNN));	\
		} while (0)