#include "stdafx.h"
#include "Utility.h"

static map<int, string> mapErrDict[MAX_ERR_DICT];

CUtility::CUtility()
{
}


CUtility::~CUtility()
{
}

bool CUtility::readText(string szPathFile, string &szText)
{
	CString strFile;
	strFile = szPathFile.c_str();
    ifstream sfile(strFile, ios::in | ios::_Nocreate);
    if (sfile.is_open())
	{
        char buf[4];
        sfile >> buf;
        szText = buf;
        sfile.close();
        return true;
    }

    return false;
}

bool CUtility::writeText(string szPathFile, string szText)
{
	CString strFile;
	strFile = szPathFile.c_str();
    ofstream sfile(strFile, ios::out | ios::trunc);
    if (sfile.is_open())
	{
        sfile << szText << endl;
        sfile.close();

        return true;
    }

    return false;
}

string CUtility::GBToUTF8(const char* str)
{
    std::string result;
    WCHAR *strSrc;
    char *szRes;

    //获得临时变量的大小
    int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    strSrc = new WCHAR[i + 1];
    MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);

    //获得临时变量的大小
    i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
    szRes = new char[i + 1];
    int j = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);

    result = szRes;
    delete[]strSrc;
    delete[]szRes;

    return result;
}

string CUtility::UTF8ToGB(const char* str)
{
    std::string result;
    WCHAR *strSrc;
    char *szRes;

    //获得临时变量的大小
    int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    strSrc = new WCHAR[i + 1];
    MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

    //获得临时变量的大小
    i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
    szRes = new char[i + 1];
    WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

    result = szRes;
    delete[]strSrc;
    delete[]szRes;

    return result;
}

void CUtility::setErrDict(int nDict, int nErrCode, string szError)
{
    mapErrDict[nDict].insert(map<int, string>::value_type(nErrCode, szError));
}

string CUtility::getErrDict(int nDict, int nErrCode)
{
    return mapErrDict[nDict][nErrCode];
    map<int, string>::iterator it = mapErrDict[nDict].find(nErrCode);
    if (it == mapErrDict[nDict].end())
	{
        return "未知的错误";
    }

    return mapErrDict[nDict][nErrCode];
}

bool CUtility::getPathFile(string szPathFile, string &szFilePath, string &szFileName)
{
    if (szPathFile.size() <= 0)
	{
        return false;
    }

    for (DWORD i = 0; i < szPathFile.size(); ++i)
	{
        if (szPathFile[i] == '\\')
		{
            szPathFile[i] = '/';
        }
    }

    int nPos = szPathFile.rfind('/');
    if (nPos < 0)
	{ // no found path, only return file name;
        szFileName = szPathFile;
        szFilePath = "";
        return false; // no path
    }
    else if (nPos == szPathFile.size())
	{ // at last, only return path
        szPathFile = szPathFile.substr(0, szPathFile.size() - 1);
        szFileName = "";
    }
    else
	{
        szFileName = szPathFile.substr(nPos + 1, szPathFile.size());
        szFilePath = szPathFile.substr(0, nPos);
    }

    return true;
}

string CUtility::getNowTime()
{
    time_t now_time;
    now_time = time(NULL);
    struct tm local;
    localtime_s(&local, &now_time);  //获取当前系统时间  

    char curTime[32] = {0,};
    sprintf_s(curTime, "%04d%02d%02d_%02d%02d%02d", local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
    return curTime;
}

string CUtility::showTime()
{
    time_t now_time;
    now_time = time(NULL);
    struct tm local;
    localtime_s(&local, &now_time);  //获取当前系统时间  

    char curTime[32] = { 0, };
    sprintf_s(curTime, "%04d/%02d/%02d %02d:%02d:%02d", local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
    return curTime;
}

int CUtility::getTimeGap()
{
    // get time gap in wave callback 
    static int g_nLastTimestamp = 0;
    int nGap = 0;
    int nTimestamp = ::GetTickCount();

    if (g_nLastTimestamp > 0)
	{
        nGap = nTimestamp - g_nLastTimestamp;
    }

    g_nLastTimestamp = nTimestamp;
    return nGap;
}