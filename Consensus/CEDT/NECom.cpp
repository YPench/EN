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
#include "NECom.h"
#include "maxen.h"

extern string Detect_Single_NE_TYPE;
extern bool CEDT_Detection_Flag;

void NECOM::Extract_Location_Name(const char* cFolderPath)
{
	string WDpath = cFolderPath;
	WDpath += "Worddb.dat";
	map<string, set<string>> WordMor_ms;
	set<string> LocationName_s;
	NLPOP::LoadWordData_AS_Map_string_MorphstrSet(WDpath, WordMor_ms);

	for(map<string, set<string>>::iterator msite = WordMor_ms.begin(); msite != WordMor_ms.end(); msite++){
		for(set<string>::iterator site = msite->second.begin(); site != msite->second.end(); site++){
			if(site->find("ns") != -1){
				LocationName_s.insert(msite->first);
				break;
			}
		}
	}
	 WDpath = cFolderPath;
	 WDpath += "LocalName.dat";
	 NLPOP::Save_Set_String_With_Comma_Divide(WDpath, LocationName_s);
}
void NECOM::Generate_Word_Boundary_Info(const char* inputchar, vector<string>& SegRtn_v, vector<string>& outpos)
{
	if(strlen(inputchar) == 0){
		return;
	}
	string inputstr = ace_op::Delet_0AH_and_20H_in_string(inputchar);
	vector<string> character_v;
	NLPOP::charseq_to_vector(inputchar, character_v);

	if(strlen(inputstr.c_str()) == 0){
		size_t i = 0;
		while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			outpos.push_back("");
			i++;
			if(!(i < character_v.size())){
				return;
			}
		}
	}
	 
	vector<string>::iterator wordvite = SegRtn_v.begin();
	size_t i = 0;
	for( ; wordvite != SegRtn_v.end(); ){
		vector<string> chars_v;
		NLPOP::charseq_to_vector(wordvite->data(), chars_v);

		for(vector<string>::iterator charvite = chars_v.begin(); charvite != chars_v.end(); charvite++){
			if(!(i < character_v.size())){
				AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 1", MB_OK);
				break;
			}
			while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				outpos.push_back("");
				i++;
				if(!(i < character_v.size())){
					AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 2", MB_OK);
					break;
				}
			}
			if(strcmp(character_v[i].c_str(), charvite->data())){
				stringstream ostream;
				ostream << "NECOM::Generate_Word_Boundary_Info()" << endl;
				ostream << character_v[i].c_str() << endl;
				ostream << charvite->data() << endl;
				AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
				return;
			}
		
			if(SegRtn_v.empty() || wordvite == SegRtn_v.end()){
				outpos.push_back("nops");
				AppCall::Secretary_Message_Box("Data Error: in CCRF::Generate_CRF_POS() 6", MB_OK);
			}
			else if(charvite == chars_v.begin()){
				outpos.push_back("wB");
			}
			else if((charvite+1) == chars_v.end()){
				outpos.push_back("wE");
			}
			else{
				outpos.push_back("wM");
			}
			i++;
		}
		wordvite++;
	}
	if(i < character_v.size()){
		while(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
			outpos.push_back("");
			i++;
			if(!(i < character_v.size())){
				break;
			}
		}
	}
	if(outpos.size() != character_v.size()){
		stringstream ostream;
		ostream << "Data Error: in CCRF::Generate_Word_Boundary_Info()\noutpos.size() != character_v.size()" << endl;
		ostream << "|" << inputstr << "|" << ": " << strlen(inputstr.c_str()) << endl;
		ostream <<  "|" << inputchar <<  "|" << ": " << strlen(inputchar) << endl;
		for(size_t i = 0; i < SegRtn_v.size(); i++){
			ostream << SegRtn_v[i] << "|";
		}
		ostream << endl;
		for(size_t i = 0; i < outpos.size(); i++){
			ostream << outpos[i];
		}
		AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
		return;
	}
}


