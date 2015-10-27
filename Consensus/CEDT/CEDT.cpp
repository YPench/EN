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
#include "ConvertUTF.h"
#include "maxen.h"
#include "CEDT.h"
#include "NECom.h"

int FEEDBACK_CASES_NUM;
int GREEDY_MATCH_NUM;


double FEEDBACE_PRO_LIMIT;
int CAND_FEEDBACK_NUM;
int BOARD_n_GRAM_NUM;
int INTERNAL_WORDS_NUM;
int BOARD_POS_NUM;
int BOARD_WORDS_NUM;

int CRF_N_BEST_NUM;

bool Greedy_Matching_Method_FLag;

bool n_Cross_Flag;

bool Collect_nCross_ACE_Evaluation_Flag;



bool CEDT_Head_Flag;
bool CEDT_Detection_Flag;
bool CEDT_Extend_Done_Flag;
bool CEDT_Head_Done_Flag;
bool CEDT_Extra_ACE_NE_Flag;
bool CEDT_R2L_or_L2R_Flag;
bool CEDT_Head_Only_Flag;
bool CEDT_Extend_Only_Flag;
bool CEDT_Candit_POS_Flag;

extern string Detect_Single_NE_TYPE;
extern bool Collect_nGross_Boundary_Info;
extern bool Collect_nGross_Candidate_Info;
extern Maxen_Rtn_map P_G_nCrossRtn_m;
extern Maxen_Rtn_map R_G_nCrossRtn_m;

bool CEDT_Segmenter_Candit_External_Words_Flag;
bool CEDT_Segmenter_Candit_Internal_Words_Flag;
bool CEDT_Candit_nG_Featrue_Flag;
bool CEDT_Word_Boundary_Flag;

bool CRF_Model_Flag;
bool CRF_Max_Flag;
bool CRF_Multi_TYPE_Flag;
bool CRF_POS_Flag;
bool CRF_Merging_Max_Min_Flag;
bool CRF_DetBound_Flag;
bool CRF_Candit_Gen_Flag;
bool CRF_Mention_Detection_Flag;
bool CEDT_MM_Feature_Flag;

bool CRF_START_or_END_DetBound_Flag;
bool START_Feedback_Ref_Flag;
bool END_Feedback_Ref_Flag;
bool CEDT_Internal_Sequence_Words_Flag;


