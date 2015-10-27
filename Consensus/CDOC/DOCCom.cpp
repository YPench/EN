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
#include "DOCCom.h"

void DOCCOM::LDA_printf(const char* cFormal, const char* cParameter)
{
	ostringstream ostream;
	ostream << "cFormal: " << cFormal;
	ostream << "cParameter: " << cParameter << endl << endl;
	AppCall::Maxen_Responce_Message(ostream.str().c_str());
}
void DOCCOM::LDA_printf(const char* cFormal, int intParameter)
{
	ostringstream ostream;
	ostream << "cFormal: " << cFormal;
	ostream << "intParameter: " << intParameter << endl << endl;
	AppCall::Maxen_Responce_Message(ostream.str().c_str());
}
void DOCCOM::LDA_printf(const char* cFormal)
{
	AppCall::Maxen_Responce_Message(cFormal);
}
void DOCCOM::Load_LDA_Phi_or_Theta(const char* cloadpath, vector<vector<double>*>& LDAphitheta_vv)
{
	char getlineBuf[MAX_DOC_BUFFER];
	double phi;
	ifstream in(cloadpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(in.peek() != EOF){
		in.getline(getlineBuf, MAX_DOC_BUFFER,'\n');
		istringstream instream(getlineBuf);
		vector<double>* pPhi_v = new vector<double>;
		LDAphitheta_vv.push_back(pPhi_v);
		while(instream.peek() != EOF){
			instream >> phi;
			instream.getline(getlineBuf, MAX_DOC_BUFFER,' ');
			pPhi_v->push_back(phi);
		}
	}
	in.close();
}




void DOCCOM::Load_Filtered_Document_Mirror(const char* loadpath, vector<bool>& pmFilteredDoc_v)
{
	char SentenceBuf[MAX_SENTENCE];
	size_t doc_cnt;
	bool loc_Flag;

	ifstream in(loadpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	in >> doc_cnt;
	in.getline(SentenceBuf, MAX_SENTENCE,'\n');
	for(size_t i = 0; i < doc_cnt; i++){
		in >> loc_Flag;
		pmFilteredDoc_v.push_back(loc_Flag);
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
	}
	in.close();
 
}

void DOCCOM::LDA_Output_Event_Words_with_Erasing(const char* samepath, vector<map<double, string>*>& TWordstr_v)
{
	ofstream out(samepath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(size_t i = 0; i < TWordstr_v.size(); i++){
		out << "Event " << i << ":" << endl;
		map<double, string>& loc_TWordstr_m = *TWordstr_v[i];
		for(map<double, string>::reverse_iterator rmite = loc_TWordstr_m.rbegin(); rmite != loc_TWordstr_m.rend(); rmite++){
			out << rmite->second << ";";
		}
		out << endl << endl;
		delete TWordstr_v[i];
	}
	TWordstr_v.clear();
	out.close();
}
void DOCCOM::Output_Filtered_Document_ID(const char* savepath, vector<string>& pmDoc_v)
{
	ofstream out(savepath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	out << pmDoc_v.size() << endl;
	for(size_t i = 0; i < pmDoc_v.size(); i++){
		out << pmDoc_v[i] << endl;
	}
	out.close();

}
void DOCCOM::Load_Filtered_Document_ID(const char* loadpath, vector<string>& pmDoc_v)
{
	char SentenceBuf[MAX_SENTENCE];
	size_t doc_cnt;

	ifstream in(loadpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	in >> doc_cnt;
	in.getline(SentenceBuf, MAX_SENTENCE,'\n');
	for(size_t i = 0; i < doc_cnt; i++){
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
		pmDoc_v.push_back(SentenceBuf);
		
	}
	in.close();

}
void DOCCOM::Load_Event_Top_Terms(const char* ladpath, vector<vector<string>*>& pmEventTopTerm_vv)
{
	char SentenceBuf[MAX_SENTENCE];

	ifstream in(ladpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
		istringstream instream(SentenceBuf);
		vector<string>* pTerm_v = new vector<string>;
		pmEventTopTerm_vv.push_back(pTerm_v);
		while(instream.peek() != EOF){
			instream.getline(SentenceBuf, MAX_SENTENCE,';');
			pTerm_v->push_back(SentenceBuf);
		}
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
	}
	in.close();
}
void DOCCOM::Load_CLUSTER_Info(const char* ladpath, vector<size_t>& pmDocEventID_v)
{

	char getlineBuf[MAX_DOC_BUFFER];
	size_t doc_cnt;
	size_t EventID;

	ifstream in(ladpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	in >> doc_cnt;
	in.getline(getlineBuf, MAX_DOC_BUFFER,'\n');
	for(size_t i = 0; i < doc_cnt; i++){
		in >> EventID;
		in.getline(getlineBuf, MAX_DOC_BUFFER,'\n');
		pmDocEventID_v.push_back(EventID);
		
	}
	in.close();

}

void DOCCOM::Output_Filtered_Document_Mirror(const char* savepath, vector<bool>& pmFilteredDoc_v)
{
	size_t FilteredDocCnt = 0;
	for(size_t i = 0; i < pmFilteredDoc_v.size(); i++){
		if(!pmFilteredDoc_v[i]){
			FilteredDocCnt++;
		}
	}
	ofstream out(savepath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	out << pmFilteredDoc_v.size() << '\t' << "TYPE 1: " << pmFilteredDoc_v.size() - FilteredDocCnt << '\t' << "TYPE 0: " << FilteredDocCnt << endl;
	for(size_t i = 0; i < pmFilteredDoc_v.size(); i++){
		if(pmFilteredDoc_v[i]){
			out << 1 << endl;
		}
		else{
			out << 0 << endl;
		}
	}
	out.close();

}
void DOCCOM::Load_Sequence_to_String_Map_Realation(string pmFilePath, map<size_t, string>& pmSeq2Str_m)
{
	if(!NLPOP::Exist_of_This_File(pmFilePath)){
		return;
	}
	char SentenceBuf[MAX_SENTENCE];
	size_t Sequence;

	ifstream in(pmFilePath.c_str());
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
		in >> Sequence;
		in.getline(SentenceBuf, MAX_SENTENCE,',');
		
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
		pmSeq2Str_m.insert(make_pair(Sequence, SentenceBuf));
	}
	in.close();
}
void DOCCOM::Load_LDA_Wordmap(const char* cloadpath, map<size_t, size_t>& LDAIndex2TermID_m)
{
	char SentenceBuf[MAX_SENTENCE];
	size_t doc_size;
	size_t TermID;
	size_t LDATermIndex;

	ifstream in(cloadpath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	in >> doc_size;
	in.getline(SentenceBuf, MAX_SENTENCE, '\n');
	for(size_t i = 0; i < doc_size; i++){
		in.getline(SentenceBuf, MAX_SENTENCE, '\n');
		istringstream instream(SentenceBuf);
		instream >> TermID;
		instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
		instream >> LDATermIndex;
		LDAIndex2TermID_m.insert(make_pair(LDATermIndex, TermID));
	}
	in.close();
}

void DOCCOM::Save_Sequence_to_String_Map(string pmSavePath, map<size_t, string>& pmSeq2Str_m)
{
	if(pmSeq2Str_m.empty())
		return;
	_mkdir(pmSavePath.c_str());
	string pmSaveName;
	if(pmSavePath.rfind('\\') == (pmSavePath.length()-1))
		pmSaveName= pmSavePath + "Seq2Str.dat";
	else
		pmSaveName = pmSavePath;
	
	ofstream out(pmSaveName.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(map<size_t, string>::iterator mite = pmSeq2Str_m.begin(); mite != pmSeq2Str_m.end(); mite++)
	{
		out << (*mite).first << ',' << (*mite).second << endl;
	}
	out.close();
}
void DOCCOM::Load_string_to_size_t_map(const char* savepath, map<string, size_t>& pmStr2size_m)
{
	if(!NLPOP::Exist_of_This_File(savepath)){
		return;
	}
	char SentenceBuf[MAX_SENTENCE];
	size_t loc_size;

	ifstream in(savepath);
	if(in.bad())
		return;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
		return;
	}
	while(in.peek() != EOF){
		in.getline(SentenceBuf, MAX_SENTENCE,',');
		in >> loc_size;
		pmStr2size_m.insert(make_pair(SentenceBuf, loc_size));
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
	}
	in.close();

}
void DOCCOM::Save_string_to_size_t_map(const char* savepath, map<string, size_t>& pmStr2size_m)
{

	ofstream out(savepath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(map<string,size_t >::iterator mite = pmStr2size_m.begin(); mite != pmStr2size_m.end(); mite++)
	{
		out << mite->first << ',' << mite->second << endl;
	}
	out.close();

}