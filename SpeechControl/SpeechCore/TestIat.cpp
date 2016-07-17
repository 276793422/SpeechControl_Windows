#include "stdafx.h"
#include "../SpeechControlDlg.h"
#include "TestIat.h"
#include "Utility.h"

#include "../iflyLib/include/qisr.h"
#include "../iflyLib/include/msp_cmn.h"
#include "../iflyLib/include/msp_errors.h"

#pragma comment(lib, "Winmm.lib")          //x86
#pragma comment(lib, "iflyLib/lib/msc.lib")   //x86


#define IATLISTEN_MAX_AUDIO_SIZE      10*1024*1024  // max audio size 10 M

// ifly error string
#define MAX_STRING_SIZE     32
char g_iflyRecStatus[][MAX_STRING_SIZE] =
{
	"ʶ��ɹ�",
	"ʶ�������û��ʶ����",
	"����ʶ����",
	"����",
	"������Ч��Ƶ",
	"ʶ�����",
	"����",
	"����",
	"����",
	"����"
};

char g_iflyEpStatus[][MAX_STRING_SIZE] =
{
	"��û�м�⵽��Ƶ��ǰ�˵�",
	"���ڽ�����Ƶ����",
	"NULL",
	"��⵽��Ƶ�ĺ�˵�",
	"��ʱ",
	"���ִ���",
	"��Ƶ����"
};

#define IL_MSP_ERR_CODE_DICT   0
#define IL_WAVE_ERR_CODE_DICT  1

CTestIat::CTestIat(string szOutputPathFile)
{
	m_bInit = false;
	m_bStart = false;
	m_sessionID = NULL;
	m_pWaveData = NULL;
	m_dwWaveDataLen = IATLISTEN_MAX_AUDIO_SIZE;
	m_nWaveDataSize = 0;

	m_szOutputPathFile = szOutputPathFile;

	initErrCode();
}

CTestIat::~CTestIat()
{
	m_bInit = false;
	m_bStart = false;
	m_sessionID = NULL;
	m_nWaveDataSize = 0;

	if (m_pWaveData)
	{
		free(m_pWaveData);
		m_pWaveData = NULL;
	}

	Fini();
}

bool CTestIat::Init(LPVOID pParam)
{
	if (pParam == NULL)
	{
		return false;
	}
	m_pMainDlg = pParam;

	if (!iflyInit())
	{
		PostMessageToMainDialogSE("�޷����ӵ�Ѷ������ƽ̨��������������");
		return false;
	}

	if (!waveInit())
	{
		PostMessageToMainDialogSE("�޷��򿪱�����Ƶ�����豸");
		iflyFini();
		return false;
	}

	if (!m_pWaveData)
	{
		m_pWaveData = (char *)malloc(m_dwWaveDataLen);
		if (!m_pWaveData)
		{
			iflyFini();
			waveFini();
			PostMessageToMainDialogSE("�����ڴ�ʧ��");
			return false;
		}

		m_nWaveDataSize = 0;
		memset(m_pWaveData, 0, m_dwWaveDataLen);
	}
	else
	{

	}

	m_bInit = true;
	return true;
}

void CTestIat::Fini()
{
	iflyFini();
	waveFini();
	m_bInit = false;
}

bool CTestIat::LoopKey(DWORD *pdwLoop)
{
	if (!m_bInit)
	{
		PostMessageToMainDialogSE("����δ��ʼ��");
		return false;
	}

	DWORD dwStartStatus = 0;
	while (1)
	{
		if (dwStartStatus == 0)
		{
			//	�����ǰֹͣ�ˣ���ô������һ��
			PostMessageToMainDialogSI("��ʼ¼��");
			StartAudio();
			dwStartStatus = 1;
		}

		if (*pdwLoop == FALSE)
		{
			//	����Run״̬���رգ�����
			PostMessageToMainDialogSI("��ǰֹͣ");
			StopAudio();
			break;
		}

		if (m_bStopAudio)
		{
			PostMessageToMainDialogSI("¼������");
			StopAudio();
			dwStartStatus = 0;
		}

		::Sleep(100); // delay 1s
	}

	return true;
}

