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
#include "Act.h"
#include "ENCom.h"
#include "ConvertUTF.h"
#include "..\\Consensus.h"
CAct::CAct()
{
}
CAct::~CAct()
{

}
void CAct::Action_Analysis_Port(CENET_Dlg* p_m_CENET_Dlg)
{
	string modelpath = ENET_FOLDER;
	modelpath += "model";
	if(!NLPOP::Exist_of_This_File(modelpath.c_str())){
		Generating_Maxen_Model(p_m_CENET_Dlg);
	}

	string Action_File = ENET_FOLDER;
	Action_File += "Event-0-024.ACTION";

	if(!NLPOP::Exist_of_This_File(Action_File.c_str())){
		Generating_ACTION_Documents_in_Events (p_m_CENET_Dlg);
	}


	string Analysis_File = ENET_FOLDER;
	Analysis_File += "ActionAnalysis.ACTION";
	if(!NLPOP::Exist_of_This_File(Analysis_File.c_str())){
		Generating_Action_Analysis_File(Action_File.c_str(), Analysis_File.c_str());
	}

	p_m_CENET_Dlg->Collected_EventName = "ActionAnalysis";
	p_m_CENET_Dlg->Collected_EventPath = Action_File;
	p_m_CENET_Dlg->Collected_KeyWords = "";
	p_m_CENET_Dlg->Collected_FilePrix = Analysis_File.substr(0, Analysis_File.rfind('.'));
	p_m_CENET_Dlg->m_REL_TYPE_s.insert("A");

	p_m_CENET_Dlg->m_CENET.Collected_EventName = "ActionAnalysis";
	p_m_CENET_Dlg->m_CENET.Collected_EventPath = Action_File;
	p_m_CENET_Dlg->m_CENET.Collected_KeyWords = "";
	p_m_CENET_Dlg->m_CENET.Collected_FilePrix = Analysis_File.substr(0, Analysis_File.rfind('.'));
	p_m_CENET_Dlg->m_CENET.Updata_Configurations();


	string ENTITY_PAJ_File;
	ENTITY_PAJ_File = p_m_CENET_Dlg->Collected_FilePrix;
	ENTITY_PAJ_File += "_entity.paj";
	string ENTITY_File = ENET_FOLDER;
	ENTITY_File += "Event-0-024.ENTITY";
	if(!NLPOP::Exist_of_This_File(ENTITY_PAJ_File.c_str())){
		Generating_Action_Analysis_ENTITY_pajek(ENTITY_File.c_str(), Analysis_File.c_str(), ENTITY_PAJ_File.c_str(), p_m_CENET_Dlg);
	}

	string RELATION_PAJ_File;
	RELATION_PAJ_File = p_m_CENET_Dlg->Collected_FilePrix;
	RELATION_PAJ_File += "_relation.paj";

	if(!NLPOP::Exist_of_This_File(RELATION_PAJ_File.c_str())){
		Generating_Action_Analysis_RELATION_pajek(Analysis_File.c_str(), RELATION_PAJ_File.c_str(), p_m_CENET_Dlg);
	}
	
	p_m_CENET_Dlg->m_FilterRelation_s.insert("A");
	p_m_CENET_Dlg->m_CENET.m_ENGen.Generating_and_Viewing_Event_Network(p_m_CENET_Dlg);
	
}

