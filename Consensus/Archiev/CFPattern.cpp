#include "StdAfx.h"
#include "CFPattern.h"


DWORD WINAPI Sentence_Pattern_Recognize_Thread(LPVOID pParam);
DWORD WINAPI Delete_SubSent_FreqPattern_Thread(LPVOID pParam);

DWORD WINAPI CAcina_Parallel_Dispatch_Thread(LPVOID pParam);

CFPattern::CFPattern()
{
	UseCurrentFreqFlag = false;
}
CFPattern::~CFPattern()
{
}
//===========================================================================
void CFPattern::CFPattern_Output_Message(const char* c_msg, UINT nType)
{
	//AppCall::Response_Diplay(c_msg, nType);
}
void CFPattern::Init_Frequent_Pattern()
{
	m_MultidichTaskInfo.pCopeThread = NULL;
	m_MultidichTaskInfo.sContent_v.clear();
	m_MultidichTaskInfo.pParam_v.clear();
	m_MultidichTaskInfo.pCAcina = NULL;
	
	MultiThreadFPModifyFlag = false;
	SortedFreqPattern_mm.clear();;
	CorrespondSubstr_m.clear();
	Current_FP_FolderPath = "";
	CurrentFreqSet.clear();
	UseCurrentFreqFlag = false;
}

string CFPattern::CFPattern_Port(string pmService, vector<string> sParam_v, vector<LPVOID> pParam_v)
{
	Init_Frequent_Pattern();

	string folderpath = sParam_v[0];

	string FP_FolderPath = sParam_v[1];
	
	Find_Pattern_From_Sentence_Sequence(folderpath, FP_FolderPath);

	Current_FP_FolderPath = FP_FolderPath + "SubFPSent_Info\\";

	Load_Row_FreqPattern_For_CommFreq_Delete(FP_FolderPath);

	Delete_SubSent_With_MultiThred_And_AutoSaveMidleResult(SortedFreqPattern_mm, CorrespondSubstr_m);

	return "";
}

//===========================================================================
string CFPattern::Find_Pattern_From_Sentence_Sequence(string pmfolderpath, string FP_FolderPath)
{
	ostringstream ostrsteam;
	ostrsteam << "�Ƿ�ȷʵ��Ҫ���أ�" <<  pmfolderpath << "�µ��������ϣ�...";
	//if(IDYES != AppCall::Secretary_Message_Box(ostrsteam.str(), MB_YESNOCANCEL)){
	//	return "����ȡ��...";
	//}
	//---------------init
	set<string> FrequentPatternSet;
	map<string, unsigned long> Total_FP_Map;
	multimap<unsigned long, string> SortedFreqPattern_mm;
	//string FP_FolderPath = ".\\..\\..\\Result\\FrequentPattern\\";
	_mkdir(FP_FolderPath.c_str());
	string Current_FP_FolderPath;
	unsigned int FrequentPatternLength = 1;
	unsigned int Last_FP_Number = 0;
	vector<map<string, unsigned int>*> pt_ThreadFP_v;

	for(unsigned int i = 0; i < DIS_ESSAISTA_MAXTHREAD; i++){
		pt_ThreadFP_v.push_back(new map<string, unsigned int>);
	}
//-----------------Create Length 0 Folder;
	CFPattern_Output_Message("��ʼ������Ϊ1���Ӵ�...", 0);
	while(true){
		ostrsteam.str("");
		ostrsteam << "��ʼ���س���Ϊ" << FrequentPatternLength - 1 << "���Ӵ�..." << endl;
		CFPattern_Output_Message(ostrsteam.str().c_str(), 0);
//--------------Read Sub Frequence Pattern
		for(multimap<unsigned long, string>::iterator mmite = SortedFreqPattern_mm.begin(); mmite != SortedFreqPattern_mm.end(); mmite++){
			FrequentPatternSet.insert((*mmite).second);
		}
		SortedFreqPattern_mm.clear();
//-------------------Find Frequence Pattern----------
		ostrsteam.str("");
		ostrsteam << "��ʼʶ�𳤶�Ϊ" << FrequentPatternLength << "��Ƶ����..." << endl;
		CFPattern_Output_Message(ostrsteam.str().c_str(), 0);

		m_MultidichTaskInfo.sContent_v.clear();
		m_MultidichTaskInfo.pParam_v.clear();
		m_MultidichTaskInfo.pCopeThread = Sentence_Pattern_Recognize_Thread;
		m_MultidichTaskInfo.sContent_v.push_back(pmfolderpath);
		m_MultidichTaskInfo.pParam_v.push_back((LPVOID)&FrequentPatternSet);
		m_MultidichTaskInfo.pParam_v.push_back((LPVOID)&pt_ThreadFP_v);
		m_MultidichTaskInfo.pParam_v.push_back((LPVOID)FrequentPatternLength);
		
		//HANDLE DispThread;
		//DWORD DisphreadId;
		//DispThread = CreateThread(NULL, 0, CAcina_Parallel_Dispatch_Thread, (LPVOID)&m_MultidichTaskInfo, 0, &DisphreadId);
		Parallel_File_Handler(m_MultidichTaskInfo);

		CFPattern_Output_Message("Find_Pattern_From_Sentence_Sequence", 0);
//------------------------------------------
		ostrsteam.str("");
		ostrsteam << "����Ϊ" << FrequentPatternLength << "��Ƶ����ʶ����ϣ��ͷ��ڴ�..." << endl;
		CFPattern_Output_Message(ostrsteam.str().c_str(), 0);

		FrequentPatternSet.clear();
		CFPattern_Output_Message("�ڴ��ͷŽ�����ɾ����Ƶ����...", 0);

		for(vector<map<string, unsigned int>*>::iterator vite = pt_ThreadFP_v.begin(); vite != pt_ThreadFP_v.end(); vite++){
			for(map<string, unsigned int>::iterator mite = (*vite)->begin(); mite != (*vite)->end(); ){
				if(Total_FP_Map.find((*mite).first) == Total_FP_Map.end()){
					Total_FP_Map[(*mite).first];
					Total_FP_Map[(*mite).first] = (*mite).second;
				}
				else{
					Total_FP_Map[(*mite).first] += (*mite).second;
				}
				mite = (*vite)->erase(mite);
			}
		}
		for(map<string, unsigned long>::iterator mite = Total_FP_Map.begin(); mite != Total_FP_Map.end(); ){
			if((*mite).second < MIN_FREQUENT_PATTERN)
				mite = Total_FP_Map.erase(mite);
			else
				mite++;
		}
		if(Total_FP_Map.empty()){// || (Last_FP_Number == SortedFreqPattern_mm.size()))
			CFPattern_Output_Message("���޸���ƥ���ַ���...", 0);
			break;
		}
		Last_FP_Number = Total_FP_Map.size();
//------------Save .txt file for init FrequentPatternSet(sub string.)	
		CFPattern_Output_Message("׼������.txt�ļ�...", 0);
		ostrsteam.str("");
		ostrsteam << FrequentPatternLength << "\\";
		Current_FP_FolderPath = FP_FolderPath + ostrsteam.str();
		FrequentPatternLength++;
		_mkdir(Current_FP_FolderPath.c_str());

		Save_txt_map_string_long_with_Freq(Total_FP_Map, Current_FP_FolderPath, "");

//------------------Save Frequence Pattern----------
		CFPattern_Output_Message("��ʽת����׼������.dat�ļ�...", 0);
		for(map<string, unsigned long>::iterator mite = Total_FP_Map.begin();  mite != Total_FP_Map.end(); ){
			SortedFreqPattern_mm.insert(make_pair((*mite).second, (*mite).first));
			mite = Total_FP_Map.erase(mite);
		}
//----Save Files Info-------------------------------------------------------------------
		CFPattern_Output_Message("��ʼ�洢Info.dat�ļ�...\n", 0);
		Save_Info_multimap_long_string(SortedFreqPattern_mm, Current_FP_FolderPath + "Info.dat", "Ƶ��,����,����");
//----Save .dat file----------------------------------------------------------
		Save_Sorted_dat_multimap_string_long(SortedFreqPattern_mm, Current_FP_FolderPath, "");

//-----------------------------------------------------------------------------------
		ostrsteam.str("");
		ostrsteam << "����Ϊ" << FrequentPatternLength - 1 << "��Ƶ���Ӵ�������ϣ��ͷ��ڴ棬׼������..." << endl;
		CFPattern_Output_Message(ostrsteam.str().c_str(), 0);

		if(!(FrequentPatternLength < MAX_SENTENCE_LENGTH/2)){
			CFPattern_Output_Message("�ִ����ȴﵽ���ޣ��������ݴ������...", 0);
			break;
		}
	}
	SortedFreqPattern_mm.clear();

	for(vector<map<string, unsigned int>*>::iterator vite = pt_ThreadFP_v.begin(); vite != pt_ThreadFP_v.end(); vite++)
	{
		(*vite)->clear();
		delete *vite;
	}
	return "OK!";
}
DWORD WINAPI Sentence_Pattern_Recognize_Thread(LPVOID pParam)
{
	pCOPEFIlEINFO pCopeinfo = (pCOPEFIlEINFO)pParam;
	CFPattern* pCFPattern = (CFPattern*)pCopeinfo->pCAcina;
	set<string>& FrequentPatternSet = *((set<string>*)pCopeinfo->pParam_v[0]);
	map<string, unsigned int>& tMapRet = *((*((vector<map<string, unsigned int>*>*)pCopeinfo->pParam_v[1]))[pCopeinfo->ThreadCode]);
	char SentenceBuf[MAX_SENTENCE];
	char SubBuf[MAX_SENTENCE];
	unsigned int FrequentPatternLength = (unsigned int)(pCopeinfo->pParam_v[2]);
	unsigned int SubLength = FrequentPatternLength - 1;
	unsigned int CharLength;
	char sChar[3];

	ifstream in(pCopeinfo->sContent_v[0].c_str());
	if(in.bad())
		return false;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF)
	{
		in.close();
		return -1;
	}
