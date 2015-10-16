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
#include "Config.h"

#define EXCLUSIVE "Exclusive"
#define Pronoun_Backtracking_Number 3

typedef vector<pair<string, double>>::iterator CoPairValueItor;
typedef vector<pair<ACE_entity_mention*, ACE_entity_mention*>>::iterator CoMPairItor;
typedef pair<vector<pair<string, double>>::iterator,vector<pair<string, double>>::iterator> corvaluepairite;
typedef pair<vector<pair<ACE_entity_mention*, ACE_entity_mention*>>::iterator,vector<pair<ACE_entity_mention*, ACE_entity_mention*>>::iterator> corcanditpairite;
typedef vector<pair<ACE_entity_mention*, ACE_entity_mention*>> CoMentionPairVector;
typedef vector<pair<string, double>> CoValueVector;

namespace ECCOM{

	void Get_Acronym(vector<string>& words_v, size_t num, char* cAcronymBuf);
	void English_Token(const char* cstr, vector<string>& output_v);
	
	char* ToLowerCase(char *str);
	char* ToUpperCase(char *str);
	char* prestrcat_s(char *dest, size_t num, const char* src);
	bool words_nested(vector<string>& pmFstr_v, vector<string>& pmSstr_v);
	
	void Greedy_SubTYPE_Matching_Method(map<string, ACE_entity_mention*>& MentionSpace_m, CoMentionPairVector& CoCandit_v, map<string, map<string, set<ACE_entity_mention*>*>>& CoferenceSet_mm);
	void Document_Entity_Mention_Ording(vector<ACE_entity_mention*>& pmEntityMeniton_v);
	void ACE_Corpus_to_Document_Entity_Vector_Point(ACE_Corpus& pmACE_Corpus, map<string, vector<ACE_entity*>*>& DocEntityVectorP_m);
	void Output_Coreference_Doc(const char* FilePath, vector<ACE_entity*>& DocEntity_v, ACE_DocInfo& pmDocInfo);
	void ACE_Perl_Evaluation(const char* cparameters);

	
	void Get_Mention_Space(map<string, ACE_entity>& ACE_Entity_Info_map, map<string, ACE_entity_mention*>& MentionSpace_m);
	void Get_Docs_Entity_Mentions(ACE_Corpus& pmACE_Corpus, vector<string>& pmCanditDoc_v, map<string, vector<ACE_entity_mention*>*>& DocsMentons_v);
	void Get_Docs_Related_Mentions(ACE_Corpus& pmACE_Corpus, map<string, map<string, set<string>>>& DocsRelatedMentions_mms);
	bool Nested_Entity_Mentions(ACE_entity_mention& pmF_mention, ACE_entity_mention& pmS_mention);
	void Mention_Space_to_Doc_Mention_Set(map<string, ACE_entity_mention*>& pmMentionSpace_m, map<string, set<string>>& pmDocMentionsSet_ms);
};

