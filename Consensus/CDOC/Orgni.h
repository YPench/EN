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

class CDOC;
class COrgni
{
public:
	CDOC* m_p_CDOC;
	//map<string, size_t> m_FeatureCoords;
public:
	COrgni();
	~COrgni();
	bool Checking_Iterativ_Condition(int hierarchy,  vector<bool>& pmDocFilter_v);
	bool Checking_and_Save_Document_Event_Info(int hierarchy, const char* docmatrix, const char* cWorkSpace, vector<bool>& pmDocFilter_v);
	void Documents_Organizing_Segmenation(const char* cOrganizingFolder, vector<string>& TimeSetpFolder_v,  int pm_TimeStep,  BOOL pCDOC_Incremental_Flag);
	bool Checking_and_Generating_Events_Iterative_Filter_Mirror_Vector(map<string, size_t>& pmDocID2Event_m, map<size_t, vector<bool>*>& pmIteFilter_mv, vector<bool>& pmDocFilter_v);
};
