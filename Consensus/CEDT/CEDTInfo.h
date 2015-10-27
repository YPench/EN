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
	CEDTInfo();
	~CEDTInfo();
	void FREE();

	void Save_Boundary_Info();
	bool Load_Boundary_Info(ACE_Corpus& pmACE_Corpus);

	void Save_Boundary_Rtn_Information(const char* pmFilePath, map<size_t, map<size_t, double>*>& pmRtn_mm);
	void Load_Boundary_Rtn_Information(const char* pmFilePath, map<size_t, map<size_t, double>*>& pmRtn_mm);

	void Save_Named_Entity_Candit_DismCase(const char* savename, vector<CanditCase*>& CanditCase_v);
	void Load_Named_Entity_Candit_DismCase(const char* openname, vector<CanditCase*>& CanditCase_v, map<string, ACE_entity>& ACE_Entity_Info_map);
};