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
#include "stdafx.h"
#include "ENGen.h"
#include "..\\Consensus.h"
#include "ConvertUTF.h"



CEGen::CEGen()
{
}
CEGen::~CEGen()
{
}

//===================================================

void CEGen::Generating_and_Viewing_Event_Network(CENET_Dlg* p_m_CENET_Dlg)
{
	string FileName;
	FileName = Collected_FilePrix;
	FileName += ".ENTITY";
	ostringstream osteam;
	osteam << "Entity: " << ENCOM::Get_ENTITY_Num(FileName.c_str());

	FileName = Collected_FilePrix;
	FileName += ".RELATION";
	osteam << "; Relation: " << ENCOM::Get_RELATION_Num(FileName.c_str());
	p_m_CENET_Dlg->m_BottonEdit.SetWindowTextW(NLPOP::string2CString(osteam.str().c_str()));

	Reload_Vertex_and_Edge();

	Filtering_Named_Entity_and_Relation_Info(p_m_CENET_Dlg);

	if(m_vertex_l.empty()){
		AppCall::Secretary_Message_Box("Empty Event Network...");
		return;
	}

	Enable_Cential_Entity_and_Short_Path_Box(p_m_CENET_Dlg);
	
	FileName = Collected_FilePrix;
	FileName += "_Temp.paj";
	if(!p_m_CENET_Dlg->Loading_EN_Info_Flag){
		AppCall::Consensus_Open_Process(FileName.c_str(), Pajet_Path);
	}
	PAJEK::Save_Pajek_by_Vertex_and_Edges_Info(FileName.c_str(), m_vertex_l, m_edge_l, m_partition_l);
	Release_Vertex_and_Edge();
}

void CEGen::Cellected_Entity_Star_Layout(CENET_Dlg* p_m_CENET_Dlg)
{
	Reload_Vertex_and_Edge();
	Filtering_Named_Entity_and_Relation_Info(p_m_CENET_Dlg);
	Rearrange_Vertex_and_Edge();

	map<string, size_t> name2id_map;
	for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); lite++){
		pajekvertex& loc_vertex = **lite;
		name2id_map.insert(make_pair(loc_vertex.name, loc_vertex.vex));
	}
	CString CboBoxText;
	p_m_CENET_Dlg->m_CentboBox.GetLBText(p_m_CENET_Dlg->m_CentboBox.GetCurSel(),CboBoxText);
	string centname = ENCOM::EncodeHtml(SCONVERT::StringToWString(NLPOP::CString2string(CboBoxText)));
	if(name2id_map.find(centname) == name2id_map.end()){
		AppCall::Secretary_Message_Box("The cential vertex is not finded: CEGen::Cellected_Entity_Star_Layout()");
		return;
	}
	//==============================
	igraph_t event_network;
	size_t limitID = m_vertex_l.size();
	limitID++;//Pajek has vertex id form 1;
	igraph_empty(&event_network, m_vertex_l.size(), 0);
	for(list<pajekedge*>::iterator lite = m_edge_l.begin(); lite != m_edge_l.end(); lite++){
		pajekedge& loc_edge = **lite;
		igraph_add_edge(&event_network, loc_edge.vex1-1, loc_edge.vex2-1);
	}
	igraph_matrix_t layout_matrix;
	igraph_matrix_init(&layout_matrix, m_vertex_l.size(), 2);
	igraph_layout_star(&event_network, &layout_matrix, name2id_map[centname], 0);
	igraph_destroy(&event_network);

	list<pajekvertex*>::iterator verite = m_vertex_l.begin();
	for(int i = 0; i < igraph_matrix_nrow(&layout_matrix); i++, verite++) {
		pajekvertex& loc_vex = **verite;
		for(int j = 0; j < igraph_matrix_ncol(&layout_matrix); j++) {
			if(1 == j){
				loc_vex.x = LAYOUT_REARRANGE(MATRIX(layout_matrix, i, j));
			}
			else if(0 == j){
				loc_vex.x = LAYOUT_REARRANGE(MATRIX(layout_matrix, i, j));
			}
		}
		if(!strcmp(loc_vex.name.c_str(), centname.c_str())){
			loc_vex.weight *= 1.5;
			continue;
		}
	}
	igraph_matrix_destroy(&layout_matrix);

	//===========save------------
	string FileName = Collected_FilePrix;
	FileName += "_Temp.paj";
	PAJEK::Save_Pajek_by_Vertex_and_Edges_Info(FileName.c_str(), m_vertex_l, m_edge_l, m_partition_l);

	AppCall::Consensus_Open_Process(FileName.c_str(), Pajet_Path);

	Release_Vertex_and_Edge();
}


