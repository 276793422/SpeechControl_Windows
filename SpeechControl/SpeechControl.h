
// SpeechControl.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSpeechControlApp:
// �йش����ʵ�֣������ SpeechControl.cpp
//

class CSpeechControlApp : public CWinAppEx
{
public:
	CSpeechControlApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSpeechControlApp theApp;