CCEDT::CCEDT()
//: m_CNEFeator(m_CSegmter, m_CMaxmatch, Lexicon_s)
//, m_CCRF(m_CSegmter, m_CMaxmatch, Lexicon_s)
{
	FEEDBACK_CASES_NUM = 2;
	START_Feedback_Ref_Flag = true;
	END_Feedback_Ref_Flag = true;

	FEEDBACE_PRO_LIMIT = 0.0;
	CAND_FEEDBACK_NUM = -1;
	BOARD_n_GRAM_NUM = 2;
	
	BOARD_POS_NUM = 1;
	BOARD_WORDS_NUM = 3;
	INTERNAL_WORDS_NUM = -1;

	m_CDetBound.pCCEDT = this;
	m_CDetCandit.pCCEDT = this;
	m_CDiscrimer.pCCEDT = this;
	m_CAntical.pCCEDT = this;
	m_CExtra.pCCEDT = this;

	ACE_Corpus_Flag = false;
	Model_Loaded_Flag = false;


	//------------------------------------Extend or Head
	CEDT_Extend_Done_Flag = false;
	CEDT_Head_Done_Flag = false;
	CEDT_Head_Only_Flag = false;
	CEDT_Extend_Only_Flag = true;
	CEDT_Head_Flag = false;
	//-------------------------------------Cross
	n_Cross_Flag = false;
	Collect_nGross_Boundary_Info = false;
	Collect_nGross_Candidate_Info = false;

	Collect_nCross_ACE_Evaluation_Flag = false;
	CEDT_Extra_ACE_NE_Flag = false;

	//-------------------------------------CRF control
	CRF_Model_Flag = false;
	CRF_Merging_Max_Min_Flag = false;
	CRF_POS_Flag = false;
	CRF_Max_Flag = true;
	CRF_Multi_TYPE_Flag = true;
	CRF_DetBound_Flag = false;
	CRF_Candit_Gen_Flag = false;

	CRF_Mention_Detection_Flag = false;
	CRF_N_BEST_NUM = 1;
	
	//-------------------------------------Greedy_Matching
	Greedy_Matching_Method_FLag = true;
	CEDT_R2L_or_L2R_Flag = false;
	GREEDY_MATCH_NUM = 3;
	
	//-------------------------------------Feature set
	CEDT_MM_Feature_Flag = true;
	CEDT_Candit_POS_Flag = false;
	CEDT_Segmenter_Candit_Internal_Words_Flag = true;
	CEDT_Segmenter_Candit_External_Words_Flag = true;
	CEDT_Candit_nG_Featrue_Flag = true;
	CEDT_Internal_Sequence_Words_Flag = false;

	CEDT_Word_Boundary_Flag = false;

	Detect_Single_NE_TYPE = "";

	m_Modelspace = DATA_FOLDER;
	m_Modelspace += "CEDT\\Model\\";
	if(NLPOP::FolderExist(NLPOP::string2CString(m_Modelspace))){
		//NLPOP::DeleteDir(NLPOP::string2CString(m_Modelspace));
		//_mkdir(m_Modelspace.c_str());
	}
	else{
		_mkdir(m_Modelspace.c_str());
	}


}
CCEDT::~CCEDT(){
	if(!n_CrossDismCase_v.empty()){
		for(size_t i = 0; i < n_CrossDismCase_v.size(); i++){
			if(n_CrossDismCase_v[i]->Head_Held_Flag){
				delete n_CrossDismCase_v[i]->pHead;
			}
			if(n_CrossDismCase_v[i]->Extend_Held_Flag){
				delete n_CrossDismCase_v[i]->pExtend;
			}
			delete n_CrossDismCase_v[i];
		}
	}
	if(n_CrossSurround_v.empty()){
		for(size_t i = 0; i < n_CrossSurround_v.size(); i++){
			delete n_CrossSurround_v[i];
		}
	}
}
void CCEDT::Init_CCEDT()
{
	Reset_CEDT_Memories();
	if(!n_CrossDismCase_v.empty()){
		for(size_t i = 0; i < n_CrossDismCase_v.size(); i++){
			if(n_CrossDismCase_v[i]->Head_Held_Flag){
				delete n_CrossDismCase_v[i]->pHead;
			}
			if(n_CrossDismCase_v[i]->Extend_Held_Flag){
				delete n_CrossDismCase_v[i]->pExtend;
			}
			delete n_CrossDismCase_v[i];
		}
	}
	if(n_CrossSurround_v.empty()){
		for(size_t i = 0; i < n_CrossSurround_v.size(); i++){
			delete n_CrossSurround_v[i];
		}
	}
	SegChar_s.insert("。");SegChar_s.insert("；");SegChar_s.insert("？");SegChar_s.insert("！");SegChar_s.insert("，");
	SegChar_s.insert("\t");SegChar_s.insert(";");SegChar_s.insert("?");SegChar_s.insert("!");SegChar_s.insert(",");//SegChar_s.insert(".");

	//Extract_Location_Name();
	string LexiconPath = DATA_FOLDER;
	if(CRF_Model_Flag){
		NLPOP::LoadWordsStringSet(LexiconPath+"Surname.dat", m_CCRF.Surname_s);
		NLPOP::LoadWordsStringSet(LexiconPath+"LocalName.dat", m_CCRF.Locname_s);
		NLPOP::LoadWordsStringSet(LexiconPath+"Pronoun.dat", m_CCRF.Pronoun_s);
		NLPOP::Converting_strng_set_into_2Gram_set(m_CCRF.Locname_s);
	}
	NLPOP::LoadWordsStringSet(LexiconPath+"Surname.dat", m_CNEFeator.Surname_s);
	NLPOP::LoadWordsStringSet(LexiconPath+"LocalName.dat", m_CNEFeator.Locname_s);
	NLPOP::LoadWordsStringSet(LexiconPath+"Pronoun.dat", m_CNEFeator.Pronoun_s);
	NLPOP::Converting_strng_set_into_2Gram_set(m_CNEFeator.Locname_s);
}
void CCEDT::Named_Entity_Training_or_Testing(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v)
{
	if(training_Surround_v.empty() && !testing_Surround_v.empty()){
		if(!Model_Loaded_Flag){
			AppCall::Secretary_Message_Box("Models should be loaded first...", MB_OK);
			return;
		}
	}
	if(!CEDT_Head_Only_Flag){
		//=================================Extend Detection
		AppCall::Maxen_Responce_Message(AppCall::Subsection_Responce_Message_Memo("Extend Detection").c_str());
		CEDT_Head_Flag = false;
		CEDT_Detection_Flag = true;
		//Collect_nGross_Candidate_Info = true;

		m_TrainingCEDTInfo.FREE();
		m_TestingCEDTInfo.FREE();

		if(!CRF_DetBound_Flag){
			m_CDetBound.START_Boundary_Training_and_Testing_Port(training_Surround_v, testing_Surround_v);
			m_CDetBound.END_Boundary_Training_and_Testing_Port(training_Surround_v, testing_Surround_v);
		}
		else{
			m_CCRF.CRF_Boundary_Training_and_Testing_Port(training_Surround_v, testing_Surround_v, m_TrainingCEDTInfo, m_TestingCEDTInfo);
		}
		
		if(!Greedy_Matching_Method_FLag){
			m_CDetCandit.Candidate_Training_and_Testing_Port(training_Surround_v, testing_Surround_v);
		}
		else{
			//m_CDetCandit.Testing_Named_Entity_Candidates_By_Greedy_Matching(testing_Surround_v, m_TestingCEDTInfo);
			m_CDetCandit.Candidate_Training_and_Testing_Port(training_Surround_v, testing_Surround_v);
		}
		CEDT_Extend_Done_Flag = true;
	}
	
	//=================================Head Detection
	if(CRF_Mention_Detection_Flag){
		AppCall::Maxen_Responce_Message(AppCall::Subsection_Responce_Message_Memo("Head Detection").c_str());
		CEDT_Head_Flag = true;//---------------------
		CEDT_Detection_Flag = true;
		m_TrainingCEDTInfo.FREE();

		//m_CCRF.CRF_Generate_Routine_and_Reverse_Sequence_Candit_Port(training_Surround_v, testing_Surround_v, m_TrainingCEDTInfo, m_TestingCEDTInfo);
		//CRF_Max_Flag = true;
		//m_CCRF.CRF_Generate_nBest_Candit_Port(training_Surround_v, testing_Surround_v, m_TrainingCEDTInfo, m_TestingCEDTInfo);
		
		CRF_Max_Flag = false;
		m_CCRF.CRF_Generate_nBest_Candit_Port(training_Surround_v, testing_Surround_v, m_TrainingCEDTInfo, m_TestingCEDTInfo);

		CEDT_Head_Done_Flag = true;
	}
	else if(!CEDT_Extend_Only_Flag){
		AppCall::Maxen_Responce_Message(AppCall::Subsection_Responce_Message_Memo("Head Detection").c_str());
		CEDT_Head_Flag = true;//---------------------
		CEDT_Detection_Flag = false;
		m_TrainingCEDTInfo.FREE();

		if(!CRF_DetBound_Flag){
			m_CDetBound.START_Boundary_Training_and_Testing_Port(training_Surround_v, testing_Surround_v);
			m_CDetBound.END_Boundary_Training_and_Testing_Port(training_Surround_v, testing_Surround_v);
		}
		else{
			m_CCRF.CRF_Boundary_Training_and_Testing_Port(training_Surround_v, testing_Surround_v, m_TrainingCEDTInfo, m_TestingCEDTInfo);
		}
		CRF_Candit_Gen_Flag = false;//True disable Candit generating, and collect n-Best Candit;
		
		if(CRF_Candit_Gen_Flag){
			//m_CCRF.CRF_Generate_Routine_and_Reverse_Sequence_Candit_Port(training_Surround_v, testing_Surround_v, m_TrainingCEDTInfo, m_TestingCEDTInfo);
			CRF_Max_Flag = true;
			m_CCRF.CRF_Generate_nBest_Candit_Port(training_Surround_v, testing_Surround_v, m_TrainingCEDTInfo, m_TestingCEDTInfo);
			CRF_Max_Flag = false;
			m_CCRF.CRF_Generate_nBest_Candit_Port(training_Surround_v, testing_Surround_v, m_TrainingCEDTInfo, m_TestingCEDTInfo);
		}
		if(!Greedy_Matching_Method_FLag){
			m_CDetCandit.Candidate_Training_and_Testing_Port(training_Surround_v, testing_Surround_v);
		}
		else{
			m_CDetCandit.Testing_Named_Entity_Candidates_By_Greedy_Matching(testing_Surround_v, m_TestingCEDTInfo);
		}
		CEDT_Head_Done_Flag = true;
	}

	AppCall::Maxen_Responce_Message(AppCall::Subsection_Responce_Message_Memo("Entity Recognition").c_str());

	CEDT_Detection_Flag = true; //---true, the positive and negative cases are divided, no discrimer distinguishes between different TYPE;
	//Greedy_Matching_Method_FLag = false;
	if(Detect_Single_NE_TYPE.length() == 0){
		m_CDiscrimer.Discrimer_Training_and_Testing_Port(training_Surround_v, testing_Surround_v);
		m_CAntical.Analysizing_Performance_Evaluation_on_Mention(testing_Surround_v, m_TestingCEDTInfo.DismCase_v, P_nCrossRtn_m, R_nCrossRtn_m);
	}
	else{
		m_CAntical.Candidates_Performance_Analysis_Single_Extend_or_Head_Mention(testing_Surround_v,  CEDT_Head_Flag, P_nCrossRtn_m, R_nCrossRtn_m);
	}

	if(CEDT_Extra_ACE_NE_Flag){
		m_CExtra.ACE_Extra_Information_Extraction_Port(m_TrainingCEDTInfo.DismCase_v, m_TestingCEDTInfo.DismCase_v);
	}
	if(!Collect_nCross_ACE_Evaluation_Flag && CEDT_Extend_Done_Flag && CEDT_Head_Done_Flag){
		m_CAntical.ACE_Named_Entity_Recognition_Evaluation_Port(testing_Surround_v, m_TestingCEDTInfo.DismCase_v);
	}
	else if(CEDT_Extend_Done_Flag && CEDT_Head_Done_Flag){
		vector<DismCase*>& loc_Dism_v = m_TestingCEDTInfo.DismCase_v;
		for(size_t i = 0; i < loc_Dism_v.size(); i++){
			n_CrossDismCase_v.push_back(loc_Dism_v[i]);
		}
		loc_Dism_v.clear();
		m_TestingCEDTInfo.CanditHead_v.clear();
		m_TestingCEDTInfo.CanditExtend_v.clear();
		vector<NE_Surround*>& loc_NE_Surround_v = testing_Surround_v;
		for(size_t i = 0; i < loc_NE_Surround_v.size(); i++){
			n_CrossSurround_v.push_back(loc_NE_Surround_v[i]);
		}
		loc_NE_Surround_v.clear();
	}
	m_TrainingCEDTInfo.FREE();
	m_TestingCEDTInfo.FREE();
}

