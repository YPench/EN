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
#include "ENCom.h"

class CCEDT;
class CRDC;
class CENET_Dlg;



class CEGen
{
public:
	CEGen();
	~CEGen();


	bool Collected_Doc_Event_Flag;
	bool ENET_Incremental_Flag;
	string Collected_EventName;
	string Collected_EventPath;
	string Collected_KeyWords;
	string Collected_FilePrix;

	list<pajekvertex*> m_vertex_l;
	list<pajekedge*> m_edge_l;
	list<pair<string, size_t>> m_partition_l;

public:
	//===========================
	void Generating_and_Viewing_Event_Network(CENET_Dlg* p_m_CENET_Dlg);
	void Cellected_Entity_Star_Layout(CENET_Dlg* p_m_CENET_Dlg);
	void Cellected_Entity_Short_Path(CENET_Dlg* p_m_CENET_Dlg);

	//===========================
	void Filtering_Named_Entity_and_Relation_Info(CENET_Dlg* p_m_CENET_Dlg);
	void Merging_Named_Entity_and_Relation_Info();
	void Generating_Pajek_of_Named_Entity_Info(CENET_Dlg* p_m_CENET_Dlg);
	void Generating_Pajek_of_Relation_Info(CENET_Dlg* p_m_CENET_Dlg);

	//===========================
	void Reload_Vertex_and_Edge();
	void Release_Vertex_and_Edge();
	void Rearrange_Vertex_and_Edge();
	void Enable_Cential_Entity_and_Short_Path_Box(CENET_Dlg* p_m_CENET_Dlg);
	void Pajek_Info_Checking(list<pajekvertex*>& vertex_l, list<pajekedge*>& edge_l, list<pair<string, size_t>>& partition_l);
};
