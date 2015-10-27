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


namespace NECOM{

	void Generate_Word_Boundary_Info(const char* inputchar, vector<string>& SegRtn_v, vector<string>& outpos);

	void Generated_Candidate_Testing_Cases_Information(map<string, size_t>& pmPositive_Cases, vector<CanditCase*>& pmCandit_v, Maxen_Rtn_map& ProcesedCasesInfor_m);

	int Get_Left_Outer_Feature_Range_For_START(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit);
	int Get_Right_Outer_Feature_Range_For_END(set<size_t>& START_s, set<size_t>& END_s, int Curposit, int limit);

	void Corpus_Entity_and_Entity_Mention_Information(map<string, ACE_entity>& ACE_Entity_Info_map);
	void Init_START_or_END_Rtn_map_with_Erasing(vector<BoundCase*>& BoundCase_v, map<size_t, map<size_t, double>*>& rtn_mm);
	void Init_START_or_END_Rtn_map_with_NE_Surround(vector<NE_Surround*>& Surround_v, map<size_t, map<size_t, double>*>& rtn_mm);
	
	void Positvie_Mention_Collection(vector<NE_Surround*>& NE_Surround_v, map<string, size_t>& pmPositive_Mentions, bool Combining_Flag);
	void Building_Head_to_Extend_Lookup_Table_map(vector<NE_Surround*>& NE_Surround_v, map<size_t, map<size_t, map<size_t, ACE_extent*>>*>& HeadExtend_mm);
	
	void Extracting_Named_Entity_Surrounding(map<string, ACE_sgm>& ACE_sgm_mmap, map<string, ACE_entity>& ACE_Entity_Info_map, vector<NE_Surround*>& NE_Surround_v, size_t& Sentence_ID);
	void NE_Surround_Relative_Position_Calculate(vector<NE_Surround*>& NE_Surround_v);
	void NE_Surround_Consistence_Check(vector<NE_Surround*>& NE_Surround_v, ACE_Corpus& pmACE_Corpus, bool DOC_Flag);
	
	string Pretreatment_Sent_For_Named_Entity(const char* pmcharstr, set<string>& Number_Set, set<string>& Eng_set);
	void Pretreatment_Sent_For_Named_Entity(string& pmstr, set<string>& Number_Set, set<string>& Eng_set);

	void Save_of_NE_Surround_Named_Entity(const char* savepath, vector<NE_Surround*>& NE_Surround_v);
	void Load_of_NE_Surround_Named_Entity(const char* Loadpath, map<string, ACE_entity_mention*>& EntityMentionInfo_map, vector<NE_Surround*>& NE_Surround_v);
	
	void Save_START_and_END_rtn_Info(const char* savepath, vector<NE_Surround*>& NE_Surround_v);
	bool Load_START_and_END_rtn_Info(const char* loadpath, vector<NE_Surround*>& NE_Surround_v);

	void Segment_Sentence_for_Named_Entity(const char* senchar, set<string>& inSegChar_s, vector<string>& outstr_v, vector<bool>& outrtn_v);

	void Extract_Location_Name(const char* cFolderPath);
};

