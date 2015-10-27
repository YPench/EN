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


#include "stdafx.h"
#include "DetCandit.h"
#include "NECom.h"

#include <algorithm>
#include "CEDT.h"
extern int FEEDBACK_CASES_NUM;
extern int GREEDY_MATCH_NUM;

extern bool START_Feedback_Ref_Flag;
extern bool END_Feedback_Ref_Flag;

extern double FEEDBACE_PRO_LIMIT;
extern int CAND_FEEDBACK_NUM;
extern int BOARD_n_GRAM_NUM;
extern int INTERNAL_WORDS_NUM;
extern int BOARD_POS_NUM;

extern bool CEDT_Head_Flag;
extern bool CEDT_Detection_Flag;
extern bool CEDT_Extend_Done_Flag;
extern bool CEDT_Head_Done_Flag;
extern bool Greedy_Matching_Method_FLag;

extern string Detect_Single_NE_TYPE;
extern bool CEDT_R2L_or_L2R_Flag;

extern bool CRF_Candit_Gen_Flag;

CDetCandit::CDetCandit()
{
}
CDetCandit::~CDetCandit()
{
}
	//==============================================
void CDetCandit::Candidates_Recognization_Port(const char* inputchar, MaxentModel& pm_maxen, CEDTInfo& pmCEDTInfo)
{
	vector<CanditCase*>& locCandit_v = CEDT_Head_Flag?pmCEDTInfo.CanditHead_v:pmCEDTInfo.CanditExtend_v;
	Gen_Candit_Inputchar_Case_By_FeedBack_Info(inputchar, locCandit_v, pmCEDTInfo);
	
	FeatureVctor LocFeature_v;

	Generate_Candit_Cases_Feature(locCandit_v, LocFeature_v);
	
	MAXEN::Candit_Maxen_Recognization_with_Erasing(LocFeature_v, pm_maxen, locCandit_v);

}

void CDetCandit::Candidate_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v)
{
	MaxentModel loc_maxent;
	//----------------
	if(pCCEDT->Apply_for_Model_Training(training_Surround_v.empty(), "Candit")){
		Training_Named_Entity_Candidates(training_Surround_v, loc_maxent, pCCEDT->m_TrainingCEDTInfo);
		if(!pCCEDT->Forbid_Rewrite_Models_Flag){
			pCCEDT->Save_MaxentModel("Candit", loc_maxent);
		}
	}
	else{
		pCCEDT->Load_MaxentModel("Candit", loc_maxent);
	}

	//----------------Testing
	if(!testing_Surround_v.empty()){
		Testing_Named_Entity_Candidates(testing_Surround_v, loc_maxent, pCCEDT->m_TestingCEDTInfo);
	}
}

//==============================================
void CDetCandit::Training_Named_Entity_Candidates(vector<NE_Surround*>& training_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTrainingCEDTInfo)
{
	AppCall::Maxen_Responce_Message("-----------------------Generating Named Entity training features---------------------------\n");

	vector<CanditCase*>& locCandit_v = CEDT_Head_Flag?pmTrainingCEDTInfo.CanditHead_v:pmTrainingCEDTInfo.CanditExtend_v;
	locCandit_v.reserve(RESERVED_NUM);

	if(!Greedy_Matching_Method_FLag){
		Gen_Candit_Training_Case_By_FeedBack_Info_Erasing(training_Surround_v, locCandit_v, pmTrainingCEDTInfo);
	}
	else{
		Gen_Greedy_Right_or_Left_Testing_Match_Candit(training_Surround_v, locCandit_v, pmTrainingCEDTInfo);
	}

	FeatureVctor Training_v;
	Training_v.reserve(RESERVED_NUM);

	Generate_Candit_Cases_Feature(locCandit_v, Training_v);

	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	AppCall::Maxen_Responce_Message("**********************Training corpus Named Entity classifier\n");

	MAXEN::Maxen_Training_with_Erasing(Training_v, pm_maxent, pCCEDT->m_CanditIte);
}

