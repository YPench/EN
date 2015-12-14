/*
 * Copyright (C) 2015 by Yanping Chen <ypench@gmail.com>
 * Begin       : 01-Oct-2015
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser GPL (LGPL) as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.
 */
#pragma once

#include "Config.h"
#include "..\\Include\\ace.h"
#include "maxen.h"
#include "NEFeator.h"

#include "DetBound.h"
#include "DetCandit.h"
#include "Discrimer.h"
//#include "CEDTInfo.h"
#include "CSegmter.h"
#include "NECom.h"

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
public:
	CNEFeator m_CNEFeator;

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
	void Named_Entity_Training_or_Testing(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	void Reset_CEDT_Memories();
	bool Apply_for_Model_Training(bool EmptyData_Flag, const char* pTYPEchar);
	string Return_Model_Path(const char* pTYPEchar);
	void Save_MaxentModel(const char* pTYPEchar, MaxentModel& pmaxent);
	bool Load_MaxentModel(const char* pTYPEchar, MaxentModel& pmaxent);

	string Generate_Marked_NE_Sentence_with_Erasing(const char* inputchar, vector<DismCase*>& pmDismCase_v, SentNEInfo& pmSentNEInfo, size_t migration);


};