void CEGen::Cellected_Entity_Short_Path(CENET_Dlg* p_m_CENET_Dlg)
{
	Reload_Vertex_and_Edge();
	Filtering_Named_Entity_and_Relation_Info(p_m_CENET_Dlg);
	Rearrange_Vertex_and_Edge();

	CString CboBoxText;
	p_m_CENET_Dlg->m_SPArg1boBox.GetLBText(p_m_CENET_Dlg->m_SPArg1boBox.GetCurSel(),CboBoxText);
	string arg1name = ENCOM::EncodeHtml(SCONVERT::StringToWString(NLPOP::CString2string(CboBoxText)));

	p_m_CENET_Dlg->m_SPArg2boBox.GetLBText(p_m_CENET_Dlg->m_SPArg2boBox.GetCurSel(),CboBoxText);
	string arg2name = ENCOM::EncodeHtml(SCONVERT::StringToWString(NLPOP::CString2string(CboBoxText)));

	map<string, size_t> name2id_map;
	for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); lite++){
		pajekvertex& loc_vertex = **lite;
		name2id_map.insert(make_pair(loc_vertex.name, loc_vertex.vex));
	}

	if(name2id_map.find(arg1name) == name2id_map.end() || name2id_map.find(arg2name) == name2id_map.end()){
		AppCall::Secretary_Message_Box("The cential vertex is not finded: CEGen::Cellected_Entity_Star_Layout()");
		return;
	}
	//============================================igraph processing
	igraph_t event_network;
	size_t limitID = m_vertex_l.size();
	limitID++;
	igraph_empty(&event_network, m_vertex_l.size(), 0);
	for(list<pajekedge*>::iterator lite = m_edge_l.begin(); lite != m_edge_l.end(); lite++){
		pajekedge& loc_edge = **lite;
		igraph_add_edge(&event_network, loc_edge.vex1-1, loc_edge.vex2-1);
	}
	igraph_vector_t vertices;
	igraph_vector_t edges;

	igraph_vector_init(&vertices, 0);
	igraph_vector_init(&edges, 0);
	int ig_return = igraph_get_shortest_path(&event_network, &vertices, &edges, name2id_map[arg1name], name2id_map[arg2name], IGRAPH_ALL);
	igraph_destroy(&event_network);

	if(0 == igraph_vector_size(&vertices)){
		AppCall::Secretary_Message_Box("The named entities are not connected...");
		return;
	}
	set<size_t> ReminedVex_s;
	for(long int i = 0; i < igraph_vector_size(&vertices); i++){
		ReminedVex_s.insert((size_t)(VECTOR(vertices)[i]));
	}
	igraph_vector_destroy(&vertices);
	igraph_vector_destroy(&edges);
	list<pair<string, size_t>>::iterator parite = m_partition_l.begin();
	for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); ){
		pajekvertex& loc_case = **lite;
		if(ReminedVex_s.find(loc_case.vex) == ReminedVex_s.end()){
			delete *lite;
			lite = m_vertex_l.erase(lite);
			parite = m_partition_l.erase(parite);
		}
		else{
			lite++;
			parite++;
		}
	}
	for(list<pajekedge*>::iterator lite = m_edge_l.begin(); lite != m_edge_l.end(); ){
		pajekedge& loc_case = **lite;
		if(ReminedVex_s.find(loc_case.vex1) == ReminedVex_s.end() || ReminedVex_s.find(loc_case.vex2) == ReminedVex_s.end()){
			delete *lite;
			lite = m_edge_l.erase(lite);
		}
		else{
			lite++;
		}
	}

	//===========save------------
	string FileName = Collected_FilePrix;
	FileName += "_Temp.paj";
	PAJEK::Save_Pajek_by_Vertex_and_Edges_Info(FileName.c_str(), m_vertex_l, m_edge_l, m_partition_l);
	
	AppCall::Consensus_Open_Process(FileName.c_str(), Pajet_Path);
	
	Release_Vertex_and_Edge();
}



//===================================================

