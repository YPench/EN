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
#include "ENRun.h"
#include "ENCom.h"
#include "..\\Consensus.h"

CENRun::CENRun()
{
}
CENRun::~CENRun()
{

}
void CENRun::Extracting_Person_Location_Time_Analysis_RELATION(CENET_Dlg* p_m_CENET_Dlg)
{
	vector<string> Path_v;
	NLPOP::Get_Specified_Files(p_m_CENET_Dlg->GigawordFolder.c_str(), Path_v, ".ENTITY");

	map<string, vector<relationinfo*>*> PersonLocations_mv;
	for(size_t i = 0; i < Path_v.size(); i++){
		string FileName = Path_v[i];
		FileName = FileName.substr(0, FileName.rfind('.'));
		FileName = FileName.substr(FileName.rfind('\\')+1, FileName.length()-FileName.rfind('\\')+1);
		ostringstream ostream;
		ostream << "Processing Event: " << FileName << " (" << i+1 << "/" << Path_v.size() << ")" << endl;
		AppCall::Maxen_Responce_Message(ostream.str().c_str());

		vector<entity_mention*> entity_mention_v;
		ENCOM::Load_Extracted_Entities_Result(Path_v[i].c_str(), entity_mention_v);
		map<string, map<size_t, entity_mention*>> Event_IDMAP_m;
		for(size_t j = 0; j < entity_mention_v.size(); j++){
			entity_mention& loc_case = *entity_mention_v[j];
			if(Event_IDMAP_m.find(loc_case.DOCID) == Event_IDMAP_m.end()){
				Event_IDMAP_m[loc_case.DOCID];
			}
			map<size_t, entity_mention*>& IDMAP_m = Event_IDMAP_m[loc_case.DOCID];
			if(IDMAP_m.find(loc_case.CASID) != IDMAP_m.end()){
				AppCall::Secretary_Message_Box("Data Error in: ENET::Person_Location_Time_Analysis_Port()");
			}
			if(strcmp("PER", loc_case.TYPE.c_str()) && strcmp("LOC", loc_case.TYPE.c_str())){
				delete entity_mention_v[j];
				entity_mention_v[j] = NULL;
				continue;
			}
			IDMAP_m.insert(make_pair(loc_case.CASID, entity_mention_v[j]));
		}
		FileName = Path_v[i].substr(0, Path_v[i].rfind('.'));
		FileName += ".RELATION";
		
		vector<relationinfo*> relation_mention_v;
		ENCOM::Load_Extracted_Relations_Result(FileName.c_str(), relation_mention_v);
		for(size_t j = 0; j < relation_mention_v.size(); j++){
			relationinfo& loc_case = *relation_mention_v[j];
			if(Event_IDMAP_m.find(loc_case.DOCID) == Event_IDMAP_m.end()){
				AppCall::Secretary_Message_Box("Data Error in: ENET::Person_Location_Time_Analysis_Port()");
			}
			map<size_t, entity_mention*>& IDMAP_m = Event_IDMAP_m[loc_case.DOCID];
			if(IDMAP_m.find(loc_case.Arg1_ID) == IDMAP_m.end() || IDMAP_m.find(loc_case.Arg2_ID) == IDMAP_m.end()){
				delete relation_mention_v[j];
				relation_mention_v[j] = NULL;
				continue;
			}
			entity_mention& loc_Arg1 = *IDMAP_m[loc_case.Arg1_ID];
			entity_mention& loc_Arg2 = *IDMAP_m[loc_case.Arg2_ID];
			if((!strcmp("PER", loc_Arg1.TYPE.c_str()) && !strcmp("LOC", loc_Arg2.TYPE.c_str())) || 
				(!strcmp("LOC", loc_Arg1.TYPE.c_str()) && !strcmp("PER", loc_Arg2.TYPE.c_str()))){
				entity_mention* pPersonR;
				entity_mention* pLocationR;
				if(!strcmp("PER", loc_Arg1.TYPE.c_str())){
					pPersonR = IDMAP_m[loc_case.Arg1_ID];
					pLocationR = IDMAP_m[loc_case.Arg2_ID];
				}
				else{
					pPersonR = IDMAP_m[loc_case.Arg2_ID];
					pLocationR = IDMAP_m[loc_case.Arg1_ID];
				}
				if(PersonLocations_mv.find(pPersonR->extent.charseq) == PersonLocations_mv.end()){
					PersonLocations_mv.insert(make_pair(pPersonR->extent.charseq, new vector<relationinfo*>));
				}
				PersonLocations_mv[pPersonR->extent.charseq]->push_back(relation_mention_v[j]);
			}
			else{
				delete relation_mention_v[j];
				relation_mention_v[j] = NULL;
			}
		}
		for(map<string, map<size_t, entity_mention*>>::iterator mmite = Event_IDMAP_m.begin();  mmite != Event_IDMAP_m.end(); mmite++){
			for(map<size_t, entity_mention*>::iterator mite = mmite->second.begin(); mite != mmite->second.end(); mite++){
				delete mite->second;
			}
		}
	}
	string outputfilename = ENET_FOLDER;
	outputfilename += "PLT.dat";
	ofstream out(outputfilename.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	vector<relationinfo*> Results_v;
	for(map<string, vector<relationinfo*>*>::iterator mvite = PersonLocations_mv.begin(); mvite != PersonLocations_mv.end(); mvite++){
		vector<relationinfo*>& loc_v = *mvite->second;
		bool Collecting_Results_Flag = false;
		if(!strcmp("Ã«Ôó¶«", mvite->first.c_str()) || !strcmp("Ã«Ö÷Ï¯", mvite->first.c_str())){
			Collecting_Results_Flag = true;
		}
		bool Enable_Output_Flag = true;
		if(loc_v.size() < 10){
			Enable_Output_Flag = false;
		}
		if(Enable_Output_Flag){
			out << mvite->first << ": " << loc_v.size() << endl;
		}
		for(size_t i = 0; i < loc_v.size(); i++){
			relationinfo& loc_case = *loc_v[i];
			if(Enable_Output_Flag){
				out << "(" << loc_case.arg1 << ", " << loc_case.arg2 << "); ";
			}
			if(!Collecting_Results_Flag){
				delete loc_v[i];
			}
			else{
				Results_v.push_back(loc_v[i]);
			}
		}
		if(Enable_Output_Flag){
			out << endl  << endl;
		}
		delete mvite->second;
	}
	out.close();

	outputfilename = ENET_FOLDER;
	outputfilename += "PLT.RELATION";
	out.open(outputfilename.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(size_t i = 0; i < Results_v.size(); i++){
		relationinfo& loc_Relation_Case = *Results_v[i];
		out << '\t' << loc_Relation_Case.arg1 << '\t' << loc_Relation_Case.arg2;
		out << '\t' << loc_Relation_Case.TYPE << '\t' << loc_Relation_Case.value;
		out << '\t' << loc_Relation_Case.Arg1_ID << '\t' << loc_Relation_Case.Arg2_ID << endl;
		out << "" << endl;
		out << loc_Relation_Case.DOCID << endl;
		out << loc_Relation_Case.SENID << endl;
		delete Results_v[i];
	}
	out.close();

}


void CENRun::Extracting_Entity_Relation_From_Documents(const char* cWorkSpace, CRDC* pm_CRDC)
{
	vector<string> FolderPath_v;
	NLPOP::Get_Child_Folders(cWorkSpace, FolderPath_v);

	for(size_t i = 0; i < FolderPath_v.size(); i++){
		string timestepname = FolderPath_v[i];
		timestepname = timestepname.substr(0, timestepname.rfind('\\'));
		timestepname = timestepname.substr(timestepname.rfind('\\')+1, timestepname.length()-timestepname.rfind('\\')+1);

		vector<string> loc_EventPath_v;
		NLPOP::Get_Specified_Files(FolderPath_v[i].c_str(), loc_EventPath_v, ".ENTITY");
		for(size_t j = 0; j < loc_EventPath_v.size(); j++){
			string eventname = loc_EventPath_v[j];
			eventname = eventname.substr(eventname.rfind('\\')+1, eventname.length()-eventname.rfind('\\')+1);
			eventname = eventname.substr(0, eventname.rfind('.'));

			ostringstream ostream;
			ostream << "Relation Extracting..." << endl;
			ostream << "Time step: " << timestepname << " (" << i+1 << "/" <<  FolderPath_v.size() << ")" << endl;
			ostream << "Processing Event: " << eventname << " (" << j+1 << "/" <<  loc_EventPath_v.size() << ")" << endl;
			ostream << endl;
			AppCall::Maxen_Responce_Message(ostream.str().c_str());

			string SavePath = loc_EventPath_v[j];
			SavePath = SavePath.substr(0, SavePath.rfind('.'));
			SavePath += ".RELATION";
			if(ENET_Incremental_Flag){
				if(NLPOP::Exist_of_This_File(SavePath.c_str())){
					continue;
				}
			}

			Extracting_Entity_Relation_By_ENTITY_Document(loc_EventPath_v[j].c_str(), SavePath.c_str(), pm_CRDC);
		}
	}
}

void CENRun::Extracting_Entity_Relation_By_ENTITY_Document(const char* cENTITY_Path, const char* cRELATION_Path, CRDC* pm_CRDC)
{
	ofstream out;
	ifstream in;
	in.open(cENTITY_Path);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	out.open(cRELATION_Path);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	//---------------------------------------------------------------
	vector<entity_mention> entity_mention_v;
	string SentMention;
	while(ENCOM::Get_Next_Entity_Mention(in, entity_mention_v, SentMention)){
		bool Detected_Relation_Flag = false;
		if(entity_mention_v.size() > 10){
			continue;
		}
		string DOCID;
		size_t SENID;
		for(size_t arg1 = 0; arg1 < entity_mention_v.size(); arg1++){
			for(size_t arg2 = 0; arg2 < entity_mention_v.size(); arg2++){
			//for(size_t arg2 = arg1+1; arg2 < entity_mention_v.size(); arg2++){
				if(arg1 == arg2){
					continue;
				}
				Relation_Case loc_Relation_Case;
				ENCOM::Generating_Relation_Case(SentMention.c_str(), entity_mention_v[arg1], entity_mention_v[arg2], loc_Relation_Case);
				if(loc_Relation_Case.Faulted_Flag){
					continue;
				}
				pm_CRDC->Relation_Case_Recognition_Port(loc_Relation_Case);
				if(!strcmp(loc_Relation_Case.TYPE.c_str(), NEGETIVE)){
					continue;
				}
				out << '\t' << loc_Relation_Case.first_entity.extent.charseq << '\t' << loc_Relation_Case.sencond_entity.extent.charseq;
				out << '\t' << loc_Relation_Case.TYPE << '\t' << loc_Relation_Case.TYPE_V;
				out << '\t' << loc_Relation_Case.first_entity.CASID << '\t' << loc_Relation_Case.sencond_entity.CASID << endl;
				if(!Detected_Relation_Flag){
					Detected_Relation_Flag = true;
					DOCID = loc_Relation_Case.DOCID;
					SENID = loc_Relation_Case.SENID;
				}
			}
		}
		if(Detected_Relation_Flag){
			out << SentMention << endl;
			out << DOCID << endl;
			out << SENID << endl;
		}
		entity_mention_v.clear();
	}
	out.close();
	in.close();
}




void CENRun::Get_Document_Events(const char* cWorkSpace, map<string, vector<pair<string, const char*>>*>& EventDoc_mv, map<string, string>& EventPath_m)
{
	SXMLer loc_SXMLer;
	vector<pCGigaDOC> loc_GigaDOC_v;
	ostringstream ostream;

	loc_SXMLer.Xercesc_Gigaword_Content_Extractor(cWorkSpace, loc_GigaDOC_v, ".new");
	map<string, const char*> DocID2TEXT_m;
	for(size_t j = 0; j < loc_GigaDOC_v.size(); j++){
		DocID2TEXT_m.insert(make_pair(loc_GigaDOC_v[j]->p_DOCID, loc_GigaDOC_v[j]->p_TEXT));
		delete loc_GigaDOC_v[j];
	}
	loc_GigaDOC_v.clear();
	vector<string> EventsPath_v;
	NLPOP::Get_Specified_Files(cWorkSpace, EventsPath_v, ".EVENT");

	for(size_t event_i = 0; event_i < EventsPath_v.size(); event_i++){
		map<string, size_t> DocID2Event_m;
		string FartherEvent = EventsPath_v[event_i];
		string FartherFolder = EventsPath_v[event_i];
		AppCall::Load_Document_Events_Info(FartherEvent.c_str(), DocID2Event_m);
		FartherFolder = FartherFolder.substr(0, FartherFolder.rfind('\\')+1);
		FartherEvent = FartherEvent.substr(0, FartherEvent.rfind('\\'));
		FartherEvent = FartherEvent.substr(FartherEvent.rfind('\\')+1, FartherEvent.length()-FartherEvent.rfind('\\')+1);
		//------------------------
		map<size_t, set<string>> EventDocs_m;
		for(map<string, size_t>::iterator mite = DocID2Event_m.begin(); mite != DocID2Event_m.end(); mite++){
			if(EventDocs_m.find(mite->second) == EventDocs_m.end()){
				EventDocs_m[mite->second];
			}
			EventDocs_m[mite->second].insert(mite->first);
		}
		for(size_t j = 0; j < max_event_NUM; j++){
			if(EventDocs_m.find(j) != EventDocs_m.end()){
				char prev;
				ostringstream formatedostream;
				prev = formatedostream.fill('0');
				formatedostream.width(3);
				formatedostream  << j;
				formatedostream.fill(prev);
				ostream.str("");
				ostream << FartherEvent << "-" << formatedostream.str();
				if(EventDoc_mv.find(ostream.str()) == EventDoc_mv.end()){
					EventDoc_mv[ostream.str()];
					EventDoc_mv[ostream.str()] = new vector<pair<string, const char*>>;
					EventPath_m.insert(make_pair(ostream.str(), FartherFolder));
				}
				else{
					AppCall::Secretary_Message_Box("Data Error in: CENRun::Get_Document_Events()");
				}
				vector<pair<string, const char*>>& loc_docs_v = *EventDoc_mv[ostream.str()];
				for(set<string>::iterator site = EventDocs_m[j].begin(); site != EventDocs_m[j].end(); site++){
					if(DocID2TEXT_m.find(site->data()) == DocID2TEXT_m.end()){
						AppCall::Secretary_Message_Box("Data Error in: CENRun::Get_Document_Events()");
					}
					loc_docs_v.push_back(make_pair(site->data(), DocID2TEXT_m[site->data()]));
				}
			}
		}
	}
}

void CENRun::Extracting_Named_Entity_From_Documents(const char* cWorkSpace, CCEDT* pm_CCEDT)
{
	vector<string> FolderPath_v;
	NLPOP::Get_Child_Folders(cWorkSpace, FolderPath_v);
	
	for(size_t i = 0; i < FolderPath_v.size(); i++){
		string TimestepBufFolder = FolderPath_v[i]+"IE_BUF\\";
		if(!NLPOP::FolderExist(NLPOP::string2CString(TimestepBufFolder.c_str()))){
			_mkdir(TimestepBufFolder.c_str());
		}
		map<string, string> DocBufPath_m;
		map<string, vector<pair<string, const char*>>*> EventDoc_mv;
		map<string, string> EventPath_m;
		string timestepname = FolderPath_v[i];
		timestepname = timestepname.substr(0, timestepname.rfind('\\'));
		timestepname = timestepname.substr(timestepname.rfind('\\')+1, timestepname.length()-timestepname.rfind('\\')+1);
		Get_Document_Events(FolderPath_v[i].c_str(), EventDoc_mv, EventPath_m);

		size_t evnetcnt = 1;
		for(map<string, vector<pair<string, const char*>>*>::iterator mvite = EventDoc_mv.begin(); mvite != EventDoc_mv.end(); mvite++, evnetcnt++){
			if(Collected_Doc_Event_Flag){
				if(strcmp(mvite->first.c_str(), Collected_EventName.c_str())){
					delete mvite->second;
					continue;
				}
			}
			string FileName = EventPath_m[mvite->first];
			FileName += mvite->first;
			FileName += "\\";
			if(!NLPOP::FolderExist(NLPOP::string2CString(FileName.c_str()))){
				_mkdir(FileName.c_str());
			}
			FileName += mvite->first;
			FileName += ".ENTITY";
			if(ENET_Incremental_Flag){
				if(NLPOP::Exist_of_This_File(FileName.c_str())){
					delete mvite->second;
					continue;
				}
			}
			ostringstream ostream;
			ostream << "Named Entity Extracting..." << endl;
			ostream << "Time step: " << timestepname << " (" << i+1 << "/" <<  FolderPath_v.size() << ")" << endl;
			ostream << "Processing Event: " << mvite->first << " (" << evnetcnt << "/" <<  EventDoc_mv.size() << ")" << endl;
			ostream << "Number of Documents: " << mvite->second->size() << endl;
			ostream << endl;
			AppCall::Maxen_Responce_Message(ostream.str().c_str());
			ofstream out(FileName.c_str());
			if(out.bad())
				return;
			out.clear();
			out.seekp(0, ios::beg);
			vector<pair<string, const char*>>& loc_docs_v = *mvite->second;
			size_t CASID = 0;
			for(size_t j = 0; j < loc_docs_v.size(); j++){
				string DOCID = loc_docs_v[j].first;
				char* pTEXTchar;
				size_t TEXTsize;
				if(DocBufPath_m.find(DOCID) != DocBufPath_m.end()){
					CFile inputfile(NLPOP::string2CString(DocBufPath_m[DOCID].c_str()),CFile::modeRead);
					TEXTsize = (size_t)inputfile.GetLength();
					pTEXTchar = new char[TEXTsize+1];
					inputfile.Read(pTEXTchar, TEXTsize);
					pTEXTchar[TEXTsize]=0;
					inputfile.Close();
					out << pTEXTchar;
					delete []pTEXTchar;
					continue;
				}
				TEXTsize = strlen(loc_docs_v[j].second);
				pTEXTchar = new char[TEXTsize];
				ace_op::Delet_0AH_and_20H_in_string(loc_docs_v[j].second, pTEXTchar, TEXTsize);
				vector<string> Sent_v;
				NLPOP::Segmenting_TEXT_into_Sentences_By_Character(pTEXTchar, PBreakpoint_s, Sent_v);
				delete pTEXTchar;
						
				ostringstream outstream;
				for(size_t k = 0; k < Sent_v.size(); k++){
					vector<entity_mention> entity_mention_v;
					pm_CCEDT->Sentence_Named_Entity_Recognization(Sent_v[k].c_str(), entity_mention_v);
					if(entity_mention_v.size() < 2){
						continue;
					}
					outstream << Sent_v[k].c_str() << '\t' << entity_mention_v.size() << endl;
					for(vector<entity_mention>::iterator vite = entity_mention_v.begin(); vite != entity_mention_v.end(); vite++){
						outstream << DOCID << '\t';
						outstream << CASID++ << '\t' <<  k << '\t' << vite->TYPE << '\t' << vite->SUBTYPE << '\t' << vite->TYPE_V << '\t' << vite->SUBTYPE_V << '\t';
						outstream << vite->extent.START << '\t' << vite->extent.END << '\t' << vite->extent.charseq << '\t';
						outstream << vite->head.START << '\t' << vite->head.END << '\t' << vite->head.charseq << endl;
					}
				}
				out << outstream.str();
				DocBufPath_m[DOCID];
				DocBufPath_m[DOCID] = TimestepBufFolder + DOCID;
				DocBufPath_m[DOCID] += ".ENTITY";
				CFile BufFile(NLPOP::string2CString(DocBufPath_m[DOCID].c_str()), CFile::modeCreate | CFile::modeWrite);
				BufFile.Write(outstream.str().c_str(), strlen(outstream.str().c_str()));
				BufFile.Close();
			}
			out.close();
			delete mvite->second;
		}
		NLPOP::Delete_Files_In_Dir(NLPOP::string2CString(TimestepBufFolder));
	}
}

void CENRun::Extracting_Named_Entity_by_Event_Document(const char* cEVENT_Path, map<string, const char*>& DocID2TEXT_m, CCEDT* pm_CCEDT)
{
	ofstream out;
	out.open(Collected_FilePrix.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	map<string, size_t> DocID2Event_m;
	AppCall::Load_Document_Events_Info(cEVENT_Path, DocID2Event_m);
	for(map<string, size_t>::iterator mite = DocID2Event_m.begin(); mite != DocID2Event_m.end(); mite++){
		if(DocID2TEXT_m.find(mite->first) == DocID2TEXT_m.end()){
			AppCall::Secretary_Message_Box("Data Error in: ENET::Run_Named_Entity_Port()");
		}
		size_t TEXTsize = strlen(DocID2TEXT_m[mite->first]);
		char* TEXTchar = new char[TEXTsize];
		ace_op::Delet_0AH_and_20H_in_string(DocID2TEXT_m[mite->first], TEXTchar, TEXTsize);
		vector<string> Sent_v;
		NLPOP::Segmenting_TEXT_into_Sentences_By_Character(TEXTchar, PBreakpoint_s, Sent_v);
		delete TEXTchar;
				
		for(size_t k = 0; k < Sent_v.size(); k++){
			vector<entity_mention> entity_mention_v;
			pm_CCEDT->Sentence_Named_Entity_Recognization(Sent_v[k].c_str(), entity_mention_v);
			if(entity_mention_v.size() < 2){
				continue;
			}
			out << Sent_v[k].c_str() << '\t' << entity_mention_v.size() << endl;
			for(vector<entity_mention>::iterator vite = entity_mention_v.begin(); vite != entity_mention_v.end(); vite++){
				out << vite->CASID << '\t' <<  vite->SENID << '\t' << vite->TYPE << '\t' << vite->SUBTYPE << '\t' << vite->TYPE_V << '\t' << vite->SUBTYPE_V << '\t';
				out << vite->extent.START << '\t' << vite->extent.END << '\t' << vite->extent.charseq << '\t';
				out << vite->head.START << '\t' << vite->head.END << '\t' << vite->head.charseq << endl;
			}
		}
	}
	out.close();
}