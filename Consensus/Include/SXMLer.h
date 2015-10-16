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

class SXMLer
{

public:
	bool Xercesc_Initialize_Flag;
	size_t Pased_File_Cnt;

public:
	SXMLer();
	~SXMLer();

	string Xercesc_Initialize();
	string Xercesc_ACE_Folder_For_sgm_Files_Filter(string path, ACE_Corpus& m_ACE_Corpus);
	string Xercesc_ACE_Folder_Files_Filter(string path, ACE_Corpus& m_ACE_Corpus);

	bool Xercesc_ACE_Relation_Corpus_Extractor(const char* xmlFile, ACE_Corpus& m_ACE_Corpus);
	bool Xercesc_ACE_sgm_Content_Extractor(const char* xmlFile, ACE_Corpus& m_ACE_Corpus);
	bool Xercesc_Gigaword_Content_Extractor(const char* xmlFile, vector<pCGigaDOC>& m_GigaDOC_v, const char* TYPE);
	bool Xercesc_ACE_Action_Mention_Extractor(const char* xmlFile, ACE_Corpus& m_ACE_Corpus);
	
};
