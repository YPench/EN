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
#include "CEDTInfo.h"
#include "NECom.h"

CEDTInfo::CEDTInfo()
{
}
CEDTInfo::~CEDTInfo()
{
}

void CEDTInfo::FREE(){
	for(map<size_t, map<size_t, double>*>::iterator mmite = Extend_START_mm.begin(); mmite != Extend_START_mm.end(); mmite++){
		delete mmite->second;
	}
	Extend_START_mm.clear();
	for(map<size_t, map<size_t, double>*>::iterator mmite = Extend_END_mm.begin(); mmite != Extend_END_mm.end(); mmite++){
		delete mmite->second;
	}
	Extend_END_mm.clear();
	for(map<size_t, map<size_t, double>*>::iterator mmite = Head_START_mm.begin(); mmite != Head_START_mm.end(); mmite++){
		delete mmite->second;
	}
	Head_START_mm.clear();
	for(map<size_t, map<size_t, double>*>::iterator mmite = Head_END_mm.begin(); mmite != Head_END_mm.end(); mmite++){
		delete mmite->second;
	}
	Head_END_mm.clear();
	for(size_t i = 0; i < CanditHead_v.size(); i++){
		if(CanditHead_v[i] != NULL){
			delete CanditHead_v[i];
		}
	}
	CanditHead_v.clear();
	for(size_t i = 0; i < CanditExtend_v.size(); i++){
		if(CanditExtend_v[i] != NULL){
			delete CanditExtend_v[i];
		}
	}
	CanditExtend_v.clear();
	for(size_t i = 0; i < DismCase_v.size(); i++){
		if(DismCase_v[i] != NULL){
			delete DismCase_v[i];
		}
	}
	DismCase_v.clear();
}

void CEDTInfo::Save_Boundary_Info()
{
	string Boundary_Info = DATA_FOLDER;
	Boundary_Info += "CEDT\\Boundary_Info\\";
	if(NLPOP::FolderExist(NLPOP::string2CString(Boundary_Info))){
		NLPOP::DeleteDir(NLPOP::string2CString(Boundary_Info));
		_mkdir(Boundary_Info.c_str());
	}
	else{
		_mkdir(Boundary_Info.c_str());
	}
	string savepath;

	savepath = Boundary_Info + "Extend_START_mm.dat";
	Save_Boundary_Rtn_Information(savepath.c_str(), Extend_START_mm);
	savepath = Boundary_Info + "Extend_END_mm.dat";
	Save_Boundary_Rtn_Information(savepath.c_str(), Extend_END_mm);

	savepath = Boundary_Info + "Head_START_mm.dat";
	Save_Boundary_Rtn_Information(savepath.c_str(), Head_START_mm);
	savepath = Boundary_Info + "Head_END_mm.dat";
	Save_Boundary_Rtn_Information(savepath.c_str(), Head_END_mm);

	/*savepath = Boundary_Info + "training_Surround_v.dat";
	Save_of_NE_Surround_Named_Entity(savepath.c_str(), training_Surround_v);
	savepath = Boundary_Info + "testing_Surround_v.dat";
	Save_of_NE_Surround_Named_Entity(savepath.c_str(), testing_Surround_v);*/

	//savepath = Boundary_Info + "Surround_Num.dat";
	//Save_START_and_END_rtn_Info(savepath.c_str(), training_Surround_v);

}
bool CEDTInfo::Load_Boundary_Info(ACE_Corpus& pmACE_Corpus)
{
	string Boundary_Info = DATA_FOLDER;
	Boundary_Info += "CEDT\\Boundary_Info\\";
	if(!NLPOP::FolderExist(NLPOP::string2CString(Boundary_Info))){
		return false;
	}
	if(IDYES == AppCall::Secretary_Message_Box("目录已经存在，是否需要更新？", MB_YESNOCANCEL)){
		NLPOP::DeleteDir(NLPOP::string2CString(Boundary_Info));
		return false;
	}
	/*if(!training_Surround_v.empty() || !testing_Surround_v.empty()){
		AppCall::Secretary_Message_Box("数据已经加载...", MB_YESNOCANCEL);
		return false;
	}*/
	AppCall::Maxen_Responce_Message("\n\nLoad boundary Information...\n");
	string loadpath;
	loadpath = Boundary_Info + "Surround_Num.dat";
	//if(!NECOM::Load_START_and_END_rtn_Info(loadpath.c_str(), training_Surround_v)){
	//	return false;
	//}

	loadpath = Boundary_Info + "Extend_START_mm.dat";
	Load_Boundary_Rtn_Information(loadpath.c_str(), Extend_START_mm);
	loadpath = Boundary_Info + "Extend_END_mm.dat";
	Load_Boundary_Rtn_Information(loadpath.c_str(), Extend_END_mm);

	loadpath = Boundary_Info + "Head_START_mm.dat";
	Load_Boundary_Rtn_Information(loadpath.c_str(), Head_START_mm);
	loadpath = Boundary_Info + "Head_END_mm.dat";
	Load_Boundary_Rtn_Information(loadpath.c_str(), Head_END_mm);

	/*map<string, ACE_entity>& ACE_Entity_Info_map = pmACE_Corpus.ACE_Entity_Info_map;
	map<string, ACE_entity_mention*> EntityMentionInfo_map;
	for(map<string, ACE_entity>::iterator mite = ACE_Entity_Info_map.begin(); mite != ACE_Entity_Info_map.end(); mite++){
		for(vector<ACE_entity_mention>::iterator vite = mite->second.entity_mention_v.begin(); vite != mite->second.entity_mention_v.end(); vite++){
			if(EntityMentionInfo_map.find(vite->ID) == EntityMentionInfo_map.end()){
				EntityMentionInfo_map[vite->ID];
				EntityMentionInfo_map[vite->ID] = &(*vite);
			}
			else{
				AppCall::Secretary_Message_Box("重复数据in Load_Boundary_Info", MB_OK);
			}
		}
	}*/
	/*loadpath = Boundary_Info + "training_Surround_v.dat";
	Load_of_NE_Surround_Named_Entity(loadpath.c_str(), EntityMentionInfo_map, training_Surround_v);
	loadpath = Boundary_Info + "testing_Surround_v.dat";
	Load_of_NE_Surround_Named_Entity(loadpath.c_str(), EntityMentionInfo_map, testing_Surround_v);*/
	return true;
}