void CDetCandit::Testing_Named_Entity_Candidates(vector<NE_Surround*>& testing_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTestingCEDTInfo)
{
	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	AppCall::Maxen_Responce_Message("**********************Testing corpus Named Entity\n");

	vector<CanditCase*>& locCandit_v = CEDT_Head_Flag?pmTestingCEDTInfo.CanditHead_v:pmTestingCEDTInfo.CanditExtend_v;
	locCandit_v.reserve(RESERVED_NUM);

	AppCall::Maxen_Responce_Message("Generating Named Entity Testing Cases...\n");
	if(!CRF_Candit_Gen_Flag && !Greedy_Matching_Method_FLag){
		Gen_Candit_Testing_Case_By_FeedBack_Info(testing_Surround_v, locCandit_v, pmTestingCEDTInfo);
	}
	else{
		Gen_Greedy_Right_or_Left_Testing_Match_Candit(testing_Surround_v, locCandit_v, pmTestingCEDTInfo);
	}
	//return;

	FeatureVctor Testing_v;
	AppCall::Maxen_Responce_Message("Candidate Testing Cases Information...\n");
	map<string, size_t> Positive_Mentions;
	NECOM::Positvie_Mention_Collection(testing_Surround_v, Positive_Mentions, CEDT_Detection_Flag?true:false);
	NECOM::Generated_Candidate_Testing_Cases_Information(Positive_Mentions, locCandit_v, ProcesedCasesInfor_m);

	AppCall::Maxen_Responce_Message("Generating Named Entity Testing Cases Features...\n");

	Testing_v.reserve(RESERVED_NUM);
	Generate_Candit_Cases_Feature(locCandit_v, Testing_v);
	
	//if(CRF_Candit_Gen_Flag){
	//	return;
	//}
	MAXEN::Candit_Maxen_Testing_with_Erasing(Testing_v, pm_maxent, Positive_Mentions, locCandit_v);
	
}
//==============================================
void CDetCandit::Gen_Candit_Training_Case_By_FeedBack_Info_Erasing(vector<NE_Surround*>& Surround_v, vector<CanditCase*>& CanditCase_v, CEDTInfo& pmTrainingCEDTInfo)
{
	//map<size_t, map<size_t, double>*>& rtnSTART_Train_mm = pmTrainingCEDTInfo.Extend_START_mm;
	//map<size_t, map<size_t, double>*>& rtnEND_Train_mm = pmTrainingCEDTInfo.Extend_END_mm;
	map<size_t, map<size_t, double>*>& rtnSTART_Train_mm = CEDT_Head_Flag?pmTrainingCEDTInfo.Head_START_mm:pmTrainingCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& rtnEND_Train_mm = CEDT_Head_Flag?pmTrainingCEDTInfo.Head_END_mm:pmTrainingCEDTInfo.Extend_END_mm;

	if(rtnSTART_Train_mm.size() != rtnEND_Train_mm.size()){
		AppCall::Maxen_Responce_Message("Unmatched data in Gen_Candit_Training_Case_By_FeedBack_Info_Erasing()");
	}
	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		map<size_t, set<size_t>> SavedPair_m;
		set<size_t> START_s;
		set<size_t> END_s;
		map<size_t, map<size_t, ACE_entity_mention*>> EntityMention_mm;
		//:head
		if(CEDT_Head_Flag){
			ace_op::Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}
		else{
			ace_op::Get_Entity_Mention_extent_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}

		if((rtnSTART_Train_mm.find(SENID) == rtnSTART_Train_mm.end()) || (rtnEND_Train_mm.find(SENID) == rtnEND_Train_mm.end())){
			continue;
		}
		map<size_t, double>& START_Rtn_map = *rtnSTART_Train_mm[SENID];
		map<size_t, double>& END_Rtn_map = *rtnEND_Train_mm[SENID];

		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			if((START_Rtn_map.find(i) == START_Rtn_map.end()) || (END_Rtn_map.find(i) == END_Rtn_map.end())){
				AppCall::Secretary_Message_Box("Rtn Info is absence, in CDetCandit::", MB_OK);
			}
			Creating_DismCase_Cases(insurRef.c_str(), i, SENID, CASID, START_s, END_s, 
				START_Rtn_map, END_Rtn_map, SavedPair_m, EntityMention_mm, CanditCase_v);
		}
		//delete *ne_vite;
	}
	//Surround_v.clear();
}
void CDetCandit::Testing_Named_Entity_Candidates_By_Greedy_Matching(vector<NE_Surround*>& testing_Surround_v, CEDTInfo& pmTestingCEDTInfo)
{
	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	AppCall::Maxen_Responce_Message("**********************Testing corpus Named Entity\n");

	vector<CanditCase*>& locCandit_v = CEDT_Head_Flag?pmTestingCEDTInfo.CanditHead_v:pmTestingCEDTInfo.CanditExtend_v;
	locCandit_v.reserve(RESERVED_NUM);

	AppCall::Maxen_Responce_Message("Generating Named Entity Testing Cases...\n");
	Gen_Greedy_Right_or_Left_Testing_Match_Candit(testing_Surround_v, locCandit_v, pmTestingCEDTInfo);
}

