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
#include "..\\Include\\ace.h"

class CEDTInfo{
public:
	map<size_t, map<size_t, double>*> Extend_START_mm;
	map<size_t, map<size_t, double>*> Extend_END_mm;
	map<size_t, map<size_t, double>*> Head_START_mm;
	map<size_t, map<size_t, double>*> Head_END_mm;
	vector<CanditCase*> CanditHead_v;
	vector<CanditCase*> CanditExtend_v;
	vector<DismCase*> DismCase_v;
	CEDTInfo(){};
	~CEDTInfo(){};
	void FREE(){
		for(map<size_t, map<size_t, double>*>::iterator mmite = Extend_START_mm.begin(); mmite != Extend_START_mm.end(); mmite++){delete mmite->second;}Extend_START_mm.clear();
		for(map<size_t, map<size_t, double>*>::iterator mmite = Extend_END_mm.begin(); mmite != Extend_END_mm.end(); mmite++){delete mmite->second;}Extend_END_mm.clear();
		for(map<size_t, map<size_t, double>*>::iterator mmite = Head_START_mm.begin(); mmite != Head_START_mm.end(); mmite++){delete mmite->second;}Head_START_mm.clear();
		for(map<size_t, map<size_t, double>*>::iterator mmite = Head_END_mm.begin(); mmite != Head_END_mm.end(); mmite++){delete mmite->second;}Head_END_mm.clear();
		for(size_t i = 0; i < CanditHead_v.size(); i++){if(CanditHead_v[i] != NULL){delete CanditHead_v[i];}}CanditHead_v.clear();
		for(size_t i = 0; i < CanditExtend_v.size(); i++){if(CanditExtend_v[i] != NULL){delete CanditExtend_v[i];}}CanditExtend_v.clear();
		for(size_t i = 0; i < DismCase_v.size(); i++){if(DismCase_v[i] != NULL){delete DismCase_v[i];}}DismCase_v.clear();
	};
};

namespace NECOM{
	void Segment_Sentence_for_Named_Entity(const char* senchar, set<string>& inSegChar_s, vector<string>& outstr_v, vector<bool>& outrtn_v);
	void Generated_Candidate_Testing_Cases_Information(map<string, size_t>& pmPositive_Cases, vector<CanditCase*>& pmCandit_v, Maxen_Rtn_map& ProcesedCasesInfor_m);
	void Load_of_NE_Surround_Named_Entity(const char* Loadpath, map<string, ACE_entity_mention*>& EntityMentionInfo_map, vector<NE_Surround*>& NE_Surround_v);
	void Extracting_Named_Entity_Surrounding(map<string, ACE_sgm>& ACE_sgm_mmap, map<string, ACE_entity>& ACE_Entity_Info_map, vector<NE_Surround*>& NE_Surround_v, size_t& Sentence_ID);
	void Positvie_Mention_Collection(vector<NE_Surround*>& NE_Surround_v, map<string, size_t>& pmPositive_Mentions, bool Combining_Flag);
	void NE_Surround_Relative_Position_Calculate(vector<NE_Surround*>& NE_Surround_v);
};

