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
#include "Antical.h"
#include "NECom.h"
#include <algorithm>
#include "CExtra.h"
#include "ConvertUTF.h"
#include "CEDT.h"

extern int FEEDBACK_CASES_NUM;
extern int GREEDY_MATCH_NUM;

extern double FEEDBACE_PRO_LIMIT;
extern int CAND_FEEDBACK_NUM;
extern int BOARD_n_GRAM_NUM;
extern int INTERNAL_WORDS_NUM;
extern int BOARD_POS_NUM;

CExtra::CExtra()
{

}
CExtra::~CExtra()
{
}

void CExtra::ACE_Extra_Information_Extraction_Port(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v)
{
	Load_Entity_TYPE_Info(TestingDism_v);

	Mention_TYPE_Recognition(TrainingDism_v, TestingDism_v);

	Entity_CLASS_Recognition(TrainingDism_v, TestingDism_v);

	Entity_SUBTYPE_Recognition(TrainingDism_v, TestingDism_v);

}
void CExtra::Entity_CLASS_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v)
{
	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	AppCall::Maxen_Responce_Message("**********************CExtra::Entity_CLASS_Recognition()\n");

	MaxentModel loc_maxent;
//============Training
	for(size_t i = 0; i < TrainingDism_v.size(); i++){
		DismCase& locDism = *TrainingDism_v[i];
		locDism.Entity_CLASS.first = locDism.pNE_mention->Entity_CLASS;
		locDism.Entity_CLASS.second.first == "";
		locDism.Entity_CLASS.second.second = 0;
	}
	FeatureVctor FeatureCase_v;
	FeatureCase_v.reserve(RESERVED_NUM);

	for(size_t i = 0; i < TrainingDism_v.size(); i++){
		DismCase& loc_DismCase = *TrainingDism_v[i];
		pCCEDT->m_CNEFeator.Push_Back_Extracted_Feature_Vector_of_Deterimer(loc_DismCase.Entity_CLASS.first, loc_DismCase, FeatureCase_v);
	}
	AppCall::Maxen_Responce_Message("**********************\nTraining of CExtra::Entity_CLASS_Recognition()\n");
	MAXEN::Maxen_Training_with_Erasing(FeatureCase_v, loc_maxent, pCCEDT->m_CanditIte);

	FeatureCase_v.clear();
//============Testing
	AppCall::Maxen_Responce_Message("\nGenerating Testing Cases...\n");
	
	for(size_t i = 0; i < TestingDism_v.size(); i++){
		DismCase& locDism = *TestingDism_v[i];
		if(locDism.Dism_Flag){
			locDism.Entity_CLASS.first = locDism.pNE_mention->Entity_CLASS;
		}
		else{
			locDism.Entity_CLASS.first = NEGETIVE;
		}
	}
	AppCall::Maxen_Responce_Message("\nGenerating Testing Cases Features...\n");
	for(size_t i = 0; i < TestingDism_v.size(); i++){
		DismCase& loc_DismCase = *TestingDism_v[i];
		pCCEDT->m_CNEFeator.Push_Back_Extracted_Feature_Vector_of_Deterimer(loc_DismCase.Entity_CLASS.first, loc_DismCase, FeatureCase_v);
	}
	
	AppCall::Maxen_Responce_Message("**********************\nTesting of CExtra::Entity_CLASS_Recognition()\n");
	vector<pair<string, pair<string, double>>*> Result_v;
	for(size_t i = 0; i < TestingDism_v.size(); i++){
		DismCase& locDism = *TestingDism_v[i];
		Result_v.push_back(&locDism.Entity_CLASS);
	}
	MAXEN::CExtra_Maxen_Testing_with_FeatureVector_Erasing(Result_v, FeatureCase_v, loc_maxent);

}