void CDetCandit::Gen_Greedy_Right_or_Left_Testing_Match_Candit(vector<NE_Surround*>& Surround_v, vector<CanditCase*>& Cand_DismCase_v, CEDTInfo& pmTestingCEDTInfo)
{
	map<size_t, map<size_t, double>*>& rtnSTART_Test_mm = CEDT_Head_Flag?pmTestingCEDTInfo.Head_START_mm:pmTestingCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& rtnEND_Test_mm = CEDT_Head_Flag?pmTestingCEDTInfo.Head_END_mm:pmTestingCEDTInfo.Extend_END_mm;

	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		map<size_t, set<size_t>> SavedPair_m;
		set<size_t> START_s;
		set<size_t> END_s;
		map<size_t, map<size_t, ACE_entity_mention*>> EntityMention_mm;
		//:head
		if(CEDT_Head_Flag){
			ace_op::Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}
		else{
			ace_op::Get_Entity_Mention_extent_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}
		START_s.clear();
		END_s.clear();

		if((rtnSTART_Test_mm.find(SENID) == rtnSTART_Test_mm.end()) || (rtnEND_Test_mm.find(SENID) == rtnEND_Test_mm.end())){
			continue;
		}
		map<size_t, double>& START_Rtn_map = *(rtnSTART_Test_mm[SENID]);
		map<size_t, double>& END_Rtn_map = *(rtnEND_Test_mm[SENID]);

		for(map<size_t, double>::iterator mite = START_Rtn_map.begin(); mite != START_Rtn_map.end(); mite++){
			if(mite->second > 0.5){
				START_s.insert(mite->first);
			}
		}
		for(map<size_t, double>::iterator mite = END_Rtn_map.begin(); mite != END_Rtn_map.end(); mite++){
			if(mite->second > 0.5){
				END_s.insert(mite->first);
			}
		}
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			set<size_t>& TestPosit_s = CEDT_R2L_or_L2R_Flag?END_s:START_s;

			if(TestPosit_s.find(i) != TestPosit_s.end()){
				set<size_t> matched_s;
				if(CEDT_R2L_or_L2R_Flag){
					Greedy_Right_to_Left_matching(START_s, END_s, i, 0, GREEDY_MATCH_NUM, matched_s);
				}
				else{
					Greedy_Left_to_Right_matching(START_s, END_s, i, insurRef.length()-1, GREEDY_MATCH_NUM, matched_s);
				}
				for(set<size_t>::iterator site = matched_s.begin(); site != matched_s.end(); site++){
					if(Exist_Detection_and_Updata(SavedPair_m, *site, i)){
						continue;
					}
					pCanditCase ploc_Candit = new CanditCase;
					ploc_Candit->SENID = SENID;
					ploc_Candit->CASID = CASID++;
					ploc_Candit->CEDT_Head_Flag = CEDT_Head_Flag;
					ploc_Candit->Detection_Flag = CEDT_Detection_Flag;
					ploc_Candit->START = CEDT_R2L_or_L2R_Flag?*site:i;
					ploc_Candit->END =	 CEDT_R2L_or_L2R_Flag?i:*site;

					int Left_Out_Range = Get_Left_Outer_Feature_Range_For_START(START_s, END_s, ploc_Candit->START, -1);
					int Right_Out_Range = Get_Right_Outer_Feature_Range_For_END(START_s, END_s, ploc_Candit->END, character_v.size()-1);
					ploc_Candit->l_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), Left_Out_Range+1, ploc_Candit->START-Left_Out_Range-1);
					ploc_Candit->r_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), ploc_Candit->END+1, Right_Out_Range-ploc_Candit->END-1);
					
					ploc_Candit->prix = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), 0, ploc_Candit->START);
					ploc_Candit->prox = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), ploc_Candit->END+1, -1);
					ploc_Candit->mention = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), ploc_Candit->START, ploc_Candit->END-ploc_Candit->START+1);
					
					ploc_Candit->Cand_Flag = false;
					ploc_Candit->pNE_mention = NULL;
					ploc_Candit->org_TYPE = NEGETIVE;
					if(EntityMention_mm.find(ploc_Candit->START) != EntityMention_mm.end()){
						if(EntityMention_mm[ploc_Candit->START].find(ploc_Candit->END) != EntityMention_mm[ploc_Candit->START].end()){
							ploc_Candit->Cand_Flag = true;
							ploc_Candit->pNE_mention = EntityMention_mm[ploc_Candit->START][ploc_Candit->END];
							ploc_Candit->org_TYPE = CEDT_Detection_Flag?POSITIVE:EntityMention_mm[ploc_Candit->START][ploc_Candit->END]->Entity_TYPE;
						}
					}
					if(Greedy_Matching_Method_FLag){
						ploc_Candit->predit_TYPE = (Detect_Single_NE_TYPE.length()==0)?POSITIVE:Detect_Single_NE_TYPE;
					}
					Cand_DismCase_v.push_back(ploc_Candit);
				}
			}
		}
		//delete *ne_vite;
	}
	//Surround_v.clear();	

}

