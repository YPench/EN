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