//---------Read sentence
	while(true)
	{
		if(in.peek() == EOF)
		{	
			break;
		}
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
//-------------Get information
		CharLength = strlen(SentenceBuf);
		sChar[2]=0;
		for(unsigned long BufPosit = 0; BufPosit < CharLength; )
		{
			bool FindSubStr = false;
			unsigned int SubFPLength = 0;
			strcpy_s(SubBuf, "");
			for(unsigned int SubstrLength = BufPosit; ((SubFPLength < FrequentPatternLength) && (SubstrLength < CharLength)); )
			{
				sChar[0]=SentenceBuf[SubstrLength++];
				sChar[1]=0;	
				if(sChar[0] < 0 )
				{
					sChar[1]=SentenceBuf[SubstrLength++];
				}
				if(SubFPLength == SubLength){
					if(FrequentPatternSet.find(SubBuf) != FrequentPatternSet.end())
						FindSubStr = true;
				}
				SubFPLength++;
				strcat_s(SubBuf, MAX_SENTENCE_LENGTH, sChar);
			}
			if(FindSubStr || FrequentPatternSet.empty()){
				if(tMapRet.find(SubBuf) != tMapRet.end()){
					tMapRet[SubBuf]++;
				}
				else{
					try{
						tMapRet.insert(make_pair(SubBuf, 1));
					}
					catch(CMemoryException *e){
						e->Delete();
					}
				}
			}
			if(SentenceBuf[BufPosit] < 0)
				BufPosit++;
			BufPosit++;
		}
		//in.getline(SentenceBuf, MAX_SENTENCE_LENGTH,'\n');
		//-----------------------------------------------
	} 
	in.close();
	SetEvent(*pCopeinfo->pThreadEvent);
	ExitThread(0);
}


