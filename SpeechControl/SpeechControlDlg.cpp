
// SpeechControlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SpeechControl.h"
#include "SpeechControlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSpeechControlDlg �Ի���




CSpeechControlDlg::CSpeechControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeechControlDlg::IDD, pParent) , m_TestIat("")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpeechControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CMD_CONTROL_INFO, m_lclCmdControlInfo);
}

BEGIN_MESSAGE_MAP(CSpeechControlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SPEECH_START, &CSpeechControlDlg::OnBnClickedButtonSpeechStart)
	ON_BN_CLICKED(IDC_BUTTON_SPEECH_STOP, &CSpeechControlDlg::OnBnClickedButtonSpeechStop)
	ON_MESSAGE(WM_MSG_DLG_SEND_INFO, &CSpeechControlDlg::OnRecvMessage)
END_MESSAGE_MAP()


// CSpeechControlDlg ��Ϣ�������

BOOL CSpeechControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	DWORD styles = LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES| m_lclCmdControlInfo.GetExtendedStyle();
	m_lclCmdControlInfo.SetExtendedStyle(styles);
	m_lclCmdControlInfo.InsertColumn(0, L"����", 0, 60 );
	m_lclCmdControlInfo.InsertColumn(1, L"��������",0 , 120);
	m_lclCmdControlInfo.InsertColumn(2, L"��Ӧ������",0 , 270);
	m_lclCmdControlInfo.InsertColumn(3, L"��������",0 , 60);
	m_zCtrl.Init(this);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSpeechControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSpeechControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSpeechControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static UINT AFX_CDECL SpeechControlThreadProc(LPVOID pParam)
{
	CSpeechControlDlg *pThis = (CSpeechControlDlg*) pParam;
	return pThis->SpeechControlThreadProc(pParam);
}

//	��ʼ��������
void CSpeechControlDlg::OnBnClickedButtonSpeechStart()
{
	// TODO: Add your control notification handler code here
	m_dwThreadRunning = TRUE;
	m_wtSpeechThread = AfxBeginThread(::SpeechControlThreadProc, this);
	SetWindowText(L"��ʼ��������");
}

//	ֹͣ��������
void CSpeechControlDlg::OnBnClickedButtonSpeechStop()
{
	// TODO: Add your control notification handler code here
	m_dwThreadRunning = FALSE;
	SetWindowText(L"ֹͣ��������");
}

UINT CSpeechControlDlg::SpeechControlThreadProc(LPVOID pParam)
{
	if (!m_TestIat.Init(this)){
		return 0;
	}
	m_TestIat.LoopKey(&m_dwThreadRunning);
	return 0;
}

void CSpeechControlDlg::AddUseCount(char *str)
{
	CStringW strW;
	strW = str;
	for (int i = 0 ; i < m_lclCmdControlInfo.GetItemCount(); i++)
	{
		if (strW.Compare(m_lclCmdControlInfo.GetItemText(i, 1)) == 0)
		{
			strW.Format(L"%d", wcstol(m_lclCmdControlInfo.GetItemText(i, 3), NULL, 10) + 1);
			m_lclCmdControlInfo.SetItemText(i, 3, strW);
		}
	}
}

LRESULT CSpeechControlDlg::OnRecvMessage(WPARAM wParam, LPARAM lParam) 
{
	//	�յ�������
	switch(wParam)
	{
	case MSG_TYPE_UNKNOW:
		break;
	case MSG_TYPE_RESULT_STRING:
		{
			char strBuf[MAX_PATH] = "";
			if (m_zCtrl.Work((char*)lParam, strBuf, sizeof(strBuf)))
			{
				AddUseCount(strBuf);
				m_vecResult.push_back((char*)lParam);
			}
			else
			{
				m_vecResultErr.push_back((char*)lParam);
			}
		}
		break;
	case MSG_TYPE_ERROR_MSG:
		{
			m_vecError.push_back((char*)lParam);
		}
		break;
	case MSG_TYPE_INFO_MSG:
		{
			m_vecInfo.push_back((char*)lParam);
		}
		break;
	case MSG_TYPE_INSERT_LIST:
		{
			INSERT_LIST *pil = (INSERT_LIST *)lParam;
			CString str;
			int i = m_lclCmdControlInfo.GetItemCount();
			str = pil->strType;
			m_lclCmdControlInfo.InsertItem(i , str);
			str = pil->strCmd;
			m_lclCmdControlInfo.SetItemText(i, 1, str);
			str = pil->strPath;
			m_lclCmdControlInfo.SetItemText(i, 2, str);
			str = L"0";
			m_lclCmdControlInfo.SetItemText(i, 3, str);
		}
		break;
	default:
		break;
	}
	if (lParam)
	{
		free((void*)lParam);
	}
	return 0; 
} 