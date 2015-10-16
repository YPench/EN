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
