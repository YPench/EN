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
#include "CRDC.h"
#include "RCCom.h"
#include "ConvertUTF.h"

CRDC::CRDC()
{

	CRDC_Model_Loaded_Flag = false;
	Max_Word_Legnth = 0;
	m_Modelspace = DATA_FOLDER;
	m_Modelspace += "CRDC\\Model\\";
	if(NLPOP::FolderExist(NLPOP::string2CString(m_Modelspace))){
		//NLPOP::DeleteDir(NLPOP::string2CString(m_Modelspace));
		//_mkdir(m_Modelspace.c_str());
	}
	else{
		_mkdir(m_Modelspace.c_str());
	}
	m_CGRIns.pCRDC = this;
}
CRDC::~CRDC()
{
	for(size_t i = 0; i < Training_v.size(); i++){
		if(!(Training_v[i] != NULL)){
			delete Training_v[i];
		}
	}
	Training_v.clear();
}

void CRDC::Start_Relation_Training_Port(string RelationCaseFilePath, string TrainingCaseFilePath, string infopath)
{
	if(m_nGross == 0){
		CRDC_Training_Model_Port(RelationCaseFilePath, TrainingCaseFilePath, infopath);
		return;
	}
	if(!Training_v.empty()){
		for(size_t i = 0; i < Training_v.size(); i++){
			delete Training_v[i];
		}
		Training_v.clear();
	}
	if(Training_v.empty()){
		Training_v.reserve(300000);
		MAXEN::Read_CSmaxent_Training_Data(TrainingCaseFilePath.c_str(), TYPE_Flag?true:false, Training_v);
	}

	MAXEN::cross_validation(Training_v, m_nGross, m_IterTime, "lbfgs", 0, true);

}
void CRDC::CRDC_Training_Model_Port(string RelationCaseFilePath, string TrainingCaseFilePath, string infopath)
{
	if(CRDC_Model_Loaded_Flag){
		AppCall::Secretary_Message_Box("Model already loaded...", MB_OK);
		return;
	}
	if(Training_v.empty()){
		Training_v.reserve(300000);
		MAXEN::Read_CSmaxent_Training_Data(TrainingCaseFilePath.c_str(), TYPE_Flag?true:false, Training_v);
	}
	MAXEN::Maxen_Training_with_Erasing(Training_v, m_Maxen, m_IterTime);
	
	string modelpath = m_Modelspace;
	modelpath += "model";
	m_Maxen.save(modelpath.c_str(), true);
	CRDC_Model_Loaded_Flag = true;
}

void CRDC::Loading_CRDC_Model()
{
	if(CRDC_Model_Loaded_Flag){
		//AppCall::Secretary_Message_Box("Model already loaded...", MB_OK);
		return;
	}
	string modelfile = m_Modelspace;
	modelfile+= "model";
	if(!NLPOP::Exist_of_This_File(modelfile.c_str())){
		return;
	}
	m_Maxen.load(modelfile.c_str());

	string lexiconpath;
	if(LCWCC_Flag){
		lexiconpath = DATA_FOLDER;
		lexiconpath += "LCWCC.dat";
		if(!NLPOP::Exist_of_This_File(lexiconpath)){
			AppCall::Secretary_Message_Box("Lexicon of CVMC is not existing...", MB_OK);
		}
		NLPOP::LoadWordsStringSet(lexiconpath, m_WordsSet);
	}
	if(ICTCLAS_Flag){
		lexiconpath = DATA_FOLDER;
		lexiconpath += "CRDC\\ICTCLAS_Lexicon.dat";
		NLPOP::LoadWordsStringSet(lexiconpath, m_WordsSet);
	}
	Max_Word_Legnth = 0;
	for(set<string>::iterator site = m_WordsSet.begin();  site != m_WordsSet.end(); site++){
		if( site->size() > Max_Word_Legnth){
			Max_Word_Legnth = site->size();
		}
	}
	if(Max_Word_Legnth >= MAX_CLAUSE_LENGTH){
		AppCall::Secretary_Message_Box("Max size word is out of buffer size in:CRDC::Loading_CRDC_Model() ");
	}
	
	CRDC_Model_Loaded_Flag = true;
}



string CRDC::Relation_Case_Recognition_Port(Relation_Case& pmRelation_Case)
{
	vector<pair<string, float>> Features_v;

	Generate_CRDC_Feature_Vector_Port(pmRelation_Case, Features_v);

	pmRelation_Case.TYPE = m_Maxen.predict(Features_v);

	pmRelation_Case.TYPE_V = m_Maxen.eval(Features_v, pmRelation_Case.TYPE);

	return pmRelation_Case.TYPE;
}


