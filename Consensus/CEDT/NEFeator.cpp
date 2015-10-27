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

extern double FEEDBACE_PRO_LIMIT;
extern int CAND_FEEDBACK_NUM;
extern int BOARD_n_GRAM_NUM;
extern int INTERNAL_WORDS_NUM;
extern int BOARD_POS_NUM;

extern bool CEDT_Candit_POS_Flag;
extern bool CEDT_Segmenter_Candit_Internal_Words_Flag;
extern bool CEDT_MM_Feature_Flag;
extern bool CEDT_Segmenter_Candit_External_Words_Flag;
extern bool CEDT_Candit_nG_Featrue_Flag;
extern int BOARD_WORDS_NUM;
extern bool CEDT_Internal_Sequence_Words_Flag;

extern bool CEDT_Word_Boundary_Flag;

CNEFeator::CNEFeator()//(CSegmter& rCSegmter, CMaxmatch& rCMaxmatch, set<string>& rlexicon_set): m_CSegmter(rCSegmter) //(set<string>& lexicon_set): lexicon_set(lexicon_set)
//, m_CMaxmatch(rCMaxmatch)
//, lexicon_set(rlexicon_set)
{
	Catched_Sent_Segment_Info_Flag = false;
	Catched_Sent_ID = -1;
}
CNEFeator::~CNEFeator()
{
}
bool CNEFeator::Init_Named_Entity_Featrue_Extractor(const char* pmDataFolder)
{
	Number_Set.clear();
	English_Set.clear();
	lexicon_set.clear();

	CString DataFolder(pmDataFolder);
	if(!NLPOP::FolderExist(DataFolder)){
		AppCall::Secretary_Message_Box("Data Folder should be given...", MB_OK);
	}
	//------load for number merging;
	string LexiconPath = pmDataFolder;
	LexiconPath += "Number_Set.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(LexiconPath))){
		AppCall::Secretary_Message_Box(LexiconPath.c_str(), MB_OK);
	}
	NLPOP::LoadWordsStringSet(LexiconPath, Number_Set);

	LexiconPath = pmDataFolder;
	LexiconPath += "English_Set.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(LexiconPath))){
		AppCall::Secretary_Message_Box(LexiconPath.c_str(), MB_OK);
	}
	NLPOP::LoadWordsStringSet(LexiconPath, English_Set);
	
	LexiconPath = pmDataFolder;
	LexiconPath += "LCWCC.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(LexiconPath))){
		AppCall::Secretary_Message_Box(LexiconPath.c_str(), MB_OK);
	}
	NLPOP::LoadWordsStringSet(LexiconPath, lexicon_set);

	return true;
}
void CNEFeator::Adding_Entity_Extent_and_Head_to_Lexicon(vector<NE_Surround*>& Surround_v, char TYPE)
{
	string loc_str;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		for(vector<ACE_entity_mention>::iterator vite = (*ne_vite)->entity_mention_v.begin(); vite != (*ne_vite)->entity_mention_v.end(); vite++){
			if(TYPE == 'E' || TYPE == 'A'){
				loc_str = NECOM::Pretreatment_Sent_For_Named_Entity(vite->extent.charseq.c_str(), Number_Set, English_Set);
				lexicon_set.insert(loc_str);
			}
			if(TYPE == 'H' || TYPE == 'A'){
				loc_str =  NECOM::Pretreatment_Sent_For_Named_Entity(vite->head.charseq.c_str(), Number_Set, English_Set);
				lexicon_set.insert(loc_str);
			}
		}
	}
}

void CNEFeator::Adjacent_Segmentation_Info_on_Boundary(size_t SentID, string originstr, size_t position, bool START_Flag, vector<string>& outstr_v, string attach)
{
	string inputstr =  ace_op::Delet_0AH_and_20H_in_string(originstr.c_str());
	vector<string> SegRtn_v;

	if(!Catched_Sent_Segment_Info_Flag || (Catched_Sent_ID != SentID)){
		Segment_Info_v.clear();
		m_CMaxmatch.Maximum_Match_Segmentation_Traditional(inputstr.c_str(), SegRtn_v, "FMM", lexicon_set);
		NECOM::Generate_Word_Boundary_Info(originstr.c_str(), SegRtn_v, Segment_Info_v);
		Catched_Sent_Segment_Info_Flag = true;
		Catched_Sent_ID = SentID;
	}

	if(NLPOP::Get_Chinese_Sentence_Length_Counter(originstr.c_str()) != Segment_Info_v.size()){
		AppCall::Secretary_Message_Box("Unmatched Data in: CNEFeator::Adjacent_Segmentation_Info_on_Boundary()", MB_OK);
	}

	vector<string> character_v;
	NLPOP::charseq_to_vector(originstr.c_str(), character_v);
	string teststr;

	teststr = Segment_Info_v[position]+"_"+attach;
	outstr_v.push_back(Segment_Info_v[position]+"_0_"+attach);// The current character segmention info

	int i = position;
	if(START_Flag && position != 0){
		i--;
		while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			i--;
			if(i<0){
				break;
			}
		}
		if(!(i<0)){
			teststr = Segment_Info_v[i]+"_"+attach;
			outstr_v.push_back(Segment_Info_v[i]+"_1_"+attach);// The Left character segmention info
		}
	}
	else if(!START_Flag && (position < character_v.size())){
		i++;
		if(i< (int)character_v.size()){
			while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				i++;
				if(!(i<(int)character_v.size())){
					break;
				}
			}
			if(i<(int)character_v.size()){
				teststr = Segment_Info_v[i]+"_"+attach;
				outstr_v.push_back(Segment_Info_v[i]+"_1_"+attach);// The right character segmention info
			}
		}
	}

}

