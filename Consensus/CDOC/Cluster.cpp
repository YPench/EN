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
#include "Cluster.h"
#include "DOCCom.h"

extern string CDOC_Clustering_Work_Space;
extern string CDOC_Documnet_Matrix_Path;
CCluster::CCluster()
{
}
CCluster::~CCluster()
{

}


void CCluster::Generating_Clustering_Training_Data(const char* docmatrix, vector<bool>& pmDocFilter_v)
{
	m_LDA_Doc_Num = 0;
	for(size_t i = 0; i < pmDocFilter_v.size(); i++){
		if(pmDocFilter_v[i]){
			m_LDA_Doc_Num++;
		}
	}
	string FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.DAT";
	ofstream out;
	out.open(FileName.c_str());
	if(out.bad()){
		AppCall::Secretary_Message_Box("Open File Failed in: Generating_Clustering_Training_Data()");
		return;
	}
	out.clear();

	vector<bool> ParDocFilter_v;
	FileName = docmatrix;
	FileName += ".FILTER";
	DOCCOM::Load_Filtered_Document_Mirror(FileName.c_str(), ParDocFilter_v);
	if(ParDocFilter_v.size() != pmDocFilter_v.size()){
		AppCall::Secretary_Message_Box("Open File Failed in: Generating_Clustering_Training_Data()");
		return;
	}

	char getlineBuf[MAX_DOC_BUFFER];
	FileName = docmatrix;
	FileName += ".DATFILT";
	ifstream in(FileName.c_str());
	if(in.bad()){
		AppCall::Secretary_Message_Box("Open File Failed in: Generating_Clustering_Training_Data()");
		return;
	}
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		AppCall::Secretary_Message_Box("Open File Failed in: Generating_Clustering_Training_Data()");
		in.close();
		return;
	}
	out << m_LDA_Doc_Num << endl;
	size_t doc_cnt = 0;
	for(size_t i = 0; i < ParDocFilter_v.size(); i++){
		if(!ParDocFilter_v[i]){
			continue;
		}
		if(!pmDocFilter_v[i]){
			in.getline(getlineBuf, MAX_DOC_BUFFER, '\n');
			continue;
		}
		in.getline(getlineBuf, MAX_DOC_BUFFER, '\n');
		out << getlineBuf << endl;
		doc_cnt++;
	}
	in.close();
	out.close();

	if(doc_cnt != m_LDA_Doc_Num){
		AppCall::Secretary_Message_Box("Open File Failed in: Generating_Clustering_Training_Data()");
		return;
	}
	FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.FILTER";
	DOCCOM::Output_Filtered_Document_Mirror(FileName.c_str(), pmDocFilter_v);
}

