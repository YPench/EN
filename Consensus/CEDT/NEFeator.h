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

#include "CGCom.h"
#include "ace.h"
#include "CMaxmatch.h"
#include "CSegmter.h"

class CNEFeator
{
private:
	

	set<string> lexicon_set;
	set<string> Number_Set;
	set<string> English_Set;
	int maxtoken_size;
	
	//CRegex m_CRegex;

	bool Catched_Sent_Segment_Info_Flag;
	int Catched_Sent_ID;
	vector<string> Segment_Info_v;

public:

	set<string> Surname_s;
	set<string> Locname_s;
	set<string> Pronoun_s;

	CSegmter m_CSegmter;
	CMaxmatch m_CMaxmatch;
	

	CNEFeator();
	~CNEFeator();

	void Combined_Feature_In_Candit_Two_Boundary(const CanditCase& pmCandit, vector<string>& pmFeatureVector, string attach);
	void Extract_Local_Feature_Vector_of_Boundary_with_nGram(const BoundCase& pmBoundary, vector<string>& pmFeatureVector, bool START_Flag, string attach);

	bool Init_Named_Entity_Featrue_Extractor(const char* pmDataFolder);
	void Adding_Entity_Extent_and_Head_to_Lexicon(vector<NE_Surround*>& Surround_v, char TYPE);

	//-------Feature Extraction Port
	void Extract_Feature_Vector_of_Boundary(const BoundCase& pmBoundary, vector<string>& pmFeatureVector, bool START_Flag, string attach);
	void Extract_Feature_Vector_of_Candit(const CanditCase& pmCandit, vector<string>& pmFeatureVector, string attach);
	void Extract_Feature_Vector_of_Deterimer(const DismCase& pmDismCase, vector<string>& pmFeatureVector);

	void Extract_Baseline_Feature_Vector_of_Boundary(const BoundCase& pmBoundary, vector<string>& pmFeatureVector, bool START_Flag, string attach);
	void Local_Segmentation_Info_on_Boundary(size_t SentID, string originstr, size_t position, bool START_Flag, vector<string>& outstr_v, string attach);
	void Push_Back_Extracted_Feature_Vector_of_Deterimer(string TYPE, DismCase& pmDismCase, FeatureVctor& FeatureCase_v);

	void Internal_n_Gram(const char* strchar, int Distance, vector<string>& outstr_v, string attach);
private:
	//----Feature Extraction Function--------------
	void Adjacent_Segmentation_Info_on_Boundary(size_t SentID, string originstr, size_t position, bool START_Flag, vector<string>& outstr_v, string attach);

	void Internal_Candit_Words_Feature(const char* strchar, vector<string>& pmFeatureVector, bool Order_Flag, string attach);

	void Adjacent_Words_POS_Feature_Extracting_with_Segmeter(const char* sentchar, int Distance, vector<string>& pmFeatureVector, string prix, string prox);
	void Adjacent_Border_Words_with_Segmeter(const char* strchar, size_t Distance, vector<string>& pmFeatureVector, string prix, string prox);

	void Adjacent_Left_Border_n_Gram(const char* strchar, int Distance, vector<string>& outstr_v, string attach);
	void Adjacent_Right_Border_n_Gram(const char* strchar, int Distance, vector<string>& outstr_v, string attach);

	void Adjacent_Left_Border_Words(const char* strchar, bool OuterFlag, size_t Distance, vector<string>& outstr_v);
	void Adjacent_Right_Border_Words(const char* strchar, bool OuterFlag, size_t Distance, vector<string>& outstr_v);

	//----Support Function----------
	void Extract_Regex_Feature_of_Named_Entity(CanditCase& pmCandit, vector<string>& pmFeatureVector);

	void Reset_Maximum_Matcher_Lexicon(set<string>& pmOuterLexicion_s);
	void Reset_Maximum_Matcher_Lexicon(const char* lexiconpath);

	void CEDT_ACE_Data_Preprocessing(CanditCase& pmCanditCase);
	void CEDT_ACE_Data_Preprocessing(DismCase& pmDismCase);
	void CEDT_ACE_Data_Preprocessing(BoundCase& pmBoundary);

	//**********************************************************
	void Adjacent_Right_Border_Characters(const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v);
	void Adjacent_Left_Border_Characters(const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v);

	void Adjacent_Left_Maxmatch_Overlapping_Words(set<string>& pmLexicon_s, const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v);
	void Adjacent_Right_Maxmatch_Overlapping_Words(set<string>& pmLexicon_s, const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v);

	void Adjacent_Left_Second_Maxmatch_Overlapping_Words(bool Combined_Flag, set<string>& pmLexicon_s, const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v);
	void Adjacent_Right_Second_Maxmatch_Overlapping_Words(bool Combined_Flag, set<string>& pmLexicon_s, const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v);

	void Left_Max_Match_Segmentor(const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v);
	void Right_Max_Match_Segmentor(const char* strchar, bool OuterFlag, int Distance, vector<string>& outstr_v);


};

