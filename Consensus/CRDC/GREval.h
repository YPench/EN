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
#include "..\\Include\\CGCom.h"
#include "..\\Include\\ace.h"
#include "CSegmter.h"
#include "..\\ECOR\\wordnet.h"

typedef struct _Relation_Envi{
	string E1_Extend;
	string E1_Head;
	string E2_Extend;
	string E2_Head;
	string E1_inBetw_E2;
	string CBM1;
	string CAM1;
	string CBM2;
	string CAM2;
}Relation_Envi, *pRelation_Envi;


class CRDC;
class _PsDocInfo;
class CGREval
{
public:
	CRDC* pCRDC;
	CSegmter& m_CSegmter;
	Cwordnet m_Cwordnet;
	bool m_Output_Training_Data_Dynamically;

	map<string,string> Synonym_map;
	map<string, size_t> BinLeftcnt_m;
	map<string, size_t> BinMiddlecnt_m;
	map<string, size_t> BinRightcnt_m;

	CGREval(CSegmter& ref_Segmter);
	~CGREval();
	void Extracting_English_radical();
	void POS_of_Entity_Mention(ACE_entity_mention& Ref_Ei, map<string, _PsDocInfo*> PsDocsInfo_map, map<string, float>& WordsCnt_map, string index);
	void POS_and_Dependency_of_Entity_Mention(Relation_Case& pm_RelCase, map<string, _PsDocInfo*> PsDocsInfo_map, map<string, float>& WordsCnt_map, string index);
	//-------------------------------Generate Training and Testing Relation Cases
	void Generate_YPench_Evaluation_Port(const char * savepath, vector<Relation_Case*>& Relation_Case_v, DulTYPE_Vctor& pmTraining_v);
	void Generate_CRDC_Feature_Vector_Port(Relation_Case& pmRelation_Case, vector<pair<string, float>>& Features_v);

	void Generate_Che_Evaluation_Port(vector<Relation_Case*>& Relation_Case_v, DulTYPE_Vctor& pmTraining_v);
	void Generate_Zhang_Evaluation_Port(vector<Relation_Case*>& Relation_Case_v, DulTYPE_Vctor& pmTraining_v);

	void Generate_YPench_English_Evaluation_Port(const char * savepath, vector<Relation_Case*>& Relation_Case_v, DulTYPE_Vctor& pmTraining_v);
	//----------------------------------------------------------------------------------------------
	void ACE_Entity_Mention_in_Sentence(map<string, map<size_t, map<size_t, ACE_entity_mention>>*>& EntityMention_mm);
	void Delet_0AH_and_20H_in_Relation_Case(Relation_Envi& pmRelEnvi);
	string Get_Relation_Envi_and_Return_9_Types_Position_Structure(Relation_Case& pmRelCase, Relation_Envi& pmRelEnvi, map<string, map<size_t, map<size_t, ACE_entity_mention>>*>& pmEntityMention_mm);
	void N_Gram_of_Sent_Sequence_with_Range_Limination(const char* sentchar, char CharTYPE, string FeatureID, size_t n, size_t range, map<string, float>& nGramFeature_map);
	void N_Gram_of_Sent_Sequence(const char* sentchar, size_t range, map<string, float>& WordsCnt_map, string prix, string prox);

	void Generating_Synonyms_Framework(vector<Relation_Case*>& Relation_Case_v);
	void Generating_Synonyms_Framework_Cases_Features(RelationContext& loc_Context, map<string, float>& WordsCnt_map);
	void Generate_Nanda_English_Evaluation_Port(const char * savepath, vector<Relation_Case*>& Relation_Case_v, DulTYPE_Vctor& pmTraining_v);

	void English_WordNet_Extractor(const char* sentchar, map<string, float>& WordsCnt_map, string prix, string prox);
	void Extracting_WordNet_Features(Relation_Case& ref_relation, map<string, _PsDocInfo*> PsDocsInfo_map, map<string, float>& WordsCnt_map);
	void Get_Words_Sense_in_WordNet_with_POS(size_t START, size_t END, _PsDocInfo& pmPsDocInfo, map<string, float>& WordsCnt_map, string prix, string prox);
};