void CEDTInfo::Save_Boundary_Rtn_Information(const char* pmFilePath, map<size_t, map<size_t, double>*>& pmRtn_mm)
{
	ofstream out(pmFilePath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	for(map<size_t, map<size_t, double>*>::iterator mmite = pmRtn_mm.begin(); mmite != pmRtn_mm.end(); mmite++){
		out << mmite->first << ' ' << mmite->second->size() << '\n';
		for(map<size_t, double>::iterator mite = mmite->second->begin(); mite != mmite->second->end(); mite++){
			out << mite->first << ' ' << mite->second << ' ';
		}
		out << '\n';
	}
	out.close();

}
void CEDTInfo::Load_Boundary_Rtn_Information(const char* pmFilePath, map<size_t, map<size_t, double>*>& pmRtn_mm)
{
	char SentenceBuf[MAX_SENTENCE];
	ifstream in(pmFilePath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	size_t positsize;
	size_t SENID;
	size_t position;
	double value;
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		istringstream sstream(SentenceBuf);
		sstream >> SENID;
		sstream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		sstream >> positsize;

		pmRtn_mm[SENID];
		pmRtn_mm[SENID] = new map<size_t, double>;

		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		istringstream _sstream(SentenceBuf);

		for(size_t i = 0; i < positsize; i++){
			_sstream >> position;
			_sstream.getline(SentenceBuf, MAX_SENTENCE, ' ');
			_sstream >> value;
			_sstream.getline(SentenceBuf, MAX_SENTENCE, ' ');
			pmRtn_mm[SENID]->insert(make_pair(position, value));
		}
	}
	in.close();
}

void CEDTInfo::Save_Named_Entity_Candit_DismCase(const char* savename, vector<CanditCase*>& CanditCase_v)
{
	ofstream out(savename);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	for(vector<CanditCase*>::iterator vmeite = CanditCase_v.begin(); vmeite != CanditCase_v.end(); vmeite++){
		CanditCase& loc_Candit = **vmeite;
		/*out << loc_Candit.CASID << '\t' << loc_Candit.SENID << '\t' << loc_Candit.position << '\t' << loc_Candit.Cand_Flag << '\t'
			<< loc_Candit.l_outmstr << '\t' << loc_Candit.r_outmstr << '\t' 
			<< loc_Candit.prixstr << '\t' << loc_Candit.proxstr << '\t' << loc_Candit.mentionstr << '\t';
			if(loc_Candit.Cand_Flag){
				out << loc_Candit.pNE_mention->ID << '\r';
			}
			else{
				out << "" << '\r';
			}*/
	}

	out.close();
}

void CEDTInfo::Load_Named_Entity_Candit_DismCase(const char* openname, vector<CanditCase*>& CanditCase_v, map<string, ACE_entity>& ACE_Entity_Info_map)
{
	map<string, ACE_entity_mention*> NE_Mention_ID_m;
	for(map<string, ACE_entity>::iterator mite = ACE_Entity_Info_map.begin(); mite != ACE_Entity_Info_map.end(); mite++){
		for(vector<ACE_entity_mention>::iterator vite = mite->second.entity_mention_v.begin(); vite != mite->second.entity_mention_v.end(); vite++){
			if(NE_Mention_ID_m.find(vite->ID) == NE_Mention_ID_m.end()){
				NE_Mention_ID_m.insert(make_pair(vite->ID, &*vite));
			}
			else{
				AppCall::Secretary_Message_Box("Dupilicated Entity Mention information...", MB_OK);
			}
		}
	}
	char SentenceBuf[MAX_SENTENCE];
	ifstream in(openname);

	if(in.bad()){
		AppCall::Secretary_Message_Box("打开文件出错：ace_op::Load_Named_Entity_Candit_DismCase()", MB_OK);
	}
	in.clear();
	in.seekg(0, ios::beg);
	
	while(true){
		if(in.peek() == EOF){	
			break;
		}
		Relation_Case loc_RCase;
		in.getline(SentenceBuf, MAX_SENTENCE, '\r');
		istringstream sstream(SentenceBuf);

		CanditCase* pCanditCase = new CanditCase;
		
		/*sstream >> pDismCase->CASID;
		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		sstream >> pDismCase->SENID;
		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		sstream >> pDismCase->position;
		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		sstream >> pDismCase->Cand_Flag;
		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');

		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		pDismCase->l_outmstr = SentenceBuf;
		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		pDismCase->r_outmstr = SentenceBuf;

		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		pDismCase->prixstr = SentenceBuf;
		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		pDismCase->proxstr = SentenceBuf;
		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		pDismCase->mentionstr = SentenceBuf;

		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');

		if(strlen(SentenceBuf) == 0){
			pDismCase->pNE_mention = NULL;

		}else if(NE_Mention_ID_m.find(SentenceBuf) == NE_Mention_ID_m.end()){
			AppCall::Secretary_Message_Box("Missing of Entity Mention information...", MB_OK);
			return;
		}
		else{
			pDismCase->pNE_mention = NE_Mention_ID_m[SentenceBuf];
		}
		CanditCase_v.push_back(pDismCase);*/
	}
	in.close();

}