void CFPattern::Delete_SubSent_With_MultiThred_And_AutoSaveMidleResult(multimap<unsigned long, string>& pmFreqPattern, map<string, string>& pmCorrespondSubstr)
{
	//-------Computer run time....
	CTime StartTime = CTime::GetCurrentTime();

	if(pmFreqPattern.empty()){
		CFPattern_Output_Message("û�����ݣ������������", 0);
		return;
	}
	bool UsedCodeFlag[MAX_DIS_ESSAISTA_MAXTHREAD];
	ostringstream ostrsteam;
	string sOutstr;
	vector<map<string, string>*> t_SubStrm_v;
	pair<multimap<unsigned long, string>::iterator, multimap<unsigned long, string>::iterator> m_Multimap_Pair;
	multimap<unsigned long, string>::iterator Current_iter;
	multimap<unsigned long, string>::iterator Compare_iter;

	for (unsigned int i = 0; i < DIS_ESSAISTA_MAXTHREAD; i++) 
    {
		t_SubStrm_v.push_back(new map<string, string>);
		UsedCodeFlag[i] = false;
        if(!ResetEvent(MultThreadEvents[i])) {
			CFPattern_Output_Message("���ݳ�ʼ��������������������", 0);
            return; 
		}
    }
	CFPattern_Output_Message("�������Ƶ����������", 0);
	set<unsigned long> FreqypeSet;
	for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); mmite++)
	{
		FreqypeSet.insert((*mmite).first);
	}
	//---Ƶ�ʵ������ҵ���ǰƵ��
	set<unsigned long>::iterator freqsite = FreqypeSet.begin();
	if(UseCurrentFreqFlag)
	{
		while(freqsite != FreqypeSet.end())
		{
			if(*freqsite == CurrentFreq)
				break;
			freqsite++;
		}
	}
	for(; freqsite != FreqypeSet.end(); freqsite++)
	{
		ostrsteam.str("");
		ostrsteam << "����Ƶ��Ϊ��" << *freqsite << "��Ƶ����...";
		sOutstr = ostrsteam.str();
		CFPattern_Output_Message(sOutstr.c_str(), 0);
		unsigned long sizeinidenticalfreq = pmFreqPattern.count(*freqsite);

		//------------------------------------------------------------------------------------------
		//The switch for multithread or single thread;
		if(true)//UseCurrentFreqFlag || (sizeinidenticalfreq > EVERY_FILE_NUMBER*5))
		{
			CFPattern_Output_Message("ʹ�ö��߳�ģʽ...", 0);

			CFPattern_Output_Message("ȡ����ǰƵ����...", 0);

			set<string> TempFreqPattern_s;
			if(!UseCurrentFreqFlag)
			{
				for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); )
				{
					if((*mmite).first == *freqsite)
					{
						TempFreqPattern_s.insert((*mmite).second);
						mmite = pmFreqPattern.erase(mmite);
					}
					else
					{
						mmite++;
					}
				}
			}
			else
			{
				for(set<string>::iterator site = CurrentFreqSet.begin(); site != CurrentFreqSet.end(); )
				{
					TempFreqPattern_s.insert(*site);
					site = CurrentFreqSet.erase(site);
				}
			}
			//--���㵱ǰƵ����ĳ��ȣ�LengthFreqPattern_mm
			CFPattern_Output_Message("ͳ�Ƶ�ǰƵ���������ַ�������...", 0);
			multimap<unsigned long, string> LengthFreqPattern_mm;
			set<unsigned int> LenthTypSet;
			for(set<string>::iterator site = TempFreqPattern_s.begin(); site != TempFreqPattern_s.end(); site++)
			{
				LengthFreqPattern_mm.insert(make_pair((*site).length(), *site));
				LenthTypSet.insert((*site).length());
				//site = TempFreqPattern_s.erase(site);
			}
			//--���ȵ������ҵ���ǰ������
			set<unsigned int>::iterator lengthsite = LenthTypSet.begin();
			if(UseCurrentFreqFlag)
			{
				while(lengthsite != LenthTypSet.end())
				{
					if(*lengthsite > CurrentLength)
						break;
					for(multimap<unsigned long, string>::iterator mmite = LengthFreqPattern_mm.begin(); mmite != LengthFreqPattern_mm.end(); )
					{
						if((*mmite).first <= *lengthsite)
						{
							pmFreqPattern.insert(make_pair(*freqsite, (*mmite).second));
							mmite = LengthFreqPattern_mm.erase(mmite);
						}
						else
						{
							mmite++;
						}
					}
					lengthsite++;
				}
				//--�м����������
				UseCurrentFreqFlag = false;
				//�ж��ϴ��Ƿ������굱ǰƵ����󳤶�
				//if(++lengthsite == LenthTypSet.end())
				//	continue;
			}
			for(; lengthsite != LenthTypSet.end(); lengthsite++)
			{
				//---��ǰƵ�����������SeqSubPattern_m
				map<unsigned long, string> SeqSubPattern_m;
				unsigned long SequenceNumber = 0;
				for(multimap<unsigned long, string>::iterator mmite = LengthFreqPattern_mm.begin(); mmite != LengthFreqPattern_mm.end(); )
				{
					if((*mmite).first == *lengthsite)
					{
						SeqSubPattern_m.insert(make_pair(SequenceNumber++, (*mmite).second));
						//if(TempFreqPattern_s.find((*mmite).second) != TempFreqPattern_s.end())
						TempFreqPattern_s.erase((*mmite).second);
						mmite = LengthFreqPattern_mm.erase(mmite);
					}
					else
					{
						//TempFreqPattern_s.insert((*mmite).second);
						mmite++;
					}
				}
//---------------Decide how much kernal shull be use	
				Current_Thread_Number = DIS_ESSAISTA_MAXTHREAD;
					
				unsigned int temp_DIS_ESSAISTA_MAXTHREAD = DIS_ESSAISTA_MAXTHREAD;
				if(SeqSubPattern_m.size() < Current_Thread_Number)
				{
					Current_Thread_Number = SeqSubPattern_m.size();
				}
//---------------------------------------------------------------------------------------------------
				LPSYSTEMTIME lpSystemTime = new  SYSTEMTIME;
				GetLocalTime(lpSystemTime);
				ostrsteam << "ʱ�䣺" <<lpSystemTime->wYear << '_' << lpSystemTime->wMonth << '_' << lpSystemTime->wDay << ' ' << lpSystemTime->wHour << ':' << lpSystemTime->wMinute << ':' << lpSystemTime->wSecond << "\n";
				ostrsteam << "��ǰʹ��" << Current_Thread_Number<< "�������߳�..." <<  endl;
				sOutstr = ostrsteam.str();
				CFPattern_Output_Message(sOutstr.c_str(), 0);
				//-----------------------------------------------------------------------
				ostrsteam.str("");
				ostrsteam << "��ǰƵ��" << *freqsite << ",����" << *lengthsite <<  endl;
				ostrsteam << "ͬ���ȼ���" << SeqSubPattern_m.size() << "��" << endl;
				ostrsteam << "ͬƵ�ʼ���" << TempFreqPattern_s.size() << "��" << endl; 
				sOutstr = ostrsteam.str();
				CFPattern_Output_Message(sOutstr.c_str(), 0);
				
				delete lpSystemTime;
//---------------------------------------------------------------------------------------------------		
				for(unsigned int i = 0; i < Current_Thread_Number; i++)
				{
					if(!UsedCodeFlag[i])
					{
						m_WordThreadInfo[i].sContent_v.clear();
						m_WordThreadInfo[i].pParam_v.clear();

						m_WordThreadInfo[i].pCAcina = (CAcina*)this;
						m_WordThreadInfo[i].ThreadCode = i;
						m_WordThreadInfo[i].pParam_v.push_back((LPVOID)&SeqSubPattern_m);
						m_WordThreadInfo[i].pParam_v.push_back((LPVOID)&TempFreqPattern_s);
						m_WordThreadInfo[i].pParam_v.push_back((LPVOID)&t_SubStrm_v);
						m_WordThreadInfo[i].pParam_v.push_back((LPVOID)DIS_ESSAISTA_MAXTHREAD);
						m_WordThreadInfo[i].pThreadEvent = &MultThreadEvents[i];
						m_WordThreadInfo[i].hThread = CreateThread(NULL, 0, Delete_SubSent_FreqPattern_Thread, (LPVOID)&(m_WordThreadInfo[i]), 0, &m_WordThreadInfo[i].hThreadId);
							
						UsedCodeFlag[i] = true;
					}
				}

//---------------------------------------------------------------------------------------------------------
				DWORD dwEvent = WaitForMultipleObjects(Current_Thread_Number, MultThreadEvents, TRUE, INFINITE);

				for(unsigned int i = 0; i < Current_Thread_Number; i++)
				{
					UsedCodeFlag[i] = false;
				}

				Current_Thread_Number = temp_DIS_ESSAISTA_MAXTHREAD;

				for(map<unsigned long, string>::iterator mmite = SeqSubPattern_m.begin(); mmite != SeqSubPattern_m.end(); )
				{
					if(0 != (*mmite).second.length())
					{
						pmFreqPattern.insert(make_pair(*freqsite, (*mmite).second));
						
					}
					mmite = SeqSubPattern_m.erase(mmite);
				}
				for(unsigned int i = 0; i < DIS_ESSAISTA_MAXTHREAD; i++)
				{
					for(map<string, string>::iterator mite = t_SubStrm_v[i]->begin(); mite != t_SubStrm_v[i]->end(); )
					{
						CorrespondSubstr_m.insert(make_pair((*mite).first, (*mite).second));
						mite = t_SubStrm_v[i]->erase(mite);
					}
				}
				//----Decide wether should save middleresult--------------------
				CTime EndTime = CTime::GetCurrentTime();
				CTimeSpan ts = EndTime - StartTime;
				bool SouldSaveMiddleReSultFlag = false;
				if((ts.GetDays() > 1) || (ts.GetHours() > 2))
				{
					SouldSaveMiddleReSultFlag = true;
				}
				if(SouldSaveMiddleReSultFlag)//!(CorrespondSubstr_m.size() < EVERY_FILE_NUMBER))
				{ 
					ostrsteam.str("");
					ostrsteam << "�����м���...";
					ostrsteam << "���ϴα��棺" << ts.GetDays() << "��" << ts.GetHours() << "ʱ" << ts.GetMinutes() <<  "��" << ts.GetSeconds() << "��...";
					ostrsteam << "Ƶ�ʣ�" << *freqsite << "��" << "�Ѵ����ȣ�" << *lengthsite << "..." << endl;
					ostrsteam << "Ƶ����ϼƣ�" << pmFreqPattern.size() + TempFreqPattern_s.size() << "��...";
					sOutstr = ostrsteam.str();
					CFPattern_Output_Message(sOutstr.c_str(), 0);

					Save_Middle_FreqPattern_Result_In_Delete_SubSent(pmFreqPattern, TempFreqPattern_s, CorrespondSubstr_m, *freqsite, *lengthsite);
					
					StartTime = CTime::GetCurrentTime();
				}


			
			}//end for(set<unsigned int>::iterator site = LenthTypSet.begin(); site != LenthTypSet.end(); site++)
		}// end if 
		else
		{

			m_Multimap_Pair = pmFreqPattern.equal_range(*freqsite);
		
			if(m_Multimap_Pair.first != m_Multimap_Pair.second)
			{
				Current_iter = m_Multimap_Pair.first;
				for(; Current_iter != m_Multimap_Pair.second; Current_iter++)
				{
					Compare_iter = Current_iter;
					Compare_iter++;
					for(; Compare_iter != m_Multimap_Pair.second; Compare_iter++)
					{
						if((*Current_iter).second.length() == (*Compare_iter).second.length())
							continue;
						if((*Current_iter).second.length() > (*Compare_iter).second.length())
						{
							if(NLPOP::Find_Substr_In_Chinese_Sent((*Current_iter).second.c_str(), (*Compare_iter).second.c_str()))
							{
								pmCorrespondSubstr.insert(make_pair((*Compare_iter).second, (*Current_iter).second));
								(*Compare_iter).second = "";
							}
						}
						else
						{ 
							if(NLPOP::Find_Substr_In_Chinese_Sent((*Compare_iter).second.c_str(), (*Current_iter).second.c_str()))
							{
								pmCorrespondSubstr.insert(make_pair((*Current_iter).second, (*Compare_iter).second));
								(*Current_iter).second = "";
								Current_iter++;
								Compare_iter = Current_iter;
							}
						}
					}
				}
			}
			if(false)//!(CorrespondSubstr_m.size() < EVERY_FILE_NUMBER))
			{
				ostrsteam.str("");
				ostrsteam << "�����м���..." << endl;
				ostrsteam << "Ƶ�ʣ�" << *freqsite << "��" << "�Ѵ����ȣ�" << 0 << "..." << endl;
				ostrsteam << "Ƶ����ϼƣ�" << pmFreqPattern.size()  << "��...";
				sOutstr = ostrsteam.str();
				CFPattern_Output_Message(sOutstr.c_str(), 0);

				for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); )
				{
					if(0 == (*mmite).second.length())
					{
						mmite = pmFreqPattern.erase(mmite);
					}
					else
					{
						mmite++;
					}
				}

				set<string> nullset;
				Save_Middle_FreqPattern_Result_In_Delete_SubSent(pmFreqPattern, nullset, CorrespondSubstr_m, *freqsite + 1, 0);
			}
		}//end if...else... sizeinidenticalfreq > EVERY_FILE_NUMBER
	}

	//---ɾ�����ַ���
	CFPattern_Output_Message("����ͬƵ���������ϣ���ʼ��������...", 0);
	
	for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); )
	{
		if(0 == (*mmite).second.length())
		{
			mmite = pmFreqPattern.erase(mmite);
		}
		else
		{
			mmite++;
		}
	}
	if(!CorrespondSubstr_m.empty())
	{
		multimap<unsigned long, string> nullmmap;
		set<string> nullset;
		Save_Middle_FreqPattern_Result_In_Delete_SubSent(nullmmap, nullset, CorrespondSubstr_m, 0, 0);
	}

	for (unsigned int i = 0; i < DIS_ESSAISTA_MAXTHREAD; i++) 
	{ 
		delete t_SubStrm_v[i];
	}
