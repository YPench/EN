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