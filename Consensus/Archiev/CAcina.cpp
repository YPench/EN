
//#include "F:\\YPench\\Secretary\\Secretary\\stdafx.h"

#include "stdafx.h"

#include <ctime>
#include "CAcina.h"
#include <direct.h>
#include "..\\Include\\CGCom.h"


DWORD WINAPI CAcina_Serial_Dispatch_Thread(LPVOID pParam);
DWORD WINAPI CAcina_Parallel_Dispatch_Thread(LPVOID pParam);
DWORD WINAPI CAcina_Parallel_Dispatch_Thread_with_Killer(LPVOID pParam);



CAcina::CAcina()
{
//	CSecretaryApp *app = (CSecretaryApp *)AfxGetApp();
	char charresponse[1024];
//------------Get CPU Kernal Number, Set Max Work Thread;
	LPSYSTEM_INFO lpSystemInfo;
	lpSystemInfo = new  SYSTEM_INFO;
	GetSystemInfo(lpSystemInfo);
	DIS_ESSAISTA_MAXTHREAD = lpSystemInfo->dwNumberOfProcessors;
	if(DIS_ESSAISTA_MAXTHREAD > MAX_DIS_ESSAISTA_MAXTHREAD)
	{
		DIS_ESSAISTA_MAXTHREAD = MAX_DIS_ESSAISTA_MAXTHREAD;
	}
	if(DIS_ESSAISTA_MAXTHREAD > 4)
	{
		DIS_ESSAISTA_MAXTHREAD = unsigned int(DIS_ESSAISTA_MAXTHREAD*0.8);
	}
	//DIS_ESSAISTA_MAXTHREAD = 1;
	Current_Thread_Number = DIS_ESSAISTA_MAXTHREAD;

	delete lpSystemInfo;
//------------------------------------------------------------------
	for (unsigned int i = 0; i < DIS_ESSAISTA_MAXTHREAD; i++)  { 
        MultThreadEvents[i] = CreateEvent(NULL,  FALSE, FALSE, NULL);
        if (MultThreadEvents[i] == NULL) { 
			strcpy_s(charresponse, "数据初始化出错，请重新启动……");
//			::SendMessage((*(app->pdlg)).m_hWnd, WM_SRESPONCE_MESSAGE,(WPARAM)charresponse,0);
            ExitThread(0); 
        } 
    }
	m_DispatchEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

}

CAcina::~CAcina()
{

}


bool CAcina::Parallel_File_Handler(MultidichTaskInfo& pmMultidichTaskInfo)
{

	pmMultidichTaskInfo.pCAcina = this;
	HANDLE DispThread;
	DWORD DisphreadId;
	DispThread = CreateThread(NULL, 0, CAcina_Parallel_Dispatch_Thread, (LPVOID)&pmMultidichTaskInfo, 0, &DisphreadId);

	//while(WaitForSingleObject(m_DispatchEvent, 5*RESPONCE_WAIT_TIME) == WAIT_OBJECT_0){
	//	AppCall::Response_Diplay("分配线程正在运行！", 0);
	//	Sleep(4500);
	//}
	WaitForSingleObject(m_DispatchEvent, INFINITE);
	return true;
}