void CDetCandit::Gen_Candit_Testing_Case_By_FeedBack_Info(vector<NE_Surround*>& Surround_v, vector<CanditCase*>& Cand_DismCase_v, CEDTInfo& pmTestingCEDTInfo)
{
	map<size_t, map<size_t, double>*>& rtnSTART_Test_mm = CEDT_Head_Flag?pmTestingCEDTInfo.Head_START_mm:pmTestingCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& rtnEND_Test_mm = CEDT_Head_Flag?pmTestingCEDTInfo.Head_END_mm:pmTestingCEDTInfo.Extend_END_mm;

	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		map<size_t, set<size_t>> SavedPair_m;
		set<size_t> START_s;
		set<size_t> END_s;
		map<size_t, map<size_t, ACE_entity_mention*>> EntityMention_mm;
		//:head
		if(CEDT_Head_Flag){
			ace_op::Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}
		else{
			ace_op::Get_Entity_Mention_extent_Map((*ne_vite)->entity_mention_v, EntityMention_mm, START_s, END_s);
		}
		START_s.clear();
		END_s.clear();

		if((rtnSTART_Test_mm.find(SENID) == rtnSTART_Test_mm.end()) || (rtnEND_Test_mm.find(SENID) == rtnEND_Test_mm.end())){
			continue;
		}
		map<size_t, double>& START_Rtn_map = *(rtnSTART_Test_mm[SENID]);
		map<size_t, double>& END_Rtn_map = *(rtnEND_Test_mm[SENID]);

		for(map<size_t, double>::iterator mite = START_Rtn_map.begin(); mite != START_Rtn_map.end(); mite++){
			if(mite->second > 0.5){
				START_s.insert(mite->first);
			}
		}
		for(map<size_t, double>::iterator mite = END_Rtn_map.begin(); mite != END_Rtn_map.end(); mite++){
			if(mite->second > 0.5){
				END_s.insert(mite->first);
			}
		}
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			Creating_DismCase_Cases(insurRef.c_str(), i, SENID, CASID, START_s, END_s, 
				START_Rtn_map, END_Rtn_map, SavedPair_m, EntityMention_mm, Cand_DismCase_v);
		}
		//delete *ne_vite;
	}
	//Surround_v.clear();	
}
void CDetCandit::Gen_Candit_Inputchar_Case_By_FeedBack_Info(const char* inputchar, vector<CanditCase*>& Cand_DismCase_v, CEDTInfo& pmCEDTInfo)
{
	map<size_t, map<size_t, double>*>& rtnSTART_mm = CEDT_Head_Flag?pmCEDTInfo.Head_START_mm:pmCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& rtnEND_mm = CEDT_Head_Flag?pmCEDTInfo.Head_END_mm:pmCEDTInfo.Extend_END_mm;

	size_t CASID = 0;
	if(rtnSTART_mm.empty()){
		return;
	}
	if(rtnSTART_mm.size() != 1){
		return;
	}
	size_t SENID = rtnSTART_mm.begin()->first;
	vector<string> character_v;
	NLPOP::charseq_to_vector(inputchar, character_v);
	map<size_t, set<size_t>> SavedPair_m;
	set<size_t> START_s;
	set<size_t> END_s;

	if((rtnSTART_mm.find(SENID) == rtnSTART_mm.end()) || (rtnEND_mm.find(SENID) == rtnEND_mm.end())){
		return;
	}
	map<size_t, double>& START_Rtn_map = *(rtnSTART_mm[SENID]);
	map<size_t, double>& END_Rtn_map = *(rtnEND_mm[SENID]);

	for(map<size_t, double>::iterator mite = START_Rtn_map.begin(); mite != START_Rtn_map.end(); mite++){
		if(mite->second > 0.5){
			START_s.insert(mite->first);
		}
	}
	for(map<size_t, double>::iterator mite = END_Rtn_map.begin(); mite != END_Rtn_map.end(); mite++){
		if(mite->second > 0.5){
			END_s.insert(mite->first);
		}
	}
	map<size_t, map<size_t, ACE_entity_mention*>> EntityMention_mm;
	for(size_t i = 0; i < character_v.size(); i++){
		if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			continue;
		}
		Creating_DismCase_Cases(inputchar, i, SENID, CASID, START_s, END_s, 
			START_Rtn_map, END_Rtn_map, SavedPair_m, EntityMention_mm, Cand_DismCase_v);
	}
}