string CRDC::Relation_Recognition_Port(const char* charInstance, const char* Arg_1, const char* Arg_2)
{
	vector<Relation_Case> Relation_Case_v;
	
	//RCCOM::Generating_Relation_Cases(charInstance, Arg_1, Arg_2, Relation_Case_v);
	
	FeatureVctor Recogizing_v;
	//m_CGREval.Generate_YPench_Evaluation_Port(Relation_Case_v, Recogizing_v);

	vector<pair<string,double>> rtnValue_v;

	MAXEN::Maxen_Recognizing_with_eval_Erasing(m_Maxen, Recogizing_v, rtnValue_v);

	ostringstream ostream;
	if(Relation_Case_v.size() != rtnValue_v.size()){
		AppCall::Secretary_Message_Box("Unmatched data in: CRDC::Relation_Recognition_Port()...", MB_OK);
	}
	for(size_t i = 0; i < Relation_Case_v.size(); i++){
		ostream << Relation_Case_v[i].first_entity.extent.charseq << "\t" << Relation_Case_v[i].sencond_entity.extent.charseq << ": ";
		ostream << rtnValue_v[i].first << "\t" << rtnValue_v[i].second << endl;
	}
	return ostream.str();
}

//-------------------------------<Generate Training Cases>
string CRDC::Generate_Training_Cases_Port(string RelationCaseFilePath, string TrainingCaseFilePath, string infopath)
{
	DulTYPE_Vctor DulTraining_v;
	if(!NLPOP::Exist_of_This_File(RelationCaseFilePath)){
		AppCall::Secretary_Message_Box("Relation Case text isn't exist!", MB_OK);
	}
	if(!DulTraining_v.empty()){
		for(size_t i = 0; i < DulTraining_v.size(); i++){
			delete DulTraining_v[i];
		}
		DulTraining_v.clear();
	}
	
	vector<Relation_Case*> Relation_Case_v;
	Relation_Case_v.reserve(300000);
	DulTraining_v.reserve(300000);
	ace_op::Load_Relation_Case(RelationCaseFilePath.c_str(), Relation_Case_v);

	Initiate_Relation_Det_Words_set(Relation_Case_v);

	Generate_YPench_Evaluation_Port(TrainingCaseFilePath.c_str(), Relation_Case_v, DulTraining_v);

	if(!DulTraining_v.empty()){
		MAXEN::Output_Extract_Feature_Context_For_Maxent(TrainingCaseFilePath.c_str(), DulTraining_v);
	}
	m_WordsSet.clear();
	for(size_t i = 0; i < Relation_Case_v.size(); i++){
		delete Relation_Case_v[i];
	}
	Relation_Case_v.clear();
	for(size_t i = 0; i < DulTraining_v.size(); i++){
		delete DulTraining_v[i];
	}
	DulTraining_v.clear();
	return "";
}


//-------------------------------<Generate Realtion Cases> 
void CRDC::Generate_Relation_Case_Port(string savepath, ACE_Corpus& m_ACE_Corpus)
{
	m_CGRIns.Generate_Positive_And_Negetive_ACE_Relation_Cases(savepath, m_ACE_Corpus);
}

