#include "StdAfx.h"
#include "ZooControlClass.h"
#include "SpeechControlDlg.h"

#define PCRE_STATIC
#include "pcre.h"

#pragma comment(lib, "pcre.lib")

static char *g_cfgmsg = 
"//	当前文件为语音指令的配置文件，这是说明\n"
"//	当前文件格式：\n"
"//	以“//”为起始的一行为注释\n"
"//	当前文件不支持空格，所有命令中的空格都会被无视，路径中可以有空格\n"
"//	配置文件常规写法“[S]打开注册表:注册表=C:\\Windows\\Regedit.exe”\n"
"//		第一部分：“[S]”，为标识符，标识当前操作类型\n"
"//		第二部分：“打开注册表”，为显示信息，显示在街面上\n"
"//		第三部分：“注册表”，为语音命令\n"
"//		第四部分：“C:\\Windows\\Regedit.exe”，为命令对应的程序路径\n"
"//	标识符有：\n"
"//		S：表示这是一条启动命令，对应操作为启动程序\n"
"//		K：表示要结束一个进程，对应操作为结束程序\n"
"//	其他功能正在添加中\n"
"//	目前命令支持非UNICODE的正则表达式\n"
"//	每行一共最多不得超过1000个字母，或者500个汉字\n"
"//	如果语音命令有可能相同，那么会首先执行前面的命令\n"
"\n"
"[S]打开注册表:注册表=C:\\Windows\\Regedit.exe\n"
"[S]打开控制台:控制台=C:\\Windows\\system32\\cmd.exe\n"
"[K]关闭注册表:注册表=C:\\Windows\\Regedit.exe\n"
"[S]打开VS08:(.*)开[vV][sS](.*)08=C:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\Common7\\IDE\\devenv.exe\n"
"[S]打开VS08:(.*)开(.*)[sS](.*)08=C:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\Common7\\IDE\\devenv.exe\n"
"[S]打开记事本:记事本=D:\\notepad.exe\n"
;

DWORD save_file( char *path , LPBYTE buf , DWORD len )
{
	FILE *f = 0;
	int i = 0;
	while (fopen_s(&f, path , "w+" ))
	{
		i++;
		if (i = 5)
		{
			return 0;
		}
	}
	fwrite( buf , len , 1 , f );
	fclose( f );
	return 0;
}

void remove_head_trim(char src[])
{
	char *dst;

	dst = src;
	while (*src == ' ' || *src == '\t' || *src == '\n' || *src == '\r')
		src++;
	while (*dst++ = *src++);
}

void remove_tail_trim(char src[])
{
	int i;

	for (i = strlen(src) - 1; i >= 0; i--)
	{
		if (src[i] != ' ' && src[i] != '\t' && src[i] != '\n' || *src == '\r')
			break;
	}

	src[i+1] = '\0';
}

void remove_code(char *str)
{
	remove_head_trim(str);
	remove_tail_trim(str);
}

CZooControlClass::CZooControlClass(void)
{
	GetCurrentDirectoryA(sizeof(m_strDir), m_strDir);
	strcat_s(m_strDir, "\\WavSave");
	if (PathFileExistsA(m_strDir))
	{
	}
	else
	{
		CreateDirectoryA(m_strDir, NULL);
	}

	GetCurrentDirectoryA(sizeof(m_strDir), m_strDir);
	strcat_s(m_strDir, CONFIG_FILE);
	if (PathFileExistsA(m_strDir))
	{
	}
	else
	{
		save_file(m_strDir, (LPBYTE)g_cfgmsg, strlen(g_cfgmsg));
	}
}

CZooControlClass::~CZooControlClass(void)
{
}

void CZooControlClass::InsertToVector(DWORD dwType, CMD_PATH *cp)
{
	INSERT_LIST *pil = (INSERT_LIST*)malloc(sizeof(INSERT_LIST));
	pil->strCmd = cp->cmd.c_str();
	pil->strPath = cp->path.c_str();
	switch(dwType)
	{
	case CONTROL_STARTUP:
		{
			pil->strType = "启动";
			m_vecStartProcessU.push_back(*cp);
			CSpeechControlDlg *p = (CSpeechControlDlg*)m_pMainDlg;
			p->SendMessage(WM_MSG_DLG_SEND_INFO, MSG_TYPE_INSERT_LIST, (LPARAM)pil);
		}
		break;
	case CONTROL_TERMINIT:
		{
			pil->strType = "结束";
			m_vecStartProcessU.push_back(*cp);
			CSpeechControlDlg *p = (CSpeechControlDlg*)m_pMainDlg;
			p->SendMessage(WM_MSG_DLG_SEND_INFO, MSG_TYPE_INSERT_LIST, (LPARAM)pil);
		}
		break;
	default:
		break;
	}
}

BOOL CZooControlClass::GetControl(char *buf, CMD_PATH *cp)
{
	BOOL bRet = FALSE;
	char *cmd;
	char *fmt;
	char *path;
	do 
	{
		//	cmd
		cmd = buf + 3;
		path = strstr(cmd, ":");
		if (path == NULL)
		{
			continue;
		}
		*path = '\0';
		//	fmt
		fmt = path + 1;
		path = strstr(fmt, "=");
		if (path == NULL)
		{
			continue;
		}
		*path = '\0';
		//	path
		path = path + 1;
		remove_code(path);

		bRet = TRUE;
		cp->cmd = cmd;
		cp->fmt = fmt;
		cp->path = path;
	} while (FALSE);
	return bRet;
}