void CNEFeator::Local_Segmentation_Info_on_Boundary(size_t SentID, string originstr, size_t position, bool START_Flag, vector<string>& outstr_v, string attach)
{
	string inputstr =  ace_op::Delet_0AH_and_20H_in_string(originstr.c_str());
	vector<string> SegRtn_v;

	if(!Catched_Sent_Segment_Info_Flag || (Catched_Sent_ID != SentID)){
		Segment_Info_v.clear();
		m_CMaxmatch.Maximum_Match_Segmentation_Traditional(inputstr.c_str(), SegRtn_v, "FMM", lexicon_set);
		NECOM::Generate_Word_Boundary_Info(originstr.c_str(), SegRtn_v, Segment_Info_v);
		Catched_Sent_Segment_Info_Flag = true;
		Catched_Sent_ID = SentID;
	}

	if(NLPOP::Get_Chinese_Sentence_Length_Counter(originstr.c_str()) != Segment_Info_v.size()){
		AppCall::Secretary_Message_Box("Unmatched Data in: CNEFeator::Adjacent_Segmentation_Info_on_Boundary()", MB_OK);
	}

	vector<string> character_v;
	NLPOP::charseq_to_vector(originstr.c_str(), character_v);
	string teststr;

	int i = position;
	if(position != 0){
		i--;
		while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			i--;
			if(i<0){
				break;
			}
		}
		if(!(i<0)){
			if(Segment_Info_v[i] == "wE"){
				outstr_v.push_back("L_1"+attach);
			}
			else{
				outstr_v.push_back("L_0"+attach);
			}
			
		}
		else{
			outstr_v.push_back("L_0"+attach);
		}
	}
	i = position;
	if(i < (int)character_v.size()){
		i++;
		if(i< (int)character_v.size()){
			while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				i++;
				if(!(i<(int)character_v.size())){
					break;
				}
			}
			if(i<(int)character_v.size()){
				if(Segment_Info_v[i] == "wB"){
					outstr_v.push_back("R_1"+attach);
				}
				else{
					outstr_v.push_back("R_0"+attach);
				}
			}
			else{
				outstr_v.push_back("R_0"+attach);
			}
		}
	}

}
void CNEFeator::Extract_Baseline_Feature_Vector_of_Boundary(const BoundCase& pmBoundary, vector<string>& pmFeatureVector, bool START_Flag, string attach)
{

	string prixstr = pmBoundary.prixstr;
	string proxstr = pmBoundary.proxstr;
	NECOM::Pretreatment_Sent_For_Named_Entity(prixstr, Number_Set, English_Set);
	NECOM::Pretreatment_Sent_For_Named_Entity(proxstr, Number_Set, English_Set);

	vector<string> prix_v;
	vector<string> prox_v;
	NLPOP::charseq_to_vector(prixstr.c_str(), prix_v);
	NLPOP::charseq_to_vector(proxstr.c_str(), prox_v);

	string C_2, C_1, C10, C0, C01, C1, C2;

	if(START_Flag){
		size_t i;
		for(i = 0; (i < prox_v.size()) && (i < 3); i++){
			if(i == 0){
				C0 = prox_v[i];
				C01 = prox_v[i];
			}
			if(i == 1){
				C01 += prox_v[i];
				C1 = prox_v[i];
			}
			if( i == 2){
				C2 = prox_v[i];
			}
		}
		i = 0;
		for(vector<string>::reverse_iterator rvite = prix_v.rbegin(); (rvite != prix_v.rend()) && (i < 2); rvite++, i++){
			if(i == 0){
				C_1 = rvite->data();
				C10 = C_1 + C0;
			}
			if(i == 1){
				C_2 = rvite->data();
			}
		}
	}

	if(!START_Flag){
		size_t i = 0;
		for(vector<string>::reverse_iterator rvite = prix_v.rbegin(); (rvite != prix_v.rend()) && (i < 3); rvite++, i++){
			if(i == 0){
				C0 = rvite->data();
			}
			if(i == 1){
				C10 = rvite->data() + C0;
				C_1 = rvite->data();
			}
			if(i == 2){
				C_2 = rvite->data();
			}
		}
		for(i = 0; (i < prox_v.size()) && (i < 2); i++){
			if(i == 0){
				C1 = prox_v[i];
				C01 = C0 + prox_v[i];
			}
			if(i == 1){
				C2 = prox_v[i];
			}
		}	
	}

	C_2, C_1, C10, C0, C01, C1, C2;

	string MentionTYPE = attach;
	MentionTYPE += START_Flag?"_S":"_E";

	pmFeatureVector.push_back(C_2+"_nG_L_2"+MentionTYPE);
	pmFeatureVector.push_back(C_1+"_nG_L_1"+MentionTYPE);
	pmFeatureVector.push_back(C10+"_nG_10"+MentionTYPE);
	pmFeatureVector.push_back( C0+"_nG_0"+MentionTYPE);
	pmFeatureVector.push_back(C01+"_nG_01"+MentionTYPE);
	pmFeatureVector.push_back( C1+"_nG_R_1"+MentionTYPE);
	pmFeatureVector.push_back( C2+"_nG_R_2"+MentionTYPE);

	//---Is_Sur(C_0)-------------------------------------------------------1
	if(Surname_s.find(C0) != Surname_s.end()){
		pmFeatureVector.push_back("Sur_1"+MentionTYPE);
	}
	else{
		pmFeatureVector.push_back("Sur_0"+MentionTYPE);
	}
	//---Is_Pro(C_0)-------------------------------------------------------2
	if(Pronoun_s.find(C0) != Pronoun_s.end()){
		pmFeatureVector.push_back("Pro_1"+MentionTYPE);
	}
	else{
		pmFeatureVector.push_back("Pro_0"+MentionTYPE);
	}
	//---Is_Pro(C_10)-------------------------------------------------------3
	if(Pronoun_s.find(C10) != Pronoun_s.end()){
		pmFeatureVector.push_back("Pro_1_C10"+MentionTYPE);
	}
	else{
		pmFeatureVector.push_back("Pro_0_C10"+MentionTYPE);
	}
	//---Is_Pro(C_01)-------------------------------------------------------4
	if(Pronoun_s.find(C01) != Pronoun_s.end()){
		pmFeatureVector.push_back("Pro_1_C01"+MentionTYPE);
	}
	else{
		pmFeatureVector.push_back("Pro_0_C01"+MentionTYPE);
	}
	//---In_Loc(C_10)-------------------------------------------------------5
	if(Locname_s.find(C10) != Locname_s.end()){
		pmFeatureVector.push_back("Loc_1_C10"+MentionTYPE);
	}
	else{
		pmFeatureVector.push_back("Loc_0_C10"+MentionTYPE);
	}
	//---In_Loc(C_01)-------------------------------------------------------6
	if(Locname_s.find(C01) != Locname_s.end()){
		pmFeatureVector.push_back("Loc_1_C01"+MentionTYPE);
	}
	else{
		pmFeatureVector.push_back("Loc_0_C01"+MentionTYPE);
	}
	MentionTYPE = attach;
	MentionTYPE += START_Flag?"_si_S":"_si_E";
	
	if(false){
		Local_Segmentation_Info_on_Boundary(pmBoundary.SENID, pmBoundary.prixstr+pmBoundary.proxstr, pmBoundary.position, START_Flag, pmFeatureVector, MentionTYPE);
	}
}

