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
