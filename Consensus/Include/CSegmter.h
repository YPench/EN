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

//-------------------CSegmter-------------------------------------------
class CSegmter
{
public:
	bool ICTCLAS50_INIT_FLAG;
	size_t Max_Lexicon_Size;

public:
	CSegmter();
	~CSegmter();

	void ICTCLAS_Segmentation_words_feature_Extracting(const char* sentchar, map<string, float>& WordsCnt_map, string prix, string prox);
	void ICTCLAS_Segmention_Port(const char* sentstr, CLAUSEPOS& pm_PaseCS);
	void Omni_words_feature_Extracting(const char* sentchar, set<string>& pmWordSet, size_t Max_Word_Legnth, map<string, float>& WordsCnt_map, string prix, string prox);
	void Omni_words_feature_Extracting(const char* sentchar, set<string>& pmWordSet, map<string, size_t>& WordsCnt_map);
	void Get_Sentence_Omni_Words_Cnt(const char* charstr, set<string>& pmWordsset, size_t Max_Word_Legnth,  map<string, unsigned long>& WordsCnt_map);
	void Initiate_Words_set(vector<DismCase>& pDismCase_v, set<string>& pmWordSet);
	//older function
	void Get_Sentence_Omni_Words_Cnt(const char* charstr, set<string>& pmWordsset, map<string, unsigned long>& WordsCnt_map);
	void English_Words_Extractor(const char* sentchar, set<string>& pmWordSet, size_t Max_Word_Legnth, map<string, float>& WordsCnt_map, string prix, string prox);
	void English_Radical_Extractor(const char* sentchar, set<string>& pmRadicalSet, size_t Max_Word_Legnth, map<string, float>& WordsCnt_map, string prix, string prox);

};
