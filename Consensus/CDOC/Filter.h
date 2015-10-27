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