void NECOM::Generated_Candidate_Testing_Cases_Information(map<string, size_t>& pmPositive_Cases, vector<CanditCase*>& pmCandit_v, Maxen_Rtn_map& ProcesedCasesInfor_m)
{
	size_t OtherCnt = 0;
	map<string, size_t> Candidate_Cases_m;

	for(vector<CanditCase*>::iterator vmite = pmCandit_v.begin(); vmite != pmCandit_v.end(); vmite++){
		if((*vmite)->Cand_Flag){
			if(Candidate_Cases_m.find(CEDT_Detection_Flag?POSITIVE:(*vmite)->pNE_mention->Entity_TYPE) == Candidate_Cases_m.end()){
				Candidate_Cases_m.insert(make_pair(CEDT_Detection_Flag?POSITIVE:(*vmite)->pNE_mention->Entity_TYPE, 1));
			}
			else{
				Candidate_Cases_m[CEDT_Detection_Flag?POSITIVE:(*vmite)->pNE_mention->Entity_TYPE]++;
			}
		}
		else{
			OtherCnt++;
		}
	}

	for(map<string, size_t>::iterator mite = pmPositive_Cases.begin(); mite != pmPositive_Cases.end(); mite++){
		if(Candidate_Cases_m.find(mite->first) == Candidate_Cases_m.end()){
			Candidate_Cases_m.insert(make_pair(mite->first, 0));
		}
	}

	if(pmPositive_Cases.size() != Candidate_Cases_m.size()){
		AppCall::Secretary_Message_Box("Error: in Generated_Candidate_Testing_Cases_Information()", MB_OK);
	}
	ostringstream ostream;
	ostream.str("");
	ostream << endl;
	ostream << "TYPE/SUBTYPE\t  Candit\t  Totall\t  Recall\t" << endl;
	ostream << "----------------------------------------------------------------------------------------------------------" << endl;
	AppCall::Maxen_Responce_Message_with_Save(ostream.str().c_str());

	map<string, size_t>::iterator cmite = Candidate_Cases_m.begin();
	map<string, size_t>::iterator pmite = pmPositive_Cases.begin();

	size_t C_Totall = 0;
	size_t P_Totall = 0;
	while(cmite != Candidate_Cases_m.end() && pmite != pmPositive_Cases.end()){		
		C_Totall += cmite->second;
		P_Totall += pmite->second;
		display_with_Save("%s\t\t %3d\t  %3d\t  %.4f%%\t", cmite->first.c_str(), cmite->second, pmite->second, (pmite->second == 0)?0:(1.0*cmite->second/pmite->second));
		if(ProcesedCasesInfor_m.find(cmite->first) == ProcesedCasesInfor_m.end()){
			ProcesedCasesInfor_m.insert(make_pair(cmite->first, make_pair(0, 0)));
		}
		ProcesedCasesInfor_m[cmite->first].first += cmite->second;
		ProcesedCasesInfor_m[cmite->first].second += pmite->second;
		cmite++;
		pmite++;
	}
	ostream.str("");
	ostream << "-------------------------------" << endl;
	AppCall::Maxen_Responce_Message_with_Save(ostream.str().c_str());
	display_with_Save("%s\t\t %3d\t  %3d\t  %.4f%%\t", "Totall", C_Totall, P_Totall, (P_Totall == 0)?0:(1.0*C_Totall/P_Totall));

	ostream.str("");
	double Rate = (OtherCnt == 0)?0:(1.0*P_Totall/(OtherCnt+P_Totall));
	ostream << endl;
	ostream << NEGETIVE << ": " << OtherCnt << endl;
	ostream << endl;
	ostream << POSITIVE << "/" << "(" << POSITIVE << "+" << NEGETIVE <<  "): " << Rate << endl;
	AppCall::Maxen_Responce_Message_with_Save(ostream.str().c_str());
}
int NECOM::Get_Left_Outer_Feature_Range_For_START(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit)
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

int NECOM::Get_Right_Outer_Feature_Range_For_END(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit)
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


void NECOM::Segment_Sentence_for_Named_Entity(const char* senchar, set<string>& inSegChar_s, vector<string>& outstr_v, vector<bool>& outrtn_v)
{
	vector<string> character_v;
	NLPOP::charseq_to_vector(senchar, character_v);
	string loc_str = "";
	bool Catched_Flag = false;
	for(size_t i = 0; i < character_v.size(); i++){
		if(inSegChar_s.find(character_v[i]) != inSegChar_s.end()){
			if(Catched_Flag){
				outstr_v.push_back(loc_str);
				outrtn_v.push_back(true);
				loc_str = "";
			}
			outstr_v.push_back(character_v[i]);
			outrtn_v.push_back(false);
			Catched_Flag = false;
		}
		else{
			loc_str += character_v[i];
			Catched_Flag = true;
		}
	}
	if(Catched_Flag){
		outstr_v.push_back(loc_str);
		outrtn_v.push_back(true);
	}

}

