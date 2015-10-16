#pragma once
#include "..\\Include\\CGCom.h"
#include "..\\Include\\ace.h"
#include "regex.hpp"

#include "CFPattern.h"



typedef struct _FREQPATTERNINFO
{
	double inner;
	double outer;
	unsigned long ammount;
}FREQPATTERNINFO, *pFREQPATTERNINFO;


class CRegex
{
	CFPattern m_CFPattern;
	string NE_Pattern_Folder;
	
	set<string> Number_set;
	set<string> English_set;

public:
	bool FreqPattern_Initilized_Flag;
//------------------------For named entity name match---------------------------------
	map<string, set<string>> WordMor_ms;
	set<string> FirstName_s;
	set<string> Headnoun_s;

//------------------------For named Entity inner and outer pattern--------------------------------------------
	map<size_t, boost::wregex> NE_inner_wregex_m;

	map<size_t, set<size_t>*> DulRegex_map;
	map<size_t, boost::wregex> L_wregex_m;
	map<size_t, boost::wregex> R_wregex_m;
	
//:test
	//map<string, size_t> L_ID_m;
	//map<string, size_t> R_ID_m;
	//map<size_t, string> L_String2ID_m;
	//map<size_t, string> R_String2ID_m;

public:
	CRegex();
	~CRegex();

//==================START Boundary Regex Pattern Initializtion
	void Named_Entity_START_Boundary_Regex_Pattern_Initialization(vector<NE_Surround*>& NE_Surround_v, size_t MaxPattern);
	void Generate_START_Boundary_Dual_Match_Pair(vector<NE_Surround*>& NE_Surround_v, map<string, map<string, size_t>*>& DualMatchedStr_mm);

//==================END Boundary Regex Pattern Initializtion
	void Named_Entity_END_Boundary_Regex_Pattern_Initialization(vector<NE_Surround*>& NE_Surround_v, size_t MaxPattern);
	void Generate_END_Boundary_Dual_Match_Pair(vector<NE_Surround*>& NE_Surround_v, map<string, map<string, size_t>*>& DualMatchedStr_mm);

//==================Inner Regex Pattern Initializtion
	void Named_Entity_Inner_Regex_Pattern_Initialization(size_t MaxPattern);
	void Gen_Regex_Named_Entity_Inner_Patterns(vector<string>& inner_v, map<string, size_t>& InnerMatchedStr_m);
	
//==================Outer Regex Pattern Initializtion	
	void Named_Entity_Outer_Regex_Pattern_Initialization (size_t MaxPattern);
	
	void Instantiation_Dual_Match_Regex_Pattern_Erasing(size_t MaxPattern, map<string, map<string, size_t>*>& DualMatchedStr_mm);
	void Gen_Regex_Named_Entity_Outer_Patterns_Erasing(vector<pair<string, string>*>& outerpair_v, map<string, map<string, size_t>*>& DualMatchedStr_mm);
	

//==================Init inner and outer pattern Data
	void Init_Inner_and_Outer_Pattern_Data_By_Surround(const char* pDataFolder, vector<NE_Surround*>& NE_Surround_v);
	void Generate_Inner_and_Outer_Vector_By_Surround(vector<NE_Surround*>& NE_Surround_v, vector<string>& inner_v, vector<vector<string>>& outer_v);
	void Generate_Frequent_Pattern(vector<string>& inner_v, vector<vector<string>>& outer_v, const char* pDataFolder);

	bool Named_Entity_Inner_Matched(const char* pcharseq, string& matchstr);
	bool Named_Entity_Inner_Matched(const char* pcharseq, size_t& matchID);
	void Named_Entity_Inner_Matched(const char* pcharseq, vector<string>& matchstr_v);

	bool Regex_Pattern_Dual_Match_Poart(const char* Lpcharseq, const char* Rpcharseq, vector<string>& matchstr_v);
	
	void Load_Inner_and_Outer_Vector(vector<string>& inner_v, vector<vector<string>>& outer_v, int Load_Model);
	void Load_Frequent_Pattern_and_Filtering(set<string>& InnerBoundMatch_s, set<string>& OuterBoundMatch_s);
//------------------------For named entity name match---------------------------------
	void Init_Regex_WordMor_MS(const char* wordsmspath);
	void Init_Head_Noun_Set(vector<NE_Surround*>& Surround_v);
	void Possible_Right_adjacent_Max_Words_POS(const char* strchar, bool OuterFlag, vector<string>& outstr_v);
	void Possible_Left_adjacent_Max_Words_POS(const char* strchar, bool OuterFlag, vector<string>& outstr_v);
	void START_With_Surname(const char* strchar, bool START_Flag, vector<string>& outstr_v);
	void END_of_Head_Noun(const char* strchar, bool END_Flag, vector<string>& outstr_v);

	void Regex_Reset();

	void Adding_FreqPattern_to_Lexicon(const char* pDataFolder, set<string>& pmLexicon);
	void Adding_FreqPattern_to_Lexicon(const char* pDataFolder, set<string>& pmInner_s, set<string>& pmOuter_s);

};
