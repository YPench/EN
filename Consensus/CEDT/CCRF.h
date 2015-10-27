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

#include "..\\Include\\ace.h"
#include "CMaxmatch.h"
#include "CSegmter.h"
#include "CEDTInfo.h"

class CCEDT;
typedef vector<vector<pair<string, string>>*>::iterator PFRite;

class CCRF
{
public:
	CCEDT* pCCEDT;

	Maxen_Rtn_map P_nCrossRtn_m;
	Maxen_Rtn_map R_nCrossRtn_m;
	size_t nCrossID;

	//----------- PFR
	
	vector<vector<pair<string, string>>*> mergedPFRcorpus_v;
	vector<pair<string, string>*> G_CRFResult_v;
	set<string> NEs_s;
	set<string> PBreakpoint_s;

	Maxen_Rtn_map ProcesedCasesInfor_m;
	CSegmter m_CSegmter;
	CMaxmatch m_CMaxmatch;
	//set<string>& lexicon_set;
	set<string> m_Lexicon_set;
	double DelHowRate;
	double DelHighRate;

	set<string> Surname_s;
	set<string> Locname_s;
	set<string> Pronoun_s;

public:
	

	CCRF();//(CSegmter& rCSegmter, CMaxmatch& rCMaxmatch, set<string>& rlexicon_set);
	~CCRF();
	void Collecting_CRF_n_Cross_Result_for_Inside_out_with_Erasing();
	void CCRF::Collecting_CRF_n_Cross_Result_for_Inside_in_with_Erasing();
	void Changing_CRFResult_v_To_Pair_TYPE_with_Erasing(vector<pair<string, string>*>& CRFResult_v, map<size_t, map<size_t, string>>& OriginPridic_mm, bool Origin_Flag);
	