void CCEDT::ACE_Corpus_Training_and_Testing_Port(const char* NE_Cases_Path, ACE_Corpus& pmACE_Corpus)
{
	CTime StartTime;
	StartTime = CTime::GetCurrentTime();

	Init_CCEDT();

	ACE_Corpus_Flag = true;
	vector<string> Doc_v;
	for(map<string, ACE_sgm>::iterator mite = pmACE_Corpus.ACE_sgm_mmap.begin(); mite != pmACE_Corpus.ACE_sgm_mmap.end(); mite++){
		Doc_v.push_back(mite->first);
	}
	random_shuffle(Doc_v.begin(), Doc_v.end());
	if(m_nGrossEval > 10){
		m_nGrossEval = 10;
	}
	size_t step = (m_nGrossEval == 0)?0:Doc_v.size()/m_nGrossEval;
	size_t CrossIte = (m_nGrossEval == 0)?1:m_nGrossEval;
	size_t TrainingSentID = 0;
	size_t TestingSentID = 0;
	for(size_t i = 0; i < CrossIte; i++){
		map<string, ACE_sgm> training_sgm;
		map<string, ACE_sgm> testing_sgm;
		size_t Range = (i!=(m_nGrossEval-1))?(i+1)*step:Doc_v.size();
		for(size_t j = 0; j < Doc_v.size(); j++){
			if((j >= i*step) && (j < Range) && (step != 0)){
				testing_sgm.insert(make_pair(Doc_v[j], pmACE_Corpus.ACE_sgm_mmap[Doc_v[j]]));
			}
			else{
				training_sgm.insert(make_pair(Doc_v[j], pmACE_Corpus.ACE_sgm_mmap[Doc_v[j]]));
			}
		}
		if(testing_sgm.empty()){
			AppCall::Maxen_Responce_Message("No Testing Data is selected!...");
		}
		if(NLPOP::FolderExist(NLPOP::string2CString(m_Modelspace))){
			NLPOP::DeleteDir(NLPOP::string2CString(m_Modelspace));
			_mkdir(m_Modelspace.c_str());
		}
		Reset_CEDT_Memories();
		m_CNEFeator.Init_Named_Entity_Featrue_Extractor(DATA_FOLDER);

		//NECOM::Corpus_Entity_and_Entity_Mention_Information(pmACE_Corpus.ACE_Entity_Info_map);
		
		NECOM::Extracting_Named_Entity_Surrounding(training_sgm, pmACE_Corpus.ACE_Entity_Info_map, training_Surround_v, TrainingSentID);	
		NECOM::NE_Surround_Consistence_Check(training_Surround_v, pmACE_Corpus, true);
		NECOM::NE_Surround_Relative_Position_Calculate(training_Surround_v);
		NECOM::NE_Surround_Consistence_Check(training_Surround_v, pmACE_Corpus, false);
					
		NECOM::Extracting_Named_Entity_Surrounding(testing_sgm, pmACE_Corpus.ACE_Entity_Info_map, testing_Surround_v, TestingSentID);	
		NECOM::NE_Surround_Consistence_Check(testing_Surround_v, pmACE_Corpus, true);
		NECOM::NE_Surround_Relative_Position_Calculate(testing_Surround_v);
		NECOM::NE_Surround_Consistence_Check(testing_Surround_v, pmACE_Corpus, false);

		//m_CNEFeator.Adding_Entity_Extent_and_Head_to_Lexicon(training_Surround_v, 'H');//"E,H,A"
		
		//m_CAntical.Corpus_Entity_and_Entity_Mention_Information(training_Surround_v, false);
		//m_CAntical.Corpus_Entity_and_Entity_Mention_Information(testing_Surround_v, true);

		if(CRF_Model_Flag){
			m_CCRF.Consensus_CRF_Port(training_Surround_v, testing_Surround_v);//57% 62% 60%
		}
		else{
			Named_Entity_Training_or_Testing(training_Surround_v, testing_Surround_v);
		}
		Reset_CEDT_Memories();

		//if(!n_Cross_Flag){
		//	break;
		//}
	}
	if((Collect_nGross_Boundary_Info || Collect_nGross_Candidate_Info) && !CRF_Model_Flag){
		if(CRF_Mention_Detection_Flag){
			MAXEN::Display_Performance_for_MAXEN(true, true, m_CCRF.ProcesedCasesInfor_m, m_CCRF.ProcesedCasesInfor_m, "\n****************************************\n\nThe Final Collected Cases Info...\n");
		}
		else{
			MAXEN::Display_Performance_for_MAXEN(true, true, m_CDetCandit.ProcesedCasesInfor_m, m_CDetCandit.ProcesedCasesInfor_m, "\n****************************************\n\nThe Final Collected Cases Info...\n");
		}
		MAXEN::Display_Performance_for_MAXEN(true, true, P_G_nCrossRtn_m, R_G_nCrossRtn_m, "\n****************************************\n\nThe Final Candidate Performance...\n");
		return;
	}
	if(CRF_Model_Flag){
		if(CRF_Multi_TYPE_Flag){
			m_CCRF.Output_CRF_n_Cross_Result_for_Inside_out_in_with_Erasing();
		}
		if(CRF_Merging_Max_Min_Flag){
			CEDT_Head_Flag = true;
			m_CCRF.Output_Global_CRF_Results_with_Erasing();
		}
		MAXEN::Display_Performance_for_MAXEN(true, true, m_CCRF.P_nCrossRtn_m, m_CCRF.R_nCrossRtn_m, "\n****************************************\n\nThe Final CRF n Cross Performance...\n");
		return;
	}

	MAXEN::Display_Performance_for_MAXEN(true, true, P_nCrossRtn_m, R_nCrossRtn_m, "\n****************************************\n\nThe Final n Cross Performance...\n");

	if(CEDT_Extra_ACE_NE_Flag && Collect_nCross_ACE_Evaluation_Flag){
		m_CAntical.ACE_Named_Entity_Recognition_Evaluation_Port(n_CrossSurround_v,n_CrossDismCase_v);
	}

	AppCall::Maxen_Responce_Message(AppCall::Subsection_Responce_Message_Memo("Recognition task terminates nommonly...").c_str());

	ostringstream ostrsteam;
	ostrsteam.str("");
	CTime EndTime = CTime::GetCurrentTime();
	CTimeSpan ts = EndTime - StartTime;
	ostrsteam << "开始时间：" << StartTime.GetHour() << ":" << StartTime.GetMinute() << ":" << StartTime.GetSecond() << endl;
	ostrsteam << "结束时间：" << EndTime.GetHour() << ":" << EndTime.GetMinute() << ":" << EndTime.GetSecond() << endl;
	ostrsteam << "用时：" << ts.GetDays() << "天" << ts.GetHours() << "时" << ts.GetMinutes() <<  "分" << ts.GetSeconds() << "秒..." << endl;
	AppCall::Maxen_Responce_Message(ostrsteam.str().c_str());

}
void CCEDT::Training_Models_by_Cases(const char* NE_Cases_Path)
{
	if(!NLPOP::FileExist(NLPOP::string2CString(NE_Cases_Path))){
		return;
	}
	map<string, ACE_entity_mention*> EntityMentionInfo_map;
	NECOM::Load_of_NE_Surround_Named_Entity(NE_Cases_Path, EntityMentionInfo_map, training_Surround_v);
	if(training_Surround_v.empty()){
		return;
	}
	if(NLPOP::FolderExist(NLPOP::string2CString(m_Modelspace))){
		NLPOP::DeleteDir(NLPOP::string2CString(m_Modelspace));
		_mkdir(m_Modelspace.c_str());
	}
	
	Named_Entity_Training_or_Testing(training_Surround_v, testing_Surround_v);
}

