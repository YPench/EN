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

class CECOR;
class CCandit
{
public:

	CECOR* m_pCECOR;
	ACE_Corpus* p_m_ACE_Corpus;
	map<string, map<string, set<string>>> m_DocsRelatedMentions_mms;
	map<string, set<string>>* p_m_RelatedMention_ms;
public:
	CCandit();
	~CCandit();


	void Collecting_Coreference_Candidates_with_EXCLUSIVE_Detecting(map<string, vector<ACE_entity_mention*>*>& DocsMentons_v, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v);
	void Collecting_All_Coreference_Candidates(map<string, vector<ACE_entity_mention*>*>& DocsMentons_v, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v);
	void Generate_Coreference_Candidate_Port(const char* cFolder, ACE_Corpus& pmACE_Corpus, vector<string>& pmCanditDoc_v, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v);

	bool Has_Relation(ACE_entity_mention& F_Mention, ACE_entity_mention& S_Mention);
	bool Has_Sexual_Distinction(ACE_entity_mention& F_Mention, ACE_entity_mention& S_Mention);
	bool Has_Numbered_Distinction(ACE_entity_mention& F_Mention, ACE_entity_mention& S_Mention);

	void Saving_Candidating_Mention_Pair(const char* savename, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v);
	bool Loading_Candidating_Mention_Pair(const char* loadname, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v);

	void Save_or_Load_Parting_Part(const char* cFolder, bool Save_or_Load_Flag, CoValueVector& pmCoValue_v, CoMentionPairVector& pmCoCandit_v);
};