/*
	size_t erase_freq = pmFreqPattern.size();
	erase_freq = size_t(erase_freq * 0.9);
	for(multimap<unsigned long, string>::iterator  mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); ){
		if(erase_freq-- != 0){
			mmite++;
		}
		else{
			mmite = pmFreqPattern.erase(mmite);
		}
	}
	*/
	//for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); ){
	//	if(mmite->first < 10){
	//		mmite = pmFreqPattern.erase(mmite);
	//	}
	//	else
	//		break;
	//}
	/*
	set<string> t_WordsSet;
	NLPOP::LoadWordsStringSet("F:\\YPench\\Secretary\\Data\\CVMC.dat", t_WordsSet);
	for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); ){
		if(t_WordsSet.find(mmite->second) != t_WordsSet.end()){
			mmite = pmFreqPattern.erase(mmite);
		}
		else
			mmite++;
	}
	t_WordsSet.clear();
	for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); mmite++){
		if(mmite->second.find(';') == -1)
			t_WordsSet.insert(mmite->second);
	}
	NLPOP::Save_Set_String_With_Comma_Divide("F:\\YPench\\Result\\freqstrset.txt", t_WordsSet);
	*/

	Save_FreqPattern_Result(Current_FP_FolderPath, pmFreqPattern);
	
	CFPattern_Output_Message("�����������...", 0);

//-------Display the ruined time;
/*
	CTime EndTime = CTime::GetCurrentTime();
	CTimeSpan ts = EndTime - StartTime;
	ostrsteam.str("");;
	ostrsteam << "��ʱ��" << ts.GetDays() << "��" << ts.GetHours() << "ʱ" << ts.GetMinutes() <<  "��" << ts.GetSeconds() << "��...";
	sOutstr = ostrsteam.str();
	AppCall::Secretary_Hide();
	AfxMessageBox(NLPOP::string2CString(sOutstr), MB_YESNOCANCEL);
	AppCall::Secretary_Show();
*/
}
DWORD WINAPI Delete_SubSent_FreqPattern_Thread(LPVOID pParam)
{
//	CSecretaryApp *app = (CSecretaryApp *)AfxGetApp();
	pCOPEFIlEINFO pCopeinfo = (pCOPEFIlEINFO)pParam;
	CFPattern* pCFPattern = (CFPattern*)pCopeinfo->pCAcina;
	unsigned int t_ThreadCode = pCopeinfo->ThreadCode;
	map<unsigned long, string>& t_SeqSubPattern_m = *((map<unsigned long, string>*)pCopeinfo->pParam_v[0]);
	set<string>& t_TempFreqPattern_s = *((set<string>*)pCopeinfo->pParam_v[1]);
	map<string, string>& t_SubStr_m = *((*((vector<map<string, string>*>*)pCopeinfo->pParam_v[2]))[t_ThreadCode]);
	unsigned int t_DIS_ESSAISTA_MAXTHREAD = (unsigned int)pCopeinfo->pParam_v[3];
	unsigned long TotalMapSize = t_SeqSubPattern_m.size();
	const char* SubSentBuf;

	for(unsigned long i = t_ThreadCode; i < TotalMapSize; i += t_DIS_ESSAISTA_MAXTHREAD){
		SubSentBuf = t_SeqSubPattern_m[i].c_str();
		for(set<string>::iterator site = t_TempFreqPattern_s.begin();  site != t_TempFreqPattern_s.end(); site++){
			if(NLPOP::Find_Substr_In_Chinese_Sent((*site).c_str(), SubSentBuf))
			{
				t_SubStr_m.insert(make_pair(t_SeqSubPattern_m[i], *site));
				t_SeqSubPattern_m[i] = "";
				break;
			}
		}
	}
	SetEvent(*pCopeinfo->pThreadEvent);
	ExitThread(0);
}