void CCEDT::Testing_Models_by_Cases(const char* NE_Cases_Path)
{
	if(!NLPOP::FileExist(NLPOP::string2CString(NE_Cases_Path))){
		return;
	}
	map<string, ACE_entity_mention*> EntityMentionInfo_map;
	NECOM::Load_of_NE_Surround_Named_Entity(NE_Cases_Path, EntityMentionInfo_map, testing_Surround_v);
	if(testing_Surround_v.empty()){
		return;
	}
	Named_Entity_Training_or_Testing(training_Surround_v, testing_Surround_v);
}
void CCEDT::Loading_Named_Entity_Models()
{
	if(!Model_Loaded_Flag){
		Init_CCEDT();
		bool Loaded_Flag = true;
		CEDT_Head_Flag = false;
		CEDT_Detection_Flag = true;
		Loaded_Flag = Loaded_Flag && Load_MaxentModel("START", Extend_START_Maxent);
		Loaded_Flag = Loaded_Flag && Load_MaxentModel("END", Extend_END_Maxent);
		Loaded_Flag = Loaded_Flag && Load_MaxentModel("Candit", Extend_Candit_Maxent);

		CEDT_Head_Flag = true;
		CEDT_Detection_Flag = true;
		Loaded_Flag = Loaded_Flag && Load_MaxentModel("START", Head_START_Maxent);
		Loaded_Flag = Loaded_Flag && Load_MaxentModel("END", Head_END_Maxent);
		Loaded_Flag = Loaded_Flag && Load_MaxentModel("Candit", Head_Candit_Maxent);

		if(NLPOP::FileExist(NLPOP::string2CString(m_Modelspace+"Head_Detec_Dism"))){
			CEDT_Detection_Flag = true;
		}
		else if(NLPOP::FileExist(NLPOP::string2CString(m_Modelspace+"Head_Recog_Dism"))){
			CEDT_Detection_Flag = false;
		}
		Loaded_Flag = Loaded_Flag && Load_MaxentModel("Dism", Dism_Maxent);

		if(Loaded_Flag){
			m_CNEFeator.Init_Named_Entity_Featrue_Extractor(DATA_FOLDER);
			Model_Loaded_Flag = true;
		}
		else{
			AppCall::Secretary_Message_Box("Failed to load Every Model...", MB_OK);
		}
	}
}