void CExtra::Entity_SUBTYPE_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v)
{
	map<string, set<string>> NE_TYPEs_m;
	for(size_t i = 0; i < TrainingDism_v.size(); i++){
		DismCase& locDism = *TrainingDism_v[i];
		if(!locDism.Dism_Flag){
			continue;
		}
		if(NE_TYPEs_m.find(locDism.pNE_mention->Entity_TYPE) == NE_TYPEs_m.end()){
			NE_TYPEs_m[locDism.pNE_mention->Entity_TYPE];
		}
		NE_TYPEs_m[locDism.pNE_mention->Entity_TYPE].insert(locDism.pNE_mention->Entity_SUBSTYPE);
		locDism.Entity_TYPE.first = locDism.pNE_mention->Entity_TYPE;
		locDism.Entity_TYPE.second.first = "";
		locDism.Entity_TYPE.second.second = 0;
		locDism.Entity_SUBTYPE.first = locDism.pNE_mention->Entity_SUBSTYPE;
		locDism.Entity_SUBTYPE.second.first = "";
		locDism.Entity_SUBTYPE.second.second = 0;
	}

	vector<DismCase*> EachTrainingDism_v;
	vector<DismCase*> EachTestingDism_v;

	for(map<string, set<string>>::iterator msite = NE_TYPEs_m.begin(); msite != NE_TYPEs_m.end(); msite++){
		if(msite->second.size() == 0){
			AppCall::Secretary_Message_Box("Empty data in CExtra::Entity_SUBTYPE_Recognition()", MB_OK);
			continue;
		}
		if(msite->second.size() == 1){
			for(size_t i = 0; i < TestingDism_v.size(); i++){
				DismCase& locDism = *TestingDism_v[i];
				if(!strcmp(locDism.Entity_TYPE.second.first.c_str(), msite->first.c_str())){
					locDism.Entity_SUBTYPE.second.first = msite->second.begin()->data();
				}
			}
			continue;
		}
		for(size_t i = 0; i < TrainingDism_v.size(); i++){
			DismCase& locDism = *TrainingDism_v[i];
			if(!locDism.Dism_Flag){
				AppCall::Secretary_Message_Box("no negetive Dismcase in training cases", MB_OK);
				continue;
			}
			if(!strcmp(locDism.Entity_TYPE.first.c_str(), msite->first.c_str())){
				EachTrainingDism_v.push_back(TrainingDism_v[i]);
			}
		}
		for(size_t i = 0; i < TestingDism_v.size(); i++){
			DismCase& locDism = *TestingDism_v[i];
			if(!strcmp(locDism.Entity_TYPE.second.first.c_str(), msite->first.c_str())){
				EachTestingDism_v.push_back(TestingDism_v[i]);
			}
			if(locDism.Dism_Flag){
				locDism.Entity_SUBTYPE.first = locDism.pNE_mention->Entity_SUBSTYPE;
			}
			else{
				locDism.Entity_SUBTYPE.first = NEGETIVE;
			}
		}
		ostringstream ostream;
		ostream << "** " << msite->first << " **" << endl;
		AppCall::Maxen_Responce_Message("\n\n**********************\n");
		AppCall::Maxen_Responce_Message(ostream.str().c_str());
		AppCall::Maxen_Responce_Message("**********************");
		Each_Entity_SUBTYPE_Recognition(EachTrainingDism_v, EachTestingDism_v);

		EachTrainingDism_v.clear();
		EachTestingDism_v.clear();

		ostream.str("");
		ostream << endl;
		ostream << "End of \"" <<  msite->first  << "\" Recognition..." << endl; 
		AppCall::Maxen_Responce_Message(ostream.str().c_str());
	}
}

void CExtra::Each_Entity_SUBTYPE_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v)
{
	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	AppCall::Maxen_Responce_Message("**********************CExtra::Each_Entity_SUBTYPE_Recognition()\n");

	MaxentModel loc_maxent;
	
//============Training
	FeatureVctor FeatureCase_v;
	FeatureCase_v.reserve(RESERVED_NUM);

	for(size_t i = 0; i < TrainingDism_v.size(); i++){
		DismCase& loc_DismCase = *TrainingDism_v[i];
		pCCEDT->m_CNEFeator.Push_Back_Extracted_Feature_Vector_of_Deterimer(loc_DismCase.Entity_SUBTYPE.first, loc_DismCase, FeatureCase_v);
	}
	AppCall::Maxen_Responce_Message("**********************\nTraining of CExtra::Each_Entity_SUBTYPE_Recognition()\n");
	MAXEN::Maxen_Training_with_Erasing(FeatureCase_v, loc_maxent, pCCEDT->m_CanditIte);

	FeatureCase_v.clear();
//============Testing
	AppCall::Maxen_Responce_Message("\nGenerating Testing Cases Features...\n");
	for(size_t i = 0; i < TestingDism_v.size(); i++){
		DismCase& loc_DismCase = *TestingDism_v[i];
		pCCEDT->m_CNEFeator.Push_Back_Extracted_Feature_Vector_of_Deterimer(loc_DismCase.Entity_SUBTYPE.first, loc_DismCase, FeatureCase_v);
	}
	
	AppCall::Maxen_Responce_Message("**********************\nTesting of CExtra::Each_Entity_SUBTYPE_Recognition()\n");
	vector<pair<string, pair<string, double>>*> Result_v;
	for(size_t i = 0; i < TestingDism_v.size(); i++){
		DismCase& locDism = *TestingDism_v[i];
		Result_v.push_back(&locDism.Entity_SUBTYPE);
	}
	MAXEN::CExtra_Maxen_Testing_with_FeatureVector_Erasing(Result_v, FeatureCase_v, loc_maxent);
}

