
// SpeechControlDlg.h : ͷ�ļ�
//

#pragma once
#include "Resource.h"
#include "SpeechCore\TestIat.h"
#include "ZooControlClass.h"
#include "afxcmn.h"

enum
{
	MSG_TYPE_UNKNOW = 0 ,		//	δ֪������Ϣ
	MSG_TYPE_RESULT_STRING ,	//	ָ���
	MSG_TYPE_ERROR_MSG ,		//	������Ϣ
	MSG_TYPE_INFO_MSG ,			//	������Ϣ
	MSG_TYPE_INSERT_LIST,		//	����һ��ָ��б�
};

typedef struct
{
	const char *strType;
	const char *strCmd;
	const char *strPath;
}INSERT_LIST;

// CSpeechControlDlg �Ի���
class CSpeechControlDlg : public CDialog
{
// ����
public:
	CSpeechControlDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SPEECHCONTROL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnRecvMessage(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButtonSpeechStart();
	afx_msg void OnBnClickedButtonSpeechStop();
public:
	UINT SpeechControlThreadProc(LPVOID pParam);
	void AddUseCount(char *str);
	CListCtrl m_lclCmdControlInfo;
private:
	CTestIat m_TestIat;
	CWinThread* m_wtSpeechThread;
	DWORD m_dwThreadRunning;
	CZooControlClass m_zCtrl;
	std::vector<std::string> m_vecError;
	std::vector<std::string> m_vecResult;
	std::vector<std::string> m_vecResultErr;
	std::vector<std::string> m_vecInfo;
};