void CAct::Generating_Action_Analysis_File(const char* cLoadPath, const char* cSavePath)
{
	list<ACE_Action*> Actions_l;
	Load_Action_Documents(cLoadPath, Actions_l);

	size_t Conflict_Entity_Cnt = 0;
	for(list<ACE_Action*>::iterator lite = Actions_l.begin(); lite != Actions_l.end(); ){
		ACE_Action& loc_case = **lite;
		if(loc_case.value < 1.0){
			delete *lite;
			lite = Actions_l.erase(lite);
		}
		else{
			Conflict_Entity_Cnt += (*lite)->entity_mention_l.size();
			lite++;
		}
	}
	map<string, map<string, size_t>> EntityCnt_mm;
	for(list<ACE_Action*>::iterator lite = Actions_l.begin(); lite != Actions_l.end(); lite++){
		list<string>& loc_l = (*lite)->entity_mention_l;
		for(list<string>::iterator isti = loc_l.begin(); isti != loc_l.end(); isti++){
			for(list<string>::iterator istj = loc_l.begin(); istj != loc_l.end(); istj++){
				if(isti == istj){
					continue;
				}
				if(EntityCnt_mm.find(isti->data()) == EntityCnt_mm.end()){
					EntityCnt_mm[isti->data()];
				}
				if(EntityCnt_mm[isti->data()].find(istj->data()) == EntityCnt_mm[isti->data()].end()){
					EntityCnt_mm[isti->data()].insert(make_pair(istj->data(), 0));
				}
				EntityCnt_mm[isti->data()][istj->data()]++;
			}
		}
	}
	size_t OccurEntityCnt = 0;
	set<string> vertexname_s;
	for(map<string, map<string, size_t>>::iterator mmite = EntityCnt_mm.begin(); mmite != EntityCnt_mm.end(); ){
		map<string, size_t>& loc_map = mmite->second;
		for(map<string, size_t>::iterator mite = loc_map.begin(); mite != loc_map.end(); ){
			if(mite->second < 12){
				mite = loc_map.erase(mite);
			}
			else{
				vertexname_s.insert(mite->first);
				mite++;
				OccurEntityCnt++;
				
			}
		}
		if(loc_map.empty()){
			mmite = EntityCnt_mm.erase(mmite);
		}
		else{
			vertexname_s.insert(mmite->first);
			mmite++;
		}
	}

	for(list<ACE_Action*>::iterator actlite = Actions_l.begin(); actlite != Actions_l.end(); ){
		ACE_Action& loc_Action = **actlite;
		list<string>& loc_l = loc_Action.entity_mention_l;
		for(list<string>::iterator lite = loc_l.begin(); lite != loc_l.end(); ){
			if(vertexname_s.find(lite->data()) == vertexname_s.end()){
				lite = loc_l.erase(lite);
			}
			else{
				lite++;
			}
		}
		if(loc_l.empty()){
			delete *actlite;
			actlite = Actions_l.erase(actlite);
		}
		else{
			actlite++;
		}
	}

	ofstream out(cSavePath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
		//---------------------------------------------------------------
	for(list<ACE_Action*>::iterator actlite = Actions_l.begin(); actlite != Actions_l.end(); ){
		ACE_Action& loc_Action = **actlite;
		out << loc_Action.DOCID << '\t';
		out << loc_Action.CASID << '\t';
		out << loc_Action.SENID << '\t';
		out << loc_Action.TYPE << '\t';
		out << loc_Action.value << endl;

		out << loc_Action.charseq << endl;
		out << loc_Action.entity_mention_l.size() << endl;
		for(list<string>::iterator lite = loc_Action.entity_mention_l.begin(); lite != loc_Action.entity_mention_l.end(); lite++){
			out << lite->data() << endl;
		}
		delete *actlite;
		actlite = Actions_l.erase(actlite);
	}
	out.close();
}


void CAct::Generating_Action_Analysis_ENTITY_pajek(const char* cENTITYPath, const char* cLoadPath, const char* cSavePath, CENET_Dlg* p_m_CENET_Dlg)
{

	list<ACE_Action*> Actions_l;
	Load_Action_Documents(cLoadPath, Actions_l);

	vector<entity_mention*> Entities_v;
	ENCOM::Load_Extracted_Entities_Result(cENTITYPath, Entities_v);
	
	set<string> EntityMention_s;
	for(list<ACE_Action*>::iterator actlite = Actions_l.begin(); actlite != Actions_l.end(); ){
		ACE_Action& loc_Action = **actlite;
		list<string>& loc_l = loc_Action.entity_mention_l;
		for(list<string>::iterator lite = loc_l.begin(); lite != loc_l.end(); lite++){
			EntityMention_s.insert(lite->data());
		}
		delete *actlite;
		actlite = Actions_l.erase(actlite);
	}


	map<string, map<size_t, map<size_t, entity_mention*>>> DOC_mmm;
	for(size_t i = 0; i < Entities_v.size(); i++){
		entity_mention& loc_entity = *Entities_v[i];
		if(EntityMention_s.find(loc_entity.extent.charseq) == EntityMention_s.end()){
			delete Entities_v[i];
			continue;
		}

		if(DOC_mmm.find(loc_entity.DOCID) == DOC_mmm.end()){
			DOC_mmm[loc_entity.DOCID];
		}
		map<size_t, map<size_t, entity_mention*>>& SEN_mm = DOC_mmm[loc_entity.DOCID];
		if(SEN_mm.find(loc_entity.SENID) == SEN_mm.end()){
			SEN_mm[loc_entity.SENID];
		}
		map<size_t, entity_mention*>& CAS_m = SEN_mm[loc_entity.SENID];
		if(CAS_m.find(loc_entity.CASID) == CAS_m.end()){
			CAS_m.insert(make_pair(loc_entity.CASID, Entities_v[i]));
		}
		else{
			AppCall::Secretary_Message_Box("Data Error in: CAct::Generating_Action_Analysis_ENTITY_pajek()");
		}
	}
	Entities_v.clear();

	string ENTITY_File = p_m_CENET_Dlg->Collected_FilePrix;
	ENTITY_File += ".ENTITY";

	ofstream out(ENTITY_File.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	for(map<string, map<size_t, map<size_t, entity_mention*>>>::iterator mmmite = DOC_mmm.begin(); mmmite != DOC_mmm.end(); mmmite++){
		map<size_t, map<size_t, entity_mention*>>& SEN_mm = mmmite->second;
		for(map<size_t, map<size_t, entity_mention*>>::iterator mmite = SEN_mm.begin(); mmite != SEN_mm.end(); mmite++){
			map<size_t, entity_mention*>& CAS_m = mmite->second;
			out << "" << '\t' << CAS_m.size() << endl;
			for(map<size_t, entity_mention*>::iterator mite = CAS_m.begin();  mite != CAS_m.end(); mite++){
				entity_mention& loc_case = *mite->second;
				out << loc_case.DOCID << '\t' << loc_case.CASID << '\t' << loc_case.SENID << '\t' << loc_case.TYPE << '\t';
				out << loc_case.SUBTYPE << '\t' << loc_case.TYPE_V << '\t' << loc_case.SUBTYPE_V << '\t';
				out << loc_case.extent.START << '\t' << loc_case.extent.END << '\t' << loc_case.extent.charseq << '\t';
				out << loc_case.head.START << '\t' << loc_case.head.END << '\t' << loc_case.head.charseq;
				out << endl;
				delete mite->second;
			}
		}
	}
	out.close();

	p_m_CENET_Dlg->m_CENET.m_ENGen.Generating_Pajek_of_Named_Entity_Info(p_m_CENET_Dlg);
}


void CAct::Generating_Action_Analysis_RELATION_pajek(const char* cLoadPath, const char* cSavePath, CENET_Dlg* p_m_CENET_Dlg)
{
	list<ACE_Action*> Actions_l;
	Load_Action_Documents(cLoadPath, Actions_l);

	//string ENTITY_File = p_m_CENET_Dlg->Collected_FilePrix;
	//ENTITY_File += ".ENTITY";
	//vector<entity_mention*> Entities_v;
	//ENCOM::Load_Extracted_Entities_Result(ENTITY_File.c_str(), Entities_v);

	map<string, map<string, size_t>> EntityCnt_mm;
	for(list<ACE_Action*>::iterator lite = Actions_l.begin(); lite != Actions_l.end(); ){
		list<string>& loc_l = (*lite)->entity_mention_l;
		for(list<string>::iterator isti = loc_l.begin(); isti != loc_l.end(); isti++){
			for(list<string>::iterator istj = loc_l.begin(); istj != loc_l.end(); istj++){
				if(isti == istj){
					continue;
				}
				if(EntityCnt_mm.find(isti->data()) == EntityCnt_mm.end()){
					EntityCnt_mm[isti->data()];
				}
				if(EntityCnt_mm[isti->data()].find(istj->data()) == EntityCnt_mm[isti->data()].end()){
					EntityCnt_mm[isti->data()].insert(make_pair(istj->data(), 0));
				}
				EntityCnt_mm[isti->data()][istj->data()]++;
			}
		}
		lite = Actions_l.erase(lite);
	}
	//-----------------------------------------
	string ENTITY_File = p_m_CENET_Dlg->Collected_FilePrix;
	ENTITY_File += ".RELATION";
	ofstream out(ENTITY_File.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(map<string, map<string, size_t>>::iterator mmite = EntityCnt_mm.begin(); mmite != EntityCnt_mm.end(); mmite++){
		map<string, size_t>& loc_map = mmite->second;
		for(map<string, size_t>::iterator mite = loc_map.begin(); mite != loc_map.end(); mite++){
			out << '\t' << mmite->first << '\t' << mite->first;
			out << '\t' << "A" << '\t' << 1.0;
			out << '\t' << 0 << '\t' << 0 << endl;
			out << "Undef" << endl;
			out << "Undef" << endl;
			out << 0 << endl;
		}
	}
	out.close();

	p_m_CENET_Dlg->m_CENET.m_ENGen.Generating_Pajek_of_Relation_Info(p_m_CENET_Dlg);
	//p_m_CENET_Dlg->Collected_Doc_Event_Flag = false;

	//Action_File = p_m_CENET_Dlg->Collected_FilePrix;
	//Action_File += "_relation.paj";
}
void CAct::Load_Action_Documents(const char* cPath, list<ACE_Action*>& Actions_l)
{
	char SentenceBuf[MAX_SENTENCE];

	ifstream in(cPath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(in.peek() != EOF){
		size_t entitycnt;
		ACE_Action* plocAction = new ACE_Action;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		istringstream instream(SentenceBuf);
		instream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		plocAction->DOCID = SentenceBuf;
		instream >> plocAction->CASID;
		instream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		instream >> plocAction->SENID;
		instream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		instream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		plocAction->TYPE = SentenceBuf;
		instream >> plocAction->value;

		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		plocAction->charseq = SentenceBuf;
		in >> entitycnt;
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		list<string>& loc_l = plocAction->entity_mention_l;
		for(size_t i = 0; i < entitycnt; i++){
			in.getline(SentenceBuf, MAX_SENTENCE, '\n');
			loc_l.push_back(SentenceBuf);
		}
		Actions_l.push_back(plocAction);
	}

	in.close();
}


void CAct::Generating_ACTION_Documents_in_Events (CENET_Dlg* p_m_CENET_Dlg)
{

	MaxentModel loc_Maxen;
	string modelpath = ENET_FOLDER;
	modelpath += "model";

	loc_Maxen.load(modelpath.c_str());
	set<string> Anchor_s;
	modelpath = ENET_FOLDER;
	modelpath += "anchors.dat";
	NLPOP::LoadWordsStringSet(modelpath.c_str(), Anchor_s);
	//-------------------------------------------------------
	set<string> m_WordSpace;
	size_t Max_Lexicon_Size;
	Init_Lexicon(DATA_FOLDER, m_WordSpace, Max_Lexicon_Size);
	CSegmter m_CSegmter;
	m_CSegmter.Max_Lexicon_Size = Max_Lexicon_Size;

	vector<string> ENTITY_Path_v;
	NLPOP::Get_Specified_Files(EVENT_ORGANIZATION_FOLDER, ENTITY_Path_v, ".ENTITY");
	for(size_t i = 0; i < ENTITY_Path_v.size(); i++){
		string entityname = ENTITY_Path_v[i];
		entityname = entityname.substr(entityname.rfind('\\')+1, entityname.length()-entityname.rfind('\\')+1);
		entityname = entityname.substr(0, entityname.rfind('.'));

		ostringstream ostream;
		ostream << "Active Analysis..." << endl;
		ostream << "Processing Event: " << entityname << " (" << i+1 << "/" <<  ENTITY_Path_v.size() << ")" << endl;
		ostream << endl;
		AppCall::Maxen_Responce_Message(ostream.str().c_str());

		entityname = ENTITY_Path_v[i].substr(0, ENTITY_Path_v[i].rfind('.'));
		entityname += ".ACTION";
		ofstream out(entityname.c_str());
		ifstream in(ENTITY_Path_v[i].c_str());
		if(in.bad())
			return;
		in.clear();
		in.seekg(0, ios::beg);
		if(in.peek() == EOF){
			in.close();
			return;
		}
		if(out.bad())
			return;
		out.clear();
		out.seekp(0, ios::beg);
		//---------------------------------------------------------------
		vector<entity_mention> entity_mention_v;
		string SentMention;
		while(ENCOM::Get_Next_Entity_Mention(in, entity_mention_v, SentMention)){
			if(entity_mention_v.empty()){
				AppCall::Secretary_Message_Box("Data Error in: CAct::Action_Analysis_Port()");
			}
			out << entity_mention_v.begin()->DOCID << '\t';
			out << entity_mention_v.begin()->CASID << '\t';
			out << entity_mention_v.begin()->SENID << '\t';
			out << POSITIVE << '\t';
			bool Found_Anchor_Flag = false;
			map<string, size_t> WordsCnt_map;
			size_t sentsize = strlen(SentMention.c_str());
			sentsize += 1;
			char* psentchar = new char[sentsize];
			ace_op::Delet_0AH_and_20H_in_string(SentMention.c_str(), psentchar, sentsize);
			
			for(set<string>::iterator site = Anchor_s.begin(); site != Anchor_s.end(); site++){
				if(NLPOP::Find_Substr_In_Chinese_Sent(psentchar, site->data())){
					Found_Anchor_Flag = true;
					break;
				}
			}
			if(Found_Anchor_Flag){
				m_CSegmter.Omni_words_feature_Extracting(psentchar, m_WordSpace, WordsCnt_map);
				vector<pair<string, float>> loc_features_v;
				for(map<string, size_t>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
					loc_features_v.push_back(make_pair(mite->first, (float)(mite->second)));
				}
				out << loc_Maxen.eval(loc_features_v, POSITIVE);
			}
			else{
				out << 0;
			}
			out << endl;
			out << psentchar << endl;
			out << entity_mention_v.size() << endl;
			for(size_t j = 0; j < entity_mention_v.size(); j++){
				out << entity_mention_v[j].extent.charseq << endl;
			}

			delete psentchar;
			entity_mention_v.clear();
		}
		out.close();
		in.close();

	}

}

void CAct::Generating_Maxen_Model(CENET_Dlg* p_m_CENET_Dlg)
{
	AppCall::Maxen_Responce_Message("Begin of \"CAct::Action_Analysis\"...\n\n");
	string TrainingCaseFilePath = ENET_FOLDER;
	TrainingCaseFilePath += "Training.dat";

	//if(!NLPOP::Exist_of_This_File(TrainingCaseFilePath.c_str())){
		AppCall::Maxen_Responce_Message("Generating Training Data...\n\n");
		Generating_Training_Data(TrainingCaseFilePath.c_str(), p_m_CENET_Dlg);
	//}
	FeatureVctor Training_v;
	Training_v.reserve(300000);

	MAXEN::Read_CSmaxent_Training_Data(TrainingCaseFilePath.c_str(), true, Training_v);
	
	if(false){
		MAXEN::cross_validation_with_Weight(Training_v, 5, 30, "lbfgs", 0, true, 1.0, POSITIVE);
		//MAXEN::cross_validation(Training_v, 5, 30, "lbfgs", 0, true);
	}
	else{
		//--------------------------------------
		MaxentModel loc_Maxen;
		MAXEN::Maxen_Training_with_Erasing(Training_v, loc_Maxen, 30);
		string modelpath = ENET_FOLDER;
		modelpath += "model";
		loc_Maxen.save(modelpath.c_str(), true);
		//----------------------------------------------
	}	
	for(size_t i = 0; i < Training_v.size(); i++){
		delete Training_v[i];
	}
	AppCall::Maxen_Responce_Message("End of \"CAct::Action_Analysis\"...\n\n");
}
void CAct::Generating_Training_Data(const char* cPath, CENET_Dlg* p_m_CENET_Dlg)
{
	DulTYPE_Vctor Training_v;
	Training_v.reserve(300000);

	set<string> m_WordSpace;
	size_t Max_Lexicon_Size;
	Init_Lexicon(DATA_FOLDER, m_WordSpace, Max_Lexicon_Size);
	CSegmter m_CSegmter;
	m_CSegmter.Max_Lexicon_Size = Max_Lexicon_Size;

	SXMLer loc_SXMLer;
	ACE_Corpus& loc_ACE_Corpus = p_m_CENET_Dlg->p_CParentDlg->m_ACE_Corpus;
	map<string, ACE_Event>& ACE_EventInfo_m = loc_ACE_Corpus.ACE_EventInfo_m;

	loc_SXMLer.Xercesc_ACE_Action_Mention_Extractor(p_m_CENET_Dlg->ACECorpusFolder.c_str(), loc_ACE_Corpus);
	loc_SXMLer.Xercesc_ACE_Folder_For_sgm_Files_Filter(p_m_CENET_Dlg->ACECorpusFolder.c_str(), loc_ACE_Corpus);

	map<string, list<pair<string, pair<size_t,size_t>>>> ACE_DocSentence_map;//<DOCID, list_pair: <Sentence, (START, ENd);>
	Generating_Negetive_Cases(loc_ACE_Corpus, ACE_DocSentence_map);
		
	vector<pair<string, string>> EventCases_v;
	Generating_Training_Cases(loc_ACE_Corpus, ACE_DocSentence_map, EventCases_v);

	for(size_t i = 0; i < EventCases_v.size(); i++){
		//if(8 == i){
		//	bool check_point = false;
		//}
		map<string, size_t> WordsCnt_map;
		size_t sentsize = strlen(EventCases_v[i].first.c_str());
		sentsize += 1;
		char* psentchar = new char[sentsize];
		ace_op::Delet_0AH_and_20H_in_string(EventCases_v[i].first.c_str(), psentchar, sentsize);
		m_CSegmter.Omni_words_feature_Extracting(psentchar, m_WordSpace, WordsCnt_map);
		delete psentchar;
		if(WordsCnt_map.empty()){
			continue;
		}
		pair<pair<string, string>, vector<pair<string, float>>>* p_Case_v = new pair<pair<string, string>, vector<pair<string, float>>>;
		p_Case_v->first.first  = EventCases_v[i].second;
		p_Case_v->first.second = EventCases_v[i].second;
		for(map<string, size_t>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
			p_Case_v->second.push_back(make_pair(mite->first, (float)(mite->second)));
		}
		Training_v.push_back(p_Case_v);
	}
	
	MAXEN::Output_Extract_Feature_Context_For_Maxent(cPath, Training_v);

	for(size_t i = 0; i < Training_v.size(); i++){
		delete Training_v[i];
	}
}

void CAct::Generating_Training_Cases(ACE_Corpus& pmACE_Corpus, map<string, list<pair<string, pair<size_t,size_t>>>>& ACE_DocSentence_map, vector<pair<string, string>>& EventCases_v)
{
	set<string> anchor_s;
	map<string, ACE_Event>& ACE_EventInfo_m = pmACE_Corpus.ACE_EventInfo_m;
	for(map<string, ACE_Event>::iterator mite = ACE_EventInfo_m.begin(); mite != ACE_EventInfo_m.end(); mite++){
		ACE_Event& loc_ACE_Event = mite->second;
		bool POSITIVE_Flag = false;
		//if(!strcmp("Injure", loc_ACE_Event.TYPE.c_str()) || !strcmp("Die", loc_ACE_Event.TYPE.c_str()) || !strcmp("Conflict", loc_ACE_Event.TYPE.c_str()) ||
		//	!strcmp("Justice", loc_ACE_Event.TYPE.c_str())){
		if(!strcmp("Conflict", loc_ACE_Event.TYPE.c_str())){
			POSITIVE_Flag = true;
		}
		vector<event_mention>& loc_v = mite->second.event_mention_v;
		for(size_t i = 0; i < loc_v.size(); i++){
			if(POSITIVE_Flag){
				anchor_s.insert(ace_op::Delet_0AH_and_20H_in_string(loc_v[i].anchor.charseq.c_str()));
				EventCases_v.push_back(make_pair(loc_v[i].extent.charseq, POSITIVE));
			}
			else{
				//EventCases_v.push_back(make_pair(loc_v[i].extent.charseq, NEGETIVE));
			}
		}
	}
	string anchorpath = ENET_FOLDER;
	anchorpath += "anchors.dat";
	NLPOP::Save_Set_String_With_Comma_Divide(anchorpath.c_str(), anchor_s);
	for(map<string, list<pair<string, pair<size_t,size_t>>>>::iterator mlite = ACE_DocSentence_map.begin(); mlite != ACE_DocSentence_map.end(); mlite++){
		for(list<pair<string, pair<size_t,size_t>>>::iterator lite = mlite->second.begin(); lite != mlite->second.end(); lite++){
			bool Found_Anchor_Flag = false;
			for(set<string>::iterator site = anchor_s.begin(); site != anchor_s.end(); site++){
				if(NLPOP::Find_Substr_In_Chinese_Sent(lite->first.c_str(), site->data())){
					Found_Anchor_Flag = true;
					break;
				}
			}
			if(Found_Anchor_Flag){
				EventCases_v.push_back(make_pair(lite->first, NEGETIVE));
			}
		}
	}
}

void CAct::Generating_Negetive_Cases(ACE_Corpus& pmACE_Corpus, map<string, list<pair<string, pair<size_t,size_t>>>>& ACE_DocSentence_map){
	map<string, ACE_sgm>& ACE_sgm_mmap = pmACE_Corpus.ACE_sgm_mmap;
	map<string, ACE_Event>& ACE_EventInfo_m = pmACE_Corpus.ACE_EventInfo_m;

	ace_op::Segment_ACE_sgm_to_Sentence_Among_TEXT(ACE_sgm_mmap, ACE_DocSentence_map);
	map<string, map<size_t, size_t>> DocEventInfo_mm;
	for(map<string, ACE_Event>::iterator mite = ACE_EventInfo_m.begin(); mite != ACE_EventInfo_m.end(); mite++){
		if(DocEventInfo_mm.find(mite->second.DOCID) == DocEventInfo_mm.end()){
			DocEventInfo_mm[mite->second.DOCID];
		}
		map<size_t, size_t>& DocInnerInfo_m = DocEventInfo_mm[mite->second.DOCID];
		vector<event_mention>& loc_v = mite->second.event_mention_v;
		for(size_t i = 0; i < loc_v.size(); i++){
			if(DocInnerInfo_m.find(loc_v[i].extent.START) != DocInnerInfo_m.end()){
				DocInnerInfo_m.insert(make_pair(loc_v[i].extent.START, loc_v[i].extent.END));
			}
			else if(DocInnerInfo_m[loc_v[i].extent.START] < loc_v[i].extent.END){
				DocInnerInfo_m[loc_v[i].extent.START] = loc_v[i].extent.END;
			}
		}
	}

	for(map<string, list<pair<string, pair<size_t,size_t>>>>::iterator mlite = ACE_DocSentence_map.begin();  mlite != ACE_DocSentence_map.end(); ){
		if(DocEventInfo_mm.find(mlite->first) == DocEventInfo_mm.end()){
			mlite++;
			continue;
		}
		map<size_t, size_t>& loc_DocInfo = DocEventInfo_mm[mlite->first];
		for(list<pair<string, pair<size_t,size_t>>>::iterator lite = mlite->second.begin(); lite != mlite->second.end(); ){
			pair<size_t,size_t>& loc_pair = lite->second;
			bool Overlapping_Flag = false;
			for(map<size_t, size_t>::iterator mite = loc_DocInfo.begin(); mite != loc_DocInfo.end(); mite++){
				if((loc_pair.first >= mite->first && loc_pair.first <= mite->second) || 
				   (mite->first >= loc_pair.first && mite->first <= loc_pair.second)){
					Overlapping_Flag = true;
					break;
				}
			}
			if(Overlapping_Flag){
				lite = mlite->second.erase(lite);
			}
			else{
				lite++;
			}
		}
		if(mlite->second.empty()){
			mlite = ACE_DocSentence_map.erase(mlite);
		}
		else{
			mlite++;
		}
	}

}

void CAct::Init_Lexicon(const char* leciconfolder, set<string>& m_WordSpace, size_t& Max_Lexicon_Size)
{
	string lexiconpath;
	Max_Lexicon_Size = 0;

	lexiconpath = leciconfolder;
	lexiconpath += "LCWCC.dat";
	if(!NLPOP::Exist_of_This_File(lexiconpath)){
		AppCall::Secretary_Message_Box("Lexicon of CVMC is not existing...", MB_OK);
	}
	NLPOP::LoadWordsStringSet(lexiconpath, m_WordSpace);

	lexiconpath = DATA_FOLDER;
	lexiconpath += "CRDC\\ICTCLAS_Lexicon.dat";
	if(!NLPOP::Exist_of_This_File(lexiconpath)){
		AppCall::Secretary_Message_Box("Lexicon of ICTCLAS_Lexicon is not existing...", MB_OK);
	}
	NLPOP::LoadWordsStringSet(lexiconpath, m_WordSpace);

	for(set<string>::iterator site = m_WordSpace.begin();  site != m_WordSpace.end(); site++){
		if( site->size() > Max_Lexicon_Size){
			Max_Lexicon_Size = site->size();
		}
	}
}

