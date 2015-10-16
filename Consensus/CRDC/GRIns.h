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

class CRDC;

class CGRIns
{
public:
	CRDC* pCRDC;
	CSegmter& m_CSegmter;
	CGRIns(CSegmter& ref_Segmter);
	~CGRIns();
	size_t SENID;

	//-----------------------------------Extract Positive and Negative Relation Instances
	void Generate_Positive_And_Negetive_ACE_Relation_Cases(string savepath, ACE_Corpus& m_ACE_Corpus);
	void Delete_Sentence_Contianing_Positive_Relation_Case(list<Relation_Case>& Relation_Case_l, vector<ACE_relation>& ACE_Relation_Info_v);
	void Adding_Positive_Training_Case_in_Relation_Mention(ACE_Corpus& m_ACE_Corpus, list<Relation_Case>& Relation_Case_l);

	//-----------------------------------Extract Sentence with Two Named Entities
	void Extract_Sentence_with_Two_Named_Entities(ACE_Corpus& ACE_Corpus, list<Relation_Case>& Relation_Case_l);
	void Filter_sgm_sentence_with_Relation_Mention(ACE_Corpus& ACE_Corpus, map<string, list<pair<string, pair<size_t,size_t>>>>& pm_ACE_DocSentence_map);

};
