#pragma once


#include "..\\Include\\ace.h"
#include "CMaxmatch.h"

class CEnsem
{
public:

	CMaxmatch m_CMaxmatch;
	size_t Maximum_NE_Length;

	vector<NE_Surround> NE_Surround_v;
	vector<string> Regexpression_v;

	BOOL CEDT_Gen_Info_Flag;
public:
	CEnsem();
	~CEnsem();

public:

	//--------Generate Training-------------------------------------------------------------------------
	void Generate_ACE_NE_Training_Cases(string CaseFilePath, string TrainingCaseFilePath, string infopath);

	//--------Generate Cases-------------------------------------------------------------------------
	void Named_Entity_Recognization_Ensem(const char* NE_Cases_Path, ACE_Corpus& ACE_Corpus);
	void Generate_Negetive_Case_Extend(vector<NE_Surround>& NE_Surround_v);
	void Delete_Negetive_Case_Without_Any_Char_In_Positive(vector<NE_Surround>& NE_Surround_v);
	void Case_Generation_Per_EN_Per_Sentence(const char* NE_Cases_Path, ACE_Corpus& ACE_Corpus, list<Named_Case>& Named_Case_l);


};
