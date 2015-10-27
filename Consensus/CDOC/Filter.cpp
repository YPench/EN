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
#include "Filter.h"
#include "DOCCom.h"
#include "CDOC.h"
CFilter::CFilter()
{

}
CFilter::~CFilter()
{

}
void CFilter::Init_Lexicon(const char* leciconfolder, set<string>& m_WordSpace, size_t& Max_Lexicon_Size)
{
	string lexiconpath;
	Max_Lexicon_Size = 0;
	if(m_p_CDOC->LCWCC_Flag){
		lexiconpath = leciconfolder;
		lexiconpath += "LCWCC.dat";
		if(!NLPOP::Exist_of_This_File(lexiconpath)){
			AppCall::Secretary_Message_Box("Lexicon of CVMC is not existing...", MB_OK);
		}
		NLPOP::LoadWordsStringSet(lexiconpath, m_WordSpace);
	}
}

void CFilter::Generating_Document_Matrix(const char* docmatrix, vector<pCGigaDOC>& m_GigaDOC_v)
{
	set<string> m_WordSpace;
	size_t Max_Lexicon_Size = 0;
	Init_Lexicon(DATA_FOLDER, m_WordSpace, Max_Lexicon_Size);

	size_t Coords = 0;
	map<string, size_t> loc_FeatureCoords;
	for(set<string>::iterator site = m_WordSpace.begin();  site != m_WordSpace.end(); site++){
		loc_FeatureCoords.insert(make_pair(site->data(), Coords++));
		if( site->size() > Max_Lexicon_Size){
			Max_Lexicon_Size = site->size();
		}
	}

	string FileName = docmatrix;
	FileName += ".DAT";

	ofstream out;
	out.open(FileName.c_str());
	if(out.bad())
		return;
	out.clear();
		
	vector<string> DocID_v;
	map<string, size_t> WordsCnt_map;
	size_t lexicon_size = loc_FeatureCoords.size();
	if(m_p_CDOC->m_DebugDoc_Flag){
		out << m_p_CDOC->m_DebugDocNum << endl;
	}
	else{
		out << m_GigaDOC_v.size() << endl;
	}
	size_t doccnt = 0;

	m_CSegmter.Max_Lexicon_Size = Max_Lexicon_Size;
	size_t docID = 0;
	for( ; docID < m_GigaDOC_v.size(); docID++){
		DocID_v.push_back(m_GigaDOC_v[docID]->p_DOCID);
		//string locstr = ace_op::Delet_0AH_and_20H_in_string(m_GigaDOC_v[docID]->p_TEXT);
		size_t TEXTsize = strlen(m_GigaDOC_v[docID]->p_TEXT);
		char* TEXTchar = new char[TEXTsize];
		ace_op::Delet_0AH_and_20H_in_string(m_GigaDOC_v[docID]->p_TEXT, TEXTchar, TEXTsize);
		m_CSegmter.Omni_words_feature_Extracting(TEXTchar, m_WordSpace, WordsCnt_map);
		delete TEXTchar;
		size_t test_index = 0;
		for(map<string, size_t>::iterator mite = loc_FeatureCoords.begin(); mite != loc_FeatureCoords.end(); mite++){
			if(WordsCnt_map.find(mite->first) == WordsCnt_map.end()){
				out << 0 << " ";
			}
			else{
				out << WordsCnt_map[mite->first] << " ";
			}
		}
		out << endl;
		doccnt++;
		WordsCnt_map.clear();

		if(doccnt%1000 == 0){
			ostringstream ostrsteam;
			ostrsteam << "处理文/剩余文档数：" << doccnt << "/" << m_GigaDOC_v.size() - doccnt << "篇" << endl;
			AppCall::Maxen_Responce_Message(ostrsteam.str().c_str());
		}
		if(m_p_CDOC->m_DebugDoc_Flag){
			if(m_p_CDOC->m_DebugDocNum == doccnt){
				ostringstream ostrsteam;
				ostrsteam << "Collecting Documents for Debug: " << doccnt << "篇" << endl;
				AppCall::Maxen_Responce_Message(ostrsteam.str().c_str());
				break;
			}
		}
		delete m_GigaDOC_v[docID];
	}
	while(docID < m_GigaDOC_v.size()){
		delete m_GigaDOC_v[docID++];
	}
	m_GigaDOC_v.clear();
	out.close();

	FileName = docmatrix;
	FileName += ".TERM";
	DOCCOM::Save_string_to_size_t_map(FileName.c_str(), loc_FeatureCoords);

	FileName = docmatrix;
	FileName += ".DOCID";
	DOCCOM::Output_Filtered_Document_ID(FileName.c_str(), DocID_v);
}



