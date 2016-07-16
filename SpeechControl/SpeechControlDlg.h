
// SpeechControlDlg.h : 头文件
//

#pragma once
#include "Resource.h"
#include "SpeechCore\TestIat.h"
#include "ZooControlClass.h"
#include "afxcmn.h"

enum
{
	MSG_TYPE_UNKNOW = 0 ,		//	未知类型信息
	MSG_TYPE_RESULT_STRING ,	//	指令返回
	MSG_TYPE_ERROR_MSG ,		//	错误信息
	MSG_TYPE_INFO_MSG ,			//	常规信息
	MSG_TYPE_INSERT_LIST,		//	插入一条指令到列表
};

typedef struct
{
	const char *strType;
	const char *strCmd;
	const char *strPath;
}INSERT_LIST;

// CSpeechControlDlg 对话框
class CSpeechControlDlg : public CDialog
{
// 构造
public:
	CSpeechControlDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SPEECHCONTROL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
