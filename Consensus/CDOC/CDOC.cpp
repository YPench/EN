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
#include "CDOC.h"
#include "DOCCom.h"
#include "..\\Consensus.h"
#include "SXMLer.h"
string CDOC_Clustering_Work_Space;
string CDOC_Documnet_Matrix_Path;

CDOC::CDOC()
{
	CDOC_Incremental_Flag = false;
	m_CLDA.m_p_CDOC = this;
	m_CFilter.m_p_CDOC = this;
	m_COrgni.m_p_CDOC = this;
}
CDOC::~CDOC()
{

}
//--------1: 
void CDOC::Generating_Document_Matrix_Port(const char* docmatrix, vector<pCGigaDOC>& m_GigaDOC_v)
{
	m_CFilter.Generating_Document_Matrix(docmatrix, m_GigaDOC_v);
}
//--------2: 
void CDOC::Filtering_DocMatrix_to_Terms_Port(const char* docmatrix)
{
	m_CFilter.Filtering_DocMatrix_to_Terms(docmatrix);
}

//--------3: 
void CDOC::LDA_Training_Port(const char* docmatrix)
{
	CDOC_Clustering_Work_Space = CDOC_FOLDER;
	CDOC_Clustering_Work_Space += "Root\\";
	NLPOP::Checking_Dir_if_none_then_Greate(CDOC_Clustering_Work_Space);

	string FileName = docmatrix;
	FileName += ".FILTER";
	vector<bool> DocFilter_v;
	DOCCOM::Load_Filtered_Document_Mirror(FileName.c_str(), DocFilter_v);

	m_CCluster.Generating_Clustering_Training_Data(docmatrix, DocFilter_v);
	m_CLDA.LDA_estimate_Port();
	m_CCluster.LDA_Twords_to_Document_Terms(docmatrix);
}

//--------4: 
void CDOC::Documents_Clustering_to_Event_Port(const char* docmatrix)
{
	CDOC_Clustering_Work_Space = CDOC_FOLDER;
	CDOC_Clustering_Work_Space += "Root\\";

	//m_CCluster.Sorting_Documents_by_Max_Event_Distribution(docmatrix);

	//m_CCluster.Documents_Term_Space_Distribution();
	//m_CCluster.Documents_Clustering_to_Event();
	m_CCluster.Merging_Clustered_Info(docmatrix);
}