void NECOM::Positvie_Mention_Collection(vector<NE_Surround*>& NE_Surround_v, map<string, size_t>& pmPositive_Mentions, bool Combining_Flag)
{
	pmPositive_Mentions.clear();
	for(size_t i = 0; i < NE_Surround_v.size(); i++){
		NE_Surround& loc_Surround = *NE_Surround_v[i];
		for(size_t j = 0; j < loc_Surround.entity_mention_v.size(); j++){
			map<size_t, set<size_t>> Traveled_m;
			ACE_extent& locExtend= loc_Surround.entity_mention_v[j].extent;
			string& locTYPE = loc_Surround.entity_mention_v[j].Entity_TYPE;
		
			if((Detect_Single_NE_TYPE.length() != 0) && strcmp(locTYPE.c_str(), Detect_Single_NE_TYPE.c_str())){
				continue;
			}
			if(Traveled_m.find(locExtend.START) == Traveled_m.end()){
				Traveled_m[locExtend.START];
			}
			if(Traveled_m[locExtend.START].find(locExtend.END) == Traveled_m[locExtend.START].end()){
				if(pmPositive_Mentions.find(locTYPE) == pmPositive_Mentions.end()){
					pmPositive_Mentions.insert(make_pair(locTYPE, 1));
				}
				else{
					pmPositive_Mentions[locTYPE]++;
				}
			}
		}
	}
	size_t TotalSize = 0;
	if(Combining_Flag){
		for(map<string, size_t>::iterator mite =pmPositive_Mentions.begin(); mite !=pmPositive_Mentions.end(); mite++){
			TotalSize += mite->second;
		}
		pmPositive_Mentions.clear();
		pmPositive_Mentions.insert(make_pair(POSITIVE, TotalSize));
	}


}
void NECOM::Building_Head_to_Extend_Lookup_Table_map(vector<NE_Surround*>& NE_Surround_v, map<size_t, map<size_t, map<size_t, ACE_extent*>>*>& HeadExtend_mm)
{
	for(map<size_t, map<size_t, map<size_t, ACE_extent*>>*>::iterator mmite = HeadExtend_mm.begin(); mmite != HeadExtend_mm.end(); mmite++){
		delete mmite->second;
	}
	HeadExtend_mm.clear();
	for(vector<NE_Surround*>::iterator ne_vite = NE_Surround_v.begin(); ne_vite != NE_Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		if(HeadExtend_mm.find(SENID) == HeadExtend_mm.end()){
			HeadExtend_mm[SENID];
			HeadExtend_mm[SENID] = new map<size_t, map<size_t, ACE_extent*>>;
		}
		map<size_t, map<size_t, ACE_extent*>>& locHeadExtend_m = *HeadExtend_mm[SENID];
		for(vector<ACE_entity_mention>::iterator pvite = (*ne_vite)->entity_mention_v.begin(); pvite != (*ne_vite)->entity_mention_v.end(); pvite++){	
			ACE_entity_head& locHead = pvite->head;
			if(locHeadExtend_m.find(locHead.START) == locHeadExtend_m.end()){
				locHeadExtend_m[locHead.START];
			}
			if(locHeadExtend_m[locHead.START].find(locHead.END) == locHeadExtend_m[locHead.START].end()){
				locHeadExtend_m[locHead.START].insert(make_pair(locHead.END, &(pvite->extent)));
			}
		}
	}
}




void NECOM::Pretreatment_Sent_For_Named_Entity(string& pmstr, set<string>& Number_Set, set<string>& Eng_set)
{
	pmstr = Pretreatment_Sent_For_Named_Entity(pmstr.c_str(), Number_Set, Eng_set);
}

