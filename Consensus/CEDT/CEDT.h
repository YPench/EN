/*
 * Copyright by Yanping Chen <ypench@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#pragma once

#include "Config.h"
#include "..\\Include\\ace.h"
#include "maxen.h"
#include "NEFeator.h"

#include "DetBound.h"
#include "DetCandit.h"
#include "Discrimer.h"
#include "CEDTInfo.h"
#include "Antical.h"
#include "CCRF.h"
#include "CExtra.h"
#include "CMaxmatch.h"
#include "CSegmter.h"

typedef struct _SentNEInfo{
	map<size_t, map<size_t, pair<string, double>>> Extend_mm;
	map<size_t, map<size_t, pair<string, double>>> Head_mm;
	map<size_t, map<size_t, pair<string, double>>> Mention_mm;
}SentNEInfo, *pSentNEInfo;

class CCEDT
{
	CDetBound m_CDetBound;
	CDetCandit m_CDetCandit;
	CDiscrimer m_CDiscrimer;
	CAntical m_CAntical;
	
	CExtra m_CExtra;

public:
	CCRF m_CCRF;
	CNEFeator m_CNEFeator;
//	CSegmter m_CSegmter;
//	CMaxmatch m_CMaxmatch;
//	set<string> Lexicon_s;

	size_t m_CanditIte;
	size_t m_BoundIte;
	size_t m_nGrossEval;

	BOOL CEDT_Gen_Info_Flag;
	BOOL m_DetSTART_Flag;
	BOOL m_DetEND_Flag;
	BOOL NE_Detection_Flag;
	BOOL NE_Recogniation_Flag;
	BOOL NE_Extend_Flag;
	BOOL NE_Head_Flag;
	BOOL Gen_Words_Feature_Flag;
	BOOL Gen_Char_Feature_Flag;
	BOOL Gen_nGram_Feature_Flag;
	BOOL Auto_Load_Model_Flag;
	BOOL Forbid_Rewrite_Models_Flag;

	string m_Modelspace;
	bool ACE_Corpus_Flag;
	bool Model_Loaded_Flag;

	MaxentModel Extend_START_Maxent;
	MaxentModel Extend_END_Maxent;
	MaxentModel Extend_Candit_Maxent;

	MaxentModel Head_START_Maxent;
	MaxentModel Head_END_Maxent;
	MaxentModel Head_Candit_Maxent;
	
	MaxentModel Dism_Maxent;
	//---------------------------------
	vector<NE_Surround*> training_Surround_v;
	vector<NE_Surround*> testing_Surround_v;

	CEDTInfo m_TrainingCEDTInfo;
	CEDTInfo m_TestingCEDTInfo;

	Maxen_Rtn_map P_nCrossRtn_m;
	Maxen_Rtn_map R_nCrossRtn_m;
	set<string> SegChar_s;

	vector<NE_Surround*> n_CrossSurround_v;
	vector<DismCase*> n_CrossDismCase_v;

public:
	CCEDT();
	~CCEDT();

public:
	void ACE_Corpus_Training_and_Testing_Port(const char* NE_Cases_Path, ACE_Corpus& pmACE_Corpus);

	void Training_Models_by_Cases(const char* NE_Cases_Path);
	void Testing_Models_by_Cases(const char* NE_Cases_Path);

	void Documents_Recognization(const char* Folder_Path);
	void Loading_Named_Entity_Models();
	
	string Sentence_Named_Entity_Deteceion_Port(const char* inputchar);
	string Sentence_Named_Entity_Recognization_Port(const char* inputchar);
	string Sentence_Named_Entity_Recognization(const char* inputchar, SentNEInfo& pmSentNEInfo);
	void Sentence_Named_Entity_Recognization(const char* inputchar, vector<entity_mention>& pmEntity_v);
public:
	//--------Generate Cases-------------------------------------------------------------------------
	void Init_CCEDT();
	void Named_Entity_Training_or_Testing(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	void Reset_CEDT_Memories();
	bool Apply_for_Model_Training(bool EmptyData_Flag, const char* pTYPEchar);
	string CCEDT::Return_Model_Path(const char* pTYPEchar);
	void Save_MaxentModel(const char* pTYPEchar, MaxentModel& pmaxent);
	bool Load_MaxentModel(const char* pTYPEchar, MaxentModel& pmaxent);

	string Generate_Marked_NE_Sentence_with_Erasing(const char* inputchar, vector<DismCase*>& pmDismCase_v, SentNEInfo& pmSentNEInfo, size_t migration);

	
};