bool CTestIat::StartAudio()
{
	if (m_bStart)
	{
		return false;
	}

	if (!iflyStartSession())
	{
		return false;
	}

	m_bStart = true;
	m_bFirst = true;
	m_bStopAudio = false;
	memset(m_result, 0, sizeof(m_result));

	// init dump data
	m_nWaveDataSize = 0;
	memset(m_pWaveData, 0, m_dwWaveDataLen);

	//������wHdr��ӵ�waveIn��ȥ  
	for (int i = 0; i < IAT_MAX_AUDIO_BUFFERS; ++i)
	{
		waveInAddBuffer(m_hWaveIn, &m_wHdr[i], sizeof (WAVEHDR));
	}

	//��ʼ��Ƶ�ɼ�  
	waveInStart(m_hWaveIn);
	PostMessageToMainDialogSE("��ʼ������д...");
	return true;
}

void CTestIat::StopAudio()
{
	if (m_bStart)
	{
		m_bStart = false;
		//memset(m_result, 0, sizeof(m_result));
		iflyStopSession();

		//ֹͣ��Ƶ�ɼ�  
		waveInStop(m_hWaveIn);

		// waveFini();
		//closeDumpFile();
		m_bStopAudio = false;
		PostMessageToMainDialogSE("������д������");

		//dumpWaveFile();
	}
}

bool CTestIat::waveInit()
{
	WAVEFORMATEX waveform; //�ɼ���Ƶ�ĸ�ʽ���ṹ��  
	waveform.wFormatTag = WAVE_FORMAT_PCM;//������ʽΪPCM  
	waveform.nSamplesPerSec = 16000;//�����ʣ�16000��/��  
	waveform.wBitsPerSample = 16;//�������أ�16bits/��  
	waveform.nChannels = 1;//������������������  
	waveform.nAvgBytesPerSec = 16000 * 2;//ÿ��������ʣ�����ÿ���ܲɼ������ֽڵ�����  
	waveform.nBlockAlign = 2;//һ����Ĵ�С������bit���ֽ�������������  
	waveform.cbSize = 0;//һ��Ϊ0  

	//ʹ��waveInOpen����������Ƶ�ɼ�  
	MMRESULT mmr = waveInOpen(&m_hWaveIn, WAVE_MAPPER, &waveform, (DWORD)(MicCallback), DWORD(this), CALLBACK_FUNCTION);
	if (mmr != MMSYSERR_NOERROR)
	{
		return false;
	}

	for (int i = 0; i < IAT_MAX_AUDIO_BUFFERS; ++i)
	{
		BYTE *pBuffer1;//�ɼ���Ƶʱ�����ݻ���  
		DWORD bufsize = 8 * 1024;

		pBuffer1 = new BYTE[bufsize];
		m_wHdr[i].lpData = (LPSTR)pBuffer1;
		m_wHdr[i].dwBufferLength = bufsize;
		m_wHdr[i].dwBytesRecorded = 0;
		m_wHdr[i].dwUser = 0;
		m_wHdr[i].dwFlags = 0;
		m_wHdr[i].dwLoops = 1;
		m_wHdr[i].lpNext = NULL;
		m_wHdr[i].reserved = 0;

		//�������õ�m_wHdr1��Ϊ����  
		waveInPrepareHeader(m_hWaveIn, &m_wHdr[i], sizeof(WAVEHDR));
	}

	return true;
}

void CTestIat::waveFini()
{
	m_bStart = false;
	waveInStop(m_hWaveIn);
	waveInReset(m_hWaveIn);
	//waveInUnprepareHeader(m_hWaveIn, &m_wHdr1, sizeof(WAVEHDR));
	//waveInUnprepareHeader(m_hWaveIn, &m_wHdr2, sizeof(WAVEHDR));
	for (int i = 0; i < IAT_MAX_AUDIO_BUFFERS; ++i)
	{
		waveInUnprepareHeader(m_hWaveIn, &m_wHdr[i], sizeof(WAVEHDR));
	}

	waveInClose(m_hWaveIn);
	m_hWaveIn = NULL;
}

bool CTestIat::dumpData(char *pPCM, int pcmSize)
{
	if (!m_pWaveData)
	{
		return false;
	}

	if ((DWORD)(m_nWaveDataSize + pcmSize) > m_dwWaveDataLen)
	{
		//	����������
		//	���������ڴ棬Ȼ�󻻿ռ�
		m_dwWaveDataLen = m_dwWaveDataLen * 2;
		void *p = malloc(m_dwWaveDataLen * sizeof(char));
		if (p == NULL)
		{
			MessageBoxA(NULL, "�����ڴ治�㣬��Ƶ��¼ʧ��", "���ź�", MB_OK);
			m_nWaveDataSize = 0;
			return false;
		}
		memcpy(p, m_pWaveData, m_nWaveDataSize);
		free(m_pWaveData);
		m_pWaveData = (char *)p;
	}

	memcpy(m_pWaveData + m_nWaveDataSize, pPCM, pcmSize);
	m_nWaveDataSize += pcmSize;

	return true;
}

