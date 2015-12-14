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

extern bool CEDT_Detection_Flag;
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



void NECOM::NE_Surround_Relative_Position_Calculate(vector<NE_Surround*>& NE_Surround_v)
{
	size_t offset;
	for(vector<NE_Surround*>::iterator lite = NE_Surround_v.begin(); lite != NE_Surround_v.end();lite++){
		offset = (*lite)->START;
		(*lite)->START = 0;
		(*lite)->END = (*lite)->END - offset;
		for(vector<ACE_entity_mention>::iterator pvite = (*lite)->entity_mention_v.begin(); pvite != (*lite)->entity_mention_v.end(); pvite++){
			pvite->extent.START = pvite->extent.START - offset;
			pvite->extent.END = pvite->extent.END - offset;
			pvite->head.START = pvite->head.START - offset;
			pvite->head.END = pvite->head.END - offset;
		}
	}
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