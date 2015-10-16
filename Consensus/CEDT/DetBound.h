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
class CEDTInfo;


class CDetBound
{
public:
	CCEDT* pCCEDT;
	
public:
	CDetBound();
	~CDetBound();

public:
	//=====================Boundary Port
	void START_or_END_Boundary_Detection_Port(bool START_Flag, const char* inputstr, MaxentModel& pm_maxent, map<size_t, map<size_t, double>*>& boundout_m, size_t SENID);
	void START_Boundary_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	void END_Boundary_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	
	//========================================
	void Training_START_or_END_Boundary(bool START_Flag, vector<NE_Surround*>& training_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTrainingCEDTInfo);
	void Testing_START_or_END_Boundary(bool START_Flag, vector<NE_Surround*>& testing_Surround_v, MaxentModel& pm_maxent, CEDTInfo& pmTestingCEDTInfo);
	void Generate_START_or_END_Cases_Feature(bool START_Flag, vector<BoundCase*>& START_BoundCase_v, FeatureVctor& Training_v);

	//========================================
	void Gen_END_Boundary_Cases(vector<NE_Surround*>& Surround_v, vector<BoundCase*>& END_BoundCase_v);
	void Gen_START_Boundary_Cases(vector<NE_Surround*>& Surround_v, vector<BoundCase*>& START_BoundCase_v);
	void Gen_START_Boundary_Cases(const char* inputchar, vector<BoundCase*>& START_BoundCase_v, size_t SENID);
	void Gen_END_Boundary_Cases(const char* inputchar, vector<BoundCase*>& END_BoundCase_v, size_t SENID);
};