void CTestIat::dumpWaveHead(int nWaveSize)
{
	/*------------------------Wave File Structure ------------------------------------ */
	typedef struct RIFF_CHUNK
	{
		char fccID[4];				// must be "RIFF"
		unsigned long dwSize;		// all bytes of the wave file subtracting 8,
									// which is the size of fccID and dwSize
		char fccType[4];			// must be "WAVE"
	}WAVE_HEADER;

	// 12 bytes
	typedef struct FORMAT_CHUNK
	{
		char fccID[4];				// must be "fmt "
		unsigned long dwSize;		// size of this struct, subtracting 8, which
									// is the sizeof fccID and dwSize
		unsigned short wFormatTag;	// one of these: 1: linear,6: a law,7:u-law
		unsigned short wChannels;	// channel number
		unsigned long dwSamplesPerSec;	// sampling rate
		unsigned long dwAvgBytesPerSec;	// bytes number per second
		unsigned short wBlockAlign;	// ÿ����������λ��(���ֽ���), ��ֵΪ:ͨ��
									// ��*ÿ����������λֵ/8�����������Ҫһ�δ�
									// ������ֵ��С���ֽ�����, �Ա㽫��ֵ����
									// �������ĵ���ÿ����ռ�����ֽ�:
									// NumChannels * uiBitsPerSample/8
		unsigned short uiBitsPerSample;	// quantization
	}FORMAT;

	// 12 bytes
	// ���ݽṹ
	typedef struct
	{
		char fccID[4]; // must be "data"
		unsigned long dwSize; // byte_number of PCM data in byte
	}DATA;

	// 24 bytes
	// The fact chunk is required for all new WAVE formats.
	// and is not required for the standard WAVE_FORMAT_PCM files
	// Ҳ����˵������ṹ��Ŀǰ���Ǳ���ģ�һ�㵱wav�ļ���ĳЩ���ת�����ɣ��������Chunk
	// ���������д�ˣ�����������µĽṹ���������ĸ��ṹ���е�λ��ҲҪ���ڵ���
	typedef struct
	{
		char fccID[4]; // must be "fact"
		unsigned long id; // must be 0x4
		unsigned long dwSize; // ��ʱû������ɶ��
	}FACT;

	WAVE_HEADER WaveHeader;
	FORMAT WaveFMT;
	DATA WaveData;
	FACT WaveFact;
	memset(&WaveHeader, 0, sizeof(WAVE_HEADER));
	memcpy(WaveHeader.fccID, "RIFF", 4);
	memcpy(WaveHeader.fccType, "WAVE", 4);

	// dwSize ������wave�ļ��Ĵ�С���ֽ�������������������HEADER�е�ǰ�������ṹ:
	// HEADER.fccID��HEAD.dwSize)
	// WaveHeader.dwSize = length + 0x24; // �����д��fact������36���ֽڣ�
	// 44- 8 = 36��

	WaveHeader.dwSize = nWaveSize + 0x30; // ���д��fact������48 ��bytes
	memset(&WaveFMT, 0, sizeof(FORMAT));
	memcpy(WaveFMT.fccID, "fmt ", 4);
	WaveFMT.dwSize = 0x10;
	WaveFMT.dwSamplesPerSec = 16000;
	WaveFMT.dwAvgBytesPerSec = 1 * 16000 * 2;
	WaveFMT.wChannels = 1;
	WaveFMT.uiBitsPerSample = 16;
	WaveFMT.wFormatTag = WAVE_FORMAT_PCM;
	WaveFMT.wBlockAlign = 2;
	memset(&WaveFact, 0, sizeof(FACT));
	memcpy(WaveFact.fccID, "fact", 4);
	WaveFact.dwSize = nWaveSize;				// ���ֵ��֪��ʲô��˼
	WaveFact.id = 0x4;
	memset(&WaveData, 0, sizeof(DATA));
	memcpy(WaveData.fccID, "data", 4);
	WaveData.dwSize = nWaveSize;

	m_sfile.write((char *)&WaveHeader, sizeof(WAVE_HEADER));
	m_sfile.write((char *)&WaveFMT, sizeof(FORMAT));
	m_sfile.write((char *)&WaveFact, sizeof(FACT)); // fact���Ǳ����
	m_sfile.write((char *)&WaveData, sizeof(DATA));
}