void CCluster::Documents_Clustering_to_Event()
{
	string phiname;
	string weighname;
	string outputname;
	phiname = CDOC_Clustering_Work_Space;
	phiname += "model-final.phi";
	weighname = CDOC_Clustering_Work_Space;
	weighname += "Matrix.WEIGHT";
	outputname = CDOC_Clustering_Work_Space;
	outputname += "Matrix.CLUSTER";
	vector<vector<double>*> LDA_phi_vv;
	DOCCOM::Load_LDA_Phi_or_Theta(phiname.c_str(), LDA_phi_vv);
	m_LDA_Event_Num = LDA_phi_vv.size();

	char getlineBuf[MAX_DOC_BUFFER];
	double distance;
	double max_distance;
	double weight;
	size_t max_EventID;
	vector<double> EventDistribution_v;
	ofstream out;
	out.open(outputname.c_str());
	if(out.bad()){
		AppCall::Secretary_Message_Box("Open File Failed in: Documents_Clustering_to_Event()");
		return;
	}
	out.clear();
	ifstream in(weighname.c_str());
	if(in.bad()){
		AppCall::Secretary_Message_Box("Open File Failed in: Documents_Clustering_to_Event()");
		return;
	}
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		AppCall::Secretary_Message_Box("Open File Failed in: Documents_Clustering_to_Event()");
		in.close();
		return;
	}
	in >> m_LDA_Doc_Num;
	in.getline(getlineBuf, 256, '\n');
	out << m_LDA_Doc_Num << endl;
	for(size_t i = 0; i < m_LDA_Doc_Num; i++){
		in.getline(getlineBuf, MAX_DOC_BUFFER, '\n');
		istringstream instream(getlineBuf);
		vector<double> DocWeight_v;
		while(instream.peek() != EOF){
			instream >> weight;
			instream.getline(getlineBuf, 256, ' ');
			DocWeight_v.push_back(weight);
		}
		max_distance = 0;
		max_EventID = 0;
		EventDistribution_v.clear();
		for(size_t EventID = 0; EventID < LDA_phi_vv.size(); EventID++){
			vector<double>& EventWeight_v = *LDA_phi_vv[EventID];
			if(DocWeight_v.size() != EventWeight_v.size()){
				AppCall::Secretary_Message_Box("Unmatched Data in: Documents_Clustering_to_Event_Port()");
				return;
			}
			distance = 0;
			for(size_t k = 0; k < EventWeight_v.size(); k++){
				distance += DocWeight_v[k]*EventWeight_v[k];
			}
			EventDistribution_v.push_back(distance);
			if(distance > max_distance){
				max_distance = distance;
				max_EventID = EventID;
			}
		}
		out << max_EventID << ": " << max_distance << "; [";
		for(vector<double>::iterator vite = EventDistribution_v.begin(); vite != EventDistribution_v.end(); vite++){
			out << *vite << " ";
		}
		out << "]" << endl;
	}
	in.close();
	out.close();
	for(size_t i = 0; i < LDA_phi_vv.size(); i++){
		delete LDA_phi_vv[i];
	}
}

void CCluster::Sorting_Documents_by_Max_Event_Distribution(const char* docmatrix)
{
	string FileName = docmatrix;
	FileName += ".DOCID";
	vector<string> DocID_v;
	DOCCOM::Load_Filtered_Document_ID(FileName.c_str(), DocID_v);

	FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.FILTER";
	vector<bool> FilteredDoc_v;
	DOCCOM::Load_Filtered_Document_Mirror(FileName.c_str(), FilteredDoc_v);

	if(DocID_v.size() != FilteredDoc_v.size()){
		AppCall::Secretary_Message_Box("Data Error in: Merging_Clustered_Info()");
		return;
	}

	FileName = CDOC_Clustering_Work_Space;
	FileName += "model-final.theta";
	vector<vector<double>*> LDAtheta_vv;
	DOCCOM::Load_LDA_Phi_or_Theta(FileName.c_str(), LDAtheta_vv);

	FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.TWORDS";
	vector<vector<string>*> EventTopTerm_vv;
	DOCCOM::Load_Event_Top_Terms(FileName.c_str(), EventTopTerm_vv);

	m_LDA_Doc_Num = 0;
	for(size_t i = 0; i < FilteredDoc_v.size(); i++){
		if(FilteredDoc_v[i]){
			m_LDA_Doc_Num++;
		}
	}
	FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.TOP";
	ofstream out;
	out.open(FileName.c_str());
	if(out.bad()){
		AppCall::Secretary_Message_Box("Open File Failed in: Documents_Term_Space_Distribution()");
		return;
	}
	out.clear();
	out << m_LDA_Doc_Num << endl;
	size_t DOCID = 0;
	for(size_t i = 0; i < DocID_v.size(); i++){
		if(!FilteredDoc_v[i]){
			continue;
		}
		if(!(DOCID < m_LDA_Doc_Num)){
			AppCall::Secretary_Message_Box("Open File Failed in: Documents_Term_Space_Distribution()");
			return;
		}
		vector<double>& loc_theta_v = *LDAtheta_vv[DOCID];
		double max_theta = 0;
		size_t max_event = 0;
		for(size_t j = 0; j < loc_theta_v.size(); j++){
			if(max_theta < loc_theta_v[j]){
				max_theta = loc_theta_v[j];
				max_event = j;
			}
		}
		delete LDAtheta_vv[DOCID];
		DOCID++;
		out << DocID_v[i] << "\t" << max_event << endl;
	}
	if(DOCID != m_LDA_Doc_Num){
		AppCall::Secretary_Message_Box("Data Error in: Merging_Clustered_Info()");
		return;
	}
	out << EventTopTerm_vv.size() << endl;
	for(size_t i = 0; i < EventTopTerm_vv.size(); i++){
		vector<string>& loc_Terms_v = *EventTopTerm_vv[i];
		for(size_t j = 0; j < loc_Terms_v.size(); j++){
			out << loc_Terms_v[j] << ";";
		}
		out << endl;
		delete EventTopTerm_vv[i];
	}
	out.clear();
}


