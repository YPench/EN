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
#include "StdAfx.h"
#include "maxen.h"
#include "CSegmter.h"
#include <windows.h>


void MAXEN::Output_Extract_Feature_Context_For_Maxent(const char* FilePath, DulTYPE_Vctor& RelationWrods_v)
{
	ofstream out(FilePath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	DulTYPE_Vctor::iterator mmite = RelationWrods_v.begin();
	for(; mmite != RelationWrods_v.end(); mmite++){
		for(vector<pair<string, float>>::iterator  vite = (*mmite)->second.begin(); vite != (*mmite)->second.end(); vite++){
			out << vite->first << ';' << vite->second << ';';
		}
		out << endl << (*mmite)->first.first << ";" << (*mmite)->first.second << endl;
	}
	out.close();
}

void MAXEN::Output_Extract_Feature_Context_For_Maxent(const char* FilePath, FeatureVctor& RelationWrods_v)
{
	ofstream out(FilePath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	FeatureVctor::iterator mmite = RelationWrods_v.begin();
	for(; mmite != RelationWrods_v.end(); mmite++){
		for(vector<pair<string, float>>::iterator  vite = (*mmite)->second.begin(); vite != (*mmite)->second.end(); vite++){
			out << vite->first << ';' << vite->second << ';';
		}
		out << endl << (*mmite)->first << endl;
	}
	out.close();
}

void MAXEN::Maxen_Recognizing_with_eval_Erasing(MaxentModel& pmMaxen, FeatureVctor& Recongizing_v, vector<pair<string,double>>& rtnValue_v)
{
	FeatureVctor::iterator it;

	for (it = Recongizing_v.begin(); it != Recongizing_v.end(); ++it) {
		me_context_type& context_ref = (*it)->second;
		//===eval
		string outtype = pmMaxen.predict(context_ref);
		double value = pmMaxen.eval(context_ref, outtype);
		rtnValue_v.push_back(make_pair(outtype, value));
		delete *it;
    }

	Recongizing_v.clear();
}


void MAXEN::Discrimer_Maxen_Recognization_with_Erasing(vector<DismCase*>& pmDismCase_v, FeatureVctor& Testing_v, MaxentModel& pmMaxen)
{
	if(pmDismCase_v.size() != Testing_v.size()){
		AppCall::Secretary_Message_Box("Unequal data size in:  MAXEN::Discrimer_Maxen_Testing_with_Erasing()", MB_OK);
	}
	FeatureVctor::iterator pit;
	vector<DismCase*>::iterator dism_vite = pmDismCase_v.begin();
	for (pit = Testing_v.begin(); pit != Testing_v.end(); pit++, dism_vite++){
		me_outcome_type predict_out;
		me_context_type& context_ref = (*pit)->second;
		//predict
		(*dism_vite)->predit_TYPE = pmMaxen.predict(context_ref);
		(*dism_vite)->TYPE_value = pmMaxen.eval(context_ref, (*dism_vite)->predit_TYPE);
		delete *pit;
    }
	Testing_v.clear();
}

void MAXEN::Discrimer_Maxen_Testing_with_Erasing(vector<DismCase*>& pmDismCase_v, FeatureVctor& Testing_v, MaxentModel& pmMaxen)
{
	if(pmDismCase_v.size() != Testing_v.size()){
		AppCall::Secretary_Message_Box("Unequal data size in:  MAXEN::Discrimer_Maxen_Testing_with_Erasing()", MB_OK);
	}
	
	FeatureVctor::iterator it;
	vector<DismCase*>::iterator vite = pmDismCase_v.begin();
	for (it = Testing_v.begin(); it != Testing_v.end(); it++, vite++){
		DismCase& locDismCase = **vite;
		me_context_type& context_ref = (*it)->second;
		//predict
		locDismCase.predit_TYPE = pmMaxen.predict(context_ref);
		locDismCase.TYPE_value = pmMaxen.eval(context_ref, locDismCase.predit_TYPE);
		delete *it;
    }
	Testing_v.clear();
}


//-----------------For CEDT
void MAXEN::Training_Cases_Check(FeatureVctor& Training_v)
{
	set<string> classNum_s;
	bool Empey_Feature_Flag = false;
	for(vector<pair<string, vector<pair<string, float>>>*>::iterator vite = Training_v.begin(); vite != Training_v.end(); vite++){
		classNum_s.insert((*vite)->first);
		if((*vite)->second.empty()){
			Empey_Feature_Flag = true;
		}
	}
	if((classNum_s.size() < 2) || (Empey_Feature_Flag)){
		ostringstream outstream;
		outstream << "Improper Training Data: MAXEN::Training_Cases_Check()" << endl;
		if(classNum_s.size() < 2){
			outstream << "Class Number should > 2; Now is " << classNum_s.size() << ";" << endl;
		}
		if(Empey_Feature_Flag){
			outstream << "Empty case feature is detected..." << endl;
		}
		AppCall::Secretary_Message_Box(outstream.str(), MB_OK);
	}
}
void MAXEN::Boundary_Training_without_Erasing(FeatureVctor& Training_v, MaxentModel& pmMaxen, size_t ite)
{
	Training_Cases_Check(Training_v);
	FeatureVctor::iterator vvite = Training_v.begin();
    vector<pair<me_context_type, me_outcome_type> >::iterator it;

	pmMaxen.begin_add_event();
	for( ; vvite != Training_v.end(); vvite++){
		me_context_type& context_ref = (*vvite)->second;
		pmMaxen.add_event(context_ref, (*vvite)->first);
	}
	pmMaxen.end_add_event();
	pmMaxen.train(ite, "lbfgs", 0);
}

void MAXEN::Push_Back_FeatureCase_v_from_Feature_v_with_Check(FeatureVctor& Training_v, vector<string>& pmfeature_v, string label)
{
	map<string, float> WordsCnt_map;

	for(vector<string>::iterator vite = pmfeature_v.begin(); vite != pmfeature_v.end(); vite++){
		if(WordsCnt_map.find(*vite) == WordsCnt_map.end()){
			WordsCnt_map.insert(make_pair(vite->data(), (float)1.0));
		}
		else{
			ostringstream ostream;
			ostream << "Duplicate feature..." << endl;
			ostream << *vite;
			AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
		}
	}

	if(WordsCnt_map.empty()){
		AppCall::Secretary_Message_Box("Case Feature is empty in Push_Back_FeatureCase_v_from_Feature_v_with_Check...", MB_OK);
	}
	pair<string, vector<pair<string, float>>>* ploc_pair = new pair<string, vector<pair<string, float>>>;
	ploc_pair->first = label;
		
	for(map<string, float>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
		ploc_pair->second.push_back(make_pair(mite->first, mite->second));
	}
	Training_v.push_back(ploc_pair);

}


void MAXEN::Boundary_Detection_with_eval_Erasing(FeatureVctor& Training_v, MaxentModel& pmMaxen, map<size_t, map<size_t, double>*>& rtn_mm)
{
	FeatureVctor::iterator it;
	map<size_t, map<size_t, double>*>::iterator mmite = rtn_mm.begin();
	if(mmite == rtn_mm.end()){
		return;
	}
	map<size_t, double>::iterator mite = mmite->second->begin();

	for (it = Training_v.begin(); it != Training_v.end(); ++it) {
		me_context_type& context_ref = (*it)->second;
		//===eval
		while(mite == mmite->second->end()){
			mmite++;
			if(mmite == rtn_mm.end()){
				AppCall::Secretary_Message_Box("Rtn Info is wrong, in MAXEN::Boundary_Testing_with_eval()", MB_OK);
			}
			mite = mmite->second->begin();
		}
		mite->second = pmMaxen.eval(context_ref, POSITIVE);
		mite++;
		delete *it;
    }
	Training_v.clear();
	if(mite != mmite->second->end()){
		AppCall::Secretary_Message_Box("Rtn Info is wrong, in MAXEN::Boundary_Testing_with_eval_Erasing()", MB_OK);
	}
	if(++mmite != rtn_mm.end()){
		AppCall::Secretary_Message_Box("Rtn Info is wrong, in MAXEN::Boundary_Testing_with_eval_Erasing()", MB_OK);
	}
}

void MAXEN::Boundary_Testing_with_eval_Erasing(FeatureVctor& Training_v, MaxentModel& pmMaxen, map<size_t, map<size_t, double>*>& rtn_mm, bool Save_Info_Flag)
{
	FeatureVctor::iterator it;
	Maxen_Rtn_map Recall_map;
	Maxen_Rtn_map Pricision_map;
	set<me_outcome_type> outcomtype_s;

	for(it = Training_v.begin(); it != Training_v.end(); it++){
		if(outcomtype_s.find((*it)->first) == outcomtype_s.end()){
			Recall_map.insert(make_pair((*it)->first, make_pair(0, 0)));
			Pricision_map.insert(make_pair((*it)->first, make_pair(0, 0)));
			outcomtype_s.insert((*it)->first);
		}
	}
	set<me_outcome_type>::reverse_iterator positsite = outcomtype_s.rbegin();
	if((outcomtype_s.size() != 2) || (strcmp(positsite->data(), POSITIVE))){
		AppCall::Maxen_Responce_Message("Inappropriate maxen testing funciton or Class tag....");
	}

	map<size_t, map<size_t, double>*>::iterator mmite = rtn_mm.begin();
	map<size_t, double>::iterator mite = mmite->second->begin();
	for (it = Training_v.begin(); it != Training_v.end(); ++it) {
		me_outcome_type predict_out;
		me_context_type& context_ref = (*it)->second;
		//Predict
		predict_out = pmMaxen.predict(context_ref);

		if (predict_out == (*it)->first){
			Recall_map[(*it)->first].first++;
			Pricision_map[predict_out].first++;
		}
		Recall_map[(*it)->first].second++;
		Pricision_map[predict_out].second++;
		//===eval
		while(mite == mmite->second->end()){
			mmite++;
			if(mmite == rtn_mm.end()){
				AppCall::Secretary_Message_Box("Rtn Info is wrong, in MAXEN::Boundary_Testing_with_eval()", MB_OK);
			}
			mite = mmite->second->begin();
		}
		mite->second = pmMaxen.eval(context_ref, POSITIVE);
		/*//:test
		double negativevalue = pmMaxen.eval(context_ref, NEGETIVE);
		negativevalue += mite->second;
		if((0.99 > negativevalue) || ( negativevalue > 1.01)){
			AppCall::Secretary_Message_Box("(0.99 > negativevalue) || ( negativevalue > 1.01)", MB_OK);
		}*/
		mite++;
		delete *it;
    }
	Training_v.clear();
	if(mite != mmite->second->end()){
		AppCall::Secretary_Message_Box("Rtn Info is wrong, in MAXEN::Boundary_Testing_with_eval_Erasing()", MB_OK);
	}
	if(++mmite != rtn_mm.end()){
		AppCall::Secretary_Message_Box("Rtn Info is wrong, in MAXEN::Boundary_Testing_with_eval_Erasing()", MB_OK);
	}
	Display_Performance_for_MAXEN(false, Save_Info_Flag, Pricision_map, Recall_map, "");

}

void MAXEN::Candit_Maxen_Testing_with_Erasing(FeatureVctor& Testing_v, MaxentModel& pmMaxen, map<string, size_t>& pmPositive_Cases, vector<CanditCase*>& pmCandit_v)
{
	if(Testing_v.size() != pmCandit_v.size()){
		AppCall::Secretary_Message_Box("Unequal testing data between cases and features", MB_OK);
	}
	Maxen_Rtn_map Recall_map;
	Maxen_Rtn_map Pricision_map;
	FeatureVctor::iterator pit;
	vector<CanditCase*>::iterator vite = pmCandit_v.begin();
	for (pit = Testing_v.begin(); pit != Testing_v.end(); ++pit, vite++) {
		me_context_type& context_ref = (*pit)->second;
		//predict
		CanditCase& loc_Candit = **vite; 
		loc_Candit.predit_TYPE = pmMaxen.predict(context_ref);
		loc_Candit.value = pmMaxen.eval(context_ref, loc_Candit.predit_TYPE);

		if(Recall_map.find(loc_Candit.predit_TYPE) == Recall_map.end()){
			Recall_map.insert(make_pair(loc_Candit.predit_TYPE, make_pair(0, 0)));
			Pricision_map.insert(make_pair(loc_Candit.predit_TYPE, make_pair(0, 0)));
		}
		if(Recall_map.find(loc_Candit.org_TYPE) == Recall_map.end()){
			Recall_map.insert(make_pair(loc_Candit.org_TYPE, make_pair(0, 0)));
			Pricision_map.insert(make_pair(loc_Candit.org_TYPE, make_pair(0, 0)));
		}
		
		if (loc_Candit.predit_TYPE == (*pit)->first){
			Recall_map[(*pit)->first].first++;
			Pricision_map[loc_Candit.predit_TYPE].first++;
		}
		
		Recall_map[(*pit)->first].second++;
		Pricision_map[loc_Candit.predit_TYPE].second++;
		delete *pit;
    }
	Testing_v.clear();

    Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "");

	if(!pmPositive_Cases.empty()){
		for(map<string, size_t>::iterator mite = pmPositive_Cases.begin(); mite != pmPositive_Cases.end(); mite++){
			if(Recall_map[mite->first].second > mite->second){
				AppCall::Secretary_Message_Box("Info is inappropriate in MAXEN::Candit_Maxen_Testing_with_Erasing()", MB_OK);
				return;
			}
			Recall_map[mite->first].second = mite->second;
		}
		Display_Performance_for_MAXEN(true, true, Pricision_map, Recall_map, "");

	}
}
void MAXEN::Candit_Maxen_Recognization_with_Erasing(FeatureVctor& Testing_v, MaxentModel& pmMaxen, vector<CanditCase*>& pmCandit_v)
{
	if(Testing_v.size() != pmCandit_v.size()){
		AppCall::Secretary_Message_Box("Unequal testing data between cases and features", MB_OK);
	}
	FeatureVctor::iterator pit;
	vector<CanditCase*>::iterator vite = pmCandit_v.begin();
	for (pit = Testing_v.begin(); pit != Testing_v.end(); ++pit, vite++) {
		me_outcome_type predict_out;
		me_context_type& context_ref = (*pit)->second;
		//predict
		predict_out = pmMaxen.predict(context_ref);

		(*vite)->predit_TYPE = predict_out;
		(*vite)->value = pmMaxen.eval(context_ref, predict_out);

		delete *pit;
    }
	Testing_v.clear();
}
void MAXEN::Display_Performance_for_MAXEN(bool Out_Positve_Flag, bool Save_Info_Flag, Maxen_Rtn_map&  Pricision_map, Maxen_Rtn_map& Recall_map, string memo = "")
{
	ostringstream ostream;
	double Recall_rate = 0;
	double Pricision_rate = 0;
	double F_Score = 0;

	ostream.str("");
	ostream << endl << endl;
	ostream << memo << endl << endl;
	if(Pricision_map.size() != Recall_map.size()){
		ostream << "Notaion: Precision and Recall have different outcome types...\n\n";
	}
	ostream << "Precision\t\t  Recall\t\t  F-score\t\t  Correct  Total\t  TYPE" << endl;
	ostream << "----------------------------------------------------------------------------------------------------------";
	if(Save_Info_Flag){
		AppCall::Maxen_Responce_Message_with_Save(ostream.str().c_str());
	}
	else{
		AppCall::Maxen_Responce_Message(ostream.str().c_str());
	}
	Maxen_Rtn_map::iterator pmite = Pricision_map.begin();
	Maxen_Rtn_map::iterator rmite = Recall_map.begin();
	while(pmite != Pricision_map.end() || rmite != Recall_map.end()){		
		double loc_pre_rate = 0;
		double loc_rec_rate = 0;
		double loc_Fsc_rate = 0;
		string outcome_str = "";
		bool Output_Pre_Flag = false;
		bool Output_Rec_Flag = false;
		if(pmite != Pricision_map.end() && rmite != Recall_map.end()){
			if(strcmp(pmite->first.c_str(), rmite->first.c_str()) < 0){
				Output_Pre_Flag = true;
				outcome_str = pmite->first;
			}
			else if(strcmp(pmite->first.c_str(), rmite->first.c_str()) > 0){
				Output_Rec_Flag = true;
				outcome_str = rmite->first;
			}
			else{
				Output_Pre_Flag = true;
				Output_Rec_Flag = true;
				outcome_str = pmite->first;
			}
		}
		else if(pmite != Pricision_map.end()){
				Output_Pre_Flag = true;
				outcome_str = pmite->first;
		}
		else{
			Output_Rec_Flag = true;
			outcome_str = rmite->first;
		}
		
		if(Output_Pre_Flag){
			if(pmite->second.second != 0){
				loc_pre_rate = (double)(pmite->second.first)/pmite->second.second;
				if(strcmp(pmite->first.c_str(), NEGETIVE)){
					Pricision_rate += (double)(pmite->second.first)/pmite->second.second;
				}
			}
		}
		if(Output_Rec_Flag){
			if(rmite->second.second != 0){
				loc_rec_rate = (double)(rmite->second.first)/rmite->second.second;
				if(strcmp(rmite->first.c_str(), NEGETIVE)){
					Recall_rate += (double)(rmite->second.first)/rmite->second.second;
				}
			}
		}
		if(!strcmp(outcome_str.c_str(), NEGETIVE)){
			if(Output_Pre_Flag){
				pmite++;
			}
			if(Output_Rec_Flag){
				rmite++;
			}
			continue;
		}
		loc_Fsc_rate = (loc_pre_rate+loc_rec_rate)?2*loc_pre_rate*loc_rec_rate/(loc_pre_rate+loc_rec_rate):0;
		if(Save_Info_Flag){
			display_with_Save("%.4f%%\t  %.4f%%\t  %.4f%%\t  %3d\t  %3d\t  %s", 100*loc_pre_rate, 100*loc_rec_rate, 100*loc_Fsc_rate, (rmite != Recall_map.end())?rmite->second.first:0, (rmite != Recall_map.end())?rmite->second.second:0, outcome_str.c_str());
		}
		else{
			display("%.4f%%\t  %.4f%%\t  %.4f%%\t  %3d\t  %3d\t  %s", 100*loc_pre_rate, 100*loc_rec_rate, 100*loc_Fsc_rate, (rmite != Recall_map.end())?rmite->second.first:0, (rmite != Recall_map.end())?rmite->second.second:0, outcome_str.c_str());
		}
		if(Output_Pre_Flag){
			pmite++;
		}
		if(Output_Rec_Flag){
			rmite++;
		}
	}
	if(!Out_Positve_Flag){
		return;
	}
	size_t Number_of_Class = 0;
	for(Maxen_Rtn_map::iterator mmite = Recall_map.begin(); mmite != Recall_map.end(); mmite++){
		if(0 != mmite->second.second){
			Number_of_Class++;
		}
	}
	if((Recall_map.find(NEGETIVE) != Recall_map.end()) && (0 != Recall_map[NEGETIVE].second)){
			Number_of_Class--;
	}
	if(Number_of_Class == 0){
		AppCall::Maxen_Responce_Message("Error Info in MAXEN::Display_Performance_for_MAXEN()");
		return;
	}

	Pricision_rate = Pricision_rate * 100 / Number_of_Class;
	Recall_rate = Recall_rate * 100 / Number_of_Class;
	F_Score = (Pricision_rate == 0 && Recall_rate == 0)?0:(2*Pricision_rate*Recall_rate/(Pricision_rate+Recall_rate));

	ostream.str("");
	ostream << "---------------------------------------------------------------" << endl;
	if(Save_Info_Flag){
		AppCall::Maxen_Responce_Message_with_Save(ostream.str().c_str());
	}
	else{
		AppCall::Maxen_Responce_Message(ostream.str().c_str());
	}
	if(Recall_map.find(NEGETIVE) != Recall_map.end()){
		double loc_P, loc_R, loc_F;
		loc_P = Pricision_map[NEGETIVE].second?(double)Pricision_map[NEGETIVE].first/Pricision_map[NEGETIVE].second:0;
		loc_R = Recall_map[NEGETIVE].second?(double)Recall_map[NEGETIVE].first/Recall_map[NEGETIVE].second:0;
		loc_F = (loc_P+loc_R)?2*loc_P*loc_R/(loc_P+loc_R):0;
		if(Save_Info_Flag){
			display_with_Save("%.4f%%\t  %.4f%%\t  %.4f%%\t  %3d\t  %3d\t  %s", 100*loc_P, 100*loc_R, 100*loc_F, Recall_map[NEGETIVE].first, Recall_map[NEGETIVE].second, NEGETIVE);
		}
		else{
			display("%.4f%%\t  %.4f%%\t  %.4f%%\t  %3d\t  %3d\t  %s", 100*loc_P, 100*loc_R, 100*loc_F, Recall_map[NEGETIVE].first, Recall_map[NEGETIVE].second, NEGETIVE);
		}
	}
	if((Recall_map.find(NEGETIVE) != Recall_map.end()) && (Recall_map.size() > 2)){
		double det_P, det_R, det_F;
		size_t DetCorrectNum, DetTotalNum;
		DetCorrectNum = 0;
		DetTotalNum = 0;
		for(Maxen_Rtn_map::iterator mite = Pricision_map.begin(); mite != Pricision_map.end(); mite++){
			if(strcmp(mite->first.c_str(), NEGETIVE)){
				DetCorrectNum += mite->second.first;
				DetTotalNum += mite->second.second;
			}
		}
		det_P = DetTotalNum?(double)DetCorrectNum/DetTotalNum:0;

		DetCorrectNum = 0;
		DetTotalNum = 0;
		for(Maxen_Rtn_map::iterator mite = Recall_map.begin(); mite != Recall_map.end(); mite++){
			if(strcmp(mite->first.c_str(), NEGETIVE)){
				DetCorrectNum += mite->second.first;
				DetTotalNum += mite->second.second;
			}
		}
		det_R = DetTotalNum?(double)DetCorrectNum/DetTotalNum:0;
		det_F = (det_P+det_R)?2*det_P*det_R/(det_P+det_R):0;
	
		if(Save_Info_Flag){
			display_with_Save("%.4f%%\t  %.4f%%\t  %.4f%%\t  %3d\t  %3d\t  %s", 100*det_P, 100*det_R, 100*det_F, DetCorrectNum, DetTotalNum, POSITIVE);
		}
		else{
			display("%.4f%%\t  %.4f%%\t  %.4f%%\t  %3d\t  %3d\t  %s", 100*det_P, 100*det_R, 100*det_F, DetCorrectNum, DetTotalNum, POSITIVE);
		}
	}
	size_t Totalcnt = 0;
	double acc = 0;
	for(Maxen_Rtn_map::iterator pmite = Pricision_map.begin(); pmite != Pricision_map.end(); pmite++){
		acc += pmite->second.first;
		Totalcnt += pmite->second.second;
	}
	if(Totalcnt != 0){
		acc = 1.0*acc/Totalcnt;
	}
	ostream.str("");
	ostream << endl;
	ostream << "Accuracy:\t" << acc << "%" << endl;
	ostream << endl;
	ostream << "======Performance of Average Behavior================" << endl;
	ostream << "Class size: " << Number_of_Class << endl;
	ostream << "Precision:\t" << Pricision_rate << "%" << endl;
	ostream << "Recall:\t\t" << Recall_rate << "%" << endl;
	ostream << "F-Score:\t\t" << F_Score << "%" << endl;
	if(Save_Info_Flag){
		AppCall::Maxen_Responce_Message_with_Save(ostream.str().c_str());
	}
	else{
		AppCall::Maxen_Responce_Message(ostream.str().c_str());
	}
}
//-------------------------------------For normal
void MAXEN::Maxen_Training_with_Erasing(FeatureVctor& Training_v, MaxentModel& pmMaxen, size_t ite)
{
	Training_Cases_Check(Training_v);

	FeatureVctor::iterator vvite = Training_v.begin();
    vector<pair<me_context_type, me_outcome_type> >::iterator it;

	pmMaxen.begin_add_event();
	for( ; vvite != Training_v.end(); vvite++){
		me_context_type& context_ref = (*vvite)->second;
		pmMaxen.add_event(context_ref, (*vvite)->first);
		delete *vvite;
	}
	pmMaxen.end_add_event();
	Training_v.clear();

	pmMaxen.train(ite, "lbfgs", 0);
}
void MAXEN::cross_validation_with_Weight(FeatureVctor& v, size_t n, int iter, const string& method, double gaussian, bool random, double weight, const char* c_TYPE) 
{
	Training_Cases_Check(v);
    FeatureVctor::iterator it;
    //load_events(file, AddEventToVector(v));
    if (v.size() < 5 * n)
		AppCall::Secretary_Message_Box("data set is too small to perform cross_validation", MB_OK);
    if (random) {
        random_shuffle(v.begin(), v.end());
    }
	ostringstream ostream;
    double total_acc = 0;
	double total_Recall = 0;
	double total_Precision = 0;
	double F_Score = 0;
    size_t step = v.size() / n;

	Maxen_Rtn_map Recall_map;
	Maxen_Rtn_map Pricision_map;
	Maxen_Rtn_map total_R_map;
	Maxen_Rtn_map total_P_map;

    for (size_t i = 0; i < n; ++i) {
		for(Maxen_Rtn_map::iterator mite =  Pricision_map.begin(); mite !=  Pricision_map.end(); mite++){
			mite->second.first = 0;
			mite->second.second = 0;
		}
		for(Maxen_Rtn_map::iterator mite =  Recall_map.begin(); mite !=  Recall_map.end(); mite++){
			mite->second.first = 0;
			mite->second.second = 0;
		}
        MaxentModel m;
        m.begin_add_event();
        m.add_FeatureVctor_events(v.begin(), v.begin() + i * step);
        m.add_FeatureVctor_events(v.begin() + (i + 1) * step, v.end());
        m.end_add_event();
        m.train(iter, method, gaussian);

        size_t correct = 0;
        size_t count = 0;
        for (it = v.begin() + i * step; it != v.begin() + (i + 1) * step; ++it) {
			pair<string, vector<pair<string, float>>>& locpair = **it;
			
			double rtlvalue = m.eval(locpair.second, c_TYPE);
			me_outcome_type predict_out;
			if(rtlvalue >= weight){
				predict_out = c_TYPE;
			}
			else{
				predict_out = NEGETIVE;
			}

			if(Recall_map.find(locpair.first) == Recall_map.end()){
				Recall_map.insert(make_pair(locpair.first, make_pair(0, 0)));
				Pricision_map.insert(make_pair(locpair.first, make_pair(0, 0)));
				total_R_map.insert(make_pair(locpair.first, make_pair(0, 0)));
				total_P_map.insert(make_pair(locpair.first, make_pair(0, 0)));
			}
			if(Recall_map.find(predict_out) == Recall_map.end()){
				Recall_map.insert(make_pair(predict_out, make_pair(0, 0)));
				Pricision_map.insert(make_pair(predict_out, make_pair(0, 0)));
				total_R_map.insert(make_pair(predict_out, make_pair(0, 0)));
				total_P_map.insert(make_pair(predict_out, make_pair(0, 0)));
			}

			if (predict_out == locpair.first){
                ++correct;
				Recall_map[predict_out].first++;
				Pricision_map[predict_out].first++;
				total_R_map[predict_out].first++;
				total_P_map[predict_out].first++;
			}
            ++count;
			Recall_map[locpair.first].second++;
			Pricision_map[predict_out].second++;
			total_R_map[locpair.first].second++;
			total_P_map[predict_out].second++;
        }

        double acc = double(correct)/count;
		double Recall = 0;
		double Pricision = 0;
		for(Maxen_Rtn_map::iterator mite = Pricision_map.begin(); mite != Pricision_map.end(); mite++){
			if(mite->second.second != 0){
				Pricision += (double)(mite->second.first)/mite->second.second;
			}
		}
		cout << endl;
		for(Maxen_Rtn_map::iterator mite = Recall_map.begin(); mite != Recall_map.end(); mite++){
			double loc_rate = 0;
			if(mite->second.second != 0){
				Recall += (double)(mite->second.first)/mite->second.second;
			}
		}
		Recall /= Recall_map.size();
		Pricision /= Pricision_map.size();
		ostream.str("");
		ostream << "--------------------------------------" << endl;
		ostream << "Precision[" << i + 1 << "]: " << 100 * Pricision  << "%" << endl;
		ostream << "Recall[" << i + 1 << "]: " << 100 * Recall << "%" << endl;
		ostream << "F-Score[" << i + 1 << "]: " << 100*2*Recall*Pricision/(Recall+Pricision) << "%" << endl;
		AppCall::Maxen_Responce_Message(ostream.str().c_str());
        total_acc += acc;
		total_Recall += Recall;
		total_Precision += Pricision;
		F_Score += 2*Recall*Pricision/(Recall+Pricision);
    }
	ostream.str("");
	ostream << "======Performance on Total=================================" << endl;
	ostream << "Class size: " << Recall_map.size() << endl;
	ostream << n << "-fold Cross Validation Accuracy: " << total_acc*100/n << "%" << endl;
	ostream << n << "-fold Cross Validation Precision: " << total_Precision * 100 / n << "%" << endl;
	ostream << n << "-fold Cross Validation Recall: " << total_Recall * 100 / n << "%" << endl;
	ostream << n << "-fold Cross Validation F-Score: " << F_Score * 100 / n << "%" << endl;
	AppCall::Maxen_Responce_Message(ostream.str().c_str());

	total_Precision = 0;
	total_Recall = 0;

	ostream.str("");
	ostream << endl;
	ostream << endl;
	ostream << "======Performance on Each TYPE and SUBTYPE=====================" << endl;
	ostream << "Precision    Correct     Totall      TYPE" << endl;
	ostream << "-------------------------------------------------------------------------" << endl;
	AppCall::Maxen_Responce_Message(ostream.str().c_str());
	
	ostream.str("");
	ostream << endl;
	ostream << "Precision\t\t  Recall\t\t  F-score\t\t  Correct  Total\tTYPE" << endl;
	ostream << "-------------------------------------------------------------------------" << endl;
	AppCall::Maxen_Responce_Message(ostream.str().c_str());
	Maxen_Rtn_map::iterator pmite = total_P_map.begin();
	for(Maxen_Rtn_map::iterator rmite = total_R_map.begin(); rmite != total_R_map.end(); rmite++, pmite++){
		if(strcmp(pmite->first.c_str(), rmite->first.c_str())){
			AppCall::Secretary_Message_Box("Error in MAXEN::cross_validation...", MB_OK);
		}
		double loc_pre_rate = 0;
		double loc_rec_rate = 0;
		double loc_Fsc_rate = 0;
		if(pmite->second.second != 0){
			loc_pre_rate = (double)(pmite->second.first)/pmite->second.second;
			if(strcmp(pmite->first.c_str(), NEGETIVE)){
				total_Precision += (double)(pmite->second.first)/pmite->second.second;
			}
		}
		if(rmite->second.second != 0){
			loc_rec_rate = (double)(rmite->second.first)/rmite->second.second;
			if(strcmp(rmite->first.c_str(), NEGETIVE)){
				total_Recall += (double)(rmite->second.first)/rmite->second.second;
			}
		}
		if((loc_pre_rate+loc_rec_rate) > 0){
			loc_Fsc_rate = 2*loc_pre_rate*loc_rec_rate/(loc_pre_rate+loc_rec_rate);
		}
		else{
			loc_Fsc_rate = 0;
		}

		display("%.4f%%\t  %.4f%%\t  %.4f%%\t  %3d\t  %3d\t  %s", 100*loc_pre_rate, 100*loc_rec_rate, 100*loc_Fsc_rate, rmite->second.first, rmite->second.second, rmite->first.c_str());
	}
	size_t Number_of_Class = 0;
	for(Maxen_Rtn_map::iterator mmite = total_R_map.begin(); mmite != total_R_map.end(); mmite++){
		if(0 != mmite->second.second){
			Number_of_Class++;
		}
	}
	if((total_R_map.find(NEGETIVE) != total_R_map.end()) && (0 != total_R_map[NEGETIVE].second)){
			Number_of_Class--;
	}

	double Pricision = total_Precision * 100 / Number_of_Class;
	double Recall = total_Recall * 100 / Number_of_Class;
	if(Pricision == 0 && Recall ==0){
		F_Score = 0;
	}
	else{
		F_Score = 2*Pricision*Recall/(Pricision+Recall);
	}
	ostream.str("");
	ostream << endl;
	ostream << "======Performance on Positive Cases=================================" << endl;
	double loc_P, loc_R;
	if(0 == total_P_map[NEGETIVE].second){
		loc_P = 0;
	}
	else{
		loc_P = (double)total_P_map[NEGETIVE].first/total_P_map[NEGETIVE].second;
	}
	if(0 == total_R_map[NEGETIVE].second){
		loc_R = 0;
	}
	else{
		loc_R = (double)total_R_map[NEGETIVE].first/total_R_map[NEGETIVE].second;
	}
	ostream << "Precision of \"OTHER\": " << loc_P << endl;
	ostream << "Recall of \"OTHER\": " << loc_R << endl;
	if(loc_P+loc_R){
		ostream << "F score of \"OTHER\": " << 2*loc_P*loc_R/(loc_P+loc_R) << endl;
	}
	else{
		ostream << "F score of \"OTHER\": " << 0 << endl;
	}
	ostream << endl;
	ostream << "Class size: " << Number_of_Class << endl;
	ostream << n << "-fold Cross Validation Precision: " << Pricision << "%" << endl;
	ostream << n << "-fold Cross Validation Recall: " << Recall << "%" << endl;
	ostream << n << "-fold Cross Validation F-Score: " << F_Score << "%" << endl;

	AppCall::Maxen_Responce_Message_with_Save(ostream.str().c_str());
}

// perform n-Fold cross_validation, results are printed to stdout
void MAXEN::cross_validation(FeatureVctor& v, size_t n, int iter, const string& method, double gaussian, bool random) 
{
	Training_Cases_Check(v);
    FeatureVctor::iterator it;
    //load_events(file, AddEventToVector(v));
    if (v.size() < 5 * n)
		AppCall::Secretary_Message_Box("data set is too small to perform cross_validation", MB_OK);
    if (random) {
        random_shuffle(v.begin(), v.end());
    }
	ostringstream ostream;
    double total_acc = 0;
	double total_Recall = 0;
	double total_Precision = 0;
	double F_Score = 0;
    size_t step = v.size() / n;

	Maxen_Rtn_map Recall_map;
	Maxen_Rtn_map Pricision_map;
	Maxen_Rtn_map total_R_map;
	Maxen_Rtn_map total_P_map;

    for (size_t i = 0; i < n; ++i) {
		for(Maxen_Rtn_map::iterator mite =  Pricision_map.begin(); mite !=  Pricision_map.end(); mite++){
			mite->second.first = 0;
			mite->second.second = 0;
		}
		for(Maxen_Rtn_map::iterator mite =  Recall_map.begin(); mite !=  Recall_map.end(); mite++){
			mite->second.first = 0;
			mite->second.second = 0;
		}
        MaxentModel m;
        m.begin_add_event();
        m.add_FeatureVctor_events(v.begin(), v.begin() + i * step);
        m.add_FeatureVctor_events(v.begin() + (i + 1) * step, v.end());
        m.end_add_event();
        m.train(iter, method, gaussian);

        size_t correct = 0;
        size_t count = 0;
        for (it = v.begin() + i * step; it != v.begin() + (i + 1) * step; ++it) {
			pair<string, vector<pair<string, float>>>& locpair = **it;
			me_outcome_type predict_out = m.predict(locpair.second);

			if(Recall_map.find(locpair.first) == Recall_map.end()){
				Recall_map.insert(make_pair(locpair.first, make_pair(0, 0)));
				Pricision_map.insert(make_pair(locpair.first, make_pair(0, 0)));
				total_R_map.insert(make_pair(locpair.first, make_pair(0, 0)));
				total_P_map.insert(make_pair(locpair.first, make_pair(0, 0)));
			}
			if(Recall_map.find(predict_out) == Recall_map.end()){
				Recall_map.insert(make_pair(predict_out, make_pair(0, 0)));
				Pricision_map.insert(make_pair(predict_out, make_pair(0, 0)));
				total_R_map.insert(make_pair(predict_out, make_pair(0, 0)));
				total_P_map.insert(make_pair(predict_out, make_pair(0, 0)));
			}

			if (predict_out == locpair.first){
                ++correct;
				Recall_map[predict_out].first++;
				Pricision_map[predict_out].first++;
				total_R_map[predict_out].first++;
				total_P_map[predict_out].first++;
			}
            ++count;
			Recall_map[locpair.first].second++;
			Pricision_map[predict_out].second++;
			total_R_map[locpair.first].second++;
			total_P_map[predict_out].second++;
        }

        double acc = double(correct)/count;
		double Recall = 0;
		double Pricision = 0;
		for(Maxen_Rtn_map::iterator mite = Pricision_map.begin(); mite != Pricision_map.end(); mite++){
			if(mite->second.second != 0){
				Pricision += (double)(mite->second.first)/mite->second.second;
			}
		}
		cout << endl;
		for(Maxen_Rtn_map::iterator mite = Recall_map.begin(); mite != Recall_map.end(); mite++){
			double loc_rate = 0;
			if(mite->second.second != 0){
				Recall += (double)(mite->second.first)/mite->second.second;
			}
		}
		Recall /= Recall_map.size();
		Pricision /= Pricision_map.size();
		ostream.str("");
		ostream << "--------------------------------------" << endl;
		ostream << "Precision[" << i + 1 << "]: " << 100 * Pricision  << "%" << endl;
		ostream << "Recall[" << i + 1 << "]: " << 100 * Recall << "%" << endl;
		ostream << "F-Score[" << i + 1 << "]: " << 100*2*Recall*Pricision/(Recall+Pricision) << "%" << endl;
		AppCall::Maxen_Responce_Message(ostream.str().c_str());
        total_acc += acc;
		total_Recall += Recall;
		total_Precision += Pricision;
		F_Score += 2*Recall*Pricision/(Recall+Pricision);
    }
	ostream.str("");
	ostream << "======Performance on Total=================================" << endl;
	ostream << "Class size: " << Recall_map.size() << endl;
	ostream << n << "-fold Cross Validation Accuracy: " << total_acc*100/n << "%" << endl;
	ostream << n << "-fold Cross Validation Precision: " << total_Precision * 100 / n << "%" << endl;
	ostream << n << "-fold Cross Validation Recall: " << total_Recall * 100 / n << "%" << endl;
	ostream << n << "-fold Cross Validation F-Score: " << F_Score * 100 / n << "%" << endl;
	AppCall::Maxen_Responce_Message(ostream.str().c_str());

	total_Precision = 0;
	total_Recall = 0;

	ostream.str("");
	ostream << endl;
	ostream << endl;
	ostream << "======Performance on Each TYPE and SUBTYPE=====================" << endl;
	ostream << "Precision    Correct     Totall      TYPE" << endl;
	ostream << "-------------------------------------------------------------------------" << endl;
	AppCall::Maxen_Responce_Message(ostream.str().c_str());
	
	ostream.str("");
	ostream << endl;
	ostream << "Precision\t\t  Recall\t\t  F-score\t\t  Correct  Total\tTYPE" << endl;
	ostream << "-------------------------------------------------------------------------" << endl;
	AppCall::Maxen_Responce_Message(ostream.str().c_str());
	Maxen_Rtn_map::iterator pmite = total_P_map.begin();
	for(Maxen_Rtn_map::iterator rmite = total_R_map.begin(); rmite != total_R_map.end(); rmite++, pmite++){
		if(strcmp(pmite->first.c_str(), rmite->first.c_str())){
			AppCall::Secretary_Message_Box("Error in MAXEN::cross_validation...", MB_OK);
		}
		double loc_pre_rate = 0;
		double loc_rec_rate = 0;
		double loc_Fsc_rate = 0;
		if(pmite->second.second != 0){
			loc_pre_rate = (double)(pmite->second.first)/pmite->second.second;
			if(strcmp(pmite->first.c_str(), NEGETIVE)){
				total_Precision += (double)(pmite->second.first)/pmite->second.second;
			}
		}
		if(rmite->second.second != 0){
			loc_rec_rate = (double)(rmite->second.first)/rmite->second.second;
			if(strcmp(rmite->first.c_str(), NEGETIVE)){
				total_Recall += (double)(rmite->second.first)/rmite->second.second;
			}
		}
		if((loc_pre_rate+loc_rec_rate) > 0){
			loc_Fsc_rate = 2*loc_pre_rate*loc_rec_rate/(loc_pre_rate+loc_rec_rate);
		}
		else{
			loc_Fsc_rate = 0;
		}

		display("%.4f%%\t  %.4f%%\t  %.4f%%\t  %3d\t  %3d\t  %s", 100*loc_pre_rate, 100*loc_rec_rate, 100*loc_Fsc_rate, rmite->second.first, rmite->second.second, rmite->first.c_str());
	}
	size_t Number_of_Class = 0;
	for(Maxen_Rtn_map::iterator mmite = total_R_map.begin(); mmite != total_R_map.end(); mmite++){
		if(0 != mmite->second.second){
			Number_of_Class++;
		}
	}
	if((total_R_map.find(NEGETIVE) != total_R_map.end()) && (0 != total_R_map[NEGETIVE].second)){
			Number_of_Class--;
	}

	double Pricision = total_Precision * 100 / Number_of_Class;
	double Recall = total_Recall * 100 / Number_of_Class;
	if(Pricision == 0 && Recall ==0){
		F_Score = 0;
	}
	else{
		F_Score = 2*Pricision*Recall/(Pricision+Recall);
	}
	ostream.str("");
	ostream << endl;
	ostream << "======Performance on Positive Cases=================================" << endl;
	double loc_P, loc_R;
	if(0 == total_P_map[NEGETIVE].second){
		loc_P = 0;
	}
	else{
		loc_P = (double)total_P_map[NEGETIVE].first/total_P_map[NEGETIVE].second;
	}
	if(0 == total_R_map[NEGETIVE].second){
		loc_R = 0;
	}
	else{
		loc_R = (double)total_R_map[NEGETIVE].first/total_R_map[NEGETIVE].second;
	}
	ostream << "Precision of \"OTHER\": " << loc_P << endl;
	ostream << "Recall of \"OTHER\": " << loc_R << endl;
	if(loc_P+loc_R){
		ostream << "F score of \"OTHER\": " << 2*loc_P*loc_R/(loc_P+loc_R) << endl;
	}
	else{
		ostream << "F score of \"OTHER\": " << 0 << endl;
	}
	ostream << endl;
	ostream << "Class size: " << Number_of_Class << endl;
	ostream << n << "-fold Cross Validation Precision: " << Pricision << "%" << endl;
	ostream << n << "-fold Cross Validation Recall: " << Recall << "%" << endl;
	ostream << n << "-fold Cross Validation F-Score: " << F_Score << "%" << endl;

	AppCall::Maxen_Responce_Message_with_Save(ostream.str().c_str());
}


void MAXEN::Read_CSmaxent_Training_Data(const char* FilePath, bool TYPE_Flag, FeatureVctor& Training_v)
{
	ifstream in(FilePath);
	if(in.bad())
		return ;
	in.clear();
	in.seekg(0, ios::beg);
	if(in.peek() == EOF){
		in.close();
	}
	char getlineBuf[MAX_SENTENCE];
	string outcome;
	istringstream sstream;
	float Cnt;

	while(true){
		if(in.peek() == EOF){	
			break;
		}
		in.getline(getlineBuf, MAX_SENTENCE, '\n');
		istringstream instream(getlineBuf);
		pair<string, vector<pair<string, float>>>* pCRDC_Case_v = new pair<string, vector<pair<string, float>>>;
		while(EOF != instream.peek()){
			instream.getline(getlineBuf, MAX_SENTENCE,';');
			instream >> Cnt;
			pCRDC_Case_v->second.push_back(make_pair(getlineBuf, Cnt));
			instream.getline(getlineBuf, MAX_SENTENCE,';');
		}
		in.getline(getlineBuf, MAX_SENTENCE, ';');
		if(TYPE_Flag){
			pCRDC_Case_v->first = getlineBuf;
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
		}
		else{
			in.getline(getlineBuf, MAX_SENTENCE, '\n');
			pCRDC_Case_v->first = getlineBuf;
		}
		Training_v.push_back(pCRDC_Case_v);
	}
	in.close();
}


void predict(const MaxentModel& m, const string& in_file, const string& out_file, bool output_prob) {
    ifstream input(in_file.c_str());
    if (!input)
        throw runtime_error("unable to open data file to read");

    ostream* output = 0;
    ofstream os;
    if (!out_file.empty()) {
        os.open(out_file.c_str());
        if (!os)
            throw runtime_error("unable to open data file to write");
        else
            output = &os;
    }

    bool binary_feature = is_binary_feature(in_file);
    size_t correct = 0;
    size_t count = 0;
    me_context_type context;
    me_outcome_type outcome;
    vector<pair<me_outcome_type, double> > outcomes;
    string prediction;
    line_stream_iterator<> line(input);
    line_stream_iterator<> lend;
    if (output)
        output->precision(10);
    for (; line != lend; ++line) {
        if (!get_sample(*line, context, outcome, binary_feature)) {
            char msg[100];
            sprintf_s(msg, "line [%zd] in data file broken.", count);
            throw runtime_error(msg);
        }

        m.eval_all(context, outcomes, false);
        size_t max_i = 0;
        for (size_t i = 1; i < outcomes.size(); ++i)
            if (outcomes[i].second > outcomes[max_i].second)
                max_i = i;

        prediction = outcomes[max_i].first;

        if (prediction == outcome)
            ++correct;

        if (output) {
            if (output_prob) {
                for (size_t i = 0; i < outcomes.size(); ++i)
                    *output << outcomes[i].first << '\t'
                        << outcomes[i].second << '\t';
                *output << endl;
            } else {
                *output << prediction << endl;
            }
        }

        ++count;
    }
    cout << "Accuracy: " << 100.0 * correct/count << "% (" << 
        correct << "/" << count << ")" << endl;
}

 
bool get_sample(const string& line, me_context_type& context, me_outcome_type& outcome, bool binary_feature) {
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" \t");
    tokenizer tokens(line, sep);
    tokenizer::iterator it = tokens.begin();

    outcome = *it++;
    if (outcome.empty())
        return false;
    context.clear();
    if (binary_feature) {
        for (; it != tokens.end(); ++it)
            context.push_back(make_pair(*it, (float)1.0));
    } else {
        for (; it != tokens.end(); ++it) {
            size_t pos = it->find(';');
            if (pos == string::npos)
                return false;
            context.push_back(make_pair(it->substr(0, pos),
                        (float)atof(it->substr(pos + 1).c_str())));
        }
    }
    return true;
}
bool is_binary_feature(const string& file) {
    ifstream is(file.c_str());
    if (!is)
        throw runtime_error("can not open data file to read");
    string line;
    while (getline(is, line)) {
        if (!line.empty()) {
            typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
            boost::char_separator<char> sep(" \t");
            tokenizer tokens(line, sep);
            tokenizer::iterator it = tokens.begin();
            ++it;

            for (;it != tokens.end(); ++it) {
                size_t pos = it->find(';');
                if (pos == string::npos)
                    break;
                try {
                    boost::lexical_cast<double>(it->substr(pos + 1));
                } catch (boost::bad_lexical_cast&) {
                    break;
                }
            }
            return it != tokens.end();
        }
    }
    return true;
}

template <typename Func>
void load_events(const string& file, Func add_event) {
    bool binary_feature = is_binary_feature(file);

    size_t count = 0;
    me_context_type context;
    me_outcome_type outcome;

        ifstream is(file.c_str());
        if (!is)
            throw runtime_error("can not open data file to read");
        line_stream_iterator<> line(is);
        line_stream_iterator<> lend;
        for (; line != lend; ++line) {
            if (line->empty())
                continue; 
            if (!get_sample(*line, context, outcome, binary_feature)) {
                char msg[100];
                sprintf_s(msg, "line [%zd] in data file broken.", count);
                throw runtime_error(msg);
            }
            add_event(context, outcome);
            ++count;
            if (count % 1000 == 0) {
                displayA(".");
                if (count % 10000 == 0)
                    displayA(" ");
                if (count % 50000 == 0)
                    display("\t%zd samples", count);
            }
        }

    display("");
}