void CEGen::Filtering_Named_Entity_and_Relation_Info(CENET_Dlg* p_m_CENET_Dlg)
{
	set<string>& m_FilterEntity_s = p_m_CENET_Dlg->m_FilterEntity_s;
	set<string>& m_FilterRelation_s = p_m_CENET_Dlg->m_FilterRelation_s;
	size_t m_MaxVexNum = p_m_CENET_Dlg->m_MaxVexNum;
	double m_NamedEntityWeight = p_m_CENET_Dlg->m_NamedEntityWeight;
	double m_RelationWeight = p_m_CENET_Dlg->m_RelationWeight;

	//=======================Filtering Named Entity
	set<size_t> DeletedVetex_s;
	set<string> DetetedName_s;
	for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); ){
		pajekvertex& loc_case = **lite;
		if(m_FilterEntity_s.find(loc_case.TYPE) == m_FilterEntity_s.end()){
			DeletedVetex_s.insert(loc_case.vex);
			DetetedName_s.insert(loc_case.name);
			delete *lite;
			lite = m_vertex_l.erase(lite);
			continue;
		}
		if(ENTITY_WEIGHT(loc_case.weight) < m_NamedEntityWeight){
			DeletedVetex_s.insert(loc_case.vex);
			DetetedName_s.insert(loc_case.name);
			delete *lite;
			lite = m_vertex_l.erase(lite);
			continue;
		}
		lite++;
	}
	if(m_vertex_l.size() > m_MaxVexNum){
		multimap<double, size_t> Value2Num_mm;
		size_t NumberCnt = 0;
		for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); ){
			Value2Num_mm.insert(make_pair((*lite)->weight, NumberCnt++));
		}
		set<size_t> DeletingWeights_s;
		NumberCnt = m_vertex_l.size() - m_MaxVexNum;
		for(multimap<double, size_t>::iterator mmite = Value2Num_mm.begin(); mmite != Value2Num_mm.end(); mmite++){
			DeletingWeights_s.insert(mmite->second);
			if(--NumberCnt == 0){
				break;
			}
		}
		NumberCnt = 0;
		for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); ){
			if(DeletingWeights_s.find(NumberCnt) != DeletingWeights_s.end()){
				DeletedVetex_s.insert((*lite)->vex);
				DetetedName_s.insert((*lite)->name);
				delete *lite;
				lite = m_vertex_l.erase(lite);
			}
			else{
				lite++;
			}
		}
	}

	//=======================Filtering Edges
	set<size_t> RemainedVertex_s;;
	bool Edge_Deleting_Flag;
	for(list<pajekedge*>::iterator lite = m_edge_l.begin(); lite != m_edge_l.end(); ){
		pajekedge& loc_case = **lite;
		Edge_Deleting_Flag = false;
		if(m_FilterRelation_s.find(loc_case.TYPE) == m_FilterRelation_s.end()){
			Edge_Deleting_Flag = true;
		}
		else if(loc_case.value < m_RelationWeight){
			Edge_Deleting_Flag = true;
		}
		else if(DeletedVetex_s.find(loc_case.vex1) != DeletedVetex_s.end()){
			Edge_Deleting_Flag = true;
		}
		else if(DeletedVetex_s.find(loc_case.vex2) != DeletedVetex_s.end()){
			Edge_Deleting_Flag = true;
		}
		if(Edge_Deleting_Flag){
			delete *lite;
			lite = m_edge_l.erase(lite);
		}
		else{
			RemainedVertex_s.insert(loc_case.vex1);
			RemainedVertex_s.insert(loc_case.vex2);
			lite++;
		}
	}

	for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); ){
		pajekvertex& loc_case = **lite;
		if(RemainedVertex_s.find(loc_case.vex) == RemainedVertex_s.end()){
			DetetedName_s.insert(loc_case.name);
			delete *lite;
			lite = m_vertex_l.erase(lite);
		}
		else{
			lite++;
		}
	}

	for(list<pair<string, size_t>>::iterator lite = m_partition_l.begin(); lite != m_partition_l.end(); ){
		if(DetetedName_s.find(lite->first) != DetetedName_s.end()){
			lite = m_partition_l.erase(lite);
		}
		else{
			lite++;
		}
	}
	if(m_vertex_l.size() != m_partition_l.size()){
		AppCall::Secretary_Message_Box("Data Error in: CEGen::Filtering_Named_Entity_and_Relation_Info()");
	}
}