void CNEFeator::Extract_Local_Feature_Vector_of_Boundary_with_nGram(const BoundCase& pmBoundary, vector<string>& pmFeatureVector, bool START_Flag, string attach)
{
	string prixstr = pmBoundary.prixstr;
	string proxstr = pmBoundary.proxstr;
	NECOM::Pretreatment_Sent_For_Named_Entity(prixstr, Number_Set, English_Set);
	NECOM::Pretreatment_Sent_For_Named_Entity(proxstr, Number_Set, English_Set);

	size_t l_size = START_Flag?4:3;
	size_t r_size = START_Flag?3:4;
	
	string MentionTYPE = START_Flag?"S_L_nG":"E_L_nG";
	MentionTYPE += attach;
	Adjacent_Left_Border_n_Gram(prixstr.c_str(), l_size, pmFeatureVector, MentionTYPE);//3 maxmized

	MentionTYPE = START_Flag?"S_R_nG":"E_R_nG";
	MentionTYPE += attach;
	Adjacent_Right_Border_n_Gram(proxstr.c_str(), r_size, pmFeatureVector, MentionTYPE);

	if(CEDT_Word_Boundary_Flag){
		//Adjacent_Segmentation_Info_on_Boundary(pmBoundary.SENID, pmBoundary.prixstr+pmBoundary.proxstr, pmBoundary.position, START_Flag, pmFeatureVector, START_Flag?"S_si":"E_si");
	}

	/*Adjacent_Left_Maxmatch_Overlapping_Words(locBoundCase.prixstr.c_str(), START_Flag, -1, pmFeatureVector);
	Adjacent_Right_Maxmatch_Overlapping_Words(locBoundCase.proxstr.c_str(), START_Flag, -1, pmFeatureVector);
	Left_Max_Match_Segmentor(locBoundCase.prixstr.c_str(), START_Flag, 3, pmFeatureVector);
	Right_Max_Match_Segmentor(locBoundCase.proxstr.c_str(), START_Flag, 3, pmFeatureVector);
	if(START_Flag){
		Adjacent_Left_Maxmatch_Overlapping_Words(NE_FP_Set, locBoundCase.prixstr.c_str(), START_Flag, -1, pmFeatureVector);
		Adjacent_Right_Maxmatch_Overlapping_Words(NE_FP_Set, locBoundCase.proxstr.c_str(), START_Flag, -1, pmFeatureVector);
	}
	else{
		Adjacent_Left_Maxmatch_Overlapping_Words(NE_FP_Set, locBoundCase.prixstr.c_str(), START_Flag, -1, pmFeatureVector);
		Adjacent_Right_Maxmatch_Overlapping_Words(NE_FP_Set, locBoundCase.proxstr.c_str(), START_Flag, -1, pmFeatureVector);
	}
	Adjacent_Left_Second_Maxmatch_Overlapping_Words(false, lexicon_set, locBoundCase.prixstr.c_str(), START_Flag, -1, pmFeatureVector);
	Adjacent_Right_Second_Maxmatch_Overlapping_Words(false, lexicon_set, locBoundCase.proxstr.c_str(), START_Flag, -1, pmFeatureVector);
	Adjacent_Left_Border_Characters(locBoundCase.prixstr.c_str(), START_Flag, 4, pmFeatureVector);
	Adjacent_Right_Border_Characters(locBoundCase.proxstr.c_str(), START_Flag, 4, pmFeatureVector);
	vector<string> loc_Feature;
	if(m_CRegex.FreqPattern_Initilized_Flag){
		m_CRegex.Regex_Pattern_Dual_Match_Poart(locBoundCase.prixstr.c_str(), locBoundCase.proxstr.c_str(), loc_Feature);
		for(vector<string>::iterator vite = loc_Feature.begin(); vite != loc_Feature.end(); vite++){
			pmFeatureVector.push_back(*vite+"_Bx");
		}
		loc_Feature.clear();
	}
	*/

}

