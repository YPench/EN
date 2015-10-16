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



#include "stdafx.h"
#include "Detbound.h"
#include "NECom.h"

#include <algorithm>
#include "CEDT.h"

extern int FEEDBACK_CASES_NUM;
extern int GREEDY_MATCH_NUM;

extern double FEEDBACE_PRO_LIMIT;
extern int CAND_FEEDBACK_NUM;
extern int BOARD_n_GRAM_NUM;
extern int INTERNAL_WORDS_NUM;
extern int BOARD_POS_NUM;

extern bool CEDT_Head_Flag;
extern bool CEDT_Detection_Flag;
extern bool CEDT_Extend_Done_Flag;
extern bool CEDT_Head_Done_Flag;

CDetBound::CDetBound()
{
}
CDetBound::~CDetBound()
{
}
//=====================Boundary Port
void CDetBound::START_or_END_Boundary_Detection_Port(bool START_Flag, const char* inputstr, MaxentModel& pm_maxent, map<size_t, map<size_t, double>*>& boundout_m, size_t SENID)
{
	vector<BoundCase*> BoundCase_v;
	if(START_Flag){
		Gen_START_Boundary_Cases(inputstr, BoundCase_v, SENID);
	}
	else{
		Gen_END_Boundary_Cases(inputstr, BoundCase_v, SENID);
	}
	FeatureVctor Training_v;
	Generate_START_or_END_Cases_Feature(START_Flag, BoundCase_v, Training_v);

	NECOM::Init_START_or_END_Rtn_map_with_Erasing(BoundCase_v, boundout_m);
	
	MAXEN::Boundary_Detection_with_eval_Erasing(Training_v, pm_maxent, boundout_m);
}
void CDetBound::START_Boundary_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v)
{
	MaxentModel loc_maxent;
	//----------------
	if(pCCEDT->Apply_for_Model_Training(training_Surround_v.empty(), "START")){
		Training_START_or_END_Boundary(true, training_Surround_v, loc_maxent, pCCEDT->m_TrainingCEDTInfo);
		if(!pCCEDT->Forbid_Rewrite_Models_Flag){
			pCCEDT->Save_MaxentModel("START", loc_maxent);
		}
	}
	else{
		pCCEDT->Load_MaxentModel("START", loc_maxent);
	}

	
	if(!testing_Surround_v.empty()){
		Testing_START_or_END_Boundary(true, testing_Surround_v, loc_maxent, pCCEDT->m_TestingCEDTInfo);
	}
}

void CDetBound::END_Boundary_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v)
{
	MaxentModel loc_maxent;
	//----------------
	if(pCCEDT->Apply_for_Model_Training(training_Surround_v.empty(), "END")){
		Training_START_or_END_Boundary(false, training_Surround_v, loc_maxent, pCCEDT->m_TrainingCEDTInfo);
		if(!pCCEDT->Forbid_Rewrite_Models_Flag){
			pCCEDT->Save_MaxentModel("END", loc_maxent);
		}
	}
	else{
		pCCEDT->Load_MaxentModel("END", loc_maxent);
	}
	//----------------
	if(!testing_Surround_v.empty()){
		Testing_START_or_END_Boundary(false, testing_Surround_v, loc_maxent, pCCEDT->m_TestingCEDTInfo);
	}
}