void CCEDT::Documents_Recognization(const char* Folder_Path)
{
	vector<string> FilePath_v;
	CString CPath(Folder_Path);
	if(NLPOP::FileExist(CPath)){
		FilePath_v.push_back(Folder_Path);
	}
	else if(NLPOP:: FolderExist(CPath)){
		NLPOP::Get_Specified_Files(Folder_Path, FilePath_v, "");
	}
	else{
		return;
	}
	ofstream out;
	for(size_t i = 0; i < FilePath_v.size(); i++){
		ostringstream ostrsteam;
		LPSYSTEMTIME lpSystemTime = new  SYSTEMTIME;
		GetLocalTime(lpSystemTime);
		ostrsteam <<lpSystemTime->wYear << '_' << lpSystemTime->wMonth << '_' << lpSystemTime->wDay << '_' << lpSystemTime->wHour << '_' << lpSystemTime->wMinute << '_' << lpSystemTime->wSecond ;		
		delete lpSystemTime;
		string SavePath = FilePath_v[i];
		ostrsteam << "_";
		ostrsteam << SavePath.substr(SavePath.rfind('\\')+1, SavePath.length()-SavePath.rfind('\\'));
		SavePath = SavePath.substr(0,SavePath.rfind('\\')+1);
		SavePath += ostrsteam.str();
		out.open(SavePath.c_str());
		if(out.bad())
			return;
		out.clear();
		out.seekp(0, ios::beg);

		vector<string> TotalLine_v;
		Read_Line_InFile_To_Vector_Deque_List<vector<string>>(FilePath_v[i].c_str(), TotalLine_v, 0, '\n', false);

		for(vector<string>::iterator vite = TotalLine_v.begin(); vite != TotalLine_v.end(); vite++){
			SentNEInfo LocSentNEInfo;
			out << Sentence_Named_Entity_Recognization(vite->data(), LocSentNEInfo) << endl;
		}
		out.close();
	}
}


