#pragma once

#include "CAncestry.h"

#define		MAX_DIS_ESSAISTA_MAXTHREAD 36
#define		MAX_SENTENCE_LENGTH 100
#define		MIN_FREQUENT_PATTERN 2


class CAcina;



typedef struct _COPEFIlEINFO
{
	HANDLE hThread;
	DWORD hThreadId;
	unsigned int ThreadCode;
	vector<string> sContent_v;
	vector<LPVOID> pParam_v;
	CAcina* pCAcina;
	HANDLE* pThreadEvent;

}COPEFIlEINFO, *pCOPEFIlEINFO;

//-------------------CAcina-------------------------------------------
class CAcina : public CAncestry
{
public:
	unsigned int Current_Thread_Number;
	unsigned int DIS_ESSAISTA_MAXTHREAD;

	HANDLE MultThreadEvents[MAX_DIS_ESSAISTA_MAXTHREAD];	//for "work" thread mutux;

	COPEFIlEINFO m_WordThreadInfo[MAX_DIS_ESSAISTA_MAXTHREAD];

	MultidichTaskInfo m_MultidichTaskInfo;

	HANDLE m_DispatchEvent;

	ostringstream m_ostrsteam;

	vector<string> t_Corpus_v;
	set<string> t_CopedFile_s;


public:
	CAcina();
	~CAcina();

	bool Parallel_File_Handler(MultidichTaskInfo& pmMultidichTaskInfo);


};
