
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持









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


//	消息类型，消息串，消息长度
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