void CDetCandit::Creating_DismCase_Cases(const char* inputchar, size_t i, size_t SENID, size_t& CASID,  set<size_t>&  START_s, set<size_t>& END_s, map<size_t, double>& START_Rtn_map, 
map<size_t, double>& END_Rtn_map, map<size_t, set<size_t>>& pmSavedPair, map<size_t, map<size_t, ACE_entity_mention*>>& EntityMention_mm, vector<CanditCase*>& pmCandit_v)
{
	size_t length = NLPOP::Get_Chinese_Sentence_Length_Counter(inputchar);


	if((START_s.find(i) != START_s.end()) && START_Feedback_Ref_Flag){
		multimap<double, size_t> FeedbackCase_mm;
		if(!Greedy_Matching_Method_FLag){
			size_t FeedBorder = Get_END_Boundary_Range_For_START(START_s, END_s, i, length);
			for(size_t j = i; j <= FeedBorder; j++){//:* <? <=?
				if(END_Rtn_map.find(j) == END_Rtn_map.end()){//decreasing performance...
					continue;
				}
				if(END_s.find(j) != END_s.end()){
					FeedbackCase_mm.insert(make_pair(2, j));
				}
				else if(END_Rtn_map[j] > FEEDBACE_PRO_LIMIT){
					FeedbackCase_mm.insert(make_pair(END_Rtn_map[j], j));
				}
			}
		}
		else{
			//AppCall::Secretary_Message_Box("Can not be use yet! in CDetCandit::Creating_DismCase_Cases()", MB_OK);
		}
		Erasing_Prescribed_Candidates(FeedbackCase_mm);
		for(multimap<double, size_t>::iterator mmite = FeedbackCase_mm.begin(); mmite != FeedbackCase_mm.end(); mmite++){
			if(Exist_Detection_and_Updata(pmSavedPair, i, mmite->second)){
				continue;
			}
			pCanditCase ploc_Candit = new CanditCase;
			ploc_Candit->SENID = SENID;
			ploc_Candit->CASID = CASID++;
			ploc_Candit->CEDT_Head_Flag = CEDT_Head_Flag;
//			ploc_Candit->Detection_Flag = CEDT_Detection_Flag;
			ploc_Candit->START = i;
			ploc_Candit->END = mmite->second;

			int Left_Out_Range = Get_Left_Outer_Feature_Range_For_START(START_s, END_s, ploc_Candit->START, -1);
			int Right_Out_Range = Get_Right_Outer_Feature_Range_For_END(START_s, END_s, ploc_Candit->END, length-1);
			ploc_Candit->l_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, Left_Out_Range+1, ploc_Candit->START-Left_Out_Range-1);
			ploc_Candit->r_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->END+1, Right_Out_Range-ploc_Candit->END-1);
			
			ploc_Candit->prix = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, 0, ploc_Candit->START);
			ploc_Candit->prox = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->END+1, -1);
			ploc_Candit->mention = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->START, ploc_Candit->END-ploc_Candit->START+1);

			ploc_Candit->Cand_Flag = false;
			ploc_Candit->pNE_mention = NULL;
			ploc_Candit->predit_TYPE = (Detect_Single_NE_TYPE.length()==0)?POSITIVE:Detect_Single_NE_TYPE;
			ploc_Candit->org_TYPE = NEGETIVE;
			if(EntityMention_mm.find(ploc_Candit->START) != EntityMention_mm.end()){
				if(EntityMention_mm[ploc_Candit->START].find(ploc_Candit->END) != EntityMention_mm[ploc_Candit->START].end()){
					ploc_Candit->Cand_Flag = true;
					ploc_Candit->pNE_mention = EntityMention_mm[ploc_Candit->START][ploc_Candit->END];
					ploc_Candit->org_TYPE = CEDT_Detection_Flag?POSITIVE:EntityMention_mm[ploc_Candit->START][ploc_Candit->END]->Entity_TYPE;
				}
			}
			pmCandit_v.push_back(ploc_Candit);
		}
	}
	if((END_s.find(i) != END_s.end()) && END_Feedback_Ref_Flag){
		multimap<double, size_t> FeedbackCase_mm;
		if(!Greedy_Matching_Method_FLag){
			int FeedBorder = Get_START_Boundary_Range_For_END(START_s, END_s, i, 0);
			for(int j = i; j >= FeedBorder; j--){
				if(START_Rtn_map.find(j) == START_Rtn_map.end()){
					continue;
				}
				if(START_s.find(j) != START_s.end()){
					FeedbackCase_mm.insert(make_pair(2, j));
				}
				else if(START_Rtn_map[j] > FEEDBACE_PRO_LIMIT){
					FeedbackCase_mm.insert(make_pair(START_Rtn_map[j], j));
				}
			}
		}
		else{
			set<size_t> matched_s;
			Greedy_Right_to_Left_matching(START_s, END_s, i, 0, 2, matched_s);
			//Greedy_Left_to_Right_matching(START_s, END_s, i, length-1, 2, matched_s);

			for(set<size_t>::iterator site = matched_s.begin(); site != matched_s.end(); site++){
				if(START_Rtn_map.find(*site) == START_Rtn_map.end()){
					continue;
				}
				else{
					FeedbackCase_mm.insert(make_pair(START_Rtn_map[*site], *site));
				}
			}
		}
		Erasing_Prescribed_Candidates(FeedbackCase_mm);
		for(multimap<double, size_t>::iterator mmite = FeedbackCase_mm.begin(); mmite != FeedbackCase_mm.end(); mmite++){
			if(Exist_Detection_and_Updata(pmSavedPair, mmite->second, i)){
				continue;
			}
			pCanditCase ploc_Candit = new CanditCase;
			ploc_Candit->SENID = SENID;
			ploc_Candit->CASID = CASID++;
			ploc_Candit->CEDT_Head_Flag = CEDT_Head_Flag;
			ploc_Candit->Detection_Flag = CEDT_Detection_Flag;
			ploc_Candit->START = mmite->second;
			ploc_Candit->END = i;

			int Left_Out_Range = Get_Left_Outer_Feature_Range_For_START(START_s, END_s, ploc_Candit->START, -1);
			int Right_Out_Range = Get_Right_Outer_Feature_Range_For_END(START_s, END_s, ploc_Candit->END, length-1);
			ploc_Candit->l_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, Left_Out_Range+1, ploc_Candit->START-Left_Out_Range-1);
			ploc_Candit->r_outmstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->END+1, Right_Out_Range-ploc_Candit->END-1);
			
			ploc_Candit->prix = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, 0, ploc_Candit->START);
			ploc_Candit->prox = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->END+1, -1);
			ploc_Candit->mention = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, ploc_Candit->START, ploc_Candit->END-ploc_Candit->START+1);
			
			ploc_Candit->Cand_Flag = false;
			ploc_Candit->pNE_mention = NULL;
			ploc_Candit->predit_TYPE = (Detect_Single_NE_TYPE.length()==0)?POSITIVE:Detect_Single_NE_TYPE;
			ploc_Candit->org_TYPE = NEGETIVE;
			if(EntityMention_mm.find(ploc_Candit->START) != EntityMention_mm.end()){
				if(EntityMention_mm[ploc_Candit->START].find(ploc_Candit->END) != EntityMention_mm[ploc_Candit->START].end()){
					ploc_Candit->Cand_Flag = true;
					ploc_Candit->pNE_mention = EntityMention_mm[ploc_Candit->START][ploc_Candit->END];
					ploc_Candit->org_TYPE = CEDT_Detection_Flag?POSITIVE:EntityMention_mm[ploc_Candit->START][ploc_Candit->END]->Entity_TYPE;
				}
			}
			pmCandit_v.push_back(ploc_Candit);
		}
	}
}
void CDetCandit::Greedy_Left_to_Right_matching(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit, size_t Match_Distance, set<size_t>& Match_s)
{
	if(START_s.find(Curposit) == START_s.end()){
		AppCall::Secretary_Message_Box("Data Error in CDetCandit::Greedy_Left_to_Right_matching()...", MB_OK);
	}
	size_t loc_Distance = 0;
	for(set<size_t>::iterator site = END_s.begin(); site != END_s.end(); site++){
		if((int)(*site) >= Curposit){
			Match_s.insert(*site);
			if(++loc_Distance == Match_Distance){
				break;
			}
		}
	}
	if(Match_s.empty()){
		Match_s.insert(limit);
	}
}


