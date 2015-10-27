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
#include "Orgni.h"
#include "DOCCom.h"
#include "CDOC.h"

extern string CDOC_Clustering_Work_Space;
extern string CDOC_Documnet_Matrix_Path;

COrgni::COrgni()
{
	
}
COrgni::~COrgni()
{

}
bool COrgni::Checking_and_Generating_Events_Iterative_Filter_Mirror_Vector(map<string, size_t>& pmDocID2Event_m, map<size_t, vector<bool>*>& pmIteFilter_mv, vector<bool>& pmDocFilter_v)
{
	set<size_t> Event_s;
	for(map<string, size_t>::iterator mite = pmDocID2Event_m.begin(); mite != pmDocID2Event_m.end(); mite++){
		Event_s.insert(mite->second);
	}
	//---------------------Only a Cluster is Detected;
	if(Event_s.size() == 1){
		return true;
	}
	for(set<size_t>::iterator site = Event_s.begin(); site != Event_s.end(); site++){
		pmIteFilter_mv.insert(make_pair(*site, new vector<bool>));
	}

	map<string, size_t>::iterator mite = pmDocID2Event_m.begin();
	size_t EventID;
	for(size_t i = 0; i < pmDocFilter_v.size(); i++){
		if(pmDocFilter_v[i]){	
			if(mite == pmDocID2Event_m.end()){
				AppCall::Secretary_Message_Box("Data Error in: CDOC::Iterative_Document_Event_Hierarchies_Clustering()");
				return true;
			}
			EventID = mite->second;
			mite++;
		}
		for(map<size_t, vector<bool>*>::iterator mvite = pmIteFilter_mv.begin(); mvite != pmIteFilter_mv.end(); mvite++){
			if(pmDocFilter_v[i] && mvite->first == EventID){
				mvite->second->push_back(true);
			}
			else{
				mvite->second->push_back(false);
			}
		}
	}
	return false;
}
bool COrgni::Checking_Iterativ_Condition(int hierarchy,  vector<bool>& pmDocFilter_v)
{
	size_t doc_cnt = 0;
	for(size_t i = 0; i < pmDocFilter_v.size(); i++){
		if(pmDocFilter_v[i]){
			doc_cnt++;
		}
	}
	if(m_p_CDOC->m_DebugDoc_Flag){
		if((doc_cnt > 1) && (hierarchy > 0)){
			return false;
		}
	}
	else{
		if((doc_cnt > MIN_CLUSTERING_DOC_NUM) && (hierarchy > 0)){
			return false;
		}
	}
	return true;
}
bool COrgni::Checking_and_Save_Document_Event_Info(int hierarchy, const char* docmatrix, const char* cWorkSpace, vector<bool>& pmDocFilter_v)
{
	size_t doc_cnt = 0;
	for(size_t i = 0; i < pmDocFilter_v.size(); i++){
		if(pmDocFilter_v[i]){
			doc_cnt++;
		}
	}
	if(m_p_CDOC->m_DebugDoc_Flag){
		if((doc_cnt > 1) && (hierarchy > 0)){
			return false;
		}
	}
	else{
		if((doc_cnt > MIN_CLUSTERING_DOC_NUM) && (hierarchy > 0)){
			return false;
		}
	}
	string FileName = cWorkSpace;
	FileName = FileName.substr(0, FileName.rfind('\\'));
	FileName = FileName.substr(0, FileName.rfind('\\')+1);
	FileName += "Matrix.EVENT";
	if(!NLPOP::Exist_of_This_File(FileName)){
		AppCall::Secretary_Message_Box("Data Error in: DOCCOM::Check_and_Save_Single_Document_Event_Info() 1");
		return false;
	}
	map<string, size_t> DocID2Event_m;
	vector<vector<string>*> EventTopTerm_v;
	AppCall::Load_Document_Events_Info(FileName.c_str(), DocID2Event_m, EventTopTerm_v);

	FileName = docmatrix;
	FileName += ".DOCID";
	vector<string> DocID_v;
	DOCCOM::Load_Filtered_Document_ID(FileName.c_str(), DocID_v);


	FileName = cWorkSpace;
	FileName += "Matrix.EVENT";
	ofstream out;
	out.open(FileName.c_str());
	if(out.bad()){
		AppCall::Secretary_Message_Box("Open File Failed in: Check_and_Save_Single_Document_Event_Info() 2");
		return false;
	}
	out.clear();
	out << doc_cnt << endl;
	size_t DocID = 0;
	size_t EventID = 0;
	for(size_t i = 0; i < DocID_v.size(); i++){
		if(!pmDocFilter_v[i]){
			continue;
		}
		if(DocID2Event_m.find(DocID_v[i]) == DocID2Event_m.end()){
			AppCall::Secretary_Message_Box("Data Error in: DOCCOM::Check_and_Save_Single_Document_Event_Info() 3");
			return false;
		}
		EventID = DocID2Event_m[DocID_v[i]];
		out << DocID_v[i] << "\t" << EventID << endl;
	}
	out << EventTopTerm_v.size() << endl;
	for(size_t i = 0; i < EventTopTerm_v.size(); i++){
		vector<string>& Terms_v = *EventTopTerm_v[i];
		for(size_t TermID = 0; TermID < Terms_v.size(); TermID++){
			out << Terms_v[TermID] << ";";
		}
		out << endl;
		delete EventTopTerm_v[i];
	}
	out << endl;
	out.close();

	FileName = cWorkSpace;
	FileName += "Matrix.FILTER";
	DOCCOM::Output_Filtered_Document_Mirror(FileName.c_str(), pmDocFilter_v);

	return true;
}