void CCluster::Merging_Clustered_Info(const char* docmatrix)
{
	string FileName = docmatrix;
	FileName += ".DOCID";
	vector<string> DocID_v;
	DOCCOM::Load_Filtered_Document_ID(FileName.c_str(), DocID_v);

	FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.FILTER";
	vector<bool> FilteredDoc_v;
	DOCCOM::Load_Filtered_Document_Mirror(FileName.c_str(), FilteredDoc_v);

	if(DocID_v.size() != FilteredDoc_v.size()){
		AppCall::Secretary_Message_Box("Data Error in: Merging_Clustered_Info()");
		return;
	}

	FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.CLUSTER";
	vector<size_t> DocEventID_v;
	DOCCOM::Load_CLUSTER_Info(FileName.c_str(), DocEventID_v);

	FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.TWORDS";
	vector<vector<string>*> EventTopTerm_vv;
	DOCCOM::Load_Event_Top_Terms(FileName.c_str(), EventTopTerm_vv);

	m_LDA_Doc_Num = 0;
	for(size_t i = 0; i < FilteredDoc_v.size(); i++){
		if(FilteredDoc_v[i]){
			m_LDA_Doc_Num++;
		}
	}
	FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.EVENT";
	ofstream out;
	out.open(FileName.c_str());
	if(out.bad()){
		AppCall::Secretary_Message_Box("Open File Failed in: Documents_Term_Space_Distribution()");
		return;
	}
	out.clear();
	out << m_LDA_Doc_Num << endl;
	size_t DOCID = 0;
	for(size_t i = 0; i < DocID_v.size(); i++){
		if(!FilteredDoc_v[i]){
			continue;
		}
		if(!(DOCID < m_LDA_Doc_Num)){
			AppCall::Secretary_Message_Box("Open File Failed in: Documents_Term_Space_Distribution()");
			return;
		}
		out << DocID_v[i] << "\t" << DocEventID_v[DOCID++] << endl;
	}
	out << EventTopTerm_vv.size() << endl;
	for(size_t i = 0; i < EventTopTerm_vv.size(); i++){
		vector<string>& loc_Terms_v = *EventTopTerm_vv[i];
		for(size_t j = 0; j < loc_Terms_v.size(); j++){
			out << loc_Terms_v[j] << ";";
		}
		out << endl;
		delete EventTopTerm_vv[i];
	}
	out.clear();
}
void CCluster::LDA_Twords_to_Document_Terms(const char* docmatrix)
{
	string FileName = docmatrix;
	FileName += ".TERM";
	if(!NLPOP::Exist_of_This_File(FileName)){
		AppCall::Secretary_Message_Box("读取文章不存在：CDOC::LDA_Twords_to_Document_Terms_Port()");
		return;
	}
	//*************
	map<string, size_t> m_FeatureCoords;
	DOCCOM::Load_string_to_size_t_map(FileName.c_str(), m_FeatureCoords);

	map<size_t, string> ID2STR_m;
	for(map<string, size_t>::iterator mite = m_FeatureCoords.begin(); mite != m_FeatureCoords.end(); mite++){
		ID2STR_m.insert(make_pair(mite->second, mite->first));
	}

	FileName = CDOC_Clustering_Work_Space;
	FileName += "model-final.twords";
	if(!NLPOP::Exist_of_This_File(FileName.c_str())){
		AppCall::Secretary_Message_Box("读取文章不存在：CDOC::LDA_Twords_to_Document_Terms_Port()");
		return;
	}
	vector<map<double, size_t>*> TWordsize_v;
	Load_LDA_Event_Top_Words<size_t>(FileName.c_str(), TWordsize_v);

	vector<map<double, string>*> TWordstr_v;
	for(size_t i = 0; i < TWordsize_v.size(); i++){
		map<double, size_t>& loc_TWordsize_m = *TWordsize_v[i];
		map<double, string>* p_TWordstr_m = new map<double, string>;
		for(map<double, size_t>::iterator mite = loc_TWordsize_m.begin(); mite != loc_TWordsize_m.end(); mite++){
			p_TWordstr_m->insert(make_pair(mite->first, ID2STR_m[mite->second]));
		}
		TWordstr_v.push_back(p_TWordstr_m);
		delete TWordsize_v[i];
	}

	FileName = CDOC_Clustering_Work_Space;
	FileName += "Matrix.TWORDS";
	DOCCOM::LDA_Output_Event_Words_with_Erasing(FileName.c_str(), TWordstr_v);
}


