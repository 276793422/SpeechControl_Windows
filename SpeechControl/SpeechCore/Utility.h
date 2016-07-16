#pragma once
#define MAX_ERR_DICT      12

class CUtility
{
public:
    CUtility();
    ~CUtility();

    static bool readText(string szPathFile, string &szText);
    static bool writeText(string szPathFile, string szText);

    static string GBToUTF8(const char* str);
    static string UTF8ToGB(const char* str);

    static void setErrDict(int nDict, int nErrCode, string szError);
    static string getErrDict(int nDict, int nErrCode);

    static bool getPathFile(string szPathFile, string &szFilePath, string &szFileName);

    static string getNowTime();
    static string showTime();

    static int getTimeGap();
};

