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
#include <algorithm>
#include "Exture.h"
#include "ConvertUTF.h"
#include "ECOR.h"
#include "ECCom.h"

CExtrue::CExtrue()
{

}


CExtrue::~CExtrue()
{

}




void CExtrue::Extracting_Mention_Pair_Feature_Port(pair<ACE_entity_mention*, ACE_entity_mention*>& MentionPairCandit, vector<string>& pmFeatureVector)
{
	ACE_entity_mention& F_mention = *MentionPairCandit.first;
	ACE_entity_mention& S_mention = *MentionPairCandit.second;
	
	//string adorn = F_mention.
	Prefixion_and_Suffixation_and_Nested_Words_Matching_Feature("H", F_mention.head.charseq.c_str(), S_mention.head.charseq.c_str(), pmFeatureVector);

	Prefixion_and_Suffixation_and_Nested_Words_Matching_Feature("E", F_mention.extent.charseq.c_str(), S_mention.extent.charseq.c_str(), pmFeatureVector);

	Acronym_Matching("H", F_mention.head.charseq.c_str(), S_mention.head.charseq.c_str(), pmFeatureVector);

	Acronym_Matching("E", F_mention.extent.charseq.c_str(), S_mention.extent.charseq.c_str(), pmFeatureVector);

	m_pCECOR->m_Pasture.Pased_Feature_Extracting_Port(F_mention, S_mention, pmFeatureVector);

}




void CExtrue::Acronym_Matching(const char* mentionTYPE, const char* cFirst, const char* cSecond, vector<string>& pmFeatureVector)
{
	ostringstream ostrsteam;
	const char* clong_mention;
	const char* cshort_mention;
	if(strlen(cFirst) >= strlen(cSecond)){
		clong_mention = cFirst;
		cshort_mention = cSecond;
	}
	else{
		clong_mention = cSecond;
		cshort_mention = cFirst;
	}
	
	vector<string> acronym_v;
	char cAcronymBuf[1024];
	ECCOM::English_Token(clong_mention, acronym_v);
	ECCOM::Get_Acronym(acronym_v, 1024, cAcronymBuf);
	if(strlen(cAcronymBuf) < 2){
		return;
	}
	ECCOM::ToUpperCase(cAcronymBuf);

	vector<string> words_v;
	ECCOM::English_Token(cshort_mention, words_v);
	if(words_v.empty()){
		return;
	}
	if(strstr(cAcronymBuf, words_v.rbegin()->data())){
		ostrsteam.str("");
		ostrsteam << mentionTYPE << "_sfix_AM";
		pmFeatureVector.push_back(ostrsteam.str());
	}
	else if(strstr(cAcronymBuf, words_v.begin()->data())){
		ostrsteam.str("");
		ostrsteam << mentionTYPE << "_pfix_AM";
		pmFeatureVector.push_back(ostrsteam.str());
	}
	if(words_v.size() < 3){
		return;
	}
	vector<string>::iterator vite = words_v.begin();
	vector<string>::iterator endite = words_v.end();
	vite++;
	endite--;
	for(; vite != endite; vite++){
		if(strstr(cAcronymBuf, vite->data())){
			if(strstr(cAcronymBuf, words_v.begin()->data())){
				ostrsteam.str("");
				ostrsteam << mentionTYPE << "_N_AM";
				pmFeatureVector.push_back(ostrsteam.str());
			}
		}
	}
}

void CExtrue::Prefixion_and_Suffixation_and_Nested_Words_Matching_Feature(const char* mentionTYPE, const char* cFirst, const char* cSecond, vector<string>& pmFeatureVector)
{
	char SentBuf[1024];
	ostringstream ostrsteam;
	vector<string> Fstr_v;
	vector<string> Sstr_v;

	if(!strcmp(cFirst, cSecond)){
		ostrsteam.str("");
		ostrsteam << mentionTYPE << "_mth";
		pmFeatureVector.push_back(ostrsteam.str());
	}

	ECCOM::English_Token(cFirst, Fstr_v);
	ECCOM::English_Token(cSecond, Sstr_v);
	
	size_t mini_size = Fstr_v.size()<Sstr_v.size()?Fstr_v.size():Sstr_v.size();
	size_t MatchingWordsCnt = 0;
	strcpy_s(SentBuf, 1024, "");
	for(; MatchingWordsCnt < mini_size; MatchingWordsCnt++){
		if(strcmp(Fstr_v[MatchingWordsCnt].c_str(), Sstr_v[MatchingWordsCnt].c_str())){
			break;
		}
		strcat_s(SentBuf, 1024, Fstr_v[MatchingWordsCnt].c_str());
	}
	ostrsteam.str("");
	ostrsteam << mentionTYPE << "_pfixc_" << MatchingWordsCnt;
	pmFeatureVector.push_back(ostrsteam.str());

	if(MatchingWordsCnt != 0){
		ostrsteam.str("");
		ostrsteam << mentionTYPE << "_pfix_" << SentBuf;
		pmFeatureVector.push_back(ostrsteam.str());
	}

	vector<string>::reverse_iterator fvite = Fstr_v.rbegin();
	vector<string>::reverse_iterator svite = Sstr_v.rbegin();
	MatchingWordsCnt = 0;
	strcpy_s(SentBuf, 1024, "");
	for( ; ((fvite != Fstr_v.rend()) && (svite != Sstr_v.rend())); fvite++, svite++, MatchingWordsCnt++){
		if(strcmp(fvite->data(), svite->data())){
			break;
		}
		if(strlen(SentBuf) == 0){
			strcpy_s(SentBuf, 1024, fvite->data());
		}
		else{
			ECCOM::prestrcat_s(SentBuf, 1024, fvite->data());
		}
	}
	ostrsteam.str("");
	ostrsteam << mentionTYPE << "_sfixc_" << MatchingWordsCnt;
	pmFeatureVector.push_back(ostrsteam.str());

	if(MatchingWordsCnt != 0){
		ostrsteam.str("");
		ostrsteam << mentionTYPE << "_sfix_" << SentBuf;
		pmFeatureVector.push_back(ostrsteam.str());
	}
	if(Fstr_v.size() > Sstr_v.size()){
		if(ECCOM::words_nested(Fstr_v, Sstr_v)){
			ostrsteam.str("");
			ostrsteam << mentionTYPE << "_SiF";
			pmFeatureVector.push_back(ostrsteam.str());

			ostrsteam.str("");
			ostrsteam << mentionTYPE << "_SiF_" << cSecond;
			pmFeatureVector.push_back(ostrsteam.str());
		}
	}
	if(Fstr_v.size() < Sstr_v.size()){
		if(ECCOM::words_nested(Sstr_v, Fstr_v)){
			ostrsteam.str("");
			ostrsteam << mentionTYPE << "_FiS";
			pmFeatureVector.push_back(ostrsteam.str());

			ostrsteam.str("");
			ostrsteam << mentionTYPE << "_FiS_" << cFirst;
			pmFeatureVector.push_back(ostrsteam.str());
		}
	}

	/*if(strlen(cFirst) > strlen(cSecond)){
		if(strstr(cFirst, cSecond)){
			ostrsteam.str("");
			ostrsteam << mentionTYPE << "_F_Nested_in_S";
			pmFeatureVector.push_back(ostrsteam.str());
		}
	}
	if(!(strlen(cFirst) > strlen(cSecond))){
		if(strstr(cSecond, cFirst)){
			ostrsteam.str("");
			ostrsteam << mentionTYPE << "_S_Nested_in_F";
			pmFeatureVector.push_back(ostrsteam.str());
		}
	}*/
}