//===========================================================================
void CFPattern::Delete_SubSent_In_Identical_FreqPattern(multimap<unsigned long, string>& pmFreqPattern, map<string, string>& pmCorrespondSubstr)
{
	ostringstream ostrsteam;
	string sOutstr;
	pair<multimap<unsigned long, string>::iterator, multimap<unsigned long, string>::iterator> m_Multimap_Pair;
	multimap<unsigned long, string>::iterator Current_iter;
	multimap<unsigned long, string>::iterator Compare_iter;
	map<string, string> CorrespondSubstr_m;
	set<unsigned long> FreqypeSet;
	unsigned long itercnt;
	itercnt = pmFreqPattern.size();
	for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); mmite++)
	{
		FreqypeSet.insert((*mmite).first);
	}


	for(set<unsigned long>::iterator site = FreqypeSet.begin(); site != FreqypeSet.end(); site++)
	{
		m_Multimap_Pair = pmFreqPattern.equal_range(*site);
		
		if(m_Multimap_Pair.first != m_Multimap_Pair.second)
		{
			Current_iter = m_Multimap_Pair.first;
			for(; Current_iter != m_Multimap_Pair.second; Current_iter++)
			{
				itercnt--;
				if(0 == (*Current_iter).second.length())
						continue;
				if(itercnt%EVERY_FILE_NUMBER == 0)
				{
					ostrsteam.str("");
					ostrsteam << "���ڴ���Ƶ��Ϊ" << *site << "��Ƶ����..." << "\n";
					ostrsteam << "�ҵ�������ͬ��Ƶ�����" << pmCorrespondSubstr.size() << "��." << "\n";
					ostrsteam << "Լʣ�ࣺ" << itercnt << "��.";
					CFPattern_Output_Message(ostrsteam.str().c_str(), 0);
				}

				Compare_iter = Current_iter;
				Compare_iter++;
				for(; Compare_iter != m_Multimap_Pair.second; Compare_iter++)
				{
					if(0 == (*Compare_iter).second.length())
						continue;
					if((*Current_iter).second.length() >= (*Compare_iter).second.length())
					{
						if(string::npos != (*Current_iter).second.find((*Compare_iter).second))
						{
							pmCorrespondSubstr.insert(make_pair((*Compare_iter).second, (*Current_iter).second));
							(*Compare_iter).second = "";
						}
					}
					else
					{ 
						if(string::npos != (*Compare_iter).second.find((*Current_iter).second))
						{
							pmCorrespondSubstr.insert(make_pair((*Current_iter).second, (*Compare_iter).second));
							(*Current_iter).second = "";
							Current_iter++;
						}
					}
				}
			}
		}
	}
	for(multimap<unsigned long, string>::iterator mite = pmFreqPattern.begin();  mite != pmFreqPattern.end(); )
	{
		if(0 == (*mite).second.length())
			mite = pmFreqPattern.erase(mite);
		else
			mite++;
	}

}
string CFPattern::Count_FreqPattern_Info_In_Raw_Folder(string pmPath) 
{
	char currentfile[1024];
	char SentenceBuf[MAX_SENTENCE_LENGTH];
	unsigned int FP_Length;
	vector<string> t_Corpus_v;
	map<unsigned int, map<unsigned int, unsigned int>> m_Info_m;
//begin------Add all .dat file in folders---------------------
	NLPOP::Get_Specified_Files(pmPath, t_Corpus_v, ".dat");
	if(t_Corpus_v.size() == 0){
		CFPattern_Output_Message("�ļ�����û�д������ĵ���", 0);
	}
//--------Total Frequent Pattern Info--------------------------------------------------------------------------------------------
	map<unsigned long, unsigned long> Total_Freq_m;
	unsigned long TotalFPCnt = 0;
	for(vector<string>::iterator vite = t_Corpus_v.begin(); vite != t_Corpus_v.end(); vite++)
	{
		strcpy_s(currentfile, 1024, ((*vite).substr((*vite).rfind("\\") + 1)).c_str());
		if(NULL  != strcmp("Info.dat", currentfile))
			continue;

		ifstream in((*vite).c_str());
		if(in.bad())
			return false;
		in.clear();
		in.seekg(0, ios::beg);
		if(in.peek() == EOF)
		{
			in.close();
			return "OK";
		}
		for(unsigned int i = 0; i < 7; i++)
		{
			in.getline(SentenceBuf, MAX_SENTENCE_LENGTH,'\n');
		}
	//---------Read sentence
		unsigned int FP_Freq;
		unsigned int FP_SentCnt;
		while(true)
		{
			if(in.peek() == EOF)
			{	
				break;
			}
			in >> FP_Freq;
			in.getline(SentenceBuf, MAX_SENTENCE_LENGTH,',');
			in >> FP_SentCnt;
			in.getline(SentenceBuf, MAX_SENTENCE_LENGTH,'\n');
			
			if(Total_Freq_m.find(FP_Freq) != Total_Freq_m.end())
			{
				Total_Freq_m[FP_Freq] += FP_SentCnt;
			}
			else
			{
				Total_Freq_m.insert(make_pair(FP_Freq, FP_SentCnt));
			}
			TotalFPCnt += FP_SentCnt;
		} 
		in.close();
	}

	LPSYSTEMTIME lpSystemTime = new  SYSTEMTIME;
	string sOutFileName = pmPath + "Total_Freq_Info.dat";
	ofstream out(sOutFileName.c_str());
	if(out.bad())
		return "bad";
	out.clear();
	out.seekp(0, ios::beg);
	ostringstream ostrsteam;
	ostrsteam.str("");

	out << "#";//-------------------date;
	GetLocalTime(lpSystemTime);
	ostrsteam << lpSystemTime->wYear << '_' << lpSystemTime->wMonth << '_' << lpSystemTime->wDay << ' ' << lpSystemTime->wHour << ':' << lpSystemTime->wMinute << ':' << lpSystemTime->wSecond << "\n";
	string datastr = "���ڣ�";
	datastr += ostrsteam.str();
	out << datastr;
	out << "#";//-------------------author
	out << "Author:" << "YPench" << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#";//-------------------
	out << "Ƶ��ģʽ������" << TotalFPCnt << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#csv��ʽ";
	out << "#";//-------------------
	out << "\n";
	out << "����Ƶ��" << "," << "��Ƶ���¾�������" << "\n";
	for(map<unsigned long, unsigned long>::reverse_iterator  rmite = Total_Freq_m.rbegin(); rmite != Total_Freq_m.rend(); rmite++)
	{
		out << (*rmite).first << "," << (*rmite).second << "\n";
	}

	out.close();

	Total_Freq_m.clear();

//--------Length_Order_Info.dat-------------------------------------------------------------------------
	for(vector<string>::iterator vite = t_Corpus_v.begin(); vite != t_Corpus_v.end(); vite++)
	{
		strcpy_s(currentfile, 1024, ((*vite).substr((*vite).rfind("\\") + 1)).c_str());
		if(NULL  != strcmp("Info.dat", currentfile))
			continue;
		string tempstr = (*vite).substr(0, (*vite).rfind("\\"));
		strcpy_s(currentfile, 1024, (tempstr.substr(tempstr.rfind("\\") + 1)).c_str());

		stringstream str2unmber;
		str2unmber << currentfile;
		str2unmber >> FP_Length;
		m_Info_m[FP_Length];

		ifstream in((*vite).c_str());
		if(in.bad())
			return false;
		in.clear();
		in.seekg(0, ios::beg);
		if(in.peek() == EOF)
		{
			in.close();
			return "OK";
		}
		for(unsigned int i = 0; i < 7; i++)
		{
			in.getline(SentenceBuf, MAX_SENTENCE_LENGTH,'\n');
		}
	//---------Read sentence
		unsigned int FP_Freq;
		unsigned int FP_SentCnt;
		while(true)
		{
			if(in.peek() == EOF)
			{	
				break;
			}
			in >> FP_Freq;
			in.getline(SentenceBuf, MAX_SENTENCE_LENGTH,',');
			in >> FP_SentCnt;
			in.getline(SentenceBuf, MAX_SENTENCE_LENGTH,'\n');
			m_Info_m[FP_Length].insert(make_pair(FP_Freq, FP_SentCnt));
	//-----------------------------------------------
		} 
		in.close();
	}
	TotalFPCnt = 0;
	for(map<unsigned int, map<unsigned int, unsigned int>>::iterator mmite = m_Info_m.begin(); mmite != m_Info_m.end(); mmite++)
	{
		for(map<unsigned int, unsigned int>::iterator mite = (*mmite).second.begin(); mite != (*mmite).second.end(); mite++)
			TotalFPCnt += (*mite).second;
	
	}

	sOutFileName = pmPath + "Length_Order_Info.dat";
	out.open(sOutFileName.c_str());
	if(out.bad())
		return "bad";
	out.clear();
	out.seekp(0, ios::beg);
	ostrsteam.str("");

	out << "#";//-------------------date;
	GetLocalTime(lpSystemTime);
	ostrsteam << lpSystemTime->wYear << '_' << lpSystemTime->wMonth << '_' << lpSystemTime->wDay << ' ' << lpSystemTime->wHour << ':' << lpSystemTime->wMinute << ':' << lpSystemTime->wSecond << "\n";
	datastr = "���ڣ�";
	datastr += ostrsteam.str();
	out << datastr;
	out << "#";//-------------------author
	out << "Author:" << "YPench" << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#";//-------------------
	out << "Ƶ��ģʽ������" << TotalFPCnt << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#csv��ʽ";
	out << "#";//-------------------
	out << "\n";
	out << "�ַ�������" << "," << "����Ƶ��" << "��Ƶ���¾�������" << "\n";
	for(map<unsigned int, map<unsigned int, unsigned int>>::iterator mmite = m_Info_m.begin(); mmite != m_Info_m.end(); mmite++)
	{
		for(map<unsigned int, unsigned int>::iterator mite = (*mmite).second.begin(); mite != (*mmite).second.end(); mite++)
			out << (*mmite).first << ',' << (*mite).first << ',' << (*mite).second << '\n';
	}
	out.close();
	delete lpSystemTime;
//end------Add all .dat file in folders---------------------================
	return "OK!";
}







