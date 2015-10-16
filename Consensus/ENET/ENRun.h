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
