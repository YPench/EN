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
#include "CSegmter.h"


namespace RCCOM{

	void Generating_Relation_Cases(const char* charInstance, const char* Arg_1, const char* Arg_2, vector<Relation_Case>& Relation_Case_v);

	void Get_Relation_Case_Info(const char* infopath, list<Relation_Case>& Relation_Case_l);
	void Generate_ICTCLAS_Lexicon(CSegmter& m_CSegmter, list<Relation_Case>& Relation_Case_l);
	void Wrods_Length_Detect(const char* lexiconpath, map<size_t, size_t>& LengthCnt_map);
	
	string Get_Six_Types_of_Entity_Positional_Structures(Relation_Case& pm_RCase);
	string Get_Entity_Positional_Structures(Relation_Case& pm_RCase);
	string Get_Four_Types_of_Entity_Positional_Structures(Relation_Case& pm_RCase);
	
	void Entity_Adjacent_Words_and_POS_Feature_Extracting(CSegmter& m_CSegmter, string EntityID, string prix, string prox, map<string, float>& WordsCnt_map, size_t Range);
	void RelationContext_Adjacent_Words_and_POS_Feature_Extracting(CSegmter& m_CSegmter, RelationContext& pmContext, map<string, float>& WordsCnt_map, size_t Range);
	
	void Local_Adjacent_Words_Singleton_POS_Feature_Extracting(CSegmter& m_CSegmter, const char* sentchar, map<string, float>& WordsCnt_map, string prix, string prox);
	void Adjacent_Words_POS_Feature_Extracting(CSegmter& m_CSegmter, const char* sentchar, map<string, float>& WordsCnt_map, string prix, string prox);
	void Get_Entity_Mention_extent_Map(map<string, ACE_entity>& ACE_Entity_Info_map, map<string, ACE_entity_mention>& EntityMention_map);
	
	void Output_For_LIBSVM(const char* inputpath, bool TYPE_Flag, const char* outputfolder);

	void Adding_Entity_Extent_and_Head_to_Lexicon(vector<Relation_Case>& Relation_Case_v, set<string>& pmLexicon, char TYPE);
	void Delet_0AH_and_20H_in_Relation_Case(Relation_Case& pmRelCase);
	void Delet_0AH_and_20H_in_RelationContext(RelationContext& pm_context);
	
	void Extracting_Entity_Mention_Context_in_Relation_Mention(ACE_relation_mention& pmRMention, ACE_entity_mention& pmEMention, string& o_prix, string& o_prox);
};



