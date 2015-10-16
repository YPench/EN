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
#include "maxen.h"

class CCEDT;

class CExtra
{
public:
	CCEDT* pCCEDT;


public:
	CExtra();
	~CExtra();



	void ACE_Extra_Information_Extraction_Port(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);
	void Load_Entity_TYPE_Info(vector<DismCase*>& pmDismCase_v);

	void Entity_CLASS_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);
	void Mention_TYPE_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);

	void Entity_SUBTYPE_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);
	void Each_Entity_SUBTYPE_Recognition(vector<DismCase*>& TrainingDism_v, vector<DismCase*>& TestingDism_v);

};
