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
#include "ENET.h"
#include "..\\Consensus.h"
#include "SXMLer.h"
#include "ENCom.h"
#include "igraph.h"
#include "ConvertUTF.h"

ENET::ENET()
{
	ENET_Incremental_Flag = false;
}
ENET::~ENET()
{

}
void ENET::Person_Location_Time_Analysis_Port()
{
	string PLT_File = ENET_FOLDER;
	PLT_File += "PLT.RELATION";
	if(!NLPOP::Exist_of_This_File(PLT_File.c_str())){
		m_CENRun.Extracting_Person_Location_Time_Analysis_RELATION(p_m_CENET_Dlg);
	}

	vector<relationinfo*> relation_mention_v;
	ENCOM::Load_Extracted_Relations_Result(PLT_File.c_str(), relation_mention_v);

	set<string> DeleteEntity_s;
	DeleteEntity_s.insert("国内");
	DeleteEntity_s.insert("地区");
	DeleteEntity_s.insert("国外");
	DeleteEntity_s.insert(":世界");
	DeleteEntity_s.insert("孙中山");
	DeleteEntity_s.insert("城市之路");
	DeleteEntity_s.insert("土地");
	DeleteEntity_s.insert("山河");
	DeleteEntity_s.insert("世界");
	DeleteEntity_s.insert("大山里");
	DeleteEntity_s.insert("小山");
	DeleteEntity_s.insert("上游");
	DeleteEntity_s.insert("第一个农村");
	DeleteEntity_s.insert("各村");
	DeleteEntity_s.insert("工地");
	DeleteEntity_s.insert("国内外");
	DeleteEntity_s.insert("南渡长江");
	DeleteEntity_s.insert("太平世界");
	DeleteEntity_s.insert("南方");
	DeleteEntity_s.insert("后山");
	DeleteEntity_s.insert("海原");
	DeleteEntity_s.insert("边境");
	DeleteEntity_s.insert("东北");
	DeleteEntity_s.insert("一江山岛");
	DeleteEntity_s.insert("农村");
	DeleteEntity_s.insert("北沿大青山");
	DeleteEntity_s.insert("北沿大青山");
	DeleteEntity_s.insert("东渡黄河");
	DeleteEntity_s.insert("东渡黄河离");

	DeleteEntity_s.insert("毛主席");
	
	map<size_t, string> Timestamp_m;
	string datastr;
	size_t Data;
	size_t Year;
	size_t Month;
	size_t Day;
	set<string> vertexname_s;
	for(size_t i = 0; i < relation_mention_v.size(); i++){
		relationinfo& loc_case = *relation_mention_v[i];
		/*if(DeleteEntity_s.find(loc_case.arg1.c_str()) != DeleteEntity_s.end() || DeleteEntity_s.find(loc_case.arg2.c_str()) != DeleteEntity_s.end()){
			delete relation_mention_v[i];
			relation_mention_v[i] = NULL;
			continue;
		}*/
		datastr = loc_case.DOCID;
		datastr = datastr.substr(datastr.rfind('_')+1, datastr.length()-datastr.rfind('_')-1);
		datastr = datastr.substr(0, datastr.rfind('.'));
		istringstream instream(datastr.c_str());
		instream >> Data;
		Year = Data/10000;
		Month = Data%10000/100;
		Day = Data%100;
		ostringstream ostream;
		ostream << Year << "-" << Month << "-" << Day;
		vertexname_s.insert(loc_case.arg1);
		vertexname_s.insert(loc_case.arg2);

		if(Timestamp_m.find(Data) == Timestamp_m.end()){
			Timestamp_m.insert(make_pair(Data, ostream.str()));
		}
		if(!strcmp("毛泽东", loc_case.arg1.c_str()) || !strcmp("毛主席", loc_case.arg1.c_str())){
			loc_case.arg1 = ostream.str();
		}
		else{
			loc_case.arg2 = ostream.str();
		}
	}
	if(!(Timestamp_m.size() < 2) && false){
		map<size_t, string>::iterator mite = Timestamp_m.begin();
		string arg1 = mite->second;
		string arg2;
		for(mite++; mite != Timestamp_m.end(); mite++){
			arg2 = mite->second;
			relationinfo* prelation = new relationinfo;
			prelation->arg1 = arg1;
			prelation->arg2 = arg2;
			prelation->Arg1_ID = 0;
			prelation->Arg2_ID = 0;
			prelation->TYPE = "T";
			prelation->value = 1.0;
			prelation->SENID = 0;
			arg1 = arg2;
			relation_mention_v.push_back(prelation);
		}
	}



	PLT_File = ENET_FOLDER;
	PLT_File += "PLT_Analysis.RELATION";
	ofstream out(PLT_File.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(size_t i = 0; i < relation_mention_v.size(); i++){
		if(!relation_mention_v[i]){
			continue;
		}
		relationinfo& loc_Relation_Case = *relation_mention_v[i];
		out << '\t' << loc_Relation_Case.arg1 << '\t' << loc_Relation_Case.arg2;
		out << '\t' << loc_Relation_Case.TYPE << '\t' << loc_Relation_Case.value;
		out << '\t' << loc_Relation_Case.Arg1_ID << '\t' << loc_Relation_Case.Arg2_ID << endl;
		out << "" << endl;
		out << loc_Relation_Case.DOCID << endl;
		out << loc_Relation_Case.SENID << endl;
		delete relation_mention_v[i];
	}
	out.close();

	bool Collected_Doc_Event_Flag;
	ENET_Incremental_Flag = false;
	Collected_EventName = "PLT_Analysis";
	Collected_EventPath = PLT_File;
	Collected_KeyWords = "";
	Collected_FilePrix = PLT_File.substr(0, PLT_File.rfind('.'));
	p_m_CENET_Dlg->m_REL_TYPE_s.insert("T");


	PLT_File = Collected_FilePrix;
	PLT_File += "_entity.HtmlCode";
	out.open(PLT_File.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(set<string>::iterator site = vertexname_s.begin(); site != vertexname_s.end(); site++){
		out << ENCOM::EncodeHtml(SCONVERT::StringToWString(site->data())).c_str() << "\t" << site->data() << endl; 
	}
	out.close();
	
	Updata_Configurations();
	m_ENGen.Generating_Pajek_of_Relation_Info(p_m_CENET_Dlg);
	Collected_Doc_Event_Flag = false;

	PLT_File = Collected_FilePrix;
	PLT_File += "_relation.paj";

	AppCall::Consensus_Open_Process(PLT_File.c_str(), Pajet_Path);
}


void ENET::Deleting_Events_Organizing_Files(const char* cprox)
{
	set<string> EraseProx_s;
	if(0 == strlen(cprox)){
		EraseProx_s.insert(".EVENT");
	}
	else{
		EraseProx_s.insert(cprox);
	}
	ostringstream ostream;
	ostream << "Deleting Files in Folder: " << p_m_CENET_Dlg->GigawordFolder << endl;
	ostream << "With File Suffix: ";
	for(set<string>::iterator site = EraseProx_s.begin(); site != EraseProx_s.end(); site++){
		ostream << site->data() << "; ";
	}

	int rtn = AfxMessageBox(NLPOP::string2CString(ostream.str().c_str()), MB_YESNOCANCEL);
	if(rtn != IDYES){
		return;
	}

	vector<string> TimestepFolderPath_v;
	NLPOP::Get_Child_Folders(p_m_CENET_Dlg->GigawordFolder.c_str(), TimestepFolderPath_v);
	for(size_t i = 0; i < TimestepFolderPath_v.size(); i++){
		for(set<string>::iterator site = EraseProx_s.begin(); site != EraseProx_s.end(); site++){
			vector<string> loc_DeleteFile_v;
			NLPOP::Get_Specified_Files(TimestepFolderPath_v[i].c_str(), loc_DeleteFile_v, site->data());
			for(size_t j = 0; j < loc_DeleteFile_v.size(); j++){
				DeleteFile(NLPOP::string2CString(loc_DeleteFile_v[j].c_str()));
			}
		}
	}
}

void ENET::Collecting_Events_Organizing_Info()
{
	SXMLer loc_SXMLer;
	vector<pCGigaDOC> loc_GigaDOC_v;
	ostringstream ostream;

	vector<string> TimestepFolderPath_v;
	NLPOP::Get_Child_Folders(p_m_CENET_Dlg->GigawordFolder.c_str(), TimestepFolderPath_v);

	ostream << "Timestep\t" << "Docs Size\t" << "Events Size\t" << "Entity Size\t" << "Relation Size" << endl;
	ostream << "-------------------------------------------------------------------------------------------------------------------" << endl;
	AppCall::Maxen_Responce_Message(ostream.str().c_str());
	for(size_t i = 0; i < TimestepFolderPath_v.size(); i++){
		string timestepname = TimestepFolderPath_v[i];
		timestepname = timestepname.substr(0, timestepname.rfind('\\'));
		timestepname = timestepname.substr(timestepname.rfind('\\')+1, timestepname.length()-timestepname.rfind('\\')+1);
		
		size_t Doc_Size;
		size_t Event_Size;
		size_t Entities_Size;
		size_t Relation_Size;
		loc_SXMLer.Xercesc_Gigaword_Content_Extractor(TimestepFolderPath_v[i].c_str(), loc_GigaDOC_v, ".new");
		Doc_Size = loc_GigaDOC_v.size();
		map<string, const char*> DocID2TEXT_m;
		for(size_t j = 0; j < loc_GigaDOC_v.size(); j++){
			DocID2TEXT_m.insert(make_pair(loc_GigaDOC_v[j]->p_DOCID, loc_GigaDOC_v[j]->p_TEXT));
			delete loc_GigaDOC_v[j];
		}
		loc_GigaDOC_v.clear();
		
		vector<string> loc_Path_v;
		NLPOP::Get_Specified_Files(TimestepFolderPath_v[i].c_str(), loc_Path_v, ".EVENT");

		Event_Size = 0;
		for(size_t j = 0; j < loc_Path_v.size(); j++){
			Event_Size += ENCOM::Get_Event_Num(loc_Path_v[j].c_str());
		}
		loc_Path_v.clear();

		NLPOP::Get_Specified_Files(TimestepFolderPath_v[i].c_str(), loc_Path_v, ".ENTITY");
		Entities_Size = 0;
		for(size_t j = 0; j < loc_Path_v.size(); j++){
			Entities_Size += ENCOM::Get_ENTITY_Num(loc_Path_v[j].c_str());
		}
		loc_Path_v.clear();

		Relation_Size = 0;
		NLPOP::Get_Specified_Files(TimestepFolderPath_v[i].c_str(), loc_Path_v, ".RELATION");
		for(size_t j = 0; j < loc_Path_v.size(); j++){
			Relation_Size += ENCOM::Get_RELATION_Num(loc_Path_v[j].c_str());
		}
		ostream.str("");
		ostream << timestepname << "\t" << Doc_Size << "\t" << Event_Size << "\t" << Entities_Size << "\t" << Relation_Size << endl;
		AppCall::Maxen_Responce_Message(ostream.str().c_str());
	}
}

void ENET::Cellected_Entity_Short_Path_Port()
{
	Updata_Configurations();
	m_ENGen.Cellected_Entity_Short_Path(p_m_CENET_Dlg);
}
void ENET::Cellected_Entity_Star_Layout_Port()
{
	Updata_Configurations();
	m_ENGen.Cellected_Entity_Star_Layout(p_m_CENET_Dlg);
}

void ENET::Generating_and_Viewing_Event_Network_Port()
{
	Updata_Configurations();
	m_ENGen.Generating_and_Viewing_Event_Network(p_m_CENET_Dlg);
}

void ENET::Generating_Entity_of_Event_Network_Port()
{
	Updata_Configurations();
	m_ENGen.Generating_Pajek_of_Named_Entity_Info(p_m_CENET_Dlg);
}

void ENET::Generating_Relation_of_Event_Network_Port()
{
	Updata_Configurations();
	m_ENGen.Generating_Pajek_of_Relation_Info(p_m_CENET_Dlg);
}


void ENET::Run_Named_Entity_Port(const char* cWorkSpace, CCEDT* pm_CCEDT)
{
	Updata_Configurations();
	pm_CCEDT->Loading_Named_Entity_Models();
	//Documents should be parsed;
	m_CENRun.Extracting_Named_Entity_From_Documents(cWorkSpace, pm_CCEDT);
}

void ENET::Run_Entity_Relation_Port(const char* cWorkSpace, CRDC* pm_CRDC)
{
	Updata_Configurations();
	pm_CRDC->Loading_CRDC_Model();

	if(Collected_Doc_Event_Flag){
		string FileName = Collected_FilePrix;
		FileName += ".ENTITY";
		AppCall::Maxen_Responce_Message("Relation Extracting...\n");
		AppCall::Maxen_Responce_Message(FileName.c_str());
		AppCall::Maxen_Responce_Message("\n\n");

		string SavePath = FileName;
		SavePath = SavePath.substr(0, SavePath.rfind('.'));
		SavePath += ".RELATION";
		m_CENRun.Extracting_Entity_Relation_By_ENTITY_Document(FileName.c_str(), SavePath.c_str(), pm_CRDC);
		return;
	}
	m_CENRun.Extracting_Entity_Relation_From_Documents(cWorkSpace, pm_CRDC);
}

void ENET::Updata_Configurations()
{
	m_CENRun.PBreakpoint_s.insert("。");
	m_CENRun.PBreakpoint_s.insert("！");
	m_CENRun.PBreakpoint_s.insert("？");
	m_CENRun.PBreakpoint_s.insert("；");
	m_CENRun.PBreakpoint_s.insert("，");
	m_CENRun.PBreakpoint_s.insert(",");
	m_CENRun.PBreakpoint_s.insert(";");
	m_CENRun.PBreakpoint_s.insert("!");
	m_CENRun.PBreakpoint_s.insert("?");

	m_CENRun.Collected_Doc_Event_Flag = Collected_Doc_Event_Flag;
	m_CENRun.Collected_EventName = Collected_EventName;
	m_CENRun.Collected_EventPath = Collected_EventPath;
	m_CENRun.Collected_KeyWords = Collected_KeyWords;
	m_CENRun.Collected_FilePrix = Collected_FilePrix;
	m_CENRun.ENET_Incremental_Flag = ENET_Incremental_Flag;

	m_ENGen.Collected_Doc_Event_Flag = Collected_Doc_Event_Flag;
	m_ENGen.Collected_EventName = Collected_EventName;
	m_ENGen.Collected_EventPath = Collected_EventPath;
	m_ENGen.Collected_KeyWords = Collected_KeyWords;
	m_ENGen.Collected_FilePrix = Collected_FilePrix;
	m_ENGen.ENET_Incremental_Flag = ENET_Incremental_Flag;
}