//====================================================================================
void CDOC::Document_Events_Organizing_Port()
{
	SXMLer loc_SXMLer;
	vector<pCGigaDOC> loc_GigaDOC_v;
	vector<string> TimeSetpFolder_v;
	vector<string> HierarchiesSpaces_v;

	//ACECorpusFolder = Gigawrod_CORPUS_FOLDER;
	//Enable 
	if(!strcmp(Gigawrod_CORPUS_FOLDER, GigawordFolder.c_str())){
		ostringstream ostream;
		ostream << "\"" << GigawordFolder << "\" is selected!" << endl;
		ostream << "Funtion will return, continuing this process requres reprogram!" << endl;
		AppCall::Secretary_Message_Box(ostream.str().c_str());
		return;
	}
	m_COrgni.Documents_Organizing_Segmenation(GigawordFolder.c_str(), TimeSetpFolder_v, m_TimeStep, CDOC_Incremental_Flag);
	
	ostringstream ostream;
	string FileName;
	bool Loc_Enable_Flag = false;
	for(size_t timestep = 0; timestep < TimeSetpFolder_v.size(); timestep++){
		ostream.str("");
		CDOC_Clustering_Work_Space = TimeSetpFolder_v[timestep];
		CDOC_Clustering_Work_Space += "Event-0\\";
		if(!NLPOP::FolderExist(NLPOP::string2CString(CDOC_Clustering_Work_Space))){
			_mkdir(CDOC_Clustering_Work_Space.c_str());
		}
		if(!NLPOP::Exist_of_This_File(CDOC_Clustering_Work_Space+"Matrix.EVENT")){
			Loc_Enable_Flag = true;
		}
		else if(!CDOC_Incremental_Flag){
			Loc_Enable_Flag = true;
		}
		if(Loc_Enable_Flag){
			AppCall::Maxen_Responce_Message("Implementing Function:\nSXMLer.Xercesc_Gigaword_Content_Extractor()\n");
			loc_SXMLer.Xercesc_Gigaword_Content_Extractor(TimeSetpFolder_v[timestep].c_str(), loc_GigaDOC_v, ".new");
			Loc_Enable_Flag = false;
		}
			
		NLPOP::Checking_Dir_if_none_then_Greate(CDOC_Clustering_Work_Space);
		HierarchiesSpaces_v.push_back(CDOC_Clustering_Work_Space);
		ostream << TimeSetpFolder_v[timestep] << "DocMatrix";
		
		if(!NLPOP::Exist_of_This_File(CDOC_Clustering_Work_Space+"Matrix.EVENT")){
			Loc_Enable_Flag = true;
		}
		else if(!CDOC_Incremental_Flag){
			Loc_Enable_Flag = true;
		}
		if(Loc_Enable_Flag){
			//--------1:
			AppCall::Maxen_Responce_Message("Implementing Function:\nCFilter.Generating_Document_Matrix() ID: 1\n");
			m_CFilter.Generating_Document_Matrix(ostream.str().c_str(), loc_GigaDOC_v);
			//--------2:
			AppCall::Maxen_Responce_Message("Implementing Function:\nCFilter.Filtering_DocMatrix_to_Terms() ID: 2\n");
			m_CFilter.Filtering_DocMatrix_to_Terms(ostream.str().c_str());
			Loc_Enable_Flag = false;
		}
		FileName = ostream.str();
		FileName += ".FILTER";
		vector<bool> DocFilter_v;
		DOCCOM::Load_Filtered_Document_Mirror(FileName.c_str(), DocFilter_v);
		
		string Pre_Work_Space = CDOC_Clustering_Work_Space; 
		Iterative_Document_Event_Hierarchies_Clustering(m_Hierarchies, ostream.str().c_str(), Pre_Work_Space.c_str(), DocFilter_v);
	}
}
void CDOC::Clustering_Documents_in_the_Current_Work_Space(const char* docmatrix, vector<bool>& pmDocFilter_v)
{
	//--------3:
	AppCall::Maxen_Responce_Message("Implementing Function:\nCDOC.Clustering_Documents_in_the_Current_Work_Space() ID: 3\n");
	m_CCluster.Generating_Clustering_Training_Data(docmatrix, pmDocFilter_v);
	m_CLDA.LDA_estimate_Port();
	m_CCluster.LDA_Twords_to_Document_Terms(docmatrix);

	//--------4:
	AppCall::Maxen_Responce_Message("Implementing Function:\nCDOC.Merging_Clustered_Info() ID: 4\n");
	m_CCluster.Documents_Term_Space_Distribution();
	m_CCluster.Documents_Clustering_to_Event();
	m_CCluster.Merging_Clustered_Info(docmatrix);
}
void CDOC::Iterative_Document_Event_Hierarchies_Clustering(int hierarchy, const char* docmatrix, const char* cWorkSpace, vector<bool>& pmDocFilter_v)
{
	//------Return if a single document is left;
	if(m_COrgni.Checking_Iterativ_Condition(hierarchy, pmDocFilter_v)){
		return;
	}
	NLPOP::Checking_Dir_if_none_then_Greate(cWorkSpace);
	
	map<string, size_t> DocID2Event_m;
	string FileName = cWorkSpace;
	FileName += "Matrix.EVENT";

	//------Documents Clustering;
	bool Loc_Enable_Flag = false;
	if(!NLPOP::Exist_of_This_File(CDOC_Clustering_Work_Space+"Matrix.EVENT")){
			Loc_Enable_Flag = true;
		}
		else if(!CDOC_Incremental_Flag){
			Loc_Enable_Flag = true;
		}
	if(Loc_Enable_Flag){
		AppCall::Maxen_Responce_Message("Implementing Function:\nCDOC.Clustering_Documents_in_the_Current_Work_Space()\n");
		Clustering_Documents_in_the_Current_Work_Space(docmatrix, pmDocFilter_v);
	}
	AppCall::Load_Document_Events_Info(FileName.c_str(), DocID2Event_m);
	
	map<size_t, vector<bool>*> IteFilter_mv;
	//------Return if hierarchy is 1 or only a cluster is generated;
	if(m_COrgni.Checking_and_Generating_Events_Iterative_Filter_Mirror_Vector(DocID2Event_m, IteFilter_mv, pmDocFilter_v)){
		return;
	}

	string EventName = cWorkSpace;
	EventName = EventName.substr(0, EventName.rfind('\\'));
	EventName = EventName.substr(EventName.rfind('\\')+1, EventName.length()-EventName.rfind('\\')+1);
	
	string Pre_Work_Space = CDOC_Clustering_Work_Space;
	for(map<size_t, vector<bool>*>::iterator mite = IteFilter_mv.begin(); mite != IteFilter_mv.end(); mite++){
		string FileName;
		char prev;
		ostringstream ostream;
		prev = ostream.fill('0');
		ostream.width(3);
		ostream << mite->first;
		ostream.fill(prev);
		FileName = cWorkSpace;
		FileName += EventName;
		FileName += "-";
		FileName += ostream.str();
		FileName  += "\\";
		CDOC_Clustering_Work_Space = FileName;
		Iterative_Document_Event_Hierarchies_Clustering(hierarchy-1, docmatrix, FileName.c_str(), *(mite->second));
		CDOC_Clustering_Work_Space = Pre_Work_Space;
		delete mite->second;
	}
	
}