string NECOM::Pretreatment_Sent_For_Named_Entity(const char* pmcharstr, set<string>& Number_Set, set<string>& Eng_set)
{
	string rtnstr = ace_op::Delet_0AH_and_20H_in_string(pmcharstr);
	
	NLPOP::merging_numbers(rtnstr, Number_Set);

	//NLPOP::merging_English(rtnstr, Eng_set);

	return rtnstr;
}
void NECOM::Init_START_or_END_Rtn_map_with_NE_Surround(vector<NE_Surround*>& Surround_v, map<size_t, map<size_t, double>*>& rtn_mm)
{
	if(!rtn_mm.empty()){
		for(map<size_t, map<size_t, double>*>::iterator mmite = rtn_mm.begin(); mmite != rtn_mm.end(); mmite++){
			delete mmite->second;
		}
		rtn_mm.clear();
	}
	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = Surround_v.begin(); ne_vite != Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		if(rtn_mm.find(SENID) == rtn_mm.end()){
			rtn_mm[SENID];
			rtn_mm[SENID] = new map<size_t, double>;
		}
		map<size_t, double>& locValue_m = *rtn_mm[SENID];
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			locValue_m.insert(make_pair(i, 0));
		}
	}
}

void NECOM::Init_START_or_END_Rtn_map_with_Erasing(vector<BoundCase*>& BoundCase_v, map<size_t, map<size_t, double>*>& rtn_mm)
{
	if(!rtn_mm.empty()){
		for(map<size_t, map<size_t, double>*>::iterator mmite = rtn_mm.begin(); mmite != rtn_mm.end(); mmite++){
			delete mmite->second;
		}
		rtn_mm.clear();
	}
	for(vector<BoundCase*>::iterator vite = BoundCase_v.begin(); vite != BoundCase_v.end(); vite++){
		if(rtn_mm.find((*vite)->SENID) == rtn_mm.end()){
			rtn_mm[(*vite)->SENID];
			rtn_mm[(*vite)->SENID] = new map<size_t, double>;
		}
		if(rtn_mm[(*vite)->SENID]->find((*vite)->position) == rtn_mm[(*vite)->SENID]->end()){
			rtn_mm[(*vite)->SENID]->insert(make_pair((*vite)->position, 0));
		}
		else{
			AppCall::Secretary_Message_Box("Duplicated information, in NECOM::Init_START_or_END_Rtn_map_with_Erasing()", MB_OK);
		}
		delete *vite;
	}
	BoundCase_v.clear();
}




void NECOM::NE_Surround_Consistence_Check(vector<NE_Surround*>& NE_Surround_v, ACE_Corpus& pmACE_Corpus, bool DOC_Flag)
{
	map<string, ACE_sgm>& sgm_mmap_ref = pmACE_Corpus.ACE_sgm_mmap;
	if(DOC_Flag){
		for(vector<NE_Surround*>::iterator vite = NE_Surround_v.begin(); vite != NE_Surround_v.end(); vite++){
			for(vector<ACE_entity_mention>::iterator pvite = (*vite)->entity_mention_v.begin(); pvite != (*vite)->entity_mention_v.end(); pvite++){
				string tempstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(sgm_mmap_ref[(*vite)->DOCID].DOC.c_str(), pvite->extent.START, pvite->extent.END-pvite->extent.START+1);
				if(strcmp(pvite->extent.charseq.c_str(), tempstr.c_str())){
					if(IDYES != AppCall::Secretary_Message_Box("数据检查出错：NECOM::NE_Surround_Consistence_Check()", MB_YESNOCANCEL)){
						return;
					}
					continue;
				}
			}
			if(sgm_mmap_ref.find((*vite)->DOCID) == sgm_mmap_ref.end()){
				AppCall::Secretary_Message_Box("数据检查出错：NECOM::NE_Surround_Consistence_Check()", MB_OK);
				return;
			}
			string tempstr = Sentop::Get_Substr_by_Chinese_Character_Cnt(sgm_mmap_ref[(*vite)->DOCID].DOC.c_str(), (*vite)->START, (*vite)->END-(*vite)->START+1);
			if(strcmp((*vite)->insur.c_str(), tempstr.c_str())){
				if(IDYES != AppCall::Secretary_Message_Box("数据检查出错：NECOM::NE_Surround_Consistence_Check()", MB_YESNOCANCEL)){
					return;
				}
				continue;
			}
		}
	}
	else{
		for(vector<NE_Surround*>::iterator vite = NE_Surround_v.begin(); vite != NE_Surround_v.end(); vite++){
			for(vector<ACE_entity_mention>::iterator pvite = (*vite)->entity_mention_v.begin(); pvite != (*vite)->entity_mention_v.end(); pvite++){
				string tempstr = Sentop::Get_Substr_by_Chinese_Character_Cnt((*vite)->insur.c_str(), pvite->extent.START, pvite->extent.END-pvite->extent.START+1);
				if(strcmp(pvite->extent.charseq.c_str(), tempstr.c_str())){
					if(IDYES != AppCall::Secretary_Message_Box("数据检查出错：NECOM::NE_Surround_Consistence_Check()", MB_YESNOCANCEL)){
						return;
					}
					continue;
				}
			}
		}
	}
}