void COrgni::Documents_Organizing_Segmenation(const char* cOrganizingFolder, vector<string>& TimeSetpFolder_v, int pm_TimeStep, BOOL pCDOC_Incremental_Flag)
{
	vector<string> FilePath_v;
	CDOC_Clustering_Work_Space = DATA_FOLDER;
	CDOC_Clustering_Work_Space += "Events Organizing\\";
	
	if(!pCDOC_Incremental_Flag){
		NLPOP::Delete_Dir_and_Grate(CDOC_Clustering_Work_Space);
	}
	else{
		NLPOP::Checking_Dir_if_none_then_Greate(CDOC_Clustering_Work_Space);
	}
	
	NLPOP::Get_Specified_Files(cOrganizingFolder, FilePath_v, "");
	size_t Data;
	string Datastr;
	size_t Year;
	size_t Month;
	map<size_t, string> DataDocPath_map;
	map<size_t, string> DataDocName_map;
	for(size_t i = 0; i < FilePath_v.size(); i++){
		Datastr = FilePath_v[i].substr(FilePath_v[i].rfind('_')+1, FilePath_v[i].length()-FilePath_v[i].rfind('_')-1);
		istringstream instream(Datastr.c_str());
		instream >> Data;
		Year = Data/100;
		Month = Data%100;
		DataDocPath_map.insert(make_pair(Data, FilePath_v[i]));
		Datastr = FilePath_v[i].substr(FilePath_v[i].rfind('\\')+1, FilePath_v[i].length()-FilePath_v[i].rfind('\\')-1);
		DataDocName_map.insert(make_pair(Data, Datastr));
	}
	ostringstream ostream;
	bool DocData_Catched_Flag = false;
	string CurrentFolder;
	size_t Catched_Data;
	size_t TimeStepFolder = 0;
	size_t MonthSpan = 0;
	Data = DataDocName_map.begin()->first;
	for(map<size_t, string>::iterator mite = DataDocName_map.begin(); mite != DataDocName_map.end(); mite++){
		MonthSpan = (mite->first/100-Data/100)*12+mite->first%100-Data%100;
		if(!(MonthSpan < (size_t)pm_TimeStep)){
			DocData_Catched_Flag = false;
		}
		if(!DocData_Catched_Flag){
			ostream.str("");
			ostream << CDOC_Clustering_Work_Space;
			ostream << "Timestamp-" << mite->first << "\\";
			CurrentFolder = ostream.str().c_str();
			TimeSetpFolder_v.push_back(CurrentFolder);
			if(!pCDOC_Incremental_Flag){
				NLPOP::Delete_Dir_and_Grate(CurrentFolder.c_str());
			}
			else{
				NLPOP::Checking_Dir_if_none_then_Greate(CurrentFolder.c_str());
			}
			Catched_Data = mite->first;
			DocData_Catched_Flag = true;
			TimeStepFolder++;
			Data = mite->first;
		}
		ostream.str("");
		ostream << CurrentFolder << DataDocName_map[mite->first];
		if(!NLPOP::Exist_of_This_File(ostream.str())){
			CopyFile(NLPOP::string2CString(DataDocPath_map[mite->first]), NLPOP::string2CString(ostream.str()+".new"), FALSE);
		}
	}

}
