#pragma once
#include <vector>

class CZooControlClass
{
public:
	CZooControlClass(void);
	virtual ~CZooControlClass(void);

	void Init(LPVOID pParam);

public:
	DWORD Work(char *strCmd, char *strOut = NULL, DWORD dwOutLen = 0);

private:
	typedef struct _CMD_PATH 
	{
		string cmd;		//	����
		string fmt;		//	ģʽ��
		string path;	//	·��
	}CMD_PATH;
	typedef std::vector<CMD_PATH> PATH_ARRAY;

	LPVOID m_pMainDlg;

	enum
	{
		CONTROL_STARTUP = 1 ,
		CONTROL_TERMINIT ,
	};

	void InsertToVector(DWORD dwType, CMD_PATH *cp);

	BOOL CreateProcess(const char *str);

	bool FindFromMap(PATH_ARRAY &tVec, char *str, char *out, DWORD dwLen, char *strOut, DWORD dwOutLen);

	void LoadCfg();

	BOOL GetControl(char *buf, CMD_PATH *cp);

	BOOL StringCmp(const char *formatString, const char *srcString);

	char m_strDir[MAX_PATH*2];


	PATH_ARRAY m_vecStartProcessU;		//		��ͨ�������������ƥ��
	PATH_ARRAY m_vecKillProcessU;		//		��ͨ����Ľ�������ƥ��
};
