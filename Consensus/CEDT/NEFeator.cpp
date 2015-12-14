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

#include "StdAfx.h"
#include "NEFeator.h"
#include "NECom.h"
#include "maxen.h"

extern bool CEDT_Head_Flag;
extern bool CEDT_Detection_Flag;

CNEFeator::CNEFeator()
{
}
CNEFeator::~CNEFeator()
{
}
bool CNEFeator::Init_Named_Entity_Featrue_Extractor(const char* pmDataFolder)
{
	//------load for number merging;
	string LexiconPath = pmDataFolder;
	LexiconPath += "Number_Set.dat";
	NLPOP::LoadWordsStringSet(LexiconPath, Number_Set);

	LexiconPath = pmDataFolder;
	LexiconPath += "English_Set.dat";
	NLPOP::LoadWordsStringSet(LexiconPath, English_Set);
	
	LexiconPath = pmDataFolder;
	LexiconPath += "LCWCC.dat";
	NLPOP::LoadWordsStringSet(LexiconPath, lexicon_set);
	return true;
}
//==============================================
void CNEFeator::Generate_START_or_END_Cases_Feature(bool START_Flag, vector<BoundCase*>& START_BoundCase_v, FeatureVctor& Training_v)
{
	for(size_t i = 0; i < START_BoundCase_v.size(); i++){
		vector<string> adjacent_feature_v;
		
		BoundCase& loc_BoundCase = *START_BoundCase_v[i];

		Extract_Local_Feature_Vector_of_Boundary_with_nGram(loc_BoundCase, adjacent_feature_v, START_Flag, CEDT_Head_Flag?"_Hd":"_Exd");

		MAXEN::Push_Back_FeatureCase_v_from_Feature_v_with_Check(Training_v, adjacent_feature_v, loc_BoundCase.Bound_Falg?POSITIVE:NEGETIVE);
		
	}
}