string CCEDT::Sentence_Named_Entity_Deteceion_Port(const char* inputchar)
{
	if(!Model_Loaded_Flag){
		return inputchar;
	}
	BOOL Catched_NE_Detection_Flag = NE_Detection_Flag;
	NE_Detection_Flag = true;
	string rtnstr = Sentence_Named_Entity_Recognization_Port(inputchar);
	NE_Detection_Flag = Catched_NE_Detection_Flag;
	return rtnstr;
}
string CCEDT::Sentence_Named_Entity_Recognization_Port(const char* inputchar)
{
	if(!Model_Loaded_Flag){
		return inputchar;
	}
	SentNEInfo Loc_SentNEInf;
	
	return Sentence_Named_Entity_Recognization(inputchar, Loc_SentNEInf);
}
void CCEDT::Sentence_Named_Entity_Recognization(const char* inputchar, vector<entity_mention>& pmEntity_v)
{
	vector<string> segmentedstr_v;
	vector<bool> segmentrtn_v; 
	NECOM::Segment_Sentence_for_Named_Entity(inputchar, SegChar_s, segmentedstr_v, segmentrtn_v);

	static size_t SENID = 0;
	static size_t CASID = 0;
	SentNEInfo loc_SentNEInfo;
	string rtnstr;
	size_t SentSTART = 0;
	for(size_t i = 0; i < segmentedstr_v.size(); i++){
		if(segmentrtn_v[i]){
			CEDTInfo loc_CEDTInfo;
			CEDT_Head_Flag = false;
			CEDT_Detection_Flag = true;
			//------------------
			m_CDetBound.START_or_END_Boundary_Detection_Port(true, segmentedstr_v[i].c_str(), Extend_START_Maxent, loc_CEDTInfo.Extend_START_mm, SentSTART);
			m_CDetBound.START_or_END_Boundary_Detection_Port(false, segmentedstr_v[i].c_str(), Extend_END_Maxent, loc_CEDTInfo.Extend_END_mm, SentSTART);
			m_CDetCandit.Candidates_Recognization_Port(segmentedstr_v[i].c_str(), Extend_Candit_Maxent, loc_CEDTInfo);

			CEDT_Head_Flag = true;
			CEDT_Detection_Flag = true;
			//------------------
			m_CDetBound.START_or_END_Boundary_Detection_Port(true, segmentedstr_v[i].c_str(), Head_START_Maxent, loc_CEDTInfo.Head_START_mm, SentSTART);
			m_CDetBound.START_or_END_Boundary_Detection_Port(false, segmentedstr_v[i].c_str(), Head_END_Maxent, loc_CEDTInfo.Head_END_mm, SentSTART);
			m_CDetCandit.Candidates_Recognization_Port(segmentedstr_v[i].c_str(), Head_Candit_Maxent, loc_CEDTInfo);

			//CEDT_Detection_Flag = false;
			m_CDiscrimer.Discriminater_Recognization_Port(segmentedstr_v[i].c_str(), Dism_Maxent, loc_CEDTInfo);
			vector<DismCase*>& loc_DismCase_v = loc_CEDTInfo.DismCase_v;
			for(size_t j = 0; j < loc_DismCase_v.size(); j++){
				DismCase& loc_DismCase = *loc_DismCase_v[j];
				//if(ReqTYPE_s.find(loc_DismCase.predit_TYPE) == ReqTYPE_s.end()){
				//	continue;
				//}
				if(NLPOP::Get_Chinese_Sentence_Length_Counter(loc_DismCase.pExtend->mention.c_str())< 2){
					continue;
				}
				if(NLPOP::Get_Chinese_Sentence_Length_Counter(loc_DismCase.pExtend->mention.c_str())>6){
					continue;
				}
				entity_mention loc_entity;
				loc_entity.CASID = CASID++;
				loc_entity.SENID = SENID;
				loc_entity.TYPE = loc_DismCase.predit_TYPE;
				loc_entity.TYPE_V = loc_DismCase.TYPE_value;
				loc_entity.SUBTYPE = "";
				loc_entity.SUBTYPE_V = 0;
				loc_entity.extent.charseq = loc_DismCase.pExtend->mention;
				loc_entity.extent.START = loc_DismCase.pExtend->START;
				loc_entity.extent.END = loc_DismCase.pExtend->END;
				loc_entity.head.charseq = loc_DismCase.pHead->mention;
				loc_entity.head.START = loc_DismCase.pHead->START;
				loc_entity.head.END = loc_DismCase.pHead->END;
				pmEntity_v.push_back(loc_entity);
			}
			loc_CEDTInfo.FREE();
		}
		else{
			rtnstr += segmentedstr_v[i];
		}
		SentSTART += NLPOP::Get_Chinese_Sentence_Length_Counter(segmentedstr_v[i].c_str());
		SENID++;
	}
}