	void Read_CRF_n_Cross_Result_for_Inside_out_in_with_Erasing(const char* path, vector<pair<string, string>*>& CRFResult_v);
	void Output_CRF_n_Cross_Result_for_Inside_out_in_with_Erasing();
	void START_or_END_Boundary_Detection_Perforamnce(vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo);
	void Output_NE_Surround_for_START_or_END_Boundary_with_Local_Feature(string charpathstr, vector<NE_Surround*>& Surround_v);
	void CRF_Boundary_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v, 
										CEDTInfo& m_TrainingCEDTInfo, CEDTInfo& m_TestingCEDTInfo);
	void CRF_Generate_nBest_Candit_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v, 
										CEDTInfo& m_TrainingCEDTInfo, CEDTInfo& m_TestingCEDTInfo);
	void CRF_Generate_Routine_and_Reverse_Sequence_Candit_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v, 
										CEDTInfo& m_TrainingCEDTInfo, CEDTInfo& m_TestingCEDTInfo);

	void Read_CRF_Routine_Resusts_as_SequenceMap(const char* outputpath, map<size_t, map<size_t, map<size_t, string>>*>& Candits_mm);
	void Read_CRF_Reverse_Candits_CEDTInfo(const char* outputpath, map<size_t, map<size_t, map<size_t, string>>*>& Candits_mm);
	void Generate_CRF_Candits_From_SequenceMap_with_Erasing(map<size_t, map<size_t, map<size_t, string>>*>& Candits_mm, vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo);
	void Read_CRF_STRAT_or_END_Boundary_CEDTInfo(const char* outputpath, vector<NE_Surround*>& Surround_v, CEDTInfo& pmTestingCEDTInfo);
	void Read_CRF_Reversed_END_Boundary_CEDTInfo(const char* outputpath, vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo);
	void Read_CRF_nBest_Candit_Info(const char* outputpath, vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo);
	void Init_Labled_Sequence(map<size_t, map<size_t, ACE_entity_mention*>>& EntityMention_mm, map<size_t, size_t>& pmLabled_m);

	void Generate_Word_Boundary_Info(const char* inputchar, vector<string>& outpos, const char* cSegmentType);
	bool NE_Is_Regested(map<size_t, set<size_t>>& TotalNE_ms, size_t first, size_t second);
	void Read_Result_of_Consensus_CRF_Port(const char* pathchar, vector<pair<string, string>*>& CRFResult_v);
	void Performance_of_Consensus_CRF_Port(vector<pair<string, string>*>& CRFResult_v);
	void Performance_of_Max_Min_CRF(vector<pair<string, string>*>& pmCRFResult_v, map<size_t, set<size_t>>& TotalNE_ms, size_t& CorrectCnt);
	void Performance_of_Max_Min_CRF(vector<pair<string, string>*>& pmCRFResult_v, 
									  map<size_t, set<size_t>>& first_RecNE_ms, map<size_t, set<size_t>>& second_RecNE_ms, 
									  map<size_t, set<size_t>>& first_PreNE_ms, map<size_t, set<size_t>>& second_PreNE_ms,
									  map<string, pair<size_t, size_t>>& Recall_map, map<string, pair<size_t, size_t>>& Pricision_map);
	void Max_Min_Performance_Merging();
	void Output_Global_CRF_Results_with_Erasing();
	void Input_Global_CRF_Results(const char* pFile, vector<pair<string, string>*>& pmG_CRFResult_v);
	void Output_NE_Surround_for_CRF_Reverse_Sequence(string charpathstr, vector<NE_Surround*>& Surround_v, bool nomial_Flag);

	void Consensus_CRF_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	void Local_Feature_Extracting_for_CRF(const char* mentionchar, vector<vector<string>*>& pmFeatur_vv);
	void Output_NE_Surround_for_CRF_Reverse_with_Local_Feature(string charpathstr, vector<NE_Surround*>& Surround_v);
	void Output_NE_Surround_for_CRF_with_Local_Feature(string charpathstr, vector<NE_Surround*>& Surround_v);
	//void Output_NE_Surround_for_CRF_Routine_Sequence(string charpathstr, vector<NE_Surround*>& Surround_v, bool nomial_Flag);
	void Get_Entity_Mention_head_Map(vector<ACE_entity_mention>& EntityMention_v, map<size_t, map<size_t, string>>& mention_mm);
	void Get_Entity_Mention_TYPE_Map(vector<ACE_entity_mention>& EntityMention_v, map<size_t, map<size_t, string>>& mention_mm, map<size_t, map<size_t, string>>& TYPE_mm);
	void Get_Entity_Mention_TYPE_Map(vector<ACE_entity_mention>& EntityMention_v, map<size_t, map<size_t, string>>& TYPE_mm);
	void Get_Entity_Mention_extend_Map(vector<ACE_entity_mention>& EntityMention_v, map<size_t, map<size_t, string>>& mention_mm);
	
	void Performance_of_Consensus_CRF_Port_with_Multi_TYPE_Tags(vector<pair<string, string>*>& CRFResult_v);

	void CRF_Training_Port(const char* parameters, const char* cFilePath);
	
	//*************************************************** PFR
	void PFR_Port();
	void Merging_Nested_NE_of_PFR(vector<vector<pair<string, string>>*>& PFRcorpus_v, vector<vector<pair<string, string>>*>& mergedPFRcorpus_v);
	void Open_PFR(const char* pathchar, vector<vector<pair<string, string>>*>& pmPFRcorpus_v);
	void Output_Words_PFR_for_CRF(const char* pathchar, PFRite STARTite, PFRite ENDite);
	void Output_Characters_PFR_BEGIN_for_CRF(const char* pathchar, PFRite STARTite, PFRite ENDite);
	void Output_Characters_PFR_END_for_CRF(const char* pathchar, PFRite STARTite, PFRite ENDite);
	//void Read_Result_of_Consensus_CRF_Port(const char* pathchar, vector<pair<string, string>>& CRFResult_v);
	void Context_Statistical_Info(const char* pathchar, vector<vector<pair<string, string>>*>& pmPFRcorpus_v);
	void Generate_CRF_POS(const char* inputchar, vector<string>& outpos);
	void Collecting_Named_Entity_Info(const char* pathchar, vector<vector<pair<string, string>>*>& pmPFRcorpus_v);
};
