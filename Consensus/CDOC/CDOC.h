/*
 * Copyright (C) 2015 by Yanping Chen <ypench@gmail.com>
 * Begin       : 01-Oct-2015
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser GPL (LGPL) as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.
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