void CNEFeator::Extract_Feature_Vector_of_Boundary(const BoundCase& pmBoundary, vector<string>& pmFeatureVector, bool START_Flag, string attach)
{

	Extract_Baseline_Feature_Vector_of_Boundary(pmBoundary, pmFeatureVector, START_Flag, attach);

	/*Extract_Local_Feature_Vector_of_Boundary_with_nGram(pmBoundary, pmFeatureVector, START_Flag, attach);


	string prixstr = pmBoundary.prixstr;
	string proxstr = pmBoundary.proxstr;
	NECOM::Pretreatment_Sent_For_Named_Entity(prixstr, Number_Set, English_Set);
	NECOM::Pretreatment_Sent_For_Named_Entity(proxstr, Number_Set, English_Set);

	vector<string> prix_v;
	vector<string> prox_v;
	NLPOP::charseq_to_vector(prixstr.c_str(), prix_v);
	NLPOP::charseq_to_vector(proxstr.c_str(), prox_v);

	string C_2, C_1, C10, C0, C01, C1, C2;

	if(START_Flag){
		size_t i;
		for(i = 0; (i < prox_v.size()) && (i < 3); i++){
			if(i == 0){
				C0 = prox_v[i];
				C01 = prox_v[i];
			}
			if(i == 1){
				C01 += prox_v[i];
				C1 = prox_v[i];
			}
			if( i == 2){
				C2 = prox_v[i];
			}
		}
		i = 0;
		for(vector<string>::reverse_iterator rvite = prix_v.rbegin(); (rvite != prix_v.rend()) && (i < 2); rvite++, i++){
			if(i == 0){
				C_1 = rvite->data();
				C10 = C_1 + C0;
			}
			if(i == 1){
				C_2 = rvite->data();
			}
		}
	}

	if(!START_Flag){
		size_t i = 0;
		for(vector<string>::reverse_iterator rvite = prix_v.rbegin(); (rvite != prix_v.rend()) && (i < 3); rvite++, i++){
			if(i == 0){
				C0 = rvite->data();
			}
			if(i == 1){
				C10 = rvite->data() + C0;
				C_1 = rvite->data();
			}
			if(i == 2){
				C_2 = rvite->data();
			}
		}
		for(i = 0; (i < prox_v.size()) && (i < 2); i++){
			if(i == 0){
				C1 = prox_v[i];
				C01 = C0 + prox_v[i];
			}
			if(i == 1){
				C2 = prox_v[i];
			}
		}	
	}

	C_2, C_1, C10, C0, C01, C1, C2;
	attach += START_Flag?"_S":"_E";

	pmFeatureVector.push_back(C_2+"_nG_L_2"+attach);
	pmFeatureVector.push_back(C_1+"_nG_L_1"+attach);
	pmFeatureVector.push_back(C10+"_nG_10"+attach);
	pmFeatureVector.push_back( C0+"_nG_0"+attach);
	pmFeatureVector.push_back(C01+"_nG_01"+attach);
	pmFeatureVector.push_back( C1+"_nG_R_1"+attach);
	pmFeatureVector.push_back( C2+"_nG_R_2"+attach);*/

}
void CNEFeator::Combined_Feature_In_Candit_Two_Boundary(const CanditCase& pmCandit, vector<string>& pmFeatureVector, string attach)
{

	CanditCase locCandit = pmCandit;
	CEDT_ACE_Data_Preprocessing(locCandit);

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
	CEDT_ACE_Data_Preprocessing(locCandit);

	string MentionTYPE = attach;//Head or Extend
	//START
	if(CEDT_Candit_nG_Featrue_Flag){
		BoundCase loc_Boundary;
		loc_Boundary.SENID = pmCandit.SENID;

		loc_Boundary.position = pmCandit.START;
		loc_Boundary.prixstr = pmCandit.prix;
		loc_Boundary.proxstr = pmCandit.mention + pmCandit.prox;

		Extract_Baseline_Feature_Vector_of_Boundary(loc_Boundary, pmFeatureVector, true, MentionTYPE);

		Extract_Local_Feature_Vector_of_Boundary_with_nGram(loc_Boundary, pmFeatureVector, true, MentionTYPE);

		//Adjacent_Left_Border_n_Gram(prixstr.c_str(), BOARD_n_GRAM_NUM, pmFeatureVector, "S_L_nG_"+MentionTYPE);
		//Adjacent_Right_Border_n_Gram(proxstr.c_str(), BOARD_n_GRAM_NUM, pmFeatureVector, "S_R_nG_"+MentionTYPE);

		loc_Boundary.position = pmCandit.END;
		loc_Boundary.prixstr = pmCandit.prix + pmCandit.mention;
		loc_Boundary.proxstr = pmCandit.prox;
		Extract_Baseline_Feature_Vector_of_Boundary(loc_Boundary, pmFeatureVector, false, MentionTYPE);

		Extract_Local_Feature_Vector_of_Boundary_with_nGram(loc_Boundary, pmFeatureVector, false, MentionTYPE);

		//Adjacent_Left_Border_n_Gram(prixstr.c_str(), BOARD_n_GRAM_NUM, pmFeatureVector, "E_L_nG_"+MentionTYPE);
		//Adjacent_Right_Border_n_Gram(proxstr.c_str(), BOARD_n_GRAM_NUM, pmFeatureVector, "E_R_nG_"+MentionTYPE);
	}

	Internal_n_Gram(locCandit.mention.c_str(), 3, pmFeatureVector, "_InGram_"+MentionTYPE);
	Combined_Feature_In_Candit_Two_Boundary(locCandit, pmFeatureVector, MentionTYPE);

	if(CEDT_Segmenter_Candit_External_Words_Flag){
		Adjacent_Border_Words_with_Segmeter(locCandit.l_outmstr.c_str(), BOARD_WORDS_NUM, pmFeatureVector, "BWS_L_"+MentionTYPE, "");//locCandit.l_outmstr.c_str()
		Adjacent_Border_Words_with_Segmeter(locCandit.r_outmstr.c_str(), BOARD_WORDS_NUM, pmFeatureVector, "", "BWS_R_"+MentionTYPE);//locCandit.r_outmstr.c_str()
	}

	CEDT_Internal_Sequence_Words_Flag = false;
	if(CEDT_Segmenter_Candit_Internal_Words_Flag){
		Internal_Candit_Words_Feature(locCandit.mention.c_str(), pmFeatureVector, CEDT_Internal_Sequence_Words_Flag, "IW_"+MentionTYPE);
	}

	if(CEDT_Word_Boundary_Flag){
		string originstr = pmCandit.prix + pmCandit.mention + pmCandit.prox;
		Adjacent_Segmentation_Info_on_Boundary(pmCandit.SENID, originstr, pmCandit.START, true, pmFeatureVector, "S_si"+MentionTYPE);
		Adjacent_Segmentation_Info_on_Boundary(pmCandit.SENID, originstr, pmCandit.END, false, pmFeatureVector, "E_si"+MentionTYPE);
	}

		//vector<string> test_feature_v;//test
	if(CEDT_Candit_POS_Flag){
		Adjacent_Words_POS_Feature_Extracting_with_Segmeter(locCandit.l_outmstr.c_str(), BOARD_POS_NUM, pmFeatureVector, "", "POS_ro_"+MentionTYPE);
		Adjacent_Words_POS_Feature_Extracting_with_Segmeter(locCandit.r_outmstr.c_str(), BOARD_POS_NUM, pmFeatureVector, "POS_lo_"+MentionTYPE, "");
		//Adjacent_Words_POS_Feature_Extracting_with_Segmeter(locCandit.mention.c_str(), BOARD_POS_NUM, pmFeatureVector, "", "POS_rm_"+MentionTYPE);
		//Adjacent_Words_POS_Feature_Extracting_with_Segmeter(locCandit.mention.c_str(), BOARD_POS_NUM, pmFeatureVector, "POS_lm_"+MentionTYPE, "");
	}


	/*ostringstream ostream;
	string mentionstr = ace_op::Delet_0AH_and_20H_in_string(locCandit.mention.c_str());
	ostream << NLPOP::Get_Chinese_Sentence_Length_Counter(mentionstr.c_str());
	ostream << "_ML" << "_" << attach;
	pmFeatureVector.push_back(ostream.str());*/
	
	/*map<string, unsigned long> WordsCnt_map;
	pCCEDT->m_CSegmter.Omni_words_feature_Extracting(locCandit.l_outmstr.c_str(), NE_Lexicon_Set, WordsCnt_map, "i_", "");
	pCCEDT->m_CSegmter.Omni_words_feature_Extracting(locCandit.r_outmstr.c_str(), NE_Lexicon_Set, WordsCnt_map, "o_", "");
	pCCEDT->m_CSegmter.Omni_words_feature_Extracting(locCandit.mentionstr.c_str(), NE_Lexicon_Set, WordsCnt_map, "m_", "");
	for(map<string, unsigned long>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
		pmFeatureVector.push_back(mite->first);
	}
	Adjacent_Left_Border_n_Gram(locCandit.prixstr.c_str(), true, 3, pmFeatureVector);
	Adjacent_Right_Border_n_Gram(locCandit.proxstr.c_str(), false, 3, pmFeatureVector);
	Left_Max_Match_Segmentor(locCandit.l_outmstr.c_str(), true, 2, pmFeatureVector);
	Right_Max_Match_Segmentor(locCandit.r_outmstr.c_str(), false, 2, pmFeatureVector);
	//NE Regex
	if(m_CRegex.FreqPattern_Initilized_Flag){
		Extract_Regex_Feature_of_Named_Entity(locCandit, locFeature_v);
		for(vector<string>::iterator vite = locFeature_v.begin(); vite != locFeature_v.end(); vite++){
			pmFeatureVector.push_back(*vite+"x_");
		}
		locFeature_v.clear();
	}*/
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
	//----Extend boundary Feature
	/*if(pmDismCase.Extend_Held_Flag && pmDismCase.Head_Held_Flag){
		vector<string> Loc_Feature;
		string prixstr;
		string proxstr;
		CanditCase& locHead = *pmDismCase.pHead;
		CanditCase locExtend = *pmDismCase.pExtend;
		CEDT_ACE_Data_Preprocessing(locExtend);

		if(locExtend.START != locHead.START){
			prixstr = locExtend.prix;
			proxstr = locExtend.mention + locExtend.prox;
			Adjacent_Left_Border_n_Gram(prixstr.c_str(), BOARD_n_GRAM_NUM, Loc_Feature, "S_L_nG_ED");
			Adjacent_Right_Border_n_Gram(proxstr.c_str(), BOARD_n_GRAM_NUM, Loc_Feature, "S_R_nG_ED");
			if(CEDT_Candit_POS_Flag){
				Adjacent_Words_POS_Feature_Extracting_with_Segmeter(locExtend.l_outmstr.c_str(), BOARD_POS_NUM, Loc_Feature, "", "_POS");
			}
		}
		if(locExtend.END != locHead.END){
			prixstr = locExtend.prix + locExtend.mention;
			proxstr = locExtend.prox;
			Adjacent_Left_Border_n_Gram(prixstr.c_str(), false, BOARD_n_GRAM_NUM, Loc_Feature);
			Adjacent_Right_Border_n_Gram(proxstr.c_str(), false, BOARD_n_GRAM_NUM, Loc_Feature);
			if(CEDT_Candit_POS_Flag){
				Adjacent_Words_POS_Feature_Extracting_with_Segmeter(locExtend.r_outmstr.c_str(), BOARD_POS_NUM, Loc_Feature, "POS_", "");
			}
		}
		for(size_t i = 0; i < Loc_Feature.size(); i++){
			pmFeatureVector.push_back(Loc_Feature[i]+"_ED");
		}
	}*/
}

