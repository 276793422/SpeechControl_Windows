#pragma once
#include <iosfwd>
using namespace std;



#define CMD_STATUS_STOP      0
#define CMD_STATUS_READY     1
#define CMD_STATUS_RUN       2
#define CMD_STATUS_DONE      3

#define IAT_MAX_AUDIO_BUFFERS     5

class CTestIat
{
public:
    CTestIat(string szOutputPathFile);
    ~CTestIat();

	bool LoopKey(DWORD *pdwLoop);

    bool Init(LPVOID pParam);
    void Fini();

private:
	LPVOID m_pMainDlg;

private:
    bool StartAudio();
    void StopAudio();


    bool waveInit();
    void waveFini();
    static DWORD CALLBACK CTestIat::MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

    bool iflyInit();
    void iflyFini();

    bool iflyStartSession();
    void iflyStopSession();
    bool iflySendSession(char *pPCM, int pcmSize);

    // dump file
	bool dumpData(char *pPCM, int pcmSize);
    void dumpWaveHead(int nWaveSize);
    void dumpWaveFile();

    bool saveText(string szResult);

    void initErrCode();

private:
    HWAVEIN m_hWaveIn;  //输入设备  

    //采集音频时包含数据缓存的结构体  
    //WAVEHDR m_wHdr1;
    //WAVEHDR m_wHdr2; 
    WAVEHDR m_wHdr[IAT_MAX_AUDIO_BUFFERS];

    bool m_bInit;
    bool m_bStart;
    bool m_bStopAudio;

    bool m_bFirst;
    char *m_sessionID;
    char m_result[10240];

    ofstream m_sfile;

    string m_szOutputPathFile;

    char *m_pWaveData;
    int   m_nWaveDataSize;
};