void CEGen::Merging_Named_Entity_and_Relation_Info()
{
	string FileName = Collected_FilePrix;
	FileName += "_entity.paj";
	list<pajekvertex*> layoutedpajekvertex_l;
	PAJEK::Load_Pajek_Vertex_Info(FileName.c_str(), layoutedpajekvertex_l);
	PAJEK::Load_Pajek_Partition_Info(FileName.c_str(), m_partition_l);

	Pajek_Info_Checking(layoutedpajekvertex_l, m_edge_l, m_partition_l);

	FileName = Collected_FilePrix;
	FileName += "_relation.paj";
	PAJEK::Load_Pajek_Vertex_Info(FileName.c_str(), m_vertex_l);
	PAJEK::Load_Pajek_Edges_Info(FileName.c_str(), m_edge_l);

	map<size_t, string> Vertex2Name_m;
	//===========Merging------------
	//-----Collecting Layouted vertex Info
	map<string, pajekvertex*> layoutvertex_map;
	for(list<pajekvertex*>::iterator lite = layoutedpajekvertex_l.begin(); lite != layoutedpajekvertex_l.end(); lite++){
		pajekvertex& loc_vex = **lite;
		if(layoutvertex_map.find(loc_vex.name) != layoutvertex_map.end()){
			AppCall::Secretary_Message_Box("Unique name is expected! in: ENET::Merging_Event_Network_Vertex_and_Edges_Port()");
		}
		Vertex2Name_m.insert(make_pair(loc_vex.vex, loc_vex.name));
		layoutvertex_map.insert(make_pair(loc_vex.name, *lite));
	}
	
	//-----Deleting m_vertex_l without Layouted vertex Info
	map<string, size_t> PartitionTYPE_m;
	for(list<pair<string, size_t>>::iterator lite = m_partition_l.begin(); lite != m_partition_l.end(); lite++){
		PartitionTYPE_m.insert(make_pair(lite->first, lite->second));
	}

	set<size_t> RemainedVertex_s;
	m_partition_l.clear();
	for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); ){
		pajekvertex& loc_vex = **lite;
		if(layoutvertex_map.find(loc_vex.name) == layoutvertex_map.end()){
			delete *lite;
			lite = m_vertex_l.erase(lite);
			continue;
			//AppCall::Secretary_Message_Box("Unmatched Data in: ENET::Merging_Event_Network_Vertex_and_Edges_Port()");
		}
		if(PartitionTYPE_m.find(loc_vex.name) == PartitionTYPE_m.end()){
			AppCall::Secretary_Message_Box("Unmatched Data in: CEGen::Merging_Named_Entity_and_Relation_Info()");
		}
		RemainedVertex_s.insert(loc_vex.vex);
		m_partition_l.push_back(make_pair(loc_vex.name, PartitionTYPE_m[loc_vex.name]));
		loc_vex.x = layoutvertex_map[loc_vex.name]->x;
		loc_vex.y = layoutvertex_map[loc_vex.name]->y;
		loc_vex.weight = layoutvertex_map[loc_vex.name]->weight;
		loc_vex.TYPE = layoutvertex_map[loc_vex.name]->TYPE;
		lite++;
	}
	for(list<pajekvertex*>::iterator lite = layoutedpajekvertex_l.begin(); lite != layoutedpajekvertex_l.end(); lite++){
		delete *lite;
	}
	layoutedpajekvertex_l.clear();

	//-----Deleting edges without vertex
	for(list<pajekedge*>::iterator lite = m_edge_l.begin(); lite != m_edge_l.end(); ){
		pajekedge& loc_edge = **lite;
		if((RemainedVertex_s.find(loc_edge.vex1) == RemainedVertex_s.end()) || (RemainedVertex_s.find(loc_edge.vex2) == RemainedVertex_s.end())){
			delete *lite;
			lite = m_edge_l.erase(lite);
		}
		else{
			lite++;
		}
	}

	Rearrange_Vertex_and_Edge();
	Pajek_Info_Checking(m_vertex_l, m_edge_l, m_partition_l);
	//===========save------------
	FileName = Collected_FilePrix;
	FileName += ".paj";
	PAJEK::Save_Pajek_by_Vertex_and_Edges_Info(FileName.c_str(), m_vertex_l, m_edge_l, m_partition_l);
}