//===============================================================================================================
void  CFPattern::Load_FreqPattern(string pmFileName, multimap<unsigned long, string>& pmFreqPattern_mm)
{
	char SentenceBuf[MAX_SENTENCE];
	unsigned int FreqCnt;
	if(!NLPOP::Exist_of_This_File(pmFileName)){
		return;
	}
	ifstream in(pmFileName.c_str());
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
		in >> FreqCnt;		
		pmFreqPattern_mm.insert(make_pair(FreqCnt, SentenceBuf));
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
	}
	in.close();
}
void  CFPattern::Load_FreqPattern(string pmFileName, map<string, unsigned long>& pmFreqPattern_mm)
{
	char SentenceBuf[MAX_SENTENCE];
	unsigned int FreqCnt;
	if(!NLPOP::Exist_of_This_File(pmFileName)){
		return;
	}
	ifstream in(pmFileName.c_str());
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
		in >> FreqCnt;		
		pmFreqPattern_mm.insert(make_pair(SentenceBuf, FreqCnt));
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
	}
	in.close();
}

bool CFPattern::Load_Row_FreqPattern_For_CommFreq_Delete(string pmPath)
{
	char charresponse[1024];
	char SentenceBuf[MAX_SENTENCE];
	unsigned int FreqCnt;
	vector<string> t_Corpus_v;
	
	ostringstream ostrsteam;
	string sOutstr;
	Current_FP_FolderPath = pmPath + "SubFPSent_Info\\";
	_mkdir(Current_FP_FolderPath.c_str());

	CString MidleResult = NLPOP::string2CString(Current_FP_FolderPath);
	MidleResult += _T("MidlFPResult\\");
	MidleResult += _T("FreqPattern.dat");
	CFileFind finder;
	BOOL bFind = finder.FindFile(MidleResult);
	finder.Close();
	if(bFind)
	{
		sOutstr = "�Ƿ���Ҫ�����ж������в����ģ�";
		sOutstr += "MidlFPResult.dat...";
		AppCall::Secretary_Hide();
		if(IDYES == AppCall::Secretary_Message_Box(sOutstr, MB_YESNOCANCEL))
		{
			AppCall::Secretary_Show();
			ifstream in(NLPOP::CString2string(MidleResult).c_str());
			if(in.bad())
				return false;
			in.clear();
			in.seekg(0, ios::beg);
			for(int i = 0; i < 7; i++)
			{
				in.getline(SentenceBuf, MAX_SENTENCE,'\n');
			}
			in.getline(SentenceBuf, MAX_SENTENCE,':');
			in >> CurrentFreq;
			in.getline(SentenceBuf, MAX_SENTENCE,'\n');

			in.getline(SentenceBuf, MAX_SENTENCE,':');
			in >> CurrentLength;
			in.getline(SentenceBuf, MAX_SENTENCE,'\n');

			if(in.peek() == EOF)
			{
				in.close();
				return false;
			}
			while(true)
			{
				if(in.peek() == EOF)
				{	
					break;
				}
				in.getline(SentenceBuf, MAX_SENTENCE,',');
				in >> FreqCnt;
				
				SortedFreqPattern_mm.insert(make_pair(FreqCnt, SentenceBuf));
			
				in.getline(SentenceBuf, MAX_SENTENCE,'\n');
			}
			in.close();
		//------CurrentFreqSet
			string curretset = Current_FP_FolderPath;
			curretset += "MidlFPResult\\";
			curretset += "TempFreqSet.dat";
			in.open(curretset.c_str());
			if(in.bad())
				return false;
			in.clear();
			in.seekg(0, ios::beg);
			if(in.peek() == EOF)
			{
				in.close();
				return false;
			}
			while(true)
			{
				if(in.peek() == EOF)
				{	
					break;
				}
				in.getline(SentenceBuf, MAX_SENTENCE,',');
				CurrentFreqSet.insert(SentenceBuf);
			}
			in.close();

			UseCurrentFreqFlag = true;
			return true;	
		}
		else
		{
			if(IDYES == AppCall::Secretary_Message_Box("�Ƿ���Ҫɾ�����ļ���...", MB_YESNOCANCEL))
			{
				SHFILEOPSTRUCT FileOp={0};
				FileOp.fFlags = FOF_ALLOWUNDO |   //����Żػ���վ
				FOF_NOCONFIRMATION; //������ȷ�϶Ի���
				FileOp.pFrom = MidleResult;
				FileOp.pTo = NULL;      //һ��Ҫ��NULL
				FileOp.wFunc = FO_DELETE;    //ɾ������
				SHFileOperation(&FileOp);
			}

		}
		AppCall::Secretary_Show();
	}
/*	
	sOutstr = "�Ƿ�ȷʵ��Ҫ���أ�" + pmPath + "�µ��������ϣ�...";
	AppCall::Secretary_Hide();
	if(IDYES != AfxMessageBox(NLPOP::string2CString(sOutstr), MB_YESNOCANCEL))
	{
		AppCall::Secretary_Show();
		return "����ȡ��...";
	}
	AppCall::Secretary_Show();
*/

	NLPOP::Get_Specified_Files(pmPath, t_Corpus_v, ".txt");
	if(t_Corpus_v.size() == 0){
		CFPattern_Output_Message("�ļ�����û�д������ĵ���", 0);
	}
	
	for(vector<string>::iterator vite = t_Corpus_v.begin(); vite != t_Corpus_v.end(); vite++)
	{
		strcpy_s(charresponse, "���ڴ������£�");
		strcat_s(charresponse, ((*vite).substr((*vite).rfind("\\") + 1)).c_str());
		CFPattern_Output_Message(charresponse, 0);

		ifstream in((*vite).c_str());
		if(in.bad())
			return false;
		in.clear();
		in.seekg(0, ios::beg);
		if(in.peek() == EOF)
		{
			in.close();
			return true;
		}
		while(true)
		{
			if(in.peek() == EOF)
			{	
				break;
			}
			in.getline(SentenceBuf, MAX_SENTENCE,'\t');
			//FreqSent = SentenceBuf;
			in >> FreqCnt;
			
			SortedFreqPattern_mm.insert(make_pair(FreqCnt, SentenceBuf));
		
			in.getline(SentenceBuf, MAX_SENTENCE,'\n');
		}
		in.close();
	}
	CFPattern_Output_Message("���ݼ��ؽ���...", 0);

	return true;
}