string CCEDT::Sentence_Named_Entity_Recognization(const char* inputchar, SentNEInfo& pmSentNEInfo)
{
	vector<string> segmentedstr_v;
	vector<bool> segmentrtn_v; 
	NECOM::Segment_Sentence_for_Named_Entity(inputchar, SegChar_s, segmentedstr_v, segmentrtn_v);

	SentNEInfo loc_SentNEInfo;
	string rtnstr;
	size_t SentSTART = 0;
	for(size_t i = 0; i < segmentedstr_v.size(); i++){
		if(segmentrtn_v[i]){
			CEDTInfo loc_CEDTInfo;
			CEDT_Head_Flag = false;
			CEDT_Detection_Flag = true;
			//------------------
			m_CDetBound.START_or_END_Boundary_Detection_Port(true, segmentedstr_v[i].c_str(), Extend_START_Maxent, loc_CEDTInfo.Extend_START_mm, SentSTART);
			m_CDetBound.START_or_END_Boundary_Detection_Port(false, segmentedstr_v[i].c_str(), Extend_END_Maxent, loc_CEDTInfo.Extend_END_mm, SentSTART);
			m_CDetCandit.Candidates_Recognization_Port(segmentedstr_v[i].c_str(), Extend_Candit_Maxent, loc_CEDTInfo);

			CEDT_Head_Flag = true;
			CEDT_Detection_Flag = true;
			//------------------
			m_CDetBound.START_or_END_Boundary_Detection_Port(true, segmentedstr_v[i].c_str(), Head_START_Maxent, loc_CEDTInfo.Head_START_mm, SentSTART);
			m_CDetBound.START_or_END_Boundary_Detection_Port(false, segmentedstr_v[i].c_str(), Head_END_Maxent, loc_CEDTInfo.Head_END_mm, SentSTART);
			m_CDetCandit.Candidates_Recognization_Port(segmentedstr_v[i].c_str(), Head_Candit_Maxent, loc_CEDTInfo);

			//CEDT_Detection_Flag = false;
			m_CDiscrimer.Discriminater_Recognization_Port(segmentedstr_v[i].c_str(), Dism_Maxent, loc_CEDTInfo);

			rtnstr += Generate_Marked_NE_Sentence_with_Erasing(segmentedstr_v[i].c_str(), loc_CEDTInfo.DismCase_v, pmSentNEInfo, SentSTART);
			loc_CEDTInfo.FREE();
		}
		else{
			rtnstr += segmentedstr_v[i];
		}
		SentSTART += NLPOP::Get_Chinese_Sentence_Length_Counter(segmentedstr_v[i].c_str());
	}
	return rtnstr;
}
string CCEDT::Generate_Marked_NE_Sentence_with_Erasing(const char* inputchar, vector<DismCase*>& pmDismCase_v, SentNEInfo& pmSentNEInfo, size_t migration)
{
	map<size_t, map<size_t, pair<string, double>>> rExtendRange_mm;
	map<size_t, map<size_t, pair<string, double>>> rHeadRange_mm;
	map<size_t, map<size_t, pair<string, double>>> Mention_mm;
	map<size_t, size_t> ExtendSTARTcnt_m;
	map<size_t, size_t> headSTARTcnt_m;
	for(vector<DismCase*>::iterator vite = pmDismCase_v.begin(); vite != pmDismCase_v.end(); vite++){
		DismCase& loc_Dism = **vite;
		CanditCase& CanditExtend = *loc_Dism.pExtend;
		CanditCase& CanditHead = *loc_Dism.pHead;

		if(ExtendSTARTcnt_m.find(CanditExtend.START) == ExtendSTARTcnt_m.end()){
			ExtendSTARTcnt_m.insert(make_pair(CanditExtend.START, 1));
		}
		else{
			ExtendSTARTcnt_m[CanditExtend.START]++;
		}
		if(headSTARTcnt_m.find(CanditHead.START) == headSTARTcnt_m.end()){
			headSTARTcnt_m.insert(make_pair(CanditHead.START, 1));
		}
		else{
			headSTARTcnt_m[CanditHead.START]++;
		}

		if(rExtendRange_mm.find(CanditExtend.END) == rExtendRange_mm.end()){
			rExtendRange_mm[CanditExtend.END];
		}
		if(rExtendRange_mm[CanditExtend.END].find(CanditExtend.START) == rExtendRange_mm[CanditExtend.END].end()){
			rExtendRange_mm[CanditExtend.END].insert(make_pair(CanditExtend.START, make_pair(CanditExtend.mention, CanditExtend.value)));
		}
		if(rHeadRange_mm.find(CanditHead.END) == rHeadRange_mm.end()){
			rHeadRange_mm[CanditHead.END];
			Mention_mm[CanditHead.END];
		}
		if(rHeadRange_mm[CanditHead.END].find(CanditHead.START) == rHeadRange_mm[CanditHead.END].end()){
			rHeadRange_mm[CanditHead.END].insert(make_pair(CanditHead.START, make_pair(CanditHead.mention, CanditHead.value)));
			Mention_mm[CanditHead.END].insert(make_pair(CanditHead.START, make_pair(loc_Dism.predit_TYPE, loc_Dism.TYPE_value)));
		}
		
		size_t migSTART = CanditExtend.START+migration;
		size_t migEND = CanditExtend.END+migration;
		if(pmSentNEInfo.Extend_mm.find(migSTART) == pmSentNEInfo.Extend_mm.end()){
			pmSentNEInfo.Extend_mm[migSTART];
		}
		if(pmSentNEInfo.Extend_mm[migSTART].find(migEND) == pmSentNEInfo.Extend_mm[migSTART].end()){
			pmSentNEInfo.Extend_mm[migSTART].insert(make_pair(migEND, make_pair(CanditExtend.mention, CanditExtend.value)));
		}
		migSTART = CanditHead.START+migration;
		migEND = CanditHead.END+migration;
		if(pmSentNEInfo.Head_mm.find(migSTART) == pmSentNEInfo.Head_mm.end()){
			pmSentNEInfo.Head_mm[migSTART];
			pmSentNEInfo.Mention_mm[migSTART];
		}
		if(pmSentNEInfo.Head_mm[migSTART].find(migEND) == pmSentNEInfo.Head_mm[migSTART].end()){
			pmSentNEInfo.Head_mm[migSTART].insert(make_pair(migEND, make_pair(CanditHead.mention, CanditHead.value)));
			pmSentNEInfo.Mention_mm[migSTART].insert(make_pair(CanditHead.START, make_pair(loc_Dism.predit_TYPE, loc_Dism.TYPE_value)));
		}
		delete *vite;
	}
	pmDismCase_v.clear();

	ostringstream ostrsteam;
	ostrsteam.precision(2);
	vector<string> character_v;
	NLPOP::charseq_to_vector(inputchar, character_v);
	for(size_t i = 0; i < character_v.size(); i++){
		if(NE_Extend_Flag){
			if(ExtendSTARTcnt_m.find(i) != ExtendSTARTcnt_m.end()){
				for(size_t j = 0; j < ExtendSTARTcnt_m[i]; j++){ 
					ostrsteam << "[";
				}
			}
		}
		if(NE_Head_Flag){
			if(headSTARTcnt_m.find(i) != headSTARTcnt_m.end()){
				for(size_t j = 0; j < headSTARTcnt_m[i]; j++){ 
					ostrsteam << "(";
				}
			}
		}
		ostrsteam << character_v[i];
		if(NE_Head_Flag){
			if(rHeadRange_mm.find(i) != rHeadRange_mm.end()){
				for(map<size_t,  pair<string, double>>::iterator mite = rHeadRange_mm[i].begin(); mite != rHeadRange_mm[i].end(); mite++){
					ostrsteam << "," << mite->second.second << ")";
				}
			}
		}
		if(!NE_Detection_Flag){
			if(Mention_mm.find(i) != Mention_mm.end()){
				map<size_t,  pair<string, double>>::iterator menite = Mention_mm[i].begin();
				for( ; menite != Mention_mm[i].end(); menite++){
					ostrsteam << ";<" << menite->second.first << "," << menite->second.second << ">";
				}
			}
		}
		if(NE_Extend_Flag){
			if(rExtendRange_mm.find(i) != rExtendRange_mm.end()){
				for(map<size_t,  pair<string, double>>::iterator mite = rExtendRange_mm[i].begin(); mite != rExtendRange_mm[i].end(); mite++){
					ostrsteam << ";" << mite->second.second << "]";
				}
			}
		}
	}
	return ostrsteam.str();
}