void NECOM::NE_Surround_Relative_Position_Calculate(vector<NE_Surround*>& NE_Surround_v)
{
	size_t offset;
	//size_t EqualCnt = 0;
	//size_t UnequalCnt = 0;
	for(vector<NE_Surround*>::iterator lite = NE_Surround_v.begin(); lite != NE_Surround_v.end();lite++){
		offset = (*lite)->START;
		(*lite)->START = 0;
		(*lite)->END = (*lite)->END - offset;
		for(vector<ACE_entity_mention>::iterator pvite = (*lite)->entity_mention_v.begin(); pvite != (*lite)->entity_mention_v.end(); pvite++){
			pvite->extent.START = pvite->extent.START - offset;
			pvite->extent.END = pvite->extent.END - offset;
			pvite->head.START = pvite->head.START - offset;
			pvite->head.END = pvite->head.END - offset;
			/*if(pvite->head.START != pvite->extent.START){
				UnequalCnt++;
			}
			else{
				EqualCnt++;
			}*/
		}
	}
	/*ostringstream ostrsteam;
	ostrsteam << "EqualCnt: " << EqualCnt << endl;
	ostrsteam << "UnequalCnt: " << UnequalCnt << endl;
	AppCall::Secretary_Message_Box(ostrsteam.str(), MB_OK);*/
}
void NECOM::Corpus_Entity_and_Entity_Mention_Information(map<string, ACE_entity>& ACE_Entity_Info_map)
{
	ostringstream ostream;
	ostream << "Entity Number: " << ACE_Entity_Info_map.size() << endl;
	size_t MentionCnt = 0;
	set<string> Mention_s;
	map<string, map<size_t, map<size_t, string>>> MentionInfo_mm;
	for(map<string, ACE_entity>::iterator mite = ACE_Entity_Info_map.begin(); mite != ACE_Entity_Info_map.end(); mite++){
		for(size_t i = 0; i < mite->second.entity_mention_v.size(); i++){
			ACE_entity_mention& locMention = mite->second.entity_mention_v[i];
			Mention_s.insert(locMention.ID);
			if(MentionInfo_mm.find(locMention.DOCID) == MentionInfo_mm.end()){
				MentionInfo_mm[locMention.DOCID];
			}
			if(MentionInfo_mm[locMention.DOCID].find(locMention.extent.START) == MentionInfo_mm[locMention.DOCID].end()){
				MentionInfo_mm[locMention.DOCID][locMention.extent.START];
			}
			if(MentionInfo_mm[locMention.DOCID][locMention.extent.START].find(locMention.extent.END) == MentionInfo_mm[locMention.DOCID][locMention.extent.START].end()){
				MentionInfo_mm[locMention.DOCID][locMention.extent.START].insert(make_pair(locMention.extent.END, locMention.extent.charseq));
				MentionCnt++;
			}
			else{
				//string outstr = "Overlapping Mention: \n";
				//outstr += "(a) " + MentionInfo_mm[locMention.DOCID][locMention.extent.START][locMention.extent.END];
				//outstr += "\n(b) " + locMention.extent.charseq;
				//AppCall::Secretary_Message_Box(outstr.c_str(), MB_OK);
			}
		}
	}
	ostream << "Mention Number: " << MentionCnt << endl;//Mention_s.size() << endl;
	AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
}

