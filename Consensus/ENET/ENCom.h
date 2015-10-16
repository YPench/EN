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


#include "ace.h"
#include "igraph.h"

#define MAX_PAJEC_VERTEX 80
#define MAX_PAJEC_EDGE 200
#define MIN_RELATION_VALUE 0.4

#define  VERTEX_RESIZE(x) (x)*5.0+0.010101
#define  ENTITY_WEIGHT(x) (x-0.010101)/5.0
#define  LAYOUT_REARRANGE(x) (x+1)/2.0

typedef struct _relationinfo{
	string DOCID;
	size_t SENID;
	string arg1;
	string arg2;
	string TYPE;
	double value;
	size_t Arg1_ID;
	size_t Arg2_ID;
}relationinfo;

typedef struct _pajekvertex{
	size_t vex;
	string name;
	string TYPE;
	double x;
	double y;
	double weight;
}pajekvertex;

typedef struct _pajekedge{
	size_t vex1;
	size_t vex2;
	double value;
	string TYPE;
}pajekedge;
namespace PAJEK{

	void Load_Pajek_Partition_Info(const char* cFilePath,  list<pair<string, size_t>>& partition_l);
	void Load_Pajek_Vertex_Info(const char* cFilePath, list<pajekvertex*>& pajekvertex_l);
	void Load_Pajek_Edges_Info(const char* cFilePath, list<pajekedge*>& pakekedge_l);
	
	void Save_Pajek_by_Vertex_and_Edges_Info(const char* cFilePath, list<pajekvertex*>& pajekvertex_l, list<pajekedge*>& pakekedge_l, list<pair<string, size_t>>& partition_l);
	void Save_Vector_String_to_Pajek(const char* cFilePath, vector<string>& FileLine_v);
	void Save_Igraph_to_Pajek(const char* cFilePath, igraph_t& event_network, string memo);

	void Appending_Layout_to_Pajek(const char* cFilePath, igraph_matrix_t& layout_matrix);
	void Appending_Attribute_to_Edges_in_Pajek(const char* cFilePath, igraph_vector_t& attr_v);
	void Appending_Vertex_Weighting_to_Pajek(const char* cFilePath, igraph_vector_t& weight);
	void Appending_Vertex_Partition_to_Pajek(const char* cFilePath, map<string, size_t>& Partition_m);
	void Appending_Vertex_Info_to_Pajek(const char* cFilePath, map<size_t, string>& VertexInfo_m);
	void Appending_Info_to_Edges_in_Pajek(const char* cFilePath, vector<string>& EdgesInfo_v);
};

namespace ENCOM{
	string EncodeHtml(const wstring& html);

	void Generating_iGraph_by_Vertex_Set(igraph_t& event_network, map<string, double>& NmaedEntity_m, map<string, string>& HtmlDecode_m);
	void Generating_iGraph_by_Vertex_Set(igraph_t& event_network, map<string, size_t>& vertex_m);
	

	
	bool Get_Next_Entity_Mention(ifstream& in, vector<entity_mention>& entity_mention_v, string& Sent);
	void Generating_Relation_Case(const char* cSent, entity_mention& arg1, entity_mention& arg2, Relation_Case& pmRelation_Case);
	void Load_Extracted_Entities_Result(const char* cFilePath, vector<entity_mention*>& pmEntities_v);
	void Load_Extracted_Relations_Result(const char* cFilePath, vector<relationinfo*>& pmRelations_v);

	size_t Get_Event_Num(const char* cEVENT_Path);
	size_t Get_ENTITY_Num(const char* cENTITY_Path);
	size_t Get_RELATION_Num(const char* cRELATION_Path);
};
