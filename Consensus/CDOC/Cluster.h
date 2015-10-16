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

class CCluster
{
public:

	size_t m_LDA_Term_Num;
	size_t m_LDA_Doc_Num;
	size_t m_LDA_Event_Num;

public:
	CCluster();
	~CCluster();

	void Generating_Clustering_Training_Data(const char* docmatrix, vector<bool>& pmDocFilter_v);

	void Documents_Clustering_to_Event();
	void LDA_Twords_to_Document_Terms(const char* docmatrix);
	void Documents_Term_Space_Distribution();
	void Merging_Clustered_Info(const char* docmatrix);
	void Sorting_Documents_by_Max_Event_Distribution(const char* docmatrix);
};