void CDetCandit::Greedy_Right_to_Left_matching(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit, size_t Match_Distance, set<size_t>& Match_s)
{
	if(END_s.find(Curposit) == END_s.end()){
		AppCall::Secretary_Message_Box("Data Error in CDetCandit::Greedy_Right_to_Left_matching()...", MB_OK);
	}
	size_t loc_Distance = 0;
	for(set<size_t>::reverse_iterator rsite = START_s.rbegin(); rsite != START_s.rend(); rsite++){
		if((int)(*rsite) <= Curposit){
			Match_s.insert(*rsite);
			if(++loc_Distance == Match_Distance){
				break;
			}
		}
	}
	if(Match_s.empty()){
		Match_s.insert(limit);
	}
}
//==============================================
void CDetCandit::Generate_Candit_Cases_Feature(vector<CanditCase*>& CanditCase_v, FeatureVctor& Training_v)
{
	for(size_t i = 0; i < CanditCase_v.size(); i++){
		vector<string> adjacent_feature_v;
		
		CanditCase& loc_Candit = *CanditCase_v[i];

		pCCEDT->m_CNEFeator.Extract_Feature_Vector_of_Candit(loc_Candit, adjacent_feature_v, CEDT_Head_Flag?"HD":"ED");

		string label;
		if(CEDT_Detection_Flag){
			label = loc_Candit.Cand_Flag?POSITIVE:NEGETIVE;
		}
		else{
			label = loc_Candit.Cand_Flag?loc_Candit.pNE_mention->Entity_TYPE:NEGETIVE;
		}

		MAXEN::Push_Back_FeatureCase_v_from_Feature_v_with_Check(Training_v, adjacent_feature_v, label);
	}
	//CanditCase_v.clear();
}