//=====================
void CDetBound::Training_START_or_END_Boundary(bool START_Flag, vector<NE_Surround*>& training_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTrainingCEDTInfo)
{
	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	if(START_Flag){
		AppCall::Maxen_Responce_Message("**********************Training START boundary\n");
	}
	else{
		AppCall::Maxen_Responce_Message("**********************Training END boundary\n");
	}
	
	vector<BoundCase*> BoundCase_v;
	BoundCase_v.reserve(RESERVED_NUM);
	
	if(START_Flag){
		AppCall::Maxen_Responce_Message("Generate START Boundary Cases...\n");
		Gen_START_Boundary_Cases(training_Surround_v, BoundCase_v);
	}
	else{
		AppCall::Maxen_Responce_Message("Generate END Boundary Cases...\n");
		Gen_END_Boundary_Cases(training_Surround_v, BoundCase_v);
	}

	FeatureVctor Training_v;
	Training_v.reserve(RESERVED_NUM);
	
	AppCall::Maxen_Responce_Message("Generate Boundary Cases Features vectors...\n");
	Generate_START_or_END_Cases_Feature(START_Flag, BoundCase_v, Training_v);
	
	AppCall::Maxen_Responce_Message("Training...\n");

	//map<size_t, map<size_t, double>*>& rtn_Train_mm = START_Flag?pmTrainingCEDTInfo.Extend_START_mm:pmTrainingCEDTInfo.Extend_END_mm;
	map<size_t, map<size_t, double>*>& rtnSTART_mm = CEDT_Head_Flag?pmTrainingCEDTInfo.Head_START_mm:pmTrainingCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& rtnEND_mm = CEDT_Head_Flag?pmTrainingCEDTInfo.Head_END_mm:pmTrainingCEDTInfo.Extend_END_mm;
	
	NECOM::Init_START_or_END_Rtn_map_with_Erasing(BoundCase_v, START_Flag?rtnSTART_mm:rtnEND_mm);
	
	MAXEN::Boundary_Training_without_Erasing(Training_v, pm_maxent, pCCEDT->m_BoundIte);

	AppCall::Maxen_Responce_Message(START_Flag?"Testing START Boundary...\n":"Testing END Boundary...\n");
	MAXEN::Boundary_Testing_with_eval_Erasing(Training_v, pm_maxent, START_Flag?rtnSTART_mm:rtnEND_mm, false);
}

void CDetBound::Testing_START_or_END_Boundary(bool START_Flag, vector<NE_Surround*>& testing_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTestingCEDTInfo)
{
	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	if(START_Flag){
		AppCall::Maxen_Responce_Message("**********************Testing START boundary\n\n");
	}
	else{
		AppCall::Maxen_Responce_Message("**********************Testing END boundary\n\n");
	}
	
	vector<BoundCase*> BoundCase_v;
	BoundCase_v.reserve(RESERVED_NUM);

	if(START_Flag){
		Gen_START_Boundary_Cases(testing_Surround_v, BoundCase_v);
	}
	else{
		Gen_END_Boundary_Cases(testing_Surround_v, BoundCase_v);
	}

	FeatureVctor Testing_v;
	Testing_v.reserve(RESERVED_NUM);
	//----------------------
	Generate_START_or_END_Cases_Feature(START_Flag, BoundCase_v, Testing_v);

	map<size_t, map<size_t, double>*>& rtnSTART_mm = CEDT_Head_Flag?pmTestingCEDTInfo.Head_START_mm:pmTestingCEDTInfo.Extend_START_mm;
	map<size_t, map<size_t, double>*>& rtnEND_mm = CEDT_Head_Flag?pmTestingCEDTInfo.Head_END_mm:pmTestingCEDTInfo.Extend_END_mm;

	NECOM::Init_START_or_END_Rtn_map_with_Erasing(BoundCase_v, START_Flag?rtnSTART_mm:rtnEND_mm);

	MAXEN::Boundary_Testing_with_eval_Erasing(Testing_v, pm_maxent, START_Flag?rtnSTART_mm:rtnEND_mm, true);

	Testing_v.clear();
}

void CDetBound::Generate_START_or_END_Cases_Feature(bool START_Flag, vector<BoundCase*>& START_BoundCase_v, FeatureVctor& Training_v)
{
	for(size_t i = 0; i < START_BoundCase_v.size(); i++){
		vector<string> adjacent_feature_v;
		
		BoundCase& loc_BoundCase = *START_BoundCase_v[i];

		pCCEDT->m_CNEFeator.Extract_Feature_Vector_of_Boundary(loc_BoundCase, adjacent_feature_v, START_Flag, CEDT_Head_Flag?"_Hd":"_Exd");

		MAXEN::Push_Back_FeatureCase_v_from_Feature_v_with_Check(Training_v, adjacent_feature_v, loc_BoundCase.Bound_Falg?POSITIVE:NEGETIVE);
		
	}
}