void CRDC::Initiate_Relation_Det_Words_set(vector<Relation_Case*>& Relation_Case_v)
{
	CSegmter m_CSegmter;
	string lexiconpath;


	if(false){//(LCWCC_Flag){
		lexiconpath = DATA_FOLDER;
		lexiconpath += "default.dat";
		if(!NLPOP::Exist_of_This_File(lexiconpath)){
			AppCall::Secretary_Message_Box("Lexicon of default is not existing...", MB_OK);
		}
		NLPOP::LoadWordsStringSet(lexiconpath, m_WordsSet);//62,036
	}

	if(true){//(LCWCC_Flag){
		lexiconpath = DATA_FOLDER;
		lexiconpath += "LCWCC.dat";
		if(!NLPOP::Exist_of_This_File(lexiconpath)){
			AppCall::Secretary_Message_Box("Lexicon of CVMC is not existing...", MB_OK);
		}
		NLPOP::LoadWordsStringSet(lexiconpath, m_WordsSet);
	}
	
	if(true){//(ICTCLAS_Flag){
		lexiconpath = DATA_FOLDER;
		lexiconpath += "CRDC\\ICTCLAS_Lexicon.dat";
		if(!NLPOP::Exist_of_This_File(lexiconpath)){
			AppCall::Secretary_Message_Box("ICTCLAS_Lexicon.dat is not exist, A new one will be created...", MB_OK);
			for(vector<Relation_Case*>::iterator rvite = Relation_Case_v.begin(); rvite != Relation_Case_v.end(); rvite++){
				string relation_mention_str = ace_op::Delet_0AH_and_20H_in_string((*rvite)->relatin_mention.extent.charseq.c_str());
				CLAUSEPOS loc_Seg;
				m_CSegmter.ICTCLAS_Segmention_Port(relation_mention_str.c_str(), loc_Seg);
				for(vector<string>::iterator vite = loc_Seg.Clause.begin(); vite != loc_Seg.Clause.end(); vite++){
					m_WordsSet.insert(vite->data());
				}
			}
		}
		else{
			NLPOP::LoadWordsStringSet(lexiconpath, m_WordsSet);
		}
	}
	if(false){//(Internet_Flag){
		lexiconpath = DATA_FOLDER;
		lexiconpath += "Worddb_set.dat";
		if(!NLPOP::Exist_of_This_File(lexiconpath)){
			AppCall::Secretary_Message_Box("Lexicon from Internet is not existing...", MB_OK);
		}
		NLPOP::LoadWordsStringSet(lexiconpath, m_WordsSet);
	}
	if(false){//(Internet_Flag){
		lexiconpath = DATA_FOLDER;
		lexiconpath += "CSWordsSet.dat";
		if(!NLPOP::Exist_of_This_File(lexiconpath)){
			AppCall::Secretary_Message_Box("Lexicon from Internet is not existing...", MB_OK);
		}
		NLPOP::LoadWordsStringSet(lexiconpath, m_WordsSet);
	}
	Max_Word_Legnth = 0;
	for(set<string>::iterator site = m_WordsSet.begin();  site != m_WordsSet.end(); site++){
		if( site->size() > Max_Word_Legnth){
			Max_Word_Legnth = site->size();
		}
	}
}

//-------------------------------Generate Training and Testing Relation Cases
void CRDC::Generate_CRDC_Feature_Vector_Port(Relation_Case& pmRelation_Case, vector<pair<string, float>>& Features_v)
{
	ostringstream ostrsteam;
	Relation_Case& loc_Case = pmRelation_Case;
	RelationContext loc_Context;
	ACE_entity_mention& Ref_E1 = loc_Case.first_entity;
	ACE_entity_mention& Ref_E2 = loc_Case.sencond_entity;
	ACE_relation_mention& Ref_relationmention = loc_Case.relatin_mention;
	map<string, float> WordsCnt_map;
		
	Sentop::Extract_ACE_entities_Pair_Context(loc_Case, loc_Context);
	RCCOM::Delet_0AH_and_20H_in_Relation_Case(loc_Case);
	RCCOM::Delet_0AH_and_20H_in_RelationContext(loc_Context);

	//----Subtype and TYPE of entity pair;
	if(true){//(Enitity_TYPE_Flag){
		//------Multiplication Constraint   \mathcal{F}_{(E\_M)}
		ostrsteam.str("");
		ostrsteam << Ref_E1.Entity_TYPE << "_" << Ref_E2.Entity_TYPE;
		WordsCnt_map.insert(make_pair(ostrsteam.str(), (float)1));
		if(false){
			ostrsteam.str("");
			ostrsteam << Ref_E1.Entity_SUBSTYPE << "_" << Ref_E2.Entity_SUBSTYPE;
			WordsCnt_map.insert(make_pair(ostrsteam.str(), (float)1));
		}
	}
	//-----Entities Structure;
	if(true){//(EntitiesStructure_Flag){
		//------Singleton-------------------------\mathcal{F}_{(S\_S)}
		WordsCnt_map.insert(make_pair(RCCOM::Get_Entity_Positional_Structures(loc_Case), (float)1));
	}
	//-----head noun;
	if(true){//(HeadNoun_Flag){	
		//------Position	\mathcal{F}_{(H\_P)}
		WordsCnt_map.insert(make_pair(Ref_E1.head.charseq+"_E1", (float)1));	
		WordsCnt_map.insert(make_pair(Ref_E2.head.charseq+"_E2", (float)1));

	}
	//-----dual types constriant;
	if(true){//(POS_Tag_Flag){
		//------Multiply Entity TYPE and Position
		RCCOM::Adjacent_Words_POS_Feature_Extracting(m_CSegmter, loc_Context.L_str.c_str(), WordsCnt_map, "", "_"+Ref_E1.Entity_TYPE+"_E1");
		RCCOM::Adjacent_Words_POS_Feature_Extracting(m_CSegmter, loc_Context.M_str.c_str(), WordsCnt_map, "E1_"+Ref_E1.Entity_TYPE+"_", "_"+Ref_E2.Entity_TYPE+"_E2");
		RCCOM::Adjacent_Words_POS_Feature_Extracting(m_CSegmter, loc_Context.R_str.c_str(), WordsCnt_map, "E2_"+Ref_E2.Entity_TYPE+"_", "");
	}

	//-----Omni-words feature;
	if(true){//(pCRDC->OmniWords_Flag){
		//------Bin
		m_CSegmter.Omni_words_feature_Extracting(Ref_E1.extent.charseq.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "F_", "");
		m_CSegmter.Omni_words_feature_Extracting(Ref_E2.extent.charseq.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "S_", "");
		m_CSegmter.Omni_words_feature_Extracting(loc_Context.L_str.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "L_", "");
		m_CSegmter.Omni_words_feature_Extracting(loc_Context.M_str.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "M_", "");
		m_CSegmter.Omni_words_feature_Extracting(loc_Context.R_str.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "R_", "");
	}
	for(map<string, float>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
		Features_v.push_back(make_pair(mite->first, mite->second));
	}
}