void CFPattern::Save_Middle_FreqPattern_Result_In_Delete_SubSent(multimap<unsigned long, string>& pmFreqPattern, set<string>& pmTempFreqSet, map<string, string>&pmCorrespondSubstr, unsigned int ReachedFreq, unsigned int ReachedLength)
{
	LPSYSTEMTIME lpSystemTime = new  SYSTEMTIME;
	GetLocalTime(lpSystemTime);
	string sOutFileName = Current_FP_FolderPath;
	sOutFileName += "MidlFPResult\\";
	_mkdir(sOutFileName.c_str());
	ostringstream ostrsteam;
	ofstream out;
	string datastr;

	if(!pmFreqPattern.empty())
	{
		sOutFileName = Current_FP_FolderPath;
		sOutFileName += "MidlFPResult\\";
		sOutFileName += "FreqPattern.dat";
		out.open(sOutFileName.c_str());
		if(out.bad())
			return;
		out.clear();
		out.seekp(0, ios::beg);
		ostrsteam.str("");
	//----------pmFreqPattern----------------------------------------------------------	
		out << "#";//-------------------date;
		ostrsteam << lpSystemTime->wYear << '_' << lpSystemTime->wMonth << '_' << lpSystemTime->wDay << ' ' << lpSystemTime->wHour << ':' << lpSystemTime->wMinute << ':' << lpSystemTime->wSecond ;
		datastr = ostrsteam.str();
		out <<  "���ڣ�" << datastr;
		out << "\n";
		out << "#";//-------------------author
		out << "Author:" << "YPench" << "\n";
		out << "#";//-------------------
		out << "\n";
		out << "#";//-------------------
		out << "Ƶ��ģʽ������" << pmFreqPattern.size() << "\n";
		out << "#";//-------------------
		out << "\n";
		out << "#csv��ʽ";
		out << "#";//-------------------
		out << "\n";
		out << "Ƶ����" << "," << "Ƶ��" << "\n";
		out << "��ǰƵ��:" << ReachedFreq << endl;
		out << "��ǰ����:" << ReachedLength << endl;
	//--------------------------save MIddle result file-------------------------
		for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); mmite++)
		{
			out << (*mmite).second  << ',' << (*mmite).first << endl;
		}
		out.close();
	
//----------pmTempFreqSet----------------------------------------------------------
		sOutFileName = Current_FP_FolderPath;
		sOutFileName += "MidlFPResult\\";
		sOutFileName += "TempFreqSet.dat";
		out.open(sOutFileName.c_str());
		if(out.bad())
			return;
		out.clear();
		out.seekp(0, ios::beg);
		for(set<string>::iterator site = pmTempFreqSet.begin(); site != pmTempFreqSet.end(); site++)
		{
			out << *site << ",";
		}
		out.close();
	}

//---------CorrespondSubstr-----------------------------------------------------------------
	if(!pmCorrespondSubstr.empty())
	{
		ostrsteam.str("");
		ostrsteam << "_" << lpSystemTime->wMonth << '_' << lpSystemTime->wDay << '_' << lpSystemTime->wHour << '_' << lpSystemTime->wMinute << '_' << lpSystemTime->wSecond;
		sOutFileName = Current_FP_FolderPath;
		sOutFileName += "Corresponds\\";
		_mkdir(sOutFileName.c_str());
		sOutFileName += "CorrespondSubstr";
		sOutFileName += ostrsteam.str();
		sOutFileName += ".txt";

		out.open(sOutFileName.c_str());
		if(out.bad())
			return;
		out.clear();
		out.seekp(0, ios::beg);

		ostrsteam.str("");
		out << "#";//-------------------date;
		ostrsteam << lpSystemTime->wYear << '_' << lpSystemTime->wMonth << '_' << lpSystemTime->wDay << ' ' << lpSystemTime->wHour << '_' << lpSystemTime->wMinute << '_' << lpSystemTime->wSecond << "\n";
		datastr = ostrsteam.str();
		out <<  "���ڣ�" << datastr;
		out << "#";//-------------------author
		out << "Author:" << "YPench" << "\n";
		out << "#";//-------------------
		out << "\n";
		out << "#";//-------------------
		out << "Ƶ��ģʽ������" << pmCorrespondSubstr.size() << "\n";
		out << "#";//-------------------
		out << "\n";
		out << "#csv��ʽ";
		out << "#";//-------------------
		out << "\n";
		out << "����" << "," << "����" << "\n";
	//--------------------------save MIddle result file-------------------------
		for(map<string, string>::iterator mite = pmCorrespondSubstr.begin(); mite != pmCorrespondSubstr.end(); )
		{
			out << (*mite).first  << ',' << (*mite).second << endl;
			mite = pmCorrespondSubstr.erase(mite);
		}
		out.close();
	}

	delete lpSystemTime;
}




void CFPattern::Save_txt_map_string_long_with_Freq(map<string, unsigned long>& pmmap, string pmSavePath, string pmmemostr = "")
{
	if(pmmap.empty())
		return;
	char charresponse[1024];
	char intchar[64];
	map<string, unsigned long>::iterator mmite = pmmap.begin();
	unsigned int filenamesequence = 0;
	ostringstream ostrsteam;

	while(true)
	{
		string sOutFileName;
		ostrsteam.str("");
		ostrsteam << filenamesequence++ << ".txt";
		sOutFileName = ostrsteam.str();
		sOutFileName = pmSavePath +sOutFileName;
					
		ofstream out(sOutFileName.c_str());
		if(out.bad())
			continue;
		out.clear();
		out.seekp(0, ios::beg);
//--------------------------save result file;
		unsigned int sentecnecnt = 0;
		while(true)
		{
			out << (*mmite).first  << '\t' << (*mmite).second << "\n";
			sentecnecnt++;
			mmite++;
			if(!(sentecnecnt < EVERY_FILE_NUMBER) || (mmite == pmmap.end()))
				break;
		}
		out.close();

		_itoa_s(filenamesequence,intchar,64,10);
		strcpy_s(charresponse, "������:");
		strcat_s(charresponse, intchar);
		strcat_s(charresponse, ".txt");
		CFPattern_Output_Message(charresponse, 0);

		if(mmite == pmmap.end())
		{
			break;
		}
	}
	
}