//========================================
void CDetBound::Gen_START_Boundary_Cases(vector<NE_Surround*>& Surround_v, vector<BoundCase*>& START_BoundCase_v)
{
	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		set<size_t> START_s;
		set<size_t> END_s;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		//:head
		if(CEDT_Head_Flag){
			ace_op::Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, START_s, END_s);
		}
		else{
			ace_op::Get_Entity_Mention_extent_Map((*ne_vite)->entity_mention_v, START_s, END_s);
		}
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			if(START_s.find(i) != START_s.end()){
				pBoundCase ploc_BoundCase = new BoundCase;
				ploc_BoundCase->Bound_Falg = true;
				ploc_BoundCase->CASID = CASID++;
				ploc_BoundCase->position = i;
				ploc_BoundCase->SENID = SENID;
				ploc_BoundCase->prixstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), 0, i);
				ploc_BoundCase->proxstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), i, -1);		
				//ploc_BoundCase->pEntity_Mention = NULL;
				START_BoundCase_v.push_back(ploc_BoundCase);
			}
			else{
				pBoundCase ploc_BoundCase = new BoundCase;
				ploc_BoundCase->Bound_Falg = false;
				ploc_BoundCase->CASID = CASID++;
				ploc_BoundCase->position = i;
				ploc_BoundCase->SENID = SENID;
				ploc_BoundCase->prixstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), 0, i);
				ploc_BoundCase->proxstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), i, -1);
				//ploc_BoundCase->pEntity_Mention = NULL;
				START_BoundCase_v.push_back(ploc_BoundCase);
			}
		}
	}
}

void CDetBound::Gen_START_Boundary_Cases(const char* inputchar, vector<BoundCase*>& START_BoundCase_v, size_t SENID)
{
	size_t CASID = 0;
	vector<string> character_v;
	NLPOP::charseq_to_vector(inputchar, character_v);
	for(size_t i = 0; i < character_v.size(); i++){
		if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			continue;
		}
		pBoundCase ploc_BoundCase = new BoundCase;
		ploc_BoundCase->Bound_Falg = false;
		ploc_BoundCase->CASID = CASID++;
		ploc_BoundCase->position = i;
		ploc_BoundCase->SENID = SENID;
		ploc_BoundCase->prixstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, 0, i);
		ploc_BoundCase->proxstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, i, -1);
		//ploc_BoundCase->pEntity_Mention = NULL;
		START_BoundCase_v.push_back(ploc_BoundCase);
	}
}


void CDetBound::Gen_END_Boundary_Cases(vector<NE_Surround*>& Surround_v, vector<BoundCase*>& END_BoundCase_v)
{
	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		set<size_t> START_s;
		set<size_t> END_s;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		//:head
		if(CEDT_Head_Flag){
			ace_op::Get_Entity_Mention_head_Map((*ne_vite)->entity_mention_v, START_s, END_s);
		}
		else{
			ace_op::Get_Entity_Mention_extent_Map((*ne_vite)->entity_mention_v, START_s, END_s);
		}

		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			if(END_s.find(i) != END_s.end()){
				pBoundCase ploc_BoundCase = new BoundCase;
				ploc_BoundCase->Bound_Falg = true;
				ploc_BoundCase->position = i;
				ploc_BoundCase->SENID = SENID;
				ploc_BoundCase->CASID = CASID++;
				ploc_BoundCase->prixstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), 0, i+1);
				ploc_BoundCase->proxstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), i+1, -1);
				//ploc_BoundCase->pEntity_Mention = *pvite;
				END_BoundCase_v.push_back(ploc_BoundCase);
			}
			else{
				pBoundCase ploc_BoundCase = new BoundCase;
				ploc_BoundCase->Bound_Falg = false;
				ploc_BoundCase->position = i;
				ploc_BoundCase->SENID = SENID;
				ploc_BoundCase->CASID = CASID++;
				ploc_BoundCase->prixstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), 0, i+1);
				ploc_BoundCase->proxstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), i+1, -1);
				//ploc_BoundCase->pEntity_Mention = NULL;
				END_BoundCase_v.push_back(ploc_BoundCase);
			}
		}
	}
}

void CDetBound::Gen_END_Boundary_Cases(const char* inputchar, vector<BoundCase*>& END_BoundCase_v, size_t SENID)
{
	size_t CASID = 0;
	vector<string> character_v;
	NLPOP::charseq_to_vector(inputchar, character_v);

	for(size_t i = 0; i < character_v.size(); i++){
		if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			continue;
		}
		pBoundCase ploc_BoundCase = new BoundCase;
		ploc_BoundCase->Bound_Falg = false;
		ploc_BoundCase->position = i;
		ploc_BoundCase->SENID = SENID;
		ploc_BoundCase->CASID = CASID++;
		ploc_BoundCase->prixstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, 0, i+1);
		ploc_BoundCase->proxstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(inputchar, i+1, -1);
		//ploc_BoundCase->pEntity_Mention = NULL;
		END_BoundCase_v.push_back(ploc_BoundCase);
	}
}