void CRDC::Generate_YPench_Evaluation_Port(const char * savepath, vector<Relation_Case*>& Relation_Case_v, DulTYPE_Vctor& pmTraining_v)
{
	map<string, size_t> PositionStructure_m;
	for(vector<Relation_Case*>::iterator vite = Relation_Case_v.begin(); vite != Relation_Case_v.end(); vite++){
		ostringstream ostrsteam;
		Relation_Case& loc_Case = **vite;
		RelationContext loc_Context;
		ACE_entity_mention& Ref_E1 = loc_Case.first_entity;
		ACE_entity_mention& Ref_E2 = loc_Case.sencond_entity;
		ACE_relation_mention& Ref_relationmention = loc_Case.relatin_mention;
		map<string, float> WordsCnt_map;
		
		Sentop::Extract_ACE_entities_Pair_Context(loc_Case, loc_Context);
		RCCOM::Delet_0AH_and_20H_in_Relation_Case(loc_Case);
		RCCOM::Delet_0AH_and_20H_in_RelationContext(loc_Context);

		if(true){//(Enitity_TYPE_Flag){
			WordsCnt_map.insert(make_pair(Ref_E1.Entity_TYPE, (float)1));
			WordsCnt_map.insert(make_pair(Ref_E2.Entity_TYPE, (float)1));
			WordsCnt_map.insert(make_pair(Ref_E1.Entity_SUBSTYPE, (float)1));
			WordsCnt_map.insert(make_pair(Ref_E2.Entity_SUBSTYPE, (float)1));
		}
		//-----head noun;
		if(true){//(HeadNoun_Flag){		
			//------Singleton---------------\mathcal{F}_{(H\_S)}
			WordsCnt_map.insert(make_pair(Ref_E1.head.charseq, (float)1));
			WordsCnt_map.insert(make_pair(Ref_E2.head.charseq, (float)1));
		}
		//-----Entities Structure;
		if(true){//(EntitiesStructure_Flag){
			//------Singleton-------------------------\mathcal{F}_{(S\_S)}
			WordsCnt_map.insert(make_pair(RCCOM::Get_Entity_Positional_Structures(loc_Case), (float)1));

			string locstr = RCCOM::Get_Entity_Positional_Structures(**vite);
			if(PositionStructure_m.find(locstr) == PositionStructure_m.end()){
				PositionStructure_m.insert(make_pair(locstr, 0));
			}
			PositionStructure_m[locstr]++;
		}
		//-----dual types constriant;
		if(true){//(POS_Tag_Flag){
			//------Singleton   \mathcal{F}_{(P\_S)}
			RCCOM::Local_Adjacent_Words_Singleton_POS_Feature_Extracting(m_CSegmter, loc_Context.L_str.c_str(), WordsCnt_map, "", "_");
			RCCOM::Local_Adjacent_Words_Singleton_POS_Feature_Extracting(m_CSegmter, loc_Context.M_str.c_str(), WordsCnt_map, "_", "_");
			RCCOM::Local_Adjacent_Words_Singleton_POS_Feature_Extracting(m_CSegmter, loc_Context.R_str.c_str(), WordsCnt_map, "_", "");
		}
		//-----Omni-words feature;
		if(true){//(OmniWords_Flag){
			m_CSegmter.Omni_words_feature_Extracting(Ref_E1.extent.charseq.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "", "");
			m_CSegmter.Omni_words_feature_Extracting(Ref_E2.extent.charseq.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "", "");
			m_CSegmter.Omni_words_feature_Extracting(loc_Context.L_str.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "", "");
			m_CSegmter.Omni_words_feature_Extracting(loc_Context.M_str.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "", "");
			m_CSegmter.Omni_words_feature_Extracting(loc_Context.R_str.c_str(), m_WordsSet, Max_Word_Legnth, WordsCnt_map, "", "");
		}
		if(false){//(Segmentation_Flag){
			//m_CSegmter.ICTCLAS_Segmentation_words_feature_Extracting(ace_op::Delet_0AH_and_20H_in_string(loc_Case.relatin_mention.extent.charseq.c_str()).c_str(), WordsCnt_map, "", "");
			m_CSegmter.ICTCLAS_Segmentation_words_feature_Extracting(Ref_E1.extent.charseq.c_str(), WordsCnt_map, "Fs_", "");
			m_CSegmter.ICTCLAS_Segmentation_words_feature_Extracting(Ref_E2.extent.charseq.c_str(), WordsCnt_map, "Ss_", "");
			m_CSegmter.ICTCLAS_Segmentation_words_feature_Extracting(loc_Context.L_str.c_str(), WordsCnt_map, "Ls_", "");
			m_CSegmter.ICTCLAS_Segmentation_words_feature_Extracting(loc_Context.M_str.c_str(), WordsCnt_map, "Ms_", "");
			m_CSegmter.ICTCLAS_Segmentation_words_feature_Extracting(loc_Context.R_str.c_str(), WordsCnt_map, "Rs_", "");
		}
		if(false){
			size_t nGram = 1;
			//------Bin
			N_Gram_of_Sent_Sequence(Ref_E1.extent.charseq.c_str(), nGram, WordsCnt_map, "F_", "");
			N_Gram_of_Sent_Sequence(Ref_E2.extent.charseq.c_str(), nGram, WordsCnt_map, "S_", "");
			N_Gram_of_Sent_Sequence(loc_Context.L_str.c_str(), nGram, WordsCnt_map, "L_", "");
			N_Gram_of_Sent_Sequence(loc_Context.M_str.c_str(), nGram, WordsCnt_map, "M_", "");
			N_Gram_of_Sent_Sequence(loc_Context.R_str.c_str(), nGram, WordsCnt_map, "R_", "");
		}
		if(false){//(EntityCLASS_Flag){
			WordsCnt_map.insert(make_pair(Ref_E1.Entity_CLASS, (float)1));
			WordsCnt_map.insert(make_pair(Ref_E2.Entity_CLASS, (float)1));
			WordsCnt_map.insert(make_pair(Ref_E1.LDCTYPE, (float)1));
			WordsCnt_map.insert(make_pair(Ref_E2.LDCTYPE, (float)1));

		}
		if(WordsCnt_map.empty()){
			continue;
		}
		
		pair<pair<string, string>, vector<pair<string, float>>>* pCRDC_Case_v = new pair<pair<string, string>, vector<pair<string, float>>>;
		pCRDC_Case_v->first.first  = loc_Case.TYPE;
		pCRDC_Case_v->first.second = loc_Case.SUBTYPE;
		for(map<string, float>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
			pCRDC_Case_v->second.push_back(make_pair(mite->first, mite->second));
		}
		pmTraining_v.push_back(pCRDC_Case_v);

		delete *vite;
	}

	Relation_Case_v.clear();
}




void CRDC::N_Gram_of_Sent_Sequence(const char* sentchar, size_t range, map<string, float>& WordsCnt_map, string prix = "", string prox = "")
{
	//map<string, float> loc_WordsCnt_map;
	ostringstream ostrsteam;
	vector<string> locchar_v;
	NLPOP::charseq_to_vector(sentchar, locchar_v);
	size_t Counter = 0;
	for(size_t i = 0; i < locchar_v.size(); i++){
		size_t limit = i + range;
		if(limit > locchar_v.size()){
			limit = locchar_v.size();
		}
		string str;
		for(size_t j = i; j < limit; j++){
			str += locchar_v[j];
			ostrsteam.str("");
			ostrsteam << prix << str << prox;
			WordsCnt_map.insert(make_pair(ostrsteam.str(), (float)1));
			//loc_WordsCnt_map.insert(make_pair(ostrsteam.str(), (float)1));
		}
	}

}