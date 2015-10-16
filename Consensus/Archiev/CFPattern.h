#pragma once
#include "..\\Include\\CGCom.h"
#include "CAcina.h"

//---------------CFreqPattern-------------------------------------------
class CFPattern : public CAcina
{
public:

	MultidichTaskInfo m_MultidichTaskInfo;
	bool MultiThreadFPModifyFlag;
	multimap<unsigned long, string> SortedFreqPattern_mm;
	map<string, string> CorrespondSubstr_m;
	string Current_FP_FolderPath;
	unsigned int CurrentFreq, CurrentLength;
	set<string> CurrentFreqSet;
	bool UseCurrentFreqFlag;

public:

	void Init_Frequent_Pattern();
	void CFPattern_Output_Message(const char* c_msg, UINT nType);
	string CFPattern_Port(string pmService, vector<string> sParam_v, vector<LPVOID> pParam_v);

	//===================================================================================================
	string Find_Pattern_From_Sentence_Sequence(string pmfolderpath, string FP_FolderPath);
	void Delete_SubSent_With_MultiThred_And_AutoSaveMidleResult(multimap<unsigned long, string>& pmFreqPattern, map<string, string>& pmCorrespondSubstr);


	//===================================================================================================
	string Count_FreqPattern_Info_In_Raw_Folder(string pmPath);
	void Delete_SubSent_In_Identical_FreqPattern(multimap<unsigned long, string>& pmFreqPattern, map<string, string>& pmCorrespondSubstr);


	//===================================================================================================
	bool Load_Row_FreqPattern_For_CommFreq_Delete(string pmPath);
	void Load_FreqPattern(string pmFileName, multimap<unsigned long, string>& pmFreqPattern_mm);
	void Load_FreqPattern(string pmFileName, map<string, unsigned long>& pmFreqPattern_mm);
	void Save_txt_map_string_long_with_Freq(map<string, unsigned long>& pmmap, string pmSavePath, string pmmemostr);
	void Save_FreqPattern_Result(string pmSavePath, multimap<unsigned long, string>& pmFreqPattern);
	void Save_Middle_FreqPattern_Result_In_Delete_SubSent(multimap<unsigned long, string>& pmFreqPattern, set<string>& pmTempFreqSet, map<string, string>&pmCorrespondSubstr, unsigned int ReachedFreq, unsigned int ReachedLength);
	void Save_Sorted_dat_multimap_double_string(multimap<double, string>& pmMultimap, string pmSavePath, string pmmemostr);
	void Save_Info_multimap_long_string(multimap<unsigned long, string>& pmMultimap, string pmSaveName, string pmmemostr);
	void Save_Sorted_dat_multimap_string_long(multimap<unsigned long, string>& pmMultimap, string pmSavePath, string pmmemostr);
public:
	CFPattern();
	~CFPattern();

};