void CEGen::Generating_Pajek_of_Relation_Info(CENET_Dlg* p_m_CENET_Dlg)
{
	string FileName;
	set<string> vertex_s;
	vector<relationinfo*> relation_mention_v;
	vector<string> AppendingInfo_v;

	FileName = Collected_FilePrix;
	FileName += ".RELATION";
	ENCOM::Load_Extracted_Relations_Result(FileName.c_str(), relation_mention_v);

	set<string>& Ref_REL_TYPE_s = p_m_CENET_Dlg->m_REL_TYPE_s;
	size_t Edges_Num = 0;
	for(size_t i = 0; i < relation_mention_v.size(); i++){
		relationinfo& loc_case = *relation_mention_v[i];
		if(Ref_REL_TYPE_s.find(loc_case.TYPE) == Ref_REL_TYPE_s.end()){
			delete relation_mention_v[i];
			relation_mention_v[i] = NULL;
			continue;
		}
		if(!strcmp("记者", loc_case.arg1.c_str()) || !strcmp("记者", loc_case.arg2.c_str())){
			delete relation_mention_v[i];
			relation_mention_v[i] = NULL;
			continue;
		}
		if(!strcmp("新华社", loc_case.arg1.c_str()) || !strcmp("新华社", loc_case.arg2.c_str())){
			delete relation_mention_v[i];
			relation_mention_v[i] = NULL;
			continue;
		}
		if(vertex_s.find(loc_case.arg1) == vertex_s.end()){
			vertex_s.insert(loc_case.arg1);
		}
		if(vertex_s.find(loc_case.arg2) == vertex_s.end()){
			vertex_s.insert(loc_case.arg2);
		}
		AppendingInfo_v.push_back(loc_case.TYPE);
		Edges_Num++;
	}
	if(0 == Edges_Num){
		AppCall::Secretary_Message_Box("No Relation is detected. A empty event network is generated...");
	}

	map<string, size_t> Entity2Index_m;
	size_t EntityID = 0;
	for(set<string>::iterator site = vertex_s.begin(); site != vertex_s.end(); site++){
		Entity2Index_m.insert(make_pair(site->data(), EntityID++));
	}

	FileName = Collected_FilePrix;
	FileName += "_relation.paj";
	//ostringstream osteam;
	//osteam << "Entity: " << Entity2Index_m.size() << "; Relation: " << Edges_Num << ";" << endl;
	//p_m_CENET_Dlg->m_BottonEdit.SetWindowTextW(NLPOP::string2CString(osteam.str().c_str()));

	igraph_t ig_event_network;
	ENCOM::Generating_iGraph_by_Vertex_Set(ig_event_network, Entity2Index_m);

	igraph_vector_t weight;
	igraph_vector_init(&weight, Edges_Num);
	Edges_Num = 0;
	map<size_t, map<size_t, size_t>> Edges_mm;
	for(size_t i = 0; i < relation_mention_v.size(); i++){
		if(!relation_mention_v[i]){
			continue;
		}
		relationinfo& loc_relationinfo = *relation_mention_v[i];
		if(Entity2Index_m.find(loc_relationinfo.arg1) == Entity2Index_m.end() || Entity2Index_m.find(loc_relationinfo.arg2) == Entity2Index_m.end()){
			AppCall::Secretary_Message_Box("Data Error in: CEGen::Generating_Pajek_of_Relation_Info()");
		}
		size_t arg1_ID = Entity2Index_m[loc_relationinfo.arg1];
		size_t arg2_ID = Entity2Index_m[loc_relationinfo.arg2];
		if(Edges_mm.find(arg1_ID) == Edges_mm.end()){
			Edges_mm[arg1_ID];
		}
		if(Edges_mm[arg1_ID].find(arg2_ID) == Edges_mm[arg1_ID].end()){
			Edges_mm[arg1_ID].insert(make_pair(arg2_ID, Edges_Num));
			igraph_add_edge(&ig_event_network, arg1_ID, arg2_ID);
			VECTOR(weight)[Edges_Num++] = relation_mention_v[i]->value;
		}
		else{
			VECTOR(weight)[Edges_mm[arg1_ID][arg2_ID]] = relation_mention_v[i]->value;
		}
		
		delete relation_mention_v[i];
	}
	relation_mention_v.clear();

	PAJEK::Save_Igraph_to_Pajek(FileName.c_str(), ig_event_network, Collected_EventName.c_str());
	igraph_destroy(&ig_event_network);

	//======Appending Edges Weighting
	PAJEK::Appending_Attribute_to_Edges_in_Pajek(FileName.c_str(), weight);
	igraph_vector_destroy(&weight);

	//======Appending Edges Weighting
	PAJEK::Appending_Info_to_Edges_in_Pajek(FileName.c_str(), AppendingInfo_v);
}