//==============================================
int CDetCandit::Get_Left_Outer_Feature_Range_For_START(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit)
{
	int FeedBorder = limit;
	for(set<size_t>::reverse_iterator rsite = END_s.rbegin(); rsite != END_s.rend(); rsite++){
		if((int)*rsite < Curposit){
			FeedBorder = *rsite;
			break;
		}
	}
	if(FeedBorder < limit){
		FeedBorder = limit;
	}
	return FeedBorder;
}

int CDetCandit::Get_Right_Outer_Feature_Range_For_END(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit)
{
	int FeedBorder = limit;
	for(set<size_t>::iterator site = START_s.begin(); site != START_s.end(); site++){
		if((int)*site > Curposit){
			FeedBorder = *site;
			break;
		}
	}
	if(FeedBorder > limit){
		FeedBorder = limit;
	}
	return FeedBorder;
}




int CDetCandit::Get_END_Boundary_Range_For_START(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit)
{
	int FeedBorder = limit;
	for(set<size_t>::iterator site = START_s.begin(); site != START_s.end(); site++){
		if((int)*site > Curposit){
			FeedBorder = *site;
			break;
		}
	}
	for(set<size_t>::iterator site = END_s.begin(); site != END_s.end(); site++){
		if(((int)*site > Curposit) && ((int)*site > FeedBorder)){
			FeedBorder = *site;
			break;
		}
	}
	/*for(set<size_t>::iterator site = END_s.begin(); site != END_s.end(); site++){
		if((int)*site > Curposit){
			FeedBorder = *site;
			break;
		}
	}
	for(set<size_t>::iterator site = START_s.begin(); site != START_s.end(); site++){
		if(((int)*site > Curposit) && ((int)*site > FeedBorder)){
			FeedBorder = *site;
			break;
		}
	}*/
	
	/*set<size_t>::iterator site;
	for(site = END_s.begin(); site != END_s.end(); site++){
		if((int)*site > Curposit){
			FeedBorder = *site;
		}
	}
	if(site != END_s.end()){
		FeedBorder = *++site;
	}*/
	if(FeedBorder > limit){
		FeedBorder = limit;
	}
	for(int i = CAND_FEEDBACK_NUM; (i >= 0) && (FeedBorder < limit); i--){
		FeedBorder++;
	}
	return FeedBorder;
}
int CDetCandit::Get_START_Boundary_Range_For_END(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit)
{
	int FeedBorder = limit;
	for(set<size_t>::reverse_iterator rsite = START_s.rbegin(); rsite != START_s.rend(); rsite++){
		if((int)*rsite < Curposit){
			FeedBorder = *rsite;
			break;
		}
	}
	set<size_t>::reverse_iterator rsite;
	for(rsite = END_s.rbegin(); rsite != END_s.rend(); rsite++){
		if(((int)*rsite < Curposit) && ((int)*rsite < FeedBorder)){
			FeedBorder = *rsite;
			break;
		}
	}

	/*for(rsite = END_s.rbegin(); rsite != END_s.rend(); rsite++){
		if(((int)*rsite < Curposit) && ((int)*rsite < FeedBorder)){
			FeedBorder = *rsite;
			break;
		}
	}*/
	/*if(FeedBorder < Curposit){
		FeedBorder++;
	}*/

	/*set<size_t>::reverse_iterator rsite;
	for(rsite = END_s.rbegin(); rsite != END_s.rend(); rsite++){
		if((int)*rsite < Curposit){
			FeedBorder = *rsite;
			break;
		}
	}*/
	/*if(rsite != END_s.rend()){
		rsite++;
		if(rsite != END_s.rend()){
			FeedBorder = *rsite;
		}
	}
	if(rsite != END_s.rbegin()){
		FeedBorder++;
	}*/
	if(FeedBorder < limit){
		FeedBorder = limit;
	}
	for(int i = CAND_FEEDBACK_NUM; (i >= 0) && (FeedBorder > limit); i--){
		FeedBorder--;
	}
	return FeedBorder;
}

//==============================================
bool CDetCandit::Exist_Detection_and_Updata(map<size_t, set<size_t>>& SavedPair_m, size_t START, size_t END)
{
	if(SavedPair_m.find(START) == SavedPair_m.end()){
		SavedPair_m[START];
	}
	if(SavedPair_m[START].find(END) != SavedPair_m[START].end()){
		return true;
	}
	SavedPair_m[START].insert(END);
	return false;
}

void CDetCandit::Erasing_Prescribed_Candidates(multimap<double, size_t>& FeedbackCase_mm)
{
	if((int)FeedbackCase_mm.size() > FEEDBACK_CASES_NUM){
		for(int num = FeedbackCase_mm.size() - FEEDBACK_CASES_NUM; num > 0; num--){
			FeedbackCase_mm.erase(FeedbackCase_mm.begin());
		}
	}
}