void CExtra::Load_Entity_TYPE_Info(vector<DismCase*>& pmDismCase_v)
{
	for(size_t i = 0; i < pmDismCase_v.size(); i++){
		DismCase& locDism = *pmDismCase_v[i];
		locDism.Entity_TYPE.first = locDism.org_TYPE;
		locDism.Entity_TYPE.second.first = locDism.predit_TYPE;
		locDism.Entity_TYPE.second.second = locDism.TYPE_value;
	}
}

void CExtra::Mention_TYPE_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v)
{
	AppCall::Maxen_Responce_Message("\n\n\n**********************************************************************\n");
	AppCall::Maxen_Responce_Message("**********************CExtra::Mention_TYPE_Recognition()\n");

	MaxentModel loc_maxent;
	
//============Training
	for(size_t i = 0; i < TrainingDism_v.size(); i++){
		DismCase& locDism = *TrainingDism_v[i];
		locDism.TYPE.first = locDism.pNE_mention->TYPE;
		locDism.TYPE.second.first == "";
		locDism.TYPE.second.second = 0;
	}
	FeatureVctor FeatureCase_v;
	FeatureCase_v.reserve(RESERVED_NUM);

	for(size_t i = 0; i < TrainingDism_v.size(); i++){
		DismCase& loc_DismCase = *TrainingDism_v[i];
		pCCEDT->m_CNEFeator.Push_Back_Extracted_Feature_Vector_of_Deterimer(loc_DismCase.TYPE.first, loc_DismCase, FeatureCase_v);
	}
	AppCall::Maxen_Responce_Message("**********************\nTraining of CExtra::Mention_TYPE_Recognition()\n");
	MAXEN::Maxen_Training_with_Erasing(FeatureCase_v, loc_maxent, pCCEDT->m_CanditIte);

	FeatureCase_v.clear();
//============Testing
	AppCall::Maxen_Responce_Message("\nGenerating Testing Cases...\n");
	
	for(size_t i = 0; i < TestingDism_v.size(); i++){
		DismCase& locDism = *TestingDism_v[i];
		if(locDism.Dism_Flag){
			locDism.TYPE.first = locDism.pNE_mention->TYPE;
		}
		else{
			locDism.TYPE.first = NEGETIVE;
		}
	}
	AppCall::Maxen_Responce_Message("\nGenerating Testing Cases Features...\n");
	for(size_t i = 0; i < TestingDism_v.size(); i++){
		DismCase& loc_DismCase = *TestingDism_v[i];
		pCCEDT->m_CNEFeator.Push_Back_Extracted_Feature_Vector_of_Deterimer(loc_DismCase.TYPE.first, loc_DismCase, FeatureCase_v);
	}
	
	AppCall::Maxen_Responce_Message("**********************\nTesting of CExtra::Mention_TYPE_Recognition()\n");
	vector<pair<string, pair<string, double>>*> Result_v;
	for(size_t i = 0; i < TestingDism_v.size(); i++){
		DismCase& locDism = *TestingDism_v[i];
		Result_v.push_back(&locDism.TYPE);
	}
	MAXEN::CExtra_Maxen_Testing_with_FeatureVector_Erasing(Result_v, FeatureCase_v, loc_maxent);
}


