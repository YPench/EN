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
#include "CSegmter.h"

class CDOC;
class CFilter
{
public:
	CDOC* m_p_CDOC;
	CSegmter m_CSegmter;
	//map<string, size_t> m_FeatureCoords;
public:
	CFilter();
	~CFilter();


	void Init_Lexicon(const char* leciconfolder, set<string>& m_WordSpace, size_t& Max_Lexicon_Size);
	void Generating_Document_Matrix(const char* docmatrix, vector<pCGigaDOC>& m_GigaDOC_v);

	void Filtering_DocMatrix_to_Terms(const char* docmatrix);
	void Get_Filtering_Term_Set(const char* docmatrix, set<size_t>& TermIndex_s, map<string, size_t>& FeatureCoords);
	void Generating_Filtered_Document_Matrix(const char* docmatrix, set<size_t>& TermIndex_s);
	

};