DWORD WINAPI CAcina_Parallel_Dispatch_Thread(LPVOID pParam)
{
	pMultidichTaskInfo pcopestruct = (pMultidichTaskInfo)pParam;
	CAcina& t_CAcinaref = *(pcopestruct->pCAcina);
	bool UsedCodeFlag[MAX_DIS_ESSAISTA_MAXTHREAD];
	unsigned int copedessayseq = 0;
	unsigned int RemainderFile;

	for (unsigned int i = 0; i < t_CAcinaref.DIS_ESSAISTA_MAXTHREAD; i++) { 
		UsedCodeFlag[i] = false;
        if (!ResetEvent(t_CAcinaref.MultThreadEvents[i])) {
//			AppCall::Response_Diplay("分配线程初始化失败！", 0);
			SetEvent(t_CAcinaref.m_DispatchEvent);
            ExitThread(0); 
		}
    }
//------Read Every File Path in sContent----------------------------------------------------
	t_CAcinaref.t_Corpus_v.clear();
	NLPOP::Get_Specified_Files(pcopestruct->sContent_v[0], t_CAcinaref.t_Corpus_v, ".txt");
	if(t_CAcinaref.t_Corpus_v.size() == 0){
//		AppCall::Response_Diplay("文件夹中没有待处理文档！", 0);
		SetEvent(t_CAcinaref.m_DispatchEvent);
		ExitThread(0);
	}
//---------------------------------------------------------------------
	RemainderFile = t_CAcinaref.t_Corpus_v.size();
	t_CAcinaref.m_ostrsteam.str("");
	t_CAcinaref.m_ostrsteam << "待读入文章:" << RemainderFile << "篇！";
//	AppCall::Response_Diplay(t_CAcinaref.m_ostrsteam.str(), 0);

	for(vector<string>::iterator vite = t_CAcinaref.t_Corpus_v.begin(); vite != t_CAcinaref.t_Corpus_v.end(); ){
		//while(app->pdlg->StopEveryThreadFlag){
		//	Sleep(5000);
		//}
		for(unsigned int i = 0; i < t_CAcinaref.DIS_ESSAISTA_MAXTHREAD; i++){
			if(!UsedCodeFlag[i] &&  (vite != t_CAcinaref.t_Corpus_v.end())){
//				t_CAcinaref.m_ostrsteam.str("");
//				t_CAcinaref.m_ostrsteam << "处理文章:" << vite->substr(vite->rfind("\\") + 1).c_str() << "\n"
//						<< "Multi_Working Thread ID: " << i << '\n' << "Remainder Files: " << RemainderFile--;
//				AppCall::Response_Diplay(t_CAcinaref.m_ostrsteam.str(), 0);

				t_CAcinaref.t_CopedFile_s.insert(vite->data());
				t_CAcinaref.m_WordThreadInfo[i].sContent_v.clear();
				t_CAcinaref.m_WordThreadInfo[i].pParam_v.clear();

				t_CAcinaref.m_WordThreadInfo[i].sContent_v = pcopestruct->sContent_v;
				t_CAcinaref.m_WordThreadInfo[i].sContent_v[0] = vite->data();
				t_CAcinaref.m_WordThreadInfo[i].ThreadCode = i;
				t_CAcinaref.m_WordThreadInfo[i].pParam_v = pcopestruct->pParam_v;
				t_CAcinaref.m_WordThreadInfo[i].pThreadEvent = &t_CAcinaref.MultThreadEvents[i];
				t_CAcinaref.m_WordThreadInfo[i].hThread = CreateThread(NULL, 0, *(pcopestruct->pCopeThread), (LPVOID)&(t_CAcinaref.m_WordThreadInfo[i]), 0, &t_CAcinaref.m_WordThreadInfo[i].hThreadId);
				vite++;
				UsedCodeFlag[i] = true;
			}
			else if(vite == t_CAcinaref.t_Corpus_v.end()){
				break;
			}
		}
		DWORD dwEvent = WaitForMultipleObjects(t_CAcinaref.DIS_ESSAISTA_MAXTHREAD, t_CAcinaref.MultThreadEvents, FALSE, INFINITE);
		if(dwEvent == WAIT_TIMEOUT){
//			AppCall::Response_Diplay("数据处理出错！", 0);
			SetEvent(t_CAcinaref.m_DispatchEvent);
			ExitThread(0);
		}
		UsedCodeFlag[dwEvent - WAIT_OBJECT_0] = false;
		if(!ResetEvent(t_CAcinaref.MultThreadEvents[dwEvent - WAIT_OBJECT_0])){
//			AppCall::Response_Diplay("数据处理出错！", 0);
			SetEvent(t_CAcinaref.m_DispatchEvent);
			ExitThread(0);
		}
	}
	//the following code justify whether  all the thread is complete or not;
	while(true){
		bool GoonFlag = false;
		for(unsigned int i = 0; i < t_CAcinaref.DIS_ESSAISTA_MAXTHREAD; i++){
			GoonFlag = (GoonFlag || UsedCodeFlag[i]);
		}
		if(!GoonFlag){
			break;
		}
		DWORD dwEvent = WaitForMultipleObjects(t_CAcinaref.DIS_ESSAISTA_MAXTHREAD, t_CAcinaref.MultThreadEvents, FALSE, INFINITE);
		if(dwEvent == WAIT_TIMEOUT)
			continue;
		if(dwEvent == WAIT_FAILED){
//			AppCall::Response_Diplay("数据处理出错！", 0);
			SetEvent(t_CAcinaref.m_DispatchEvent);
			ExitThread(0);
		}
		UsedCodeFlag[dwEvent - WAIT_OBJECT_0] = false;	
	}
//	AppCall::Response_Diplay("文档处理结束！", 0);
	SetEvent(t_CAcinaref.m_DispatchEvent);
	ExitThread(0);
}