void CZooControlClass::LoadCfg()
{
	GetCurrentDirectoryA(sizeof(m_strDir), m_strDir);
	strcat_s(m_strDir, CONFIG_FILE);
	FILE *f = NULL;
	errno_t e = -1;
	char buf[1024];
	CMD_PATH cp;
	do 
	{
		if (!PathFileExistsA(m_strDir))
		{
			break;
		}
		e = fopen_s(&f , m_strDir, "r");
		if (e != 0 )
		{
			break;
		}
		if (f == NULL)
		{
			break;
		}
		ZeroMemory(buf, sizeof(buf));
		while(1)
		{
			if (fgets(buf, sizeof(buf), f))
			{
				if (strlen(buf) > 3)
				{
					if (buf[0] == '/' && buf[1] == '/')
					{
						//	注释部分
					}
					else if (buf[0] == '[' && buf[2] == ']')
					{
						//	Control
						cp.cmd = "";
						cp.fmt = "";
						cp.path = "";
						if (GetControl(buf, &cp))
						{
							//	指令部分
							switch (buf[1])
							{
							case 'S':
								InsertToVector(CONTROL_STARTUP, &cp);
								break;
							case 'K':
								InsertToVector(CONTROL_TERMINIT, &cp);
								break;
							default:
								break;
							}
						}
					}
				}
			}
			if (feof(f))
			{
				break;
			}
		}
		
	} while (0);
	if (f)
	{
		fclose(f);
	}
}

void CZooControlClass::Init(LPVOID pParam)
{
	if (pParam == NULL)
	{
		return;
	}
	m_pMainDlg = pParam;

	LoadCfg();
}

bool CZooControlClass::FindFromMap(PATH_ARRAY &tVec, char *str, char *out, DWORD dwLen, char *strOut, DWORD dwOutLen)
{
	bool bRet = false;
	for (PATH_ARRAY::iterator it = tVec.begin(); it != tVec.end() ; it++)
	{
		//if (strstr(str, it->cmd.c_str()))
		if (StringCmp(it->fmt.c_str(), str))
		{
			strcpy_s(out, dwLen, it->path.c_str());
			if (strOut != NULL && dwOutLen != 0)
			{
				strcpy_s(strOut, dwOutLen, it->cmd.c_str());
			}
			bRet = true;
			break;
		}
	}
	return bRet;
}

DWORD CZooControlClass::Work(char *strCmd, char *strOut, DWORD dwOutLen)
{
	DWORD dwRet = 0;
	int i = 0;
	static char *strCode[] = {"，", "。", "！", "？"};
	char *p = NULL;
	DWORD dwLen = strlen(strCmd);
	for (i = 0; i < 4; i++)
	{
		p = strstr(strCmd, strCode[i]);
		if (p)
		{
			strcpy_s(p, dwLen - (p - strCmd), p + 2);
		}
	}

#ifdef _DEBUG
	{
		char str[1024] = "";
		sprintf_s(str, "\n%s\n",strCmd);
		OutputDebugStringA(str);
	}
#endif
	char strPath[MAX_PATH * 2] = "";
	if (FindFromMap(m_vecStartProcessU, strCmd, strPath, MAX_PATH * 2, strOut, dwOutLen))
	{
		if (CreateProcess(strPath))
		{
			//	成功
			return 1;
		}
	}

	return 0;
}

BOOL CZooControlClass::CreateProcess(const char *str)
{
	BOOL bRet = FALSE;
	char strCreateProcess[1024] = "";
	strcat_s(strCreateProcess , str);
#if 1
	HINSTANCE hi = ::ShellExecuteA(0,"open",strCreateProcess,"","",SW_SHOWNORMAL);
	if ((DWORD)hi > 32)
	{
		TRACE("CreateProcessA() OK , GetLastError() = %d\n", GetLastError());
		bRet = TRUE;
	}
#else
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	ZeroMemory( &pi, sizeof(pi) );
	si.cb = sizeof(si);
	if (::CreateProcessA(NULL,strCreateProcess,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		bRet = TRUE;
	}
#endif
	else
	{
		TRACE("CreateProcessA() error , GetLastError() = %d\n", GetLastError());
	}
 	return bRet;
}

BOOL CZooControlClass::StringCmp(const char *pattern,const char *src)
{
	pcre			*re = NULL;
	const char		*error;
	int				erroffset;
#define OVECCOUNT 30    /* should be a multiple of 3 */
	int				ovector[OVECCOUNT];
	int				rc;
	BOOL			bRet = FALSE;
	//	这里用正则表达式来解析命令
	do 
	{
		re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
		if (re == NULL)
		{
			break;
		}
		rc = pcre_exec(re, NULL, src, strlen(src), 0, 0, ovector, OVECCOUNT);
		if (rc < 0)
		{
			break;
		}
		bRet = TRUE;
	} while (false);
	free(re);
#undef OVECCOUNT

	return bRet;
}