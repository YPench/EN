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
	ostrsteam << "是否确实需要加载：" <<  pmfolderpath << "下的所有资料？...";
	//if(IDYES != AppCall::Secretary_Message_Box(ostrsteam.str(), MB_YESNOCANCEL)){
	//	return "命令取消...";
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
	CFPattern_Output_Message("初始化长度为1的子串...", 0);
	while(true){
		ostrsteam.str("");
		ostrsteam << "开始加载长度为" << FrequentPatternLength - 1 << "的子串..." << endl;
		CFPattern_Output_Message(ostrsteam.str().c_str(), 0);
//--------------Read Sub Frequence Pattern
		for(multimap<unsigned long, string>::iterator mmite = SortedFreqPattern_mm.begin(); mmite != SortedFreqPattern_mm.end(); mmite++){
			FrequentPatternSet.insert((*mmite).second);
		}
		SortedFreqPattern_mm.clear();
//-------------------Find Frequence Pattern----------
		ostrsteam.str("");
		ostrsteam << "开始识别长度为" << FrequentPatternLength << "的频繁项..." << endl;
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
		ostrsteam << "长度为" << FrequentPatternLength << "的频繁项识别完毕，释放内存..." << endl;
		CFPattern_Output_Message(ostrsteam.str().c_str(), 0);

		FrequentPatternSet.clear();
		CFPattern_Output_Message("内存释放结束，删除非频繁项...", 0);

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
			CFPattern_Output_Message("再无更大匹配字符串...", 0);
			break;
		}
		Last_FP_Number = Total_FP_Map.size();
//------------Save .txt file for init FrequentPatternSet(sub string.)	
		CFPattern_Output_Message("准备保存.txt文件...", 0);
		ostrsteam.str("");
		ostrsteam << FrequentPatternLength << "\\";
		Current_FP_FolderPath = FP_FolderPath + ostrsteam.str();
		FrequentPatternLength++;
		_mkdir(Current_FP_FolderPath.c_str());

		Save_txt_map_string_long_with_Freq(Total_FP_Map, Current_FP_FolderPath, "");

//------------------Save Frequence Pattern----------
		CFPattern_Output_Message("格式转换，准备保存.dat文件...", 0);
		for(map<string, unsigned long>::iterator mite = Total_FP_Map.begin();  mite != Total_FP_Map.end(); ){
			SortedFreqPattern_mm.insert(make_pair((*mite).second, (*mite).first));
			mite = Total_FP_Map.erase(mite);
		}
//----Save Files Info-------------------------------------------------------------------
		CFPattern_Output_Message("开始存储Info.dat文件...\n", 0);
		Save_Info_multimap_long_string(SortedFreqPattern_mm, Current_FP_FolderPath + "Info.dat", "频率,数量,比率");
//----Save .dat file----------------------------------------------------------
		Save_Sorted_dat_multimap_string_long(SortedFreqPattern_mm, Current_FP_FolderPath, "");

