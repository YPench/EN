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
#include "maxen.h"
#include "LDA//LDA.h"
#include "Filter.h"
#include "Cluster.h"
#include "Orgni.h"
class CDOC
{
public:
	CLDA m_CLDA;
	CFilter m_CFilter;
	CCluster m_CCluster;
	COrgni m_COrgni;

	BOOL LCWCC_Flag;
	BOOL ACE_ENTITY_Flag;
	BOOL ICTCLAS_Flag;
	int m_HighRatio;
	int m_LowRatio;
	int m_LowFreq;
	int m_DebugDocNum;
	BOOL m_DebugDoc_Flag;
	bool CDOC_Incremental_Flag;

	int m_Events;
	float m_alpha;
	float m_beta;
	int m_twords;
	int m_niters;
	int m_Hierarchies;
	int m_TimeStep;

	string ACECorpusFolder;
	string GigawordFolder;
public:
	CDOC();
	~CDOC();

	//====================================================================================
	void Generating_Document_Matrix_Port(const char* docmatrix, vector<pCGigaDOC>& m_GigaDOC_v);
	void Filtering_DocMatrix_to_Terms_Port(const char* docmatrix);
	void LDA_Training_Port(const char* docmatrix);
	void Documents_Clustering_to_Event_Port(const char* docmatrix);

	//====================================================================================
	void Document_Events_Organizing_Port();
	void Clustering_Documents_in_the_Current_Work_Space(const char* docmatrix, vector<bool>& pmDocFilter_v);
	void Iterative_Document_Event_Hierarchies_Clustering(int hierarchy, const char* docmatrix, const char* cWorkSpace, vector<bool>& pmDocFilter_v);
};