void CNEFeator::Push_Back_Extracted_Feature_Vector_of_Deterimer(string TYPE, DismCase& pmDismCase, FeatureVctor& FeatureCase_v)
{
	vector<string> adjacent_feature_v;
	
	Extract_Feature_Vector_of_Deterimer(pmDismCase, adjacent_feature_v);

	MAXEN::Push_Back_FeatureCase_v_from_Feature_v_with_Check(FeatureCase_v, adjacent_feature_v, TYPE);
}



//----Feature Extraction Function--------------

void CNEFeator::Internal_Candit_Words_Feature(const char* strchar, vector<string>& pmFeatureVector, bool Order_Flag, string attach)
{
	vector<string> Loc_Feature;
	if(CEDT_Segmenter_Candit_Internal_Words_Flag){
		CLAUSEPOS loc_Seg;
		m_CSegmter.ICTCLAS_Segmention_Port(strchar, loc_Seg);
		for(size_t i = 0; i < loc_Seg.Clause.size(); i++){
			Loc_Feature.push_back(loc_Seg.Clause[i]);
		}
	}
	else if(CEDT_MM_Feature_Flag){
		m_CMaxmatch.Maximum_Match_Segmentation_Traditional(strchar, Loc_Feature, "BMM", lexicon_set);
	}

	size_t WordsCnt = 0;
	if(!Order_Flag){
		set<string> Words_s;
		for(vector<string>::reverse_iterator rvite = Loc_Feature.rbegin(); rvite != Loc_Feature.rend(); rvite++){
			if(((int)WordsCnt++ > INTERNAL_WORDS_NUM) && (INTERNAL_WORDS_NUM != -1)){
				break;
			}
			Words_s.insert(rvite->data());
		}
		for(set<string>::iterator site = Words_s.begin(); site != Words_s.end(); site++){
			ostringstream ostream;
			ostream << *site << "_" << attach;
			pmFeatureVector.push_back(ostream.str());
		}
	}
	else{
		for(vector<string>::reverse_iterator rvite = Loc_Feature.rbegin(); rvite != Loc_Feature.rend(); rvite++){
			if(((int)WordsCnt++ > INTERNAL_WORDS_NUM) && (INTERNAL_WORDS_NUM != -1)){
				break;
			}
			ostringstream ostream;
			ostream << rvite->data() << "_" << WordsCnt << "_" << attach;
			pmFeatureVector.push_back(ostream.str());
		}
	}
}

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