void CCluster::Documents_Term_Space_Distribution()
{
	string inputname;
	string outputname;

	inputname = CDOC_Clustering_Work_Space;
	inputname += "wordmap.txt";
	map<size_t, size_t> LDAIndex2TermID_m;
	DOCCOM::Load_LDA_Wordmap(inputname.c_str(), LDAIndex2TermID_m);

	inputname = CDOC_Clustering_Work_Space;
	inputname += "Matrix.DAT";
	outputname = CDOC_Clustering_Work_Space;
	outputname += "Matrix.WEIGHT";
	size_t TermID;
	size_t TotalCnt;
	m_LDA_Term_Num = LDAIndex2TermID_m.size();
	char getlineBuf[MAX_DOC_BUFFER];
	map<size_t, size_t> TermFreq_m;
	ofstream out;
	out.open(outputname.c_str());
	if(out.bad()){
		AppCall::Secretary_Message_Box("Open File Failed in: Documents_Term_Space_Distribution()");
		return;
	}
	out.clear();

	ifstream in(inputname.c_str());
	if(in.bad()){
		AppCall::Secretary_Message_Box("Open File Failed in: Documents_Term_Space_Distribution()");
		return;
	}
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		AppCall::Secretary_Message_Box("Open File Failed in: Documents_Term_Space_Distribution()");
		in.close();
		return;
	}
	in >> m_LDA_Doc_Num;
	in.getline(getlineBuf, 256, '\n');
	out << m_LDA_Doc_Num << endl;

	for(size_t i = 0; i < m_LDA_Doc_Num; i++){
		in.getline(getlineBuf, MAX_DOC_BUFFER, '\n');
		istringstream instream(getlineBuf);
		TotalCnt = 0;
		while(instream.peek() != EOF){
			instream >> TermID;
			instream.getline(getlineBuf, 256, ' ');
			if(TermFreq_m.find(TermID) == TermFreq_m.end()){
				TermFreq_m.insert(make_pair(TermID, 1));
			}
			else{
				TermFreq_m[TermID]++;
			}
			TotalCnt++;
		}
		for(size_t k = 0; k < m_LDA_Term_Num; k++){
			TermID = LDAIndex2TermID_m[k];
			if(TermFreq_m.find(TermID) != TermFreq_m.end()){
				out << 1.0*TermFreq_m[TermID]/TotalCnt << ' ';
			}
			else{
				out << 0 << ' ';
			}
		}
		out << endl;
		TermFreq_m.clear();
	}
	in.close();
	out.close();
}

