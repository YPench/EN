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
class CPartor
{
public:

	CECOR* m_pCECOR;
//-----Corpus Data
	map<string, set<string>> DocMentionsSet_ms;
	map<string, map<size_t, map<size_t, ACE_entity_mention*>>> m_OrderdMention_mm;
	map<string, map<string, set<ACE_entity_mention*>*>>* p_m_CoferenceSet_mm;


//-----Inner Doc
	size_t m_EntityID;
	char cDocName[64];
	map<size_t, map<size_t, ACE_entity_mention*>>* p_m_OrdDocMention_mm;
	map<string, set<ACE_entity_mention*>*>* p_m_DocCoferenceSet_ms;
	set<string>* p_m_DocCanditSpace_s;
	map<string, set<string>> m_ClusteredMention_ms;
	map<string, string> m_ResolvedMention_m;
	map<string, multimap<double, string>> D_CoValueCandit_mmm;
	map<string, map<string, double>> D_CoCanditValue_mm;
	map<string, map<string, string>> D_One2PreMany_mm;
	map<string, map<string, string>> D_One2ProMany_mm;
public:
	CPartor();
	~CPartor();

	void Coreference_Partition_Port(CoMentionPairVector& CoCandit_v, 
			vector<pair<string, double>>& Covalue_v, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm);
	
private:
	void Document_Coreference_Partition(const char* DocChar, corvaluepairite& covaluepair, corcanditpairite& cocanditpair);
	void Init_Doc_CanditPair_Coreference_Value(corvaluepairite& covaluepair, corcanditpairite& cocanditpair);
	void Greedy_Matched_to_the_Former_without_Conflict(ACE_entity_mention& pmMention);
	bool IS_Conflicting(set<string>& pmCoSet_s, string MentionID, set<string>& pmConfict_s);
	bool IS_Conflicting(const char* Pri_MentionID, const char* CanditMentionID);
	
	void Inserting_Posivie_Coreference(const CoPairValueItor& valueite, const CoMPairItor& canditite);
	void Inserting_Negetive_Coreference(const CoPairValueItor& valueite, const CoMPairItor& canditite);
	void Inserting_Exclusive_Coreference(const CoPairValueItor& valueite, const CoMPairItor& canditite);
	
	double Get_Former_Coreference_Set_Evaluation(set<string>& pmCoSet_s, string MentionID);

	void Adding_Mention_as_non_Former_Coreference(ACE_entity_mention& pmMention);
	void Insert_Mention(const char* cEntityName, const char* cMentionID, bool A_New_Nntity_Flag);
	void Insert_Mention(const char* Pri_MentionID, const char* CanditMentionID);
	void Delete_Mention(const char* pmcMentionID);
	bool Coreference_Partition_Error_Check(const char* cEntityName, const char* cMentionID);
	double Get_Two_Mention_Coreference_Evaluation(const char* cF_MentionID, const char* cS_MentionID);
};