void CNEFeator::Adjacent_Border_Words_with_Segmeter(const char* strchar, size_t Distance, vector<string>& pmFeatureVector, string prix = "", string prox = "")
{
	if(strlen(strchar) == 0){
		return;
	}
	map<string, float> WordsCnt_map;
	CLAUSEPOS loc_Seg;
	if(!CEDT_MM_Feature_Flag){
		m_CSegmter.ICTCLAS_Segmention_Port(strchar, loc_Seg);
	}
	if(prix.length() != 0){
		if(CEDT_MM_Feature_Flag){
			vector<string> segment_v;
			m_CMaxmatch.Maximum_Match_Segmentation_Traditional(strchar, segment_v, "BMM", lexicon_set);
			for(size_t i = 0; i < segment_v.size(); i++){
				loc_Seg.Clause.push_back(segment_v[i]);
			}
		}
		size_t dis_cnt = 0;
		for(vector<string>::reverse_iterator rvite = loc_Seg.Clause.rbegin(); rvite != loc_Seg.Clause.rend(); rvite++){
			if(!(dis_cnt++ < Distance)){
				break;
			}
			ostringstream ostream;
			ostream << rvite->data() << "_" << dis_cnt << "_" << prix;
			WordsCnt_map.insert(make_pair(ostream.str(), (float)1));
		}
	}
	if(prox.length() != 0){
		if(CEDT_MM_Feature_Flag){
			vector<string> segment_v;
			m_CMaxmatch.Maximum_Match_Segmentation_Traditional(strchar, segment_v, "FMM", lexicon_set);
			for(size_t i = 0; i < segment_v.size(); i++){
				loc_Seg.Clause.push_back(segment_v[i]);
			}
		}
		for(size_t i = 0; i < loc_Seg.Clause.size(); i++){
			if(!(i < Distance)){
				break;
			}
			ostringstream ostream;
			ostream << loc_Seg.Clause[i] << "_" << i << "_" << prox;
			WordsCnt_map.insert(make_pair(ostream.str(), (float)1));
		}
	}

	for(map<string, float>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
		pmFeatureVector.push_back(mite->first);
	}

}

void CNEFeator::Adjacent_Words_POS_Feature_Extracting_with_Segmeter(const char* sentchar, int Distance, vector<string>& pmFeatureVector, string prix = "", string prox = "")
{
	if(strlen(sentchar) == 0){
		return;
	}
	CLAUSEPOS loc_Seg;
	m_CSegmter.ICTCLAS_Segmention_Port(sentchar, loc_Seg);
	if(prix.length() > 0){
		for(size_t i = 0; i < loc_Seg.POS.size(); i++){
			if(!((int)i < Distance)){
				break;
			}
			ostringstream ostream;
			ostream << loc_Seg.POS[i] << "_" << i << "_" << prix;
			pmFeatureVector.push_back(ostream.str());
		}
	}
	if(prox.length() > 0){
		size_t k = 0;
		for(vector<string>::reverse_iterator rvite = loc_Seg.POS.rbegin(); rvite != loc_Seg.POS.rend(); rvite++, k++){
			if(!((int)k < Distance)){
				break;
			}
			ostringstream ostream;
			ostream << rvite->data() << "_" << k << "_" << prox;
			pmFeatureVector.push_back(ostream.str());
		}
	}
}


void CNEFeator::Adjacent_Left_Border_Words(const char* strchar, bool OuterFlag, size_t Distance, vector<string>& outstr_v)
{
	char sChar[3];
	string matchstr;
	string tempstr;
	size_t loc_dis = 1;
	size_t strlength = strlen(strchar);
	//if(strLen > maxtoken_size){
	//	strLen = maxtoken_size;
	//}
	sChar[2]=0;
	size_t k;

	for(size_t i = 0; i < strlength; ){
		matchstr = "";
		tempstr = "";
		for(size_t j = i; j < strlength; ){
			sChar[0]=strchar[j++];
			sChar[1]=0;	
			if(sChar[0] < 0 ){
				sChar[1]=strchar[j++];
			}
			tempstr += sChar;
			if(lexicon_set.find(tempstr) != lexicon_set.end()){
				matchstr = tempstr;
				k = j;
			}
		}
		if((matchstr.length() == 0) && (tempstr.length() != 0)){
			size_t j = i;
			sChar[0]=strchar[j++];
			sChar[1]=0;	
			if(sChar[0] < 0 ){
				sChar[1]=strchar[j++];
			}
			matchstr = sChar;
			k = j;
		}
		ostringstream ostrsteam;
		if(OuterFlag){
			ostrsteam << "_L_" << loc_dis;
			matchstr += ostrsteam.str();
		}
		else{
			ostrsteam << "L_" << loc_dis << "_";
			matchstr = ostrsteam.str() + matchstr;
		}
		outstr_v.push_back(matchstr);
		if(loc_dis++ == Distance){
			break;
		}
		else{
			i = k;
		}
	}
}

void CNEFeator::Adjacent_Right_Border_Words(const char* strchar, bool OuterFlag, size_t Distance, vector<string>& outstr_v)
{
	string matchstr;
	string tempstr;
	size_t k = 0;
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);
	vector<string>::reverse_iterator k_rvite;
	for(vector<string>::reverse_iterator rvite = char_v.rbegin();  rvite != char_v.rend(); rvite++){
		matchstr = "";
		tempstr = "";
		for(vector<string>::reverse_iterator inrvite = rvite;  inrvite != char_v.rend(); inrvite++){
			tempstr = inrvite->data() + tempstr;
			if(lexicon_set.find(tempstr) != lexicon_set.end()){
				matchstr = tempstr;
				k_rvite = inrvite;
			}
		}
		if((matchstr.length() == 0) && (tempstr.length() != 0)){
			matchstr = rvite->data();
			k_rvite = rvite;
		}
		if(k++ < Distance){
			ostringstream ostrsteam;
			if(OuterFlag){
				ostrsteam << "_R_" << k;
				matchstr += ostrsteam.str();
			}
			else
			{
				ostrsteam << "R_" << k << "_";
				matchstr = ostrsteam.str() + matchstr;
			}
			outstr_v.push_back(matchstr);
			rvite = k_rvite;
		}
		else{
			break;
		}
	}
}
//----Support Function----------
void CNEFeator::Extract_Regex_Feature_of_Named_Entity(CanditCase& pmCandit, vector<string>& pmFeatureVector)
{
	vector<string> MatchedStr_v;
//	m_CRegex.Named_Entity_Inner_Matched(pmCandit.mention.c_str(), MatchedStr_v);
	for(vector<string>::iterator vite = MatchedStr_v.begin(); vite != MatchedStr_v.end(); vite++){
		pmFeatureVector.push_back(*vite + "_in");
	}

	MatchedStr_v.clear();
//	m_CRegex.Regex_Pattern_Dual_Match_Poart(pmCandit.prix.c_str(), pmCandit.prox.c_str(), MatchedStr_v);
	for(vector<string>::iterator vite = MatchedStr_v.begin(); vite != MatchedStr_v.end(); vite++){
		pmFeatureVector.push_back(*vite + "_ou");
	}
}


void CNEFeator::Reset_Maximum_Matcher_Lexicon(const char* lexiconpath)
{
	lexicon_set.clear();
	NLPOP::LoadWordsStringSet(lexiconpath, lexicon_set);
	maxtoken_size = 0;
	for(set<string>::iterator site = lexicon_set.begin(); site != lexicon_set.end(); site++){
		if(site->size() > (size_t)maxtoken_size){
			maxtoken_size = site->size();
		}
	}
}

