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

class CDiscrimer
{
public:
	CCEDT* pCCEDT;


public:
	CDiscrimer();
	~CDiscrimer();

public:
//============================================
	void Discrimer_Training_and_Testing_Port(vector<NE_Surround*>& training_Surround_v, vector<NE_Surround*>& testing_Surround_v);
	void Discriminater_Recognization_Port(const char* inputchar, MaxentModel& pm_maxent, CEDTInfo& pmCEDTInfo);

//============================================
	void Training_Named_Entity_Discriminater(vector<NE_Surround*>& training_Surround_v, MaxentModel& pm_maxent);
	void Testing_Named_Entity_Discriminater(vector<NE_Surround*>& testing_Surround_v, MaxentModel& pm_maxent);

//============================================
	void Generate_Discrimer_Training_Case(vector<NE_Surround*>& Surround_v, CEDTInfo& pmCEDTInfo);
	void Combined_Performance_and_Generate_ACE_Testing_for_Discrimer(CEDTInfo& pmCEDTInfo);
	void Combined_Matched_Head_and_Extend_for_Discrimer(CEDTInfo& pmCEDTInfo);
	void Generate_Discrimer_for_Extend_or_Head_Only_Recognition(CEDTInfo& pmCEDTInfo);
//============================================
	void Generate_Named_Entity_Discrimer_Cases_Feature(vector<DismCase*>& DismCase_v, FeatureVctor& Training_v);
	
	
};