void CEGen::Generating_Pajek_of_Named_Entity_Info(CENET_Dlg* p_m_CENET_Dlg)
{
	string FileName = Collected_FilePrix;
	FileName += ".ENTITY";
	vector<entity_mention*> entity_mention_v;
	ENCOM::Load_Extracted_Entities_Result(FileName.c_str(), entity_mention_v);

	set<string>& Ref_NE_TYPE_s = p_m_CENET_Dlg->m_NE_TYPE_s;
	map<string, entity_mention*> entity_mention_m;
	for(size_t i = 0; i < entity_mention_v.size(); i++){
		entity_mention& loc_entity = *entity_mention_v[i];
		if(Ref_NE_TYPE_s.find(loc_entity.TYPE) == Ref_NE_TYPE_s.end()){
			delete entity_mention_v[i];
			continue;
		}
		if(entity_mention_m.find(loc_entity.extent.charseq) == entity_mention_m.end()){
			entity_mention_m.insert(make_pair(loc_entity.extent.charseq, entity_mention_v[i]));
		}
		else if(entity_mention_m[loc_entity.extent.charseq]->TYPE_V < loc_entity.TYPE_V){
			delete entity_mention_m[loc_entity.extent.charseq];
			entity_mention_m[loc_entity.extent.charseq] = entity_mention_v[i];
		}
		else{
			delete entity_mention_v[i];
		}
	}
	entity_mention_v.clear();

	size_t TYPE_ID = 0;
	map<string, size_t> TYPE2ID_m;
	for(set<string>::iterator site = Ref_NE_TYPE_s.begin(); site != Ref_NE_TYPE_s.end(); site++){
		TYPE2ID_m.insert(make_pair(*site, TYPE_ID++));
	}

	map<string, double> EntityWeight_m;
	map<size_t, string> Entity_TYPE_m;
	map<string, size_t> Partition_m;
	TYPE_ID = 0;
	for(map<string, entity_mention*>::iterator mite = entity_mention_m.begin(); mite != entity_mention_m.end(); ){
		EntityWeight_m.insert(make_pair(mite->first, mite->second->TYPE_V));
		Entity_TYPE_m.insert(make_pair(TYPE_ID++, mite->second->TYPE));
		Partition_m.insert(make_pair(mite->first, TYPE2ID_m[mite->second->TYPE]));
		delete mite->second;
		mite = entity_mention_m.erase(mite);
	}

	
	if(EntityWeight_m.empty()){
		AppCall::Secretary_Message_Box("No Entity is detected. A empty event network is generated...");
	}
	ostringstream osteam;
	osteam << "Number of Entity: " << EntityWeight_m.size() << ";" << endl;
	p_m_CENET_Dlg->m_BottonEdit.SetWindowTextW(NLPOP::string2CString(osteam.str().c_str()));

	//====save igraph pajek;
	FileName = Collected_FilePrix;
	FileName += "_entity.paj";
	map<string, string> HtmlDecode_m;
	igraph_t ig_event_network;
	ENCOM::Generating_iGraph_by_Vertex_Set(ig_event_network, EntityWeight_m, HtmlDecode_m);
	PAJEK::Save_Igraph_to_Pajek(FileName.c_str(), ig_event_network, Collected_EventName.c_str());
	igraph_destroy(&ig_event_network);

	FileName = Collected_FilePrix;
	FileName += "_entity.HtmlCode";
	NLPOP::Save_String_to_String_Mapping(FileName.c_str(), HtmlDecode_m);

	//====Appending layout to Pajek
	FileName = Collected_FilePrix;
	FileName += "_entity.paj";
	igraph_matrix_t layout_matrix;
	igraph_matrix_init(&layout_matrix, EntityWeight_m.size(), 2);
	igraph_layout_random(&ig_event_network, &layout_matrix);
	PAJEK::Appending_Layout_to_Pajek(FileName.c_str(), layout_matrix);
	igraph_matrix_destroy(&layout_matrix);

	//====Appending vertex weight to Pajek
	igraph_vector_t i_weight;
	igraph_vector_init(&i_weight, EntityWeight_m.size());
	size_t ID = 0;
	for(map<string, double>::iterator mite = EntityWeight_m.begin(); mite != EntityWeight_m.end(); mite++){
		VECTOR(i_weight)[ID++] = VERTEX_RESIZE(mite->second);
	}
	PAJEK::Appending_Vertex_Weighting_to_Pajek(FileName.c_str(), i_weight);
	igraph_vector_destroy(&i_weight);

	//====Appending vertex TYPE to Pajek
	PAJEK::Appending_Vertex_Info_to_Pajek(FileName.c_str(), Entity_TYPE_m);

	//====Appending vertex Partition to Pajek
	PAJEK::Appending_Vertex_Partition_to_Pajek(FileName.c_str(), Partition_m);

	//---------Data Checking
	FileName = Collected_FilePrix;
	FileName += "_entity.paj";
	PAJEK::Load_Pajek_Vertex_Info(FileName.c_str(), m_vertex_l);
	PAJEK::Load_Pajek_Partition_Info(FileName.c_str(), m_partition_l);
	Pajek_Info_Checking(m_vertex_l, m_edge_l, m_partition_l);
	Release_Vertex_and_Edge();

	//====viewing vertex in Pajek
	//AppCall::Consensus_Open_Process(FileName.c_str(), Pajet_Path);
}