void NECOM::Extracting_Named_Entity_Surrounding(map<string, ACE_sgm>& ACE_sgm_mmap, map<string, ACE_entity>& ACE_Entity_Info_map, vector<NE_Surround*>& NE_Surround_v, size_t& Sentence_ID)
{
	map<string, list<pair<string, pair<size_t,size_t>>>> ACE_DocSentence_map;//<DOCID, list_pair: <Sentence, (START, ENd);>
	map<string, vector<pair<ACE_extent, ACE_entity_mention*>>> EntityMentionInfo_map;//Save each entity mention info in doc.

	NE_Surround_v.clear();
	NE_Surround_v.reserve(20000);
	//------------------------------------------------------------------------
	ace_op::Segment_ACE_sgm_to_Sentence_Among_TEXT(ACE_sgm_mmap, ACE_DocSentence_map);
	//------------------------------------------------------------------------
	//Extract each entity mention info in doc.
	for(map<string, ACE_entity>::iterator mite = ACE_Entity_Info_map.begin(); mite != ACE_Entity_Info_map.end(); mite++){
		for(vector<ACE_entity_mention>::iterator vite = mite->second.entity_mention_v.begin(); vite != mite->second.entity_mention_v.end(); vite++){
			if(EntityMentionInfo_map.find(vite->DOCID) == EntityMentionInfo_map.end()){
				EntityMentionInfo_map[vite->DOCID];
			}
			EntityMentionInfo_map[vite->DOCID].push_back(make_pair(vite->extent, &(*vite)));
		}
	}
	//------------------------------------------------------------------------
	//<DOCID, list_pair: <Sentence, <START, ENd> >
	pair<size_t, size_t> FirstMentionExtent;
	for(map<string, list<pair<string, pair<size_t,size_t>>>>::iterator mite = ACE_DocSentence_map.begin(); mite != ACE_DocSentence_map.end(); mite++){

		list<pair<string, pair<size_t,size_t>>>::iterator next_ite;
		for(list<pair<string, pair<size_t,size_t>>>::iterator sgmlite = mite->second.begin(); sgmlite != mite->second.end(); sgmlite++){
			pNE_Surround ploc_Case = new NE_Surround;
			ploc_Case->DOCID = mite->first;
			ploc_Case->SENID = Sentence_ID++;
			ploc_Case->insur = sgmlite->first;
			ploc_Case->START = sgmlite->second.first;
			ploc_Case->END = sgmlite->second.second;
			next_ite = sgmlite;
			if(++next_ite != mite->second.end()){
				ploc_Case->proxsur = next_ite->first;
			}
			else{
				ploc_Case->proxsur = "";
			}
			if(sgmlite != mite->second.begin()){
				next_ite = sgmlite;
				next_ite--;
				ploc_Case->prixsur = next_ite->first;
			}
			else{
				ploc_Case->prixsur =  "";
			}
			vector<pair<ACE_extent, ACE_entity_mention*>>& entityref_v = EntityMentionInfo_map[mite->first];//Entity mention vectior in doc;
			for(vector<pair<ACE_extent, ACE_entity_mention*>>::iterator ne_ite = entityref_v.begin(); ne_ite != entityref_v.end(); ne_ite++){
				//Check wether the entity set is accordance with original doc;
				if(strcmp(ne_ite->first.charseq.c_str(), Sentop::Get_Substr_by_Chinese_Character_Cnt(ACE_sgm_mmap[mite->first].DOC.c_str(), ne_ite->first.START, ne_ite->first.END + 1 - ne_ite->first.START).c_str())){
					ostringstream ostream;
					ostream << "文档：" << mite->first << endl << "位置：" << ne_ite->first.START<< endl<< "命名实体：" << ne_ite->first.charseq << endl;
					ostream << "文档字符：" << Sentop::Get_Substr_by_Chinese_Character_Cnt(ACE_sgm_mmap[mite->first].DOC.c_str(), ne_ite->first.START, ne_ite->first.END + 1 - ne_ite->first.START) << endl;
					if(IDYES != AppCall::Secretary_Message_Box(ostream.str(), MB_YESNOCANCEL)){
						return;
					}
					continue;
				}
				if((sgmlite->second.first  <= ne_ite->first.START) && (sgmlite->second.second >= ne_ite->first.END)){
					ploc_Case->entity_mention_v.push_back(*(ne_ite->second));
				}
			}

			NE_Surround_v.push_back(ploc_Case);

		}//for(list<pair<
	}//for(map<string,
	//ostringstream ostrsteam;
	//ostrsteam << "Named Entity Surrounding Count: " << NE_Surround_v.size();
	//AppCall::Secretary_Message_Box(ostrsteam.str(), MB_OK);
}


