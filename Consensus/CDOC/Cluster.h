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