//===================================================
void CEGen::Reload_Vertex_and_Edge()
{
	Release_Vertex_and_Edge();
	string FileName;
	FileName = Collected_FilePrix;
	FileName += ".paj";
	if(!NLPOP::Exist_of_This_File(FileName.c_str())){
		Merging_Named_Entity_and_Relation_Info();
	}
	else{
		PAJEK::Load_Pajek_Vertex_Info(FileName.c_str(), m_vertex_l);
		PAJEK::Load_Pajek_Partition_Info(FileName.c_str(), m_partition_l);
		PAJEK::Load_Pajek_Edges_Info(FileName.c_str(), m_edge_l);
	}

	//-------------Checking
	Pajek_Info_Checking(m_vertex_l, m_edge_l, m_partition_l);
}
void CEGen::Pajek_Info_Checking(list<pajekvertex*>& vertex_l, list<pajekedge*>& edge_l, list<pair<string, size_t>>& partition_l)
{
	if(vertex_l.size() != partition_l.size()){
		AppCall::Secretary_Message_Box("Unmatched Data in: Reload_Vertex_and_Edge() 1");
	}
	size_t MaxID = partition_l.size();
	for(list<pajekedge*>::iterator lite = edge_l.begin(); lite != edge_l.end(); lite++){
		pajekedge& loc_case = **lite;
		if(loc_case.vex1 > MaxID || loc_case.vex2 > MaxID){
			AppCall::Secretary_Message_Box("Unmatched Data in: Reload_Vertex_and_Edge() 2");
		}
	}
	list<pajekvertex*>::iterator vexite = vertex_l.begin();
	list<pair<string, size_t>>::iterator parite = partition_l.begin();
	map<string, size_t> TYPE_Par_m;
	while(vexite != vertex_l.end()){
		if(TYPE_Par_m.find((**vexite).TYPE) == TYPE_Par_m.end()){
			TYPE_Par_m.insert(make_pair((**vexite).TYPE, parite->second));
		}
		else if(TYPE_Par_m[(**vexite).TYPE] != parite->second){
			AppCall::Secretary_Message_Box("Unmatched Data in: Reload_Vertex_and_Edge() 3");
		}
		vexite++;
		parite++;
	}
}
void CEGen::Release_Vertex_and_Edge()
{
	if(!m_vertex_l.empty()){
		for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); lite++){
			delete *lite;
		}
		m_vertex_l.clear();
	}
	if(!m_edge_l.empty()){
		for(list<pajekedge*>::iterator lite = m_edge_l.begin(); lite != m_edge_l.end(); lite++){
			delete *lite;
		}
		m_edge_l.clear();
	}
	if(!m_partition_l.empty()){
		m_partition_l.clear();
	}
}
void CEGen::Rearrange_Vertex_and_Edge()
{
	set<size_t> RemainedVertex_s;
	for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); lite++){
		RemainedVertex_s.insert((**lite).vex);
	}
	//--------------------------------
	map<size_t, size_t> ReOrdered_map;
	size_t vertex_id = 1;
	for(set<size_t>::iterator site = RemainedVertex_s.begin(); site != RemainedVertex_s.end(); site++){
		ReOrdered_map.insert(make_pair(*site, vertex_id++));
	}
	for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); lite++){
		pajekvertex& loc_vex = **lite;
		loc_vex.vex = ReOrdered_map[loc_vex.vex];
	}
	for(list<pajekedge*>::iterator lite = m_edge_l.begin(); lite != m_edge_l.end(); lite++){
		pajekedge& loc_edge = **lite;
		loc_edge.vex1 = ReOrdered_map[loc_edge.vex1];
		loc_edge.vex2 = ReOrdered_map[loc_edge.vex2];
	}
}