void NECOM::Save_of_NE_Surround_Named_Entity(const char* savepath, vector<NE_Surround*>& NE_Surround_v)
{
	ofstream out(savepath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	string sentmentionsrt;
	string namedstr;
	for(vector<NE_Surround*>::iterator vite = NE_Surround_v.begin(); vite != NE_Surround_v.end(); vite++){
		if((*vite)->insur.find('\r') != string::npos){
			AppCall::Secretary_Message_Box("Error: Save_of_NE_Surround_Named_Entity()", MB_OK);
		}
		out << (*vite)->DOCID << '\r' << (*vite)->insur << '\r' << (*vite)->prixsur << '\r' << (*vite)->proxsur << '\r';
		out << (*vite)->START << '\r' << (*vite)->END << '\r' << (*vite)->entity_mention_v.size() << '\r';
		for(size_t i = 0; i < (*vite)->entity_mention_v.size(); i++){
			ACE_entity_mention& loc_EM = (*vite)->entity_mention_v[i];
			out << loc_EM.DOCID << '\r' << loc_EM.Entity_TYPE << '\r'  << loc_EM.Entity_SUBSTYPE << '\r' << loc_EM.Entity_CLASS << '\r';
			out << loc_EM.ID << '\r' << loc_EM.TYPE << '\r' << loc_EM.LDCTYPE << 'r';
			out << loc_EM.extent.charseq << '\r' << loc_EM.extent.START << '\r' << loc_EM.extent.END << '\r';
			out << loc_EM.head.charseq << '\r' << loc_EM.head.START << '\r' << loc_EM.head.END << '\r';
		}
		out << endl;
	}
	out.close();
}

void NECOM::Load_of_NE_Surround_Named_Entity(const char* Loadpath, map<string, ACE_entity_mention*>& EntityMentionInfo_map, vector<NE_Surround*>& NE_Surround_v)
{
	char SentenceBuf[MAX_SENTENCE];
	ifstream in(Loadpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		NE_Surround* pNE_Surround = new NE_Surround;
		size_t mention_size;
		in.getline(SentenceBuf, MAX_SENTENCE, '\r');
		pNE_Surround->DOCID = SentenceBuf;
		in.getline(SentenceBuf, MAX_SENTENCE, '\r');
		pNE_Surround->insur = SentenceBuf;
		in.getline(SentenceBuf, MAX_SENTENCE, '\r');
		pNE_Surround->prixsur = SentenceBuf;
		in.getline(SentenceBuf, MAX_SENTENCE, '\r');
		pNE_Surround->proxsur = SentenceBuf;

		in >> pNE_Surround->START;
		in.getline(SentenceBuf, MAX_SENTENCE, '\r');
		in >> pNE_Surround->END;
		in.getline(SentenceBuf, MAX_SENTENCE, '\r');
		in >> mention_size;
		in.getline(SentenceBuf, MAX_SENTENCE, '\r');

		for(size_t i = 0; i < mention_size; i++){
			pACE_entity_mention loc_ME = new ACE_entity_mention;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			loc_ME->DOCID = SentenceBuf;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			loc_ME->Entity_TYPE = SentenceBuf;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			loc_ME->Entity_SUBSTYPE = SentenceBuf;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			loc_ME->Entity_CLASS = SentenceBuf;

			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			loc_ME->ID = SentenceBuf;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			loc_ME->TYPE = SentenceBuf;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			loc_ME->LDCTYPE = SentenceBuf;

			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			loc_ME->extent.charseq = SentenceBuf;
			in >> loc_ME->extent.START;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			in >> loc_ME->extent.END;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');

			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			loc_ME->head.charseq = SentenceBuf;
			in >> loc_ME->head.START;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
			in >> loc_ME->head.END;
			in.getline(SentenceBuf, MAX_SENTENCE, '\r');
		}
		NE_Surround_v.push_back(pNE_Surround);
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	}
	in.close();

}

void NECOM::Save_START_and_END_rtn_Info(const char* savepath, vector<NE_Surround*>& NE_Surround_v)
{
	ofstream out(savepath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	
	out << NE_Surround_v.size() << endl;
//	out << CEDT_Head_Flag << endl;

	out.close();
}
bool NECOM::Load_START_and_END_rtn_Info(const char* loadpath, vector<NE_Surround*>& NE_Surround_v)
{
	char SentenceBuf[MAX_SENTENCE];
	size_t Surround_Num;
	ifstream in(loadpath);
	if(in.bad())
		return false;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return false;
	}
	in >> Surround_Num;
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
//	in >> CEDT_Head_Flag;
	in.close();

	if(Surround_Num != NE_Surround_v.size()){
		AppCall::Secretary_Message_Box("Unmatched NE_Surround size...", MB_OK);
		return false;
	}
	return true;
}