void CTestIat::dumpWaveFile()
{
	string szOutPath, szName, szDumpFile;
	if (CUtility::getPathFile(m_szOutputPathFile, szOutPath, szName))
	{
		szDumpFile = szOutPath + "WavSave" + "/" + CUtility::getNowTime() + ".wav";
	}
	else
	{
		szDumpFile = "WavSave/"  + CUtility::getNowTime() + ".wav"; // current path
	}

	CString strFile;
	strFile = szDumpFile.c_str();
	m_sfile.open(strFile, ios::out | ios::binary | ios::trunc);
	//	���ļ�ʧ��
	//		m_sfile.is_open()
	
	// add wave header
	if (m_pWaveData && m_nWaveDataSize > 0)
	{
		dumpWaveHead(m_nWaveDataSize);
		m_sfile.write(m_pWaveData, m_nWaveDataSize);
	}

	m_sfile.close();
}

//���������callback���������ڲɼ�������Ƶ���ݶ�����������д���  
DWORD CALLBACK CTestIat::MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	//���CIMAADPCMDlg���������Ƶ�ɼ���  
	CTestIat *pThis = (CTestIat*)dwInstance;
	if (pThis->m_bStopAudio)
	{
		return -1;
	}

	switch (uMsg)
	{
	case WIM_OPEN:
		break;
	case WIM_DATA:
		//������ǶԲɼ���������������ĵط����������˷��ʹ���  
		//	((PWAVEHDR)dwParam1)->lpData			����ǲɼ���������ָ��  
		//	((PWAVEHDR)dwParam1)->dwBytesRecorded	����ǲɼ��������ݳ���  
		if (!pThis->iflySendSession(((PWAVEHDR)dwParam1)->lpData, ((PWAVEHDR)dwParam1)->dwBytesRecorded))
		{
			pThis->m_bStopAudio = true;
			return -1;
		}

		//��������֮��Ҫ�ٰ������������ӻ�ȥ  
		//pWnd->win�����Ƿ�����ɼ�����Ϊ��ֹͣ�ɼ���ʱ��ֻ�е����е�  
		//�������鶼�ڳ���֮�����close��������Ҫֹͣ�ɼ�ʱ�Ͳ�Ҫ��waveInAddBuffer��  
		// if (pWnd->win)
		waveInAddBuffer(hwavein, (PWAVEHDR)dwParam1, sizeof (WAVEHDR));
		break;
	case WIM_CLOSE:
		break;
	default:
		break;
	}

	return 0;
}

bool CTestIat::iflyInit()
{
	///APPID��������Ķ�
	const char* login_configs = "appid = " KDXF_APPID ", work_dir = .";//��¼���� test:538849f9, user:538293af	//	, work_dir = .,timeout=2000
	int ret = 0;

	//�û���¼
	PostMessageToMainDialogSE("[CTestIat::iflyInit] MSPLogin");
	ret = MSPLogin(NULL, NULL, login_configs);//��һ������Ϊ�û������ڶ�������Ϊ���룬�����������ǵ�¼�������û�����������Ҫ��http://open.voicecloud.cnע�Ტ��ȡappid
	if (ret != MSP_SUCCESS)
	{
		PostMessageToMainDialogSI("[CTestIat::iflyInit] MSPLogin failed");
		return false;
	}

	return true;
}

void CTestIat::iflyFini()
{
	MSPLogout();//�˳���¼
}

bool CTestIat::iflyStartSession()
{
	const char* param = "sub = iat, domain = iat, language = zh_ch, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312, vad_bos = 2000, vad_eos = 2000";
						//	"ssm=1,sub=iat,auf=audio/L16;rate=16000,aue=speex-wb,ent=sms16k,rst=plain,rse=gb2312,vad_speech_tail=2000,timeout=5000";//�ɲο������ò����б�
	int errCode = 10;

	m_sessionID = (char *)QISRSessionBegin(NULL, param, &errCode);//��ʼһ·�Ự
	if (errCode)
	{
		PostMessageToMainDialogSE(CUtility::getErrDict(IL_MSP_ERR_CODE_DICT, errCode).c_str());
	}

	if (!m_sessionID)
	{
		return false;
	}

	m_bFirst = true;
	return true;
}

void CTestIat::iflyStopSession()
{
	if (m_sessionID)
	{
		QISRSessionEnd(m_sessionID, NULL);
		m_sessionID = NULL;
	}
}

