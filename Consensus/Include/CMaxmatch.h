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

#include "CGCom.h"
#include "ace.h"

#define	 MAX_CLAUSE_LENGTH 128

class CMaxmatch
{
public:
	bool SegMatrix[MAX_CLAUSE_LENGTH][MAX_CLAUSE_LENGTH];
	set<string> lexicon_set;
	int maxtoken_size;
	bool Init_CMaxmatch_Flag;

public:
	CMaxmatch();
	~CMaxmatch();

	void Reset_Maximum_Matcher_Lexicon(set<string>& pmOuterLexicion_s);
	void Reset_Maximum_Matcher_Lexicon(const char* lexiconpath);

	void Maximum_Match_Segmentation(const char* Sentbuf, vector<string>& segment_v, set<string>& pmWordSet, const char* cSegmentType);
	void Maximum_Match_Segmentation_Traditional(const char* Sentbuf, vector<string>& segment_v, const char* cSegmentType);
	void Maximum_Match_Segmentation_Traditional(const char* Sentbuf, vector<string>& segment_v, const char* cSegmentType, set<string>& pmWordSet);
	
		void Generate_Segmentation_Matrix(const char* SentBuf, set<string>& locCSWords_Set, vector<string>& TokenCS_v, unsigned int Max_Word_Length, bool SegMatrix[MAX_CLAUSE_LENGTH][MAX_CLAUSE_LENGTH], unsigned int& mLength);
	void Forword_Max_Maximum_Matching_Segmentation(bool SegMatrix[MAX_CLAUSE_LENGTH][MAX_CLAUSE_LENGTH], unsigned int mLength, vector<int>& Seg_v);
	void Backword_Max_Maximum_Matching_Segmentation(bool SegMatrix[MAX_CLAUSE_LENGTH][MAX_CLAUSE_LENGTH], unsigned int mLength, vector<int>& Seg_v);
	void Generate_Segmentation_Pathn(vector<int>& pmSeg_v, vector<string>& pmTokenCS, vector<string>& pmSegment_v);


};