void CEGen::Enable_Cential_Entity_and_Short_Path_Box(CENET_Dlg* p_m_CENET_Dlg)
{
	set<string> CentEntity_s;
	set<string> Arg1_s;
	set<string> Arg2_s;

	map<size_t, string> merged_vertex_map;
	for(list<pajekvertex*>::iterator lite = m_vertex_l.begin(); lite != m_vertex_l.end(); lite++){
		merged_vertex_map.insert(make_pair((*lite)->vex, (*lite)->name));
	}
	for(list<pajekedge*>::iterator lite = m_edge_l.begin(); lite != m_edge_l.end(); lite++){
		pajekedge& loc_edge = **lite;
		if(merged_vertex_map.find(loc_edge.vex1) == merged_vertex_map.end() || merged_vertex_map.find(loc_edge.vex2) == merged_vertex_map.end()){
			AppCall::Secretary_Message_Box("Unmatched Data in: ENET::Merging_Event_Network_Vertex_and_Edges_Port()");
		}
		CentEntity_s.insert(merged_vertex_map[loc_edge.vex1]);
		CentEntity_s.insert(merged_vertex_map[loc_edge.vex2]);
		Arg1_s.insert(merged_vertex_map[loc_edge.vex1]);
		Arg2_s.insert(merged_vertex_map[loc_edge.vex2]);
	}

	map<string, string> HtmlDecode_m;
	string FileName = Collected_FilePrix;
	FileName += "_entity.HtmlCode";
	NLPOP::Load_String_to_String_Mapping(FileName.c_str(), HtmlDecode_m);

	p_m_CENET_Dlg->m_CentboBox.ResetContent();
	p_m_CENET_Dlg->m_CentboBox.AddString(_T("Null"));
	p_m_CENET_Dlg->m_CentboBox.SetCurSel(0);
	for(set<string>::iterator site = CentEntity_s.begin(); site != CentEntity_s.end(); site++){
		if(HtmlDecode_m.find(*site) == HtmlDecode_m.end()){
			AppCall::Secretary_Message_Box("Unmatched Data in HtmlDecode_m: ENET::Merging_Event_Network_Vertex_and_Edges_Port()");
			return;
		}
		p_m_CENET_Dlg->m_CentboBox.AddString(NLPOP::string2CString(HtmlDecode_m[*site]));
	}
	p_m_CENET_Dlg->m_SPArg1boBox.ResetContent();
	p_m_CENET_Dlg->m_SPArg1boBox.AddString(_T("Null"));
	p_m_CENET_Dlg->m_SPArg1boBox.SetCurSel(0);
	for(set<string>::iterator site = Arg1_s.begin(); site != Arg1_s.end(); site++){
		if(HtmlDecode_m.find(*site) == HtmlDecode_m.end()){
			AppCall::Secretary_Message_Box("Unmatched Data in: ENET::Merging_Event_Network_Vertex_and_Edges_Port()");
			return;
		}
		p_m_CENET_Dlg->m_SPArg1boBox.AddString(NLPOP::string2CString(HtmlDecode_m[*site]));
	}
	p_m_CENET_Dlg->m_SPArg2boBox.ResetContent();
	p_m_CENET_Dlg->m_SPArg2boBox.AddString(_T("Null"));
	p_m_CENET_Dlg->m_SPArg2boBox.SetCurSel(0);
	for(set<string>::iterator site = Arg2_s.begin(); site != Arg2_s.end(); site++){
		if(HtmlDecode_m.find(*site) == HtmlDecode_m.end()){
			AppCall::Secretary_Message_Box("Unmatched Data in: ENET::Merging_Event_Network_Vertex_and_Edges_Port()");
			return;
		}
		p_m_CENET_Dlg->m_SPArg2boBox.AddString(NLPOP::string2CString(HtmlDecode_m[*site]));
	}

}

