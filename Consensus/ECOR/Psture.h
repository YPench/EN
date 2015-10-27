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

#include "..\\Include\\ace.h"
#include "maxen.h"
#include "ECCom.h"
#include "Parser.h"
class CECOR;


class CPstrue
{

public:
	CECOR* m_pCECOR;

	bool Extrure_Inited_Flag;

	map<string, _PsDocInfo*> PsDocsInfo_map;
	
	string m_Current_DOCID;
	_PsDocInfo* p_m_PsDocInfo;

	_PsWordInfo* p_m_F_PsWordInfo;
	_PsWordInfo* p_m_S_PsWordInfo;
	_PsSentInfo* p_m_F_PsSentInfo;
	_PsSentInfo* p_m_S_PsSentInfo;

public:
	CPstrue();
	~CPstrue();


	void Inner_Sentence_Dependency_Feature(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention, vector<string>& pmFeatureVector);
	void Outer_Sentence_Dependency_Feature(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention, vector<string>& pmFeatureVector);
	void Same_Head_and_Tail_Dependency_Items(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention, vector<string>& pmFeatureVector);	
	void Init_Parsed_Info(ACE_Corpus& pmACE_Corpus);
	void Parsed_Info_Release();
	void Pased_Feature_Extracting_Port(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention, vector<string>& pmFeatureVector);

};