void CCEDT::Save_MaxentModel(const char* pTYPEchar, MaxentModel& pmaxent)
{
	string FilePath = Return_Model_Path(pTYPEchar);
	pmaxent.save(FilePath.c_str(), true);
}
bool CCEDT::Load_MaxentModel(const char* pTYPEchar, MaxentModel& pmaxent)
{
	string FilePath = Return_Model_Path(pTYPEchar);
	pmaxent.load(FilePath.c_str());
	return true;
}
string CCEDT::Return_Model_Path(const char* pTYPEchar)
{
	ostringstream ostrsteam;
	ostrsteam << m_Modelspace;
	ostrsteam << (CEDT_Head_Flag?"Head":"Extend");
	ostrsteam << "_";
	ostrsteam << (CEDT_Detection_Flag?"Detec":"Recog");
	ostrsteam << "_";
	ostrsteam << pTYPEchar;
	return ostrsteam.str();
}
bool CCEDT::Apply_for_Model_Training(bool EmptyData_Flag, const char* pTYPEchar)
{
	if(EmptyData_Flag){
		return true;
	}
	if(!Auto_Load_Model_Flag){
		return true;
	}
	string FilePath = Return_Model_Path(pTYPEchar);
	if(!NLPOP::Exist_of_This_File(FilePath)){
		return true;
	}

	return false;
}


void CCEDT::Reset_CEDT_Memories()
{
	m_TrainingCEDTInfo.FREE();
	m_TestingCEDTInfo.FREE();
	
	//------------------------------------------------------------------------
	for(size_t i = 0; i < training_Surround_v.size(); i++){ 
		delete training_Surround_v[i]; 
	} 
	training_Surround_v.clear();
	for(size_t i = 0; i < testing_Surround_v.size(); i++){ 
		delete testing_Surround_v[i]; 
	} 
	testing_Surround_v.clear();
}
