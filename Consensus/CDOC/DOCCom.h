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
#pragma once


#include "ace.h"

#define MIN_CLUSTERING_DOC_NUM 10
#define MAX_PARAMETERS 100



#define LDA_PRINTF_ENABLE 1
namespace DOCCOM{

	void Load_Sequence_to_String_Map_Realation(string pmFilePath, map<size_t, string>& pmSeq2Str_m);
	void Save_Sequence_to_String_Map(string pmSavePath, map<size_t, string>& pmSeq2Str_m);
	void Output_Filtered_Document_Mirror(const char* savepath, vector<bool>& pmFilteredDoc_v);
	void Load_Filtered_Document_Mirror(const char* loadpath, vector<bool>& pmFilteredDoc_v);
	void Output_Filtered_Document_ID(const char* savepath, vector<string>& pmDoc_v);
	void Load_Filtered_Document_ID(const char* loadpath, vector<string>& pmDoc_v);
	void Load_CLUSTER_Info(const char* ladpath, vector<size_t>& pmDocEventID_v);
	void Load_Event_Top_Terms(const char* ladpath, vector<vector<string>*>& pmEventTopTerm_vv);
	
	void Save_string_to_size_t_map(const char* savepath, map<string, size_t>& pmStr2size_m);
	void Load_string_to_size_t_map(const char* savepath, map<string, size_t>& pmStr2size_m);
	void LDA_printf(const char* cFormal, const char* cParameter);
	void LDA_printf(const char* cFormal, int intParameter);
	void LDA_printf(const char* cFormal);
	void LDA_Output_Event_Words_with_Erasing(const char* samepath, vector<map<double, string>*>& TWordstr_v);
	void Load_LDA_Wordmap(const char* cloadpath, map<size_t, size_t>& LDAIndex2TermID_m);
	void Load_LDA_Phi_or_Theta(const char* cloadpath, vector<vector<double>*>& LDAphitheta_vv);
};
template<typename TCLASS> void Load_LDA_Event_Top_Words(const char* filepath, vector<map<double, TCLASS>*>& TWords_v)
{
	char SentenceBuf[MAX_SENTENCE];
	TCLASS WordID;
	double Weight;

	ifstream in(filepath);
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
		if(in.peek() != 'T'){
			AppCall::Secretary_Message_Box("Data Error in: DOCCOM::Load_LDA_Event_Top_Words()");
			return;
		}
		in.getline(SentenceBuf, MAX_SENTENCE,'\n');
		map<double, TCLASS>* ploc_Twords_m = new map<double, TCLASS>;
		TWords_v.push_back(ploc_Twords_m);
		
		while(true){
			in.getline(SentenceBuf, MAX_SENTENCE,'\n');
			istringstream instream(SentenceBuf);
			instream.getline(SentenceBuf, MAX_SENTENCE, '\t');
			instream >> WordID;
			instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
			instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
			instream.getline(SentenceBuf, MAX_SENTENCE, ' ');
			instream >> Weight;
			ploc_Twords_m->insert(make_pair(Weight, WordID));
			if(in.peek() == 'T' || in.peek() == EOF){
				break;
			}
		}
	}
	in.close();
}


