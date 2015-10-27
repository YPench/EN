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

class CCEDT;
class CRDC;
class CENET_Dlg;
class CENRun
{
public:
	CENRun();
	~CENRun();
	set<string> PBreakpoint_s;

		bool Collected_Doc_Event_Flag;
		bool ENET_Incremental_Flag;
		string Collected_EventName;
		string Collected_EventPath;
		string Collected_KeyWords;
		string Collected_FilePrix;

public:
	void Extracting_Named_Entity_From_Documents(const char* cWorkSpace, CCEDT* pm_CCEDT);
	void Extracting_Entity_Relation_From_Documents(const char* cWorkSpace, CRDC* pm_CRDC);

	void Extracting_Named_Entity_by_Event_Document(const char* cEVENT_Path, map<string, const char*>& DocID2TEXT_m, CCEDT* pm_CCEDT);
	void Extracting_Entity_Relation_By_ENTITY_Document(const char* cENTITY_Path, const char* cRELATION_Path, CRDC* pm_CRDC);
	void Get_Document_Events(const char* cWorkSpace, map<string, vector<pair<string, const char*>>*>& EventDoc_mv, map<string, string>& EventPath_m);

	void Extracting_Person_Location_Time_Analysis_RELATION(CENET_Dlg* p_m_CENET_Dlg);
};