void CFPattern::Save_Info_multimap_long_string(multimap<unsigned long, string>& pmMultimap, string pmSaveName, string pmmemostr = "")
{
	if(pmMultimap.empty())
		return;
	LPSYSTEMTIME lpSystemTime = new  SYSTEMTIME;
	ostringstream ostrsteam;
	set<unsigned long> LenthTypSet;
	unsigned long TotalClauseFreqCount;
	pair<multimap<unsigned long, string>::iterator, multimap<unsigned long, string>::iterator> m_Multimap_Pair;

	TotalClauseFreqCount = 0;
	for(multimap<unsigned long, string>::iterator mmite = pmMultimap.begin();  mmite != pmMultimap.end(); mmite++){
		TotalClauseFreqCount += (*mmite).first;
		LenthTypSet.insert((*mmite).first);
	}

	ofstream out(pmSaveName.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	ostrsteam.str("");

	out << "#";//-------------------date;
	GetLocalTime(lpSystemTime);
	ostrsteam << lpSystemTime->wYear << '_' << lpSystemTime->wMonth << '_' << lpSystemTime->wDay << ' ' << lpSystemTime->wHour << ':' << lpSystemTime->wMinute << ':' << lpSystemTime->wSecond << "\n";
	string datastr = "���ڣ�";
	datastr += ostrsteam.str();
	out << datastr;
	out << "#";//-------------------author
	out << "Author:" << "YPench" << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#";//-------------------
	out << "����������" << TotalClauseFreqCount << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#csv��ʽ";
	out << "#";//-------------------
	out << "\n";
	if("" == pmmemostr)
		out << "Ƶ��" << ",\t" << "����"<< ",\t" <<"����" << ",\t" <<"����" << ",\t" << "�ۼӱ���" << "\n";
	else
		out << pmmemostr << endl;
	unsigned long PreTotalCnt = 0;
	 
	out.setf(ios::fixed | ios::left );
	out.precision(6);
	for(set<unsigned long>::reverse_iterator rsite = LenthTypSet.rbegin(); rsite != LenthTypSet.rend(); rsite++){
		m_Multimap_Pair = pmMultimap.equal_range(*rsite);
		PreTotalCnt += (*rsite)*pmMultimap.count(*rsite);
		out << *rsite << "," << pmMultimap.count(*rsite) << "," << (*rsite)*pmMultimap.count(*rsite) << "," << 1.0*(*rsite)*(pmMultimap.count(*rsite))/TotalClauseFreqCount << "," << 1.0*PreTotalCnt/TotalClauseFreqCount << "\n";
	}
	out.close();
	delete lpSystemTime;
}




void CFPattern::Save_FreqPattern_Result(string pmSavePath, multimap<unsigned long, string>& pmFreqPattern)
{
	CFPattern_Output_Message("����FreqPattern.dat...", 0);
	string pmSaveName = pmSavePath;
	pmSaveName += "FreqPattern.dat";
	ofstream out(pmSaveName.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
//--------------------------save result file;
	for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); mmite++){
		out << (*mmite).second  << '\n' << (*mmite).first << endl;
	}
	out.close();

	CFPattern_Output_Message("����Info.dat�ļ�...", 0);
	Save_Info_multimap_long_string(pmFreqPattern, (pmSavePath + "Info.dat"), "Ƶ��,����,����" );
	CFPattern_Output_Message("��ʼ����������.dat�ļ�...", 0);
	Save_Sorted_dat_multimap_string_long(pmFreqPattern, pmSavePath, "");
	CFPattern_Output_Message("����������...", 0);
}

void CFPattern::Save_Sorted_dat_multimap_string_long(multimap<unsigned long, string>& pmMultimap, string pmSavePath, string pmmemostr)
{
	if(pmMultimap.empty()){
		return;
	}
	ostringstream ostrsteam;
	set<unsigned long> LenthTypSet;
	for(multimap<unsigned long, string>::iterator mmite = pmMultimap.begin();  mmite != pmMultimap.end(); mmite++)
		LenthTypSet.insert((*mmite).first);

	set<unsigned long>::reverse_iterator rsite = LenthTypSet.rbegin();
	pair<multimap<unsigned long, string>::iterator, multimap<unsigned long, string>::iterator> m_Multimap_Pair;
	unsigned int filenamesequence = 0;
	unsigned long saveFPcnt = 0;
	bool BreakFileFlag = false;
	char charresponse[1024];
	char intchar[64];	

	while(true)
	{
		string sOutFileName;
		ostrsteam.str("");
		ostrsteam << filenamesequence++ << ".dat";
		sOutFileName = ostrsteam.str();
		sOutFileName = pmSavePath +sOutFileName;
		ofstream out(sOutFileName.c_str());
		if(out.bad())
			continue;
		out.clear();
		out.seekp(0, ios::beg);
		//--------------------------save result file;
		unsigned int sentecnecnt = 0;
		while(true)
		{
			if(!BreakFileFlag)
			{
				m_Multimap_Pair = pmMultimap.equal_range(*rsite);
				out << *rsite << "\t" << pmMultimap.count(*rsite) << "\n";
			}
			int newline = 0;
			BreakFileFlag = false;
			while(m_Multimap_Pair.first != m_Multimap_Pair.second)
			{
				out << (*m_Multimap_Pair.first).second << "\t";
				m_Multimap_Pair.first++;
				saveFPcnt++;
				if(!(newline++ < 10))
				{
					out << "\n";
					newline = 0;
					if(!(saveFPcnt < EVERY_FILE_NUMBER))
					{
						break;
					}
				}
			}
			if(!(saveFPcnt < EVERY_FILE_NUMBER))
			{
				BreakFileFlag = true;
				saveFPcnt = 0;
				break;
			}
			out << "\n";
			out << "//--------------------";
			out << "\n";
			sentecnecnt++;
			rsite++;

			if(!(sentecnecnt < 1000) || (rsite == LenthTypSet.rend()))
			{
				saveFPcnt = 0;
				break;
			}
		}
		out.close();

		_itoa_s(filenamesequence,intchar,64,10);
		strcpy_s(charresponse, "������:");
		strcat_s(charresponse, intchar);
		strcat_s(charresponse, ".dat");
		CFPattern_Output_Message(charresponse, 0);

		if(rsite == LenthTypSet.rend()){
			break;
		}
	}
}

void CFPattern::Save_Sorted_dat_multimap_double_string(multimap<double, string>& pmMultimap, string pmSavePath, string pmmemostr)
{
	if(pmMultimap.empty()){
		return;
	}
	ostringstream ostrsteam;
	unsigned int filenamesequence = 0;
	string sOutFileName;
	ostrsteam.str("");
	ostrsteam << filenamesequence++ << ".dat";
	sOutFileName = ostrsteam.str();
	sOutFileName = pmSavePath +sOutFileName;
	ofstream out(sOutFileName.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	int newline = 0;
	for(multimap<double, string>::reverse_iterator rmmite = pmMultimap.rbegin(); rmmite != pmMultimap.rend(); rmmite++){ 
		out << rmmite->second << "\t" << rmmite->first << endl;
		//if(!(newline++ < 10)){
		//	out << "\n";
		//	newline = 0;
		//}
	}
	out.close();
}


