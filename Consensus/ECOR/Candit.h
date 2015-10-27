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