//-----------------------------------------------------------------------------------
		ostrsteam.str("");
		ostrsteam << "长度为" << FrequentPatternLength - 1 << "的频繁子串处理完毕，释放内存，准备继续..." << endl;
		CFPattern_Output_Message(ostrsteam.str().c_str(), 0);

		if(!(FrequentPatternLength < MAX_SENTENCE_LENGTH/2)){
			CFPattern_Output_Message("字串长度达到上限，所有数据处理完毕...", 0);
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
		CFPattern_Output_Message("没有数据，处理结束……", 0);
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
			CFPattern_Output_Message("数据初始化出错，请重新启动……", 0);
            return; 
		}
    }
	CFPattern_Output_Message("计算出现频率总数……", 0);
	set<unsigned long> FreqypeSet;
	for(multimap<unsigned long, string>::iterator mmite = pmFreqPattern.begin(); mmite != pmFreqPattern.end(); mmite++)
	{
		FreqypeSet.insert((*mmite).first);
	}
	//---频率迭代器找到当前频率
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
		ostrsteam << "处理频率为：" << *freqsite << "的频繁项...";
		sOutstr = ostrsteam.str();
		CFPattern_Output_Message(sOutstr.c_str(), 0);
		unsigned long sizeinidenticalfreq = pmFreqPattern.count(*freqsite);

		//------------------------------------------------------------------------------------------
		//The switch for multithread or single thread;
		if(true)//UseCurrentFreqFlag || (sizeinidenticalfreq > EVERY_FILE_NUMBER*5))
		{
			CFPattern_Output_Message("使用多线程模式...", 0);

			CFPattern_Output_Message("取出当前频率项...", 0);

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
			//--计算当前频率项的长度：LengthFreqPattern_mm
			CFPattern_Output_Message("统计当前频率下所有字符串长度...", 0);
			multimap<unsigned long, string> LengthFreqPattern_mm;
			set<unsigned int> LenthTypSet;
			for(set<string>::iterator site = TempFreqPattern_s.begin(); site != TempFreqPattern_s.end(); site++)
			{
				LengthFreqPattern_mm.insert(make_pair((*site).length(), *site));
				LenthTypSet.insert((*site).length());
				//site = TempFreqPattern_s.erase(site);
			}
			//--长度迭代器找到当前长度项
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
				//--中间结果处理结束
				UseCurrentFreqFlag = false;
				//判断上次是否运行完当前频率最后长度
				//if(++lengthsite == LenthTypSet.end())
				//	continue;
			}
			for(; lengthsite != LenthTypSet.end(); lengthsite++)
			{
				//---当前频率项安长度排序：SeqSubPattern_m
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
				ostrsteam << "时间：" <<lpSystemTime->wYear << '_' << lpSystemTime->wMonth << '_' << lpSystemTime->wDay << ' ' << lpSystemTime->wHour << ':' << lpSystemTime->wMinute << ':' << lpSystemTime->wSecond << "\n";
				ostrsteam << "当前使用" << Current_Thread_Number<< "个工作线程..." <<  endl;
				sOutstr = ostrsteam.str();
				CFPattern_Output_Message(sOutstr.c_str(), 0);
				//-----------------------------------------------------------------------
				ostrsteam.str("");
				ostrsteam << "当前频率" << *freqsite << ",长度" << *lengthsite <<  endl;
				ostrsteam << "同长度集：" << SeqSubPattern_m.size() << "项" << endl;
				ostrsteam << "同频率集：" << TempFreqPattern_s.size() << "项" << endl; 
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
					ostrsteam << "保存中间结果...";
					ostrsteam << "距上次保存：" << ts.GetDays() << "天" << ts.GetHours() << "时" << ts.GetMinutes() <<  "分" << ts.GetSeconds() << "秒...";
					ostrsteam << "频率：" << *freqsite << "，" << "已处理长度：" << *lengthsite << "..." << endl;
					ostrsteam << "频繁项合计：" << pmFreqPattern.size() + TempFreqPattern_s.size() << "项...";
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
				ostrsteam << "保存中间结果..." << endl;
				ostrsteam << "频率：" << *freqsite << "，" << "已处理长度：" << 0 << "..." << endl;
				ostrsteam << "频繁项合计：" << pmFreqPattern.size()  << "项...";
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

	//---删除空字符串
	CFPattern_Output_Message("所有同频率子项处理完毕，开始数据清理...", 0);
	
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
	
	CFPattern_Output_Message("数据清理结束...", 0);

//-------Display the ruined time;
/*
	CTime EndTime = CTime::GetCurrentTime();
	CTimeSpan ts = EndTime - StartTime;
	ostrsteam.str("");;
	ostrsteam << "耗时：" << ts.GetDays() << "天" << ts.GetHours() << "时" << ts.GetMinutes() <<  "分" << ts.GetSeconds() << "秒...";
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
					ostrsteam << "正在处理频率为" << *site << "的频繁项..." << "\n";
					ostrsteam << "找到长度相同的频繁子项：" << pmCorrespondSubstr.size() << "项." << "\n";
					ostrsteam << "约剩余：" << itercnt << "项.";
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
		CFPattern_Output_Message("文件夹中没有待处理文档！", 0);
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
	string datastr = "日期：";
	datastr += ostrsteam.str();
	out << datastr;
	out << "#";//-------------------author
	out << "Author:" << "YPench" << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#";//-------------------
	out << "频繁模式数量：" << TotalFPCnt << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#csv格式";
	out << "#";//-------------------
	out << "\n";
	out << "出现频率" << "," << "该频率下句子数量" << "\n";
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
	datastr = "日期：";
	datastr += ostrsteam.str();
	out << datastr;
	out << "#";//-------------------author
	out << "Author:" << "YPench" << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#";//-------------------
	out << "频繁模式数量：" << TotalFPCnt << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#csv格式";
	out << "#";//-------------------
	out << "\n";
	out << "字符串长度" << "," << "出现频率" << "该频率下句子数量" << "\n";
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
		sOutstr = "是否需要加载中断任务中产生的：";
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
			if(IDYES == AppCall::Secretary_Message_Box("是否需要删除该文件？...", MB_YESNOCANCEL))
			{
				SHFILEOPSTRUCT FileOp={0};
				FileOp.fFlags = FOF_ALLOWUNDO |   //允许放回回收站
				FOF_NOCONFIRMATION; //不出现确认对话框
				FileOp.pFrom = MidleResult;
				FileOp.pTo = NULL;      //一定要是NULL
				FileOp.wFunc = FO_DELETE;    //删除操作
				SHFileOperation(&FileOp);
			}

		}
		AppCall::Secretary_Show();
	}
/*	
	sOutstr = "是否确实需要加载：" + pmPath + "下的所有资料？...";
	AppCall::Secretary_Hide();
	if(IDYES != AfxMessageBox(NLPOP::string2CString(sOutstr), MB_YESNOCANCEL))
	{
		AppCall::Secretary_Show();
		return "命令取消...";
	}
	AppCall::Secretary_Show();
*/

	NLPOP::Get_Specified_Files(pmPath, t_Corpus_v, ".txt");
	if(t_Corpus_v.size() == 0){
		CFPattern_Output_Message("文件夹中没有待处理文档！", 0);
	}
	
	for(vector<string>::iterator vite = t_Corpus_v.begin(); vite != t_Corpus_v.end(); vite++)
	{
		strcpy_s(charresponse, "正在处理文章：");
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
	CFPattern_Output_Message("数据加载结束...", 0);

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
		out <<  "日期：" << datastr;
		out << "\n";
		out << "#";//-------------------author
		out << "Author:" << "YPench" << "\n";
		out << "#";//-------------------
		out << "\n";
		out << "#";//-------------------
		out << "频繁模式数量：" << pmFreqPattern.size() << "\n";
		out << "#";//-------------------
		out << "\n";
		out << "#csv格式";
		out << "#";//-------------------
		out << "\n";
		out << "频繁项" << "," << "频率" << "\n";
		out << "当前频率:" << ReachedFreq << endl;
		out << "当前长度:" << ReachedLength << endl;
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
		out <<  "日期：" << datastr;
		out << "#";//-------------------author
		out << "Author:" << "YPench" << "\n";
		out << "#";//-------------------
		out << "\n";
		out << "#";//-------------------
		out << "频繁模式数量：" << pmCorrespondSubstr.size() << "\n";
		out << "#";//-------------------
		out << "\n";
		out << "#csv格式";
		out << "#";//-------------------
		out << "\n";
		out << "子项" << "," << "父项" << "\n";
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
		strcpy_s(charresponse, "保存结果:");
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
	string datastr = "日期：";
	datastr += ostrsteam.str();
	out << datastr;
	out << "#";//-------------------author
	out << "Author:" << "YPench" << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#";//-------------------
	out << "句子总数：" << TotalClauseFreqCount << "\n";
	out << "#";//-------------------
	out << "\n";
	out << "#csv格式";
	out << "#";//-------------------
	out << "\n";
	if("" == pmmemostr)
		out << "频率" << ",\t" << "种数"<< ",\t" <<"数量" << ",\t" <<"比率" << ",\t" << "累加比率" << "\n";
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
	CFPattern_Output_Message("保存FreqPattern.dat...", 0);
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

	CFPattern_Output_Message("保存Info.dat文件...", 0);
	Save_Info_multimap_long_string(pmFreqPattern, (pmSavePath + "Info.dat"), "频率,数量,比率" );
	CFPattern_Output_Message("开始保存排序后的.dat文件...", 0);
	Save_Sorted_dat_multimap_string_long(pmFreqPattern, pmSavePath, "");
	CFPattern_Output_Message("结果保存结束...", 0);
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
		strcpy_s(charresponse, "保存结果:");
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


