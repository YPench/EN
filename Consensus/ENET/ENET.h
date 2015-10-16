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
#include "ENRun.h"
#include "ENGen.h"
#include "Act.h"
class CCEDT;
class CRDC;
class CENET_Dlg;

class ENET
{
public:
		CENET_Dlg* p_m_CENET_Dlg;
		CENRun m_CENRun;
		CEGen m_ENGen;
		CAct m_CAct;

		bool Collected_Doc_Event_Flag;
		bool ENET_Incremental_Flag;
		string Collected_EventName;
		string Collected_EventPath;
		string Collected_KeyWords;
		string Collected_FilePrix;
public:
	ENET();
	~ENET();

	void Collecting_Events_Organizing_Info();
	void Deleting_Events_Organizing_Files(const char* cprox);

	void Generating_and_Viewing_Event_Network_Port();
	void Generating_Entity_of_Event_Network_Port();
	void Generating_Relation_of_Event_Network_Port();

	void Run_Named_Entity_Port(const char* cWorkSpace, CCEDT* pm_CCEDT);
	void Run_Entity_Relation_Port(const char* cWorkSpace, CRDC* pm_CRDC);
	
	void Updata_Configurations();

	void Cellected_Entity_Star_Layout_Port();
	void Cellected_Entity_Short_Path_Port();

	void Person_Location_Time_Analysis_Port();
};