void CNEFeator::Generate_Candit_Cases_Feature(vector<CanditCase*>& CanditCase_v, FeatureVctor& Training_v)
{
	for(size_t i = 0; i < CanditCase_v.size(); i++){
		vector<string> adjacent_feature_v;
		
		CanditCase& loc_Candit = *CanditCase_v[i];

		Extract_Feature_Vector_of_Candit(loc_Candit, adjacent_feature_v, CEDT_Head_Flag?"HD":"ED");

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

void CNEFeator::Extract_Local_Feature_Vector_of_Boundary_with_nGram(const BoundCase& pmBoundary, vector<string>& pmFeatureVector, bool START_Flag, string attach)
{
	string prixstr = pmBoundary.prixstr;
	string proxstr = pmBoundary.proxstr;

	size_t l_size = START_Flag?4:3;
	size_t r_size = START_Flag?3:4;
	
	string MentionTYPE = START_Flag?"S_L_nG":"E_L_nG";
	MentionTYPE += attach;
	Adjacent_Left_Border_n_Gram(prixstr.c_str(), l_size, pmFeatureVector, MentionTYPE);//3 maxmized

	MentionTYPE = START_Flag?"S_R_nG":"E_R_nG";
	MentionTYPE += attach;
	Adjacent_Right_Border_n_Gram(proxstr.c_str(), r_size, pmFeatureVector, MentionTYPE);

	
}

void CNEFeator::Combined_Feature_In_Candit_Two_Boundary(const CanditCase& pmCandit, vector<string>& pmFeatureVector, string attach)
{
	CanditCase locCandit = pmCandit;
	string MentionTYPE = attach;//Head or Extend

	vector<string> prix_v;
	vector<string> prox_v;
	NLPOP::charseq_to_vector(pmCandit.prix.c_str(), prix_v);
	NLPOP::charseq_to_vector(pmCandit.prox.c_str(), prox_v);

	string C_1, C_21, C1, C12;
	size_t index = 1;
	for(vector<string>::reverse_iterator rvite = prix_v.rbegin(); rvite != prix_v.rend(); rvite++, index++){
		if(index == 1){
			C_1 = rvite->data();
		}
		if(index == 2){
			C_21 = rvite->data() + C_1;
		}
	}

	for(size_t i = 0; i < prox_v.size(); i++){
		if(i == 0){
			C1 = prox_v[i];
		}
		if(i == 1){
			C12 = C1 + prox_v[i];
		}
	}

	char CBuff[56];
	ostringstream ostrsteam;
	ostrsteam << C_1 << "_" << C1 << "_bined_" << attach;
	pmFeatureVector.push_back(ostrsteam.str());
	strcpy_s(CBuff, 56, ostrsteam.str().c_str());

	ostrsteam.str("");
	ostrsteam << C_21 << "_" << C12 << "_bined_" << attach;

	if(strcmp(CBuff, ostrsteam.str().c_str())){
		pmFeatureVector.push_back(ostrsteam.str());
	}
	
}

void CNEFeator::Extract_Feature_Vector_of_Candit(const CanditCase& pmCandit, vector<string>& pmFeatureVector, string attach)
{
	CanditCase locCandit = pmCandit;
	string MentionTYPE = attach;//Head or Extend
	//START
	if(true){
		BoundCase loc_Boundary;
		loc_Boundary.SENID = pmCandit.SENID;

		loc_Boundary.position = pmCandit.START;
		loc_Boundary.prixstr = pmCandit.prix;
		loc_Boundary.proxstr = pmCandit.mention + pmCandit.prox;

		Extract_Local_Feature_Vector_of_Boundary_with_nGram(loc_Boundary, pmFeatureVector, true, MentionTYPE);

		//Adjacent_Left_Border_n_Gram(prixstr.c_str(), BOARD_n_GRAM_NUM, pmFeatureVector, "S_L_nG_"+MentionTYPE);
		//Adjacent_Right_Border_n_Gram(proxstr.c_str(), BOARD_n_GRAM_NUM, pmFeatureVector, "S_R_nG_"+MentionTYPE);

		loc_Boundary.position = pmCandit.END;
		loc_Boundary.prixstr = pmCandit.prix + pmCandit.mention;
		loc_Boundary.proxstr = pmCandit.prox;

		Extract_Local_Feature_Vector_of_Boundary_with_nGram(loc_Boundary, pmFeatureVector, false, MentionTYPE);

		//Adjacent_Left_Border_n_Gram(prixstr.c_str(), BOARD_n_GRAM_NUM, pmFeatureVector, "E_L_nG_"+MentionTYPE);
		//Adjacent_Right_Border_n_Gram(proxstr.c_str(), BOARD_n_GRAM_NUM, pmFeatureVector, "E_R_nG_"+MentionTYPE);
	}

	Internal_n_Gram(locCandit.mention.c_str(), 3, pmFeatureVector, "_InGram_"+MentionTYPE);
	Combined_Feature_In_Candit_Two_Boundary(locCandit, pmFeatureVector, MentionTYPE);

}

void CNEFeator::Extract_Feature_Vector_of_Deterimer(const DismCase& pmDismCase, vector<string>& pmFeatureVector)
{
	//DismCase loc_Dism = pmDismCase;
	//CEDT_ACE_Data_Preprocessing(loc_Dism); pmDismCase will be changed;
	//----Head boundary Feature
	if(pmDismCase.Head_Held_Flag){
		Extract_Feature_Vector_of_Candit(*pmDismCase.pHead, pmFeatureVector, "HD");
	}
	if(pmDismCase.Extend_Held_Flag){
		Extract_Feature_Vector_of_Candit(*pmDismCase.pExtend, pmFeatureVector, "ED");
	}
}



//----Feature Extraction Function--------------
void CNEFeator::Internal_n_Gram(const char* strchar, int Distance, vector<string>& outstr_v, string attach)
{
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);

	set<string> feature_s;
	char nGramBuff[58];
	for(size_t i = 0; i < char_v.size(); i++){
		size_t k = i +  Distance;
		strcpy_s(nGramBuff, 58, "");
		for(size_t j = i; (j < k) && (j < char_v.size()); j++){
			strcat_s(nGramBuff, 58, char_v[j].c_str());
			feature_s.insert(nGramBuff+attach);
		}
	}
	for(set<string>::iterator site = feature_s.begin(); site != feature_s.end(); site++){
		outstr_v.push_back(site->data());
	}
}

void CNEFeator::Adjacent_Left_Border_n_Gram(const char* strchar, int Distance, vector<string>& outstr_v, string attach)
{
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);
	int k = 0;
	string gramstr = "";
	for(vector<string>::reverse_iterator rvite = char_v.rbegin();  rvite != char_v.rend(); rvite++){
		gramstr = rvite->data() + gramstr;
		if(k++ < Distance){
			ostringstream ostrsteam;
			ostrsteam << gramstr << "_" << attach;
			outstr_v.push_back(ostrsteam.str());
		}
		else{
			break;
		}
	}
}
void CNEFeator::Adjacent_Right_Border_n_Gram(const char* strchar, int Distance, vector<string>& outstr_v, string attach)
{
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);
	int k = 0;
	string gramstr = "";
	for(int i = 0; i < (int)char_v.size(); i++){
		gramstr += char_v[i];
		if(k++ < Distance){
			ostringstream ostrsteam;
			ostrsteam << gramstr << "_" << attach;
			outstr_v.push_back(ostrsteam.str());
		}
		else{
			break;
		}
	}
}