void CNEFeator::Reset_Maximum_Matcher_Lexicon(set<string>& pmOuterLexicion_s)
{
	lexicon_set.clear();
	maxtoken_size = 0;
	for(set<string>::iterator site = pmOuterLexicion_s.begin(); site != pmOuterLexicion_s.end(); site++){
		if(site->size() > (size_t)maxtoken_size){
			maxtoken_size = site->size();
		}
		lexicon_set.insert(site->data());
	}
}






void CNEFeator::CEDT_ACE_Data_Preprocessing(CanditCase& pmCanditCase)
{
	NECOM::Pretreatment_Sent_For_Named_Entity(pmCanditCase.prix, Number_Set, English_Set);
	NECOM::Pretreatment_Sent_For_Named_Entity(pmCanditCase.prox, Number_Set, English_Set);
	NECOM::Pretreatment_Sent_For_Named_Entity(pmCanditCase.mention, Number_Set, English_Set);
	NECOM::Pretreatment_Sent_For_Named_Entity(pmCanditCase.l_outmstr, Number_Set, English_Set);
	NECOM::Pretreatment_Sent_For_Named_Entity(pmCanditCase.r_outmstr, Number_Set, English_Set);
}
void CNEFeator::CEDT_ACE_Data_Preprocessing(DismCase& pmDismCase)
{
	if(pmDismCase.Extend_Held_Flag){
		CanditCase& locExtend = *pmDismCase.pExtend;
		CEDT_ACE_Data_Preprocessing(locExtend);
	}
	if(pmDismCase.Head_Held_Flag){
		CanditCase& locHead = *pmDismCase.pHead;
		CEDT_ACE_Data_Preprocessing(locHead);
	}
}
void CNEFeator::CEDT_ACE_Data_Preprocessing(BoundCase& pmBoundary)
{
	NECOM::Pretreatment_Sent_For_Named_Entity(pmBoundary.prixstr, Number_Set, English_Set);
	NECOM::Pretreatment_Sent_For_Named_Entity(pmBoundary.proxstr, Number_Set, English_Set);
}


//*****************************************************

void CNEFeator::Adjacent_Right_Border_Characters(const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v)
{
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);
	int k = 0;
	for(int i = 0; i < (int)char_v.size(); i++){
		if(k++ < Distance){
			ostringstream ostrsteam;
			if(OuterFlag){
				ostrsteam << char_v[i] <<  "_R_" << k;
			}
			else{
				ostrsteam << "R_" << k << "_" << char_v[i];
			}
			ostrsteam << "_nc";
			outstr_v.push_back(ostrsteam.str());
		}
		else{
			break;
		}
	}
}

void CNEFeator::Adjacent_Left_Border_Characters(const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v)
{
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);
	int k = 0;
	for(vector<string>::reverse_iterator rvite = char_v.rbegin();  rvite != char_v.rend(); rvite++){
		if(k++ < Distance){
			ostringstream ostrsteam;
			if(OuterFlag){
				ostrsteam << rvite->data() << "_L_" << k;
			}
			else
			{
				ostrsteam << "L_" << k << "_" << rvite->data();
			}
			ostrsteam << "_nc";
			outstr_v.push_back(ostrsteam.str());
		}
		else{
			break;
		}
	}
}


void CNEFeator::Adjacent_Left_Maxmatch_Overlapping_Words(set<string>& pmLexicon_s, const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v)
{
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);
	int k = 0;
	bool Matched_Flag = false;
	string maxstr = "";
	for(vector<string>::reverse_iterator rvite = char_v.rbegin(); rvite != char_v.rend(); rvite++){
		maxstr = rvite->data() + maxstr;
		if(pmLexicon_s.find(maxstr) != pmLexicon_s.end()){
			if(!Matched_Flag){
				Matched_Flag = true;
			}
			if(k++ < Distance || Distance == -1){
				ostringstream ostrsteam;
				if(OuterFlag){
					ostrsteam << maxstr <<  "_L";
				}
				else{
					ostrsteam << "L_" << maxstr;
				}
				ostrsteam << "_mx";
				outstr_v.push_back(ostrsteam.str());
			}
			else{
				break;
			}
		}
	}
	if(!Matched_Flag && !char_v.empty()){
		maxstr = char_v.rbegin()->data();
		if(k < Distance || Distance == -1){
			ostringstream ostrsteam;
			if(OuterFlag){
				ostrsteam << maxstr <<  "_L";
			}
			else{
				ostrsteam << "L_" << maxstr;
			}
			ostrsteam << "_mx";
			outstr_v.push_back(ostrsteam.str());
		}
	}
}

void CNEFeator::Adjacent_Right_Maxmatch_Overlapping_Words(set<string>& pmLexicon_s, const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v)
{
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);
	int k = 0;
	bool Matched_Flag = false;
	string maxstr = "";
	for(int i = 0; i < (int)char_v.size(); i++){
		maxstr += char_v[i];
		if(pmLexicon_s.find(maxstr) != pmLexicon_s.end()){
			if(!Matched_Flag){
				Matched_Flag = true;
			}
			if(k < Distance || Distance == -1){
				ostringstream ostrsteam;
				if(OuterFlag){
					ostrsteam << maxstr <<  "_R";
				}
				else{
					ostrsteam << "R_" << maxstr;
				}
				ostrsteam << "_mx";
				outstr_v.push_back(ostrsteam.str());
			}
			else{
				break;
			}
		}
	}
	if(!Matched_Flag && !char_v.empty()){
		maxstr = char_v.begin()->data();
		if(k < Distance || Distance == -1){
			ostringstream ostrsteam;
			if(OuterFlag){
				ostrsteam << maxstr <<  "_R";
			}
			else{
				ostrsteam << "R_" << maxstr;
			}
			ostrsteam << "_mx";
			outstr_v.push_back(ostrsteam.str());
		}
	}
}