void CFilter::Filtering_DocMatrix_to_Terms(const char* docmatrix)
{
	AppCall::Maxen_Responce_Message("\nFiltering Terms in Document Matrix...\n");
	string FileName = docmatrix;
	FileName += ".TERM";
	if(!NLPOP::Exist_of_This_File(FileName)){
		AppCall::Secretary_Message_Box("读取文章不存在：CFilter::Filtering_DocMatrix_to_Terms_Port()");
		return;
	}
	//*************
	map<string, size_t> loc_FeatureCoords;
	DOCCOM::Load_string_to_size_t_map(FileName.c_str(), loc_FeatureCoords);
	
	set<size_t> TermIndex_s;
	AppCall::Maxen_Responce_Message("Collecting Terms...\n");
	//*************
	Get_Filtering_Term_Set(docmatrix, TermIndex_s, loc_FeatureCoords);

	if(TermIndex_s.empty()){
		AppCall::Secretary_Message_Box("Empty data in: CDOC::Filtering_DocMatrix_to_Terms_Port()");
		return;
	}

	AppCall::Maxen_Responce_Message("Filtering Terms...\n");
	//*************
	Generating_Filtered_Document_Matrix(docmatrix, TermIndex_s);

}
void CFilter::Generating_Filtered_Document_Matrix(const char* docmatrix, set<size_t>& TermIndex_s)
{
	string FileName = docmatrix;
	FileName += ".DATFILT";
	size_t doc_size;
	size_t TermFreq;
	size_t posit;
	bool Empty_Doc_Flag;
	vector<bool> FilteredDOC_v;
	char getlineBuf[MAX_DOC_BUFFER];
	ofstream out;
	out.open(FileName.c_str());
	if(out.bad())
		return;
	out.clear();

	FileName = docmatrix;
	FileName += ".DAT";
	ifstream in(FileName.c_str());
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	in >> doc_size;
	in.getline(getlineBuf, 256, '\n');
	for(size_t i = 0; i < doc_size; i++){
		in.getline(getlineBuf, MAX_DOC_BUFFER, '\n');
		istringstream instream(getlineBuf);
		posit = 0;
		Empty_Doc_Flag = true;
		while(instream.peek() != EOF){
			instream >> TermFreq;
			instream.getline(getlineBuf, 256, ' ');
			if(0 != TermFreq){
				if(TermIndex_s.find(posit) != TermIndex_s.end()){
					for(size_t k = 0; k < TermFreq; k++){
						out << posit << " ";
						if(Empty_Doc_Flag){
							Empty_Doc_Flag = false;
						}
					}
				}
			}
			posit++;
		}
		if(Empty_Doc_Flag){
			FilteredDOC_v.push_back(false);
			continue;
		}
		out << endl;
		FilteredDOC_v.push_back(true);
	}
	in.close();
	out.close();

	FileName = docmatrix;
	FileName += ".FILTER";
	DOCCOM::Output_Filtered_Document_Mirror(FileName.c_str(), FilteredDOC_v);
}

void CFilter::Get_Filtering_Term_Set(const char* docmatrix, set<size_t>& TermIndex_s, map<string, size_t>& FeatureCoords)
{
	map<string, size_t> TermFrequence_m;
	for(map<string, size_t>::iterator mite = FeatureCoords.begin(); mite != FeatureCoords.end(); mite++){
		TermFrequence_m.insert(make_pair(mite->first, 0));
	}
	size_t doc_size = 0;
	size_t TermFreq;
	char getlineBuf[MAX_DOC_BUFFER];
	map<string, size_t>::iterator mite;

	string FileName = docmatrix;
	FileName += ".DAT";
	ifstream in(FileName.c_str());
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	in >> doc_size;
	in.getline(getlineBuf, 256, '\n');
	for(size_t i = 0; i < doc_size; i++){
		in.getline(getlineBuf, MAX_DOC_BUFFER, '\n');
		istringstream instream(getlineBuf);
		mite = TermFrequence_m.begin();
		while(instream.peek() != EOF){
			if(mite == TermFrequence_m.end()){
				AppCall::Secretary_Message_Box("Data Error in: CDOC::Filtering_Term_Port()");
				break;
			}
			instream >> TermFreq;
			instream.getline(getlineBuf, 256, ' ');
			mite->second += TermFreq;
			mite++;
		}
	}
	in.close();
	//Same Term Frequence Info
	FileName = docmatrix;
	FileName += ".FREQUENCY";
	DOCCOM::Save_string_to_size_t_map(FileName.c_str(), TermFrequence_m);

	multimap<size_t, string> FreqString_mm;
	for(map<string, size_t>::iterator mite = TermFrequence_m.begin(); mite != TermFrequence_m.end(); mite++){
		if(mite->second == 0){
			continue;
		}
		FreqString_mm.insert(make_pair(mite->second, mite->first));
	}
	size_t DeleteHighSize;
	size_t DeleteLowSize;
	if((m_p_CDOC->m_HighRatio <  0 ) || (m_p_CDOC->m_HighRatio >  100 ) || (m_p_CDOC->m_LowRatio <  0) || (m_p_CDOC->m_LowRatio >  100 )){
		AppCall::Secretary_Message_Box("数据设置出错: CDOC::Filtering_Term_Port()");
	}
	DeleteHighSize = (size_t)(FreqString_mm.size()*(m_p_CDOC->m_HighRatio)/100.0);
	DeleteLowSize = (size_t)(FreqString_mm.size()*(m_p_CDOC->m_LowRatio)/100.0);
	
	//*************
	multimap<size_t, string>::iterator mmite = FreqString_mm.begin();
	DeleteHighSize = FreqString_mm.size()-DeleteHighSize;
	for(size_t k = 0; k < DeleteHighSize; k++){
		mmite++;
	}
	while(mmite != FreqString_mm.end()){
		mmite = FreqString_mm.erase(mmite);
	}

	//*************
	mmite = FreqString_mm.begin();
	for(size_t k = 0; k < DeleteLowSize; k++){
		mmite = FreqString_mm.erase(mmite);
	}

	//*************
	mmite = FreqString_mm.begin();
	while(mmite != FreqString_mm.end()){
		if((int)(mmite->first) > m_p_CDOC->m_LowFreq){
			TermIndex_s.insert(FeatureCoords[mmite->second]);
		}
		mmite++;
	}
}