bool CTestIat::iflySendSession(char *pPCM, int pcmSize)
{
	int errCode = 10;
	int audStat = MSP_AUDIO_SAMPLE_CONTINUE;
	int epStatus = MSP_EP_LOOKING_FOR_SPEECH;
	int recStatus = MSP_REC_STATUS_SUCCESS;

	if (!dumpData(pPCM, pcmSize))
	{
		return false;
	}

	//assert(pcmSize < 12800);
	
	if (m_bFirst){
		audStat = MSP_AUDIO_SAMPLE_FIRST;
		m_bFirst = false;
	}

	int ret = QISRAudioWrite(m_sessionID, (const void *)pPCM, pcmSize, audStat, &epStatus, &recStatus);//д��Ƶ

	if (ret != 0)
	{
		PostMessageToMainDialogSE(CUtility::getErrDict(IL_MSP_ERR_CODE_DICT, ret).c_str());
		return false;
	}

	if (recStatus == MSP_REC_STATUS_SUCCESS)
	{
		const char *rslt = QISRGetResult(m_sessionID, &recStatus, 0, &errCode);//������Ѿ���ʶ���������Ի�ȡ
		if (NULL != rslt)
		{
			strcat_s(m_result, rslt);
		}

		if (errCode){
			PostMessageToMainDialogSE(CUtility::getErrDict(IL_MSP_ERR_CODE_DICT, errCode).c_str());
		}
	}

	// over, stop audio input
	if (epStatus >= MSP_EP_AFTER_SPEECH)
	{
		PostMessageToMainDialog(MSG_TYPE_RESULT_STRING, m_result, strlen(m_result));
		//	����������Ϣ���ļ�
		return false;
	}

	// continue receive audio
	return true;
}

bool CTestIat::saveText(string szResult)
{
	if (m_szOutputPathFile.empty())
	{
		return false;
	}

	// filter the last ���� "��", "��","��","��"
	if (szResult.size() > 2)
	{
		string szFilter = szResult.substr(szResult.size() - 2);
		if (szFilter == "��" || szFilter == "��" || szFilter == "��" || szFilter == "��")
		{
			szResult = szResult.substr(0, szResult.size() - 2);
		}
	}

	string utf8Text = CUtility::GBToUTF8(szResult.c_str());
	if (CUtility::writeText(m_szOutputPathFile, utf8Text))
	{
		return true;
	}
	else{
	}

	return false;
}

void CTestIat::initErrCode()
{
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_SUCCESS, "�ɹ�");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_FAIL, "ʧ��");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_EXCEPTION, "�쳣");

	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_TIME_OUT, "��ʱ");

	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NOT_INIT, "δ��ʼ��");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_INVALID_HANDLE, "��Ч�ĻỰID");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_INVALID_PARA, "��Ч�Ĳ���");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NO_DATA, "û������");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NO_LICENSE, "û����Ȩ���");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_DB_INVALID_APPID, "��ЧAppid");

	// networking error
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NET_GENERAL, "����һ�����");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NET_OPENSOCK, "���׽��ִ���");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NET_CONNECTSOCK, "�׽������Ӵ���");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NET_ACCEPTSOCK, "�׽��ֽ��մ���");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NET_SENDSOCK, "���ʹ���");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NET_RECVSOCK, "���մ���");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NET_INVALIDSOCK, "��Ч���׽���");
	CUtility::setErrDict(IL_MSP_ERR_CODE_DICT, MSP_ERROR_NET_DNS, "DNS��������");

	CUtility::setErrDict(IL_WAVE_ERR_CODE_DICT, MMSYSERR_NOERROR, "�ɹ�");
	CUtility::setErrDict(IL_WAVE_ERR_CODE_DICT, MMSYSERR_ALLOCATED, "��Դ�ѱ�����");
	CUtility::setErrDict(IL_WAVE_ERR_CODE_DICT, MMSYSERR_BADDEVICEID, "��Ч���豸ID");
	CUtility::setErrDict(IL_WAVE_ERR_CODE_DICT, MMSYSERR_NODRIVER, "����Ч�豸����");
	CUtility::setErrDict(IL_WAVE_ERR_CODE_DICT, MMSYSERR_NOMEM, "�ڴ治��");
	CUtility::setErrDict(IL_WAVE_ERR_CODE_DICT, WAVERR_BADFORMAT, "��Ч��wave��Ƶ��ʽ");
}