void CNEFeator::Adjacent_Left_Second_Maxmatch_Overlapping_Words(bool Combined_Flag, set<string>& pmLexicon_s, const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v)
{
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);
	int k = 0;
	int j = 0;
	bool Matched_Flag = false;
	string first_maxstr = "";
	string second_maxstr = "";
	string Combined_maxstr = "";
	for(vector<string>::reverse_iterator rvite = char_v.rbegin(); rvite != char_v.rend(); rvite++, k++){
		first_maxstr = rvite->data() + first_maxstr;
		if((pmLexicon_s.find(first_maxstr) != pmLexicon_s.end()) || (rvite == char_v.rbegin())){
			vector<string>::reverse_iterator srvite = rvite;
			second_maxstr = "";
			j = k;
			for(srvite++; srvite != char_v.rend(); srvite++, j++){
				second_maxstr = srvite->data() + second_maxstr;
				if(pmLexicon_s.find(second_maxstr) != pmLexicon_s.end()){
					if(!Matched_Flag){
						Matched_Flag = true;
					}
					if(Combined_Flag){
						Combined_maxstr = second_maxstr + first_maxstr;
					}
					else{
						Combined_maxstr = second_maxstr;
					}
					if(j < Distance || Distance == -1){
						ostringstream ostrsteam;
						if(OuterFlag){
							ostrsteam << Combined_maxstr <<  "_L_";
						}
						else{
							ostrsteam << "L_" << "_" << Combined_maxstr;
						}
						ostrsteam << "_smx";
						if(Combined_Flag){
							outstr_v.push_back(Combined_maxstr+"_L");
						}
						else{
							outstr_v.push_back(ostrsteam.str());
						}
					}
					else{
						break;
					}
				}
			}
		}
	}
	if(!Matched_Flag && !char_v.empty()){
		first_maxstr = char_v.rbegin()->data();
		vector<string>::reverse_iterator srvite = char_v.rbegin();
		srvite++;
		second_maxstr = "";
		if(srvite != char_v.rend()){
			second_maxstr = srvite->data();
			if(Combined_Flag){
				Combined_maxstr = second_maxstr + first_maxstr;
			}
			else{
				Combined_maxstr = second_maxstr;
			}
			if(2 < Distance || Distance == -1){
				ostringstream ostrsteam;
				if(OuterFlag){
					ostrsteam << Combined_maxstr <<  "_L_";
				}
				else{
					ostrsteam << "L_" << "_" << Combined_maxstr;
				}
				ostrsteam << "_smx";
				if(Combined_Flag){
					outstr_v.push_back(Combined_maxstr+"_L");
				}
				else{
					outstr_v.push_back(ostrsteam.str());
				}
			}
		}
	}
}

void CNEFeator::Adjacent_Right_Second_Maxmatch_Overlapping_Words(bool Combined_Flag, set<string>& pmLexicon_s, const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v)
{
	vector<string> char_v;
	NLPOP::charseq_to_vector(strchar, char_v);
	int k = 0;
	int j = 0;
	bool Matched_Flag = false;
	string first_maxstr = "";
	string second_maxstr = "";
	string Combined_maxstr = "";
	for(vector<string>::iterator vite = char_v.begin(); vite != char_v.end(); vite++, k++){
		first_maxstr += vite->data();
		if((pmLexicon_s.find(first_maxstr) != pmLexicon_s.end()) || (vite == char_v.begin())){
			vector<string>::iterator svite = vite;
			second_maxstr = "";
			j = k;
			for(svite++; svite != char_v.end(); svite++, j++){
				second_maxstr += svite->data();
				if(pmLexicon_s.find(second_maxstr) != pmLexicon_s.end()){
					if(!Matched_Flag){
						Matched_Flag = true;
					}
					if(Combined_Flag){
						Combined_maxstr = first_maxstr + second_maxstr;
					}
					else{
						Combined_maxstr = second_maxstr;
					}
					if(j < Distance || Distance == -1){
						ostringstream ostrsteam;
						if(OuterFlag){
							ostrsteam << Combined_maxstr <<  "_R_";
						}
						else{
							ostrsteam << "R_" << "_" << Combined_maxstr;
						}
						ostrsteam << "_smx";
						if(Combined_Flag){
							outstr_v.push_back(ostrsteam.str());
						}
						else{
							outstr_v.push_back(ostrsteam.str());
						}
					}
					else{
						break;
					}
				}
			}
		}
	}
	if(!Matched_Flag && !char_v.empty()){
		first_maxstr = char_v.begin()->data();
		vector<string>::iterator svite = char_v.begin();
		svite++;
		second_maxstr = "";
		if(svite != char_v.end()){
			second_maxstr = svite->data();
			if(Combined_Flag){
				Combined_maxstr = first_maxstr + second_maxstr;
			}
			else{
				Combined_maxstr = second_maxstr;
			}
			if(2 < Distance || Distance == -1){
				ostringstream ostrsteam;
				if(OuterFlag){
					ostrsteam << Combined_maxstr <<  "_R_";
				}
				else{
					ostrsteam << "R_" << "_" << Combined_maxstr;
				}
				ostrsteam << "_smx";
				if(Combined_Flag){
					outstr_v.push_back(ostrsteam.str());
				}
				else{
					outstr_v.push_back(ostrsteam.str());
				}
			}
		}
		
	}
}

void CNEFeator::Left_Max_Match_Segmentor(const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v)
{
	vector<string> segment_v;
	
	m_CMaxmatch.Maximum_Match_Segmentation_Traditional(strchar, segment_v, "BMM", lexicon_set);

	int k = 0;
	for(vector<string>::reverse_iterator rvite = segment_v.rbegin(); rvite != segment_v.rend(); rvite++ ){
		if(k++ < Distance){
			ostringstream ostrsteam;
			if(OuterFlag){
				ostrsteam << rvite->data() << "_L_" << k;
			}
			else
			{
				ostrsteam << "L_" << k << "_" << rvite->data();
			}
			ostrsteam << "_seg";
			outstr_v.push_back(ostrsteam.str());
		}
		else{
			break;
		}
	}

}
void CNEFeator::Right_Max_Match_Segmentor(const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v)
{
	vector<string> segment_v;
	
	m_CMaxmatch.Maximum_Match_Segmentation_Traditional(strchar, segment_v, "FMM", lexicon_set);

	int k = 0;
	for(vector<string>::iterator vite = segment_v.begin(); vite != segment_v.end(); vite++){
		if(k++ < Distance){
			ostringstream ostrsteam;
			if(OuterFlag){
				ostrsteam << vite->data() << "_R_" << k;
			}
			else
			{
				ostrsteam << "R_" << k << "_" << vite->data();
			}
			ostrsteam << "_seg";
			outstr_v.push_back(ostrsteam.str());
		}
		else{
			break;
		}
	}

}


