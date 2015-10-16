#include "StdAfx.h"
#include "CRegex.h"
#include "ConvertUTF.h"
#include <iostream>
#include <sstream>
#include "NECom.h"

#define MIN_CONCRETE_THRESHOLD 3
#define MIN_THRESHOLD_FOR_NE_PROB 0.8
#define MIN_THRESHOLD_FOR_NE_FREQ 3

using namespace boost;

CRegex::CRegex()
{
	FreqPattern_Initilized_Flag = false;

	string Regex_WorkSpace = DATA_FOLDER;
	Regex_WorkSpace += "CEDT\\Pattern\\";
	//------------------
	/*m_CRegex.Init_Inner_and_Outer_Pattern_Data_By_Surround(Regex_WorkSpace.c_str(), training_Surround_v);
	//m_CRegex.Adding_FreqPattern_to_Lexicon(Regex_WorkSpace.c_str(), NE_Lexicon_Set);
	//m_CRegex.Named_Entity_START_Boundary_Regex_Pattern_Initialization(training_Surround_v, 5000);
	//m_CRegex.Named_Entity_END_Boundary_Regex_Pattern_Initialization(training_Surround_v, 5000);
	m_CRegex.Named_Entity_Inner_Regex_Pattern_Initialization(15000);
	m_CRegex.Named_Entity_Outer_Regex_Pattern_Initialization (15000);
	//m_CRegex.Regex_Reset();*/
}
CRegex::~CRegex()
{
	Regex_Reset();
}

void CRegex::Regex_Reset()
{
	WordMor_ms.clear();
	FirstName_s.clear();
	Headnoun_s.clear();
	NE_inner_wregex_m.clear();
	L_wregex_m.clear();
	R_wregex_m.clear();
	for(map<size_t, set<size_t>*>::iterator msite = DulRegex_map.begin(); msite != DulRegex_map.end(); msite++){
		delete msite->second;
	}
	DulRegex_map.clear();
}


//==================START Boundary Regex Pattern Initializtion
void CRegex::Named_Entity_START_Boundary_Regex_Pattern_Initialization(vector<NE_Surround*>& NE_Surround_v, size_t MaxPattern)
{
	if(!FreqPattern_Initilized_Flag){
		return;
	}
	map<string, map<string, size_t>*> DualMatchedStr_mm;
	vector<pair<string, string>*> DualMatchpair_v;

	Generate_START_Boundary_Dual_Match_Pair(NE_Surround_v, DualMatchedStr_mm);

	Instantiation_Dual_Match_Regex_Pattern_Erasing(MaxPattern, DualMatchedStr_mm);
}
void  CRegex::Load_Frequent_Pattern_and_Filtering(set<string>& InnerBoundMatch_s, set<string>& OuterBoundMatch_s)
{
	map<string, unsigned long> su_inner_m;
	map<string, unsigned long> su_outer_m;

	string Pattern_Folder = NE_Pattern_Folder;
	Pattern_Folder += "Inner\\Workspace\\";
	m_CFPattern.Load_FreqPattern(Pattern_Folder + "SubFPSent_Info\\FreqPattern.dat", su_inner_m);

	Pattern_Folder = NE_Pattern_Folder;
	Pattern_Folder += "Outer\\Workspace\\";
	m_CFPattern.Load_FreqPattern(Pattern_Folder + "SubFPSent_Info\\FreqPattern.dat", su_outer_m);

	//set<string> ChineseChar_s;
	//set<string> Number_Set;
	set<string> UnRegexSymbol_s;
	/*string pathstr = DATA_FOLDER;
	pathstr += "ChineseChar.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(pathstr))){
		AppCall::Secretary_Message_Box(PATH_WORDDB, MB_OK);
	}
	NLPOP::LoadWordsStringSet(pathstr, ChineseChar_s);

	pathstr = DATA_FOLDER;
	pathstr += "Number_Set.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(pathstr))){
		AppCall::Secretary_Message_Box(PATH_WORDDB, MB_OK);
	}
	NLPOP::LoadWordsStringSet(pathstr, Number_Set);*/
	UnRegexSymbol_s.insert("(");
	UnRegexSymbol_s.insert(")");
	UnRegexSymbol_s.insert("-");
	UnRegexSymbol_s.insert("*");
	UnRegexSymbol_s.insert("+");

	for(map<string, unsigned long>::iterator mmite = su_inner_m.begin(); mmite != su_inner_m.end(); ){
		if(mmite->second < MIN_THRESHOLD_FOR_NE_FREQ){
			mmite = su_inner_m.erase(mmite);
		}
		else if(mmite->first.length() < 2){
			mmite = su_inner_m.erase(mmite);
		}
		else{
			bool UnRegex_Symbol_Flag = false;
			vector<string> character_v;
			NLPOP::charseq_to_vector(mmite->first.c_str(), character_v);
			for(vector<string>::iterator vite = character_v.begin(); vite != character_v.end(); vite++){
				if(UnRegexSymbol_s.find(vite->data()) != UnRegexSymbol_s.end()){
					UnRegex_Symbol_Flag = true;
					break;
				}
			}
			if(UnRegex_Symbol_Flag){
				mmite = su_inner_m.erase(mmite);
			}
			else{
				InnerBoundMatch_s.insert(mmite->first);
				mmite++;
			}
		}
	}
		/*else{
			bool nonChinese_Char_Flag = false;
			vector<string> character_v;
			NLPOP::charseq_to_vector(mmite->first.c_str(), character_v);
			for(vector<string>::iterator vite = character_v.begin(); vite != character_v.end(); vite++){
				if(ChineseChar_s.find(vite->data()) == ChineseChar_s.end()){
					if(Number_Set.find(vite->data()) == Number_Set.end()){
						nonChinese_Char_Flag = true;
						break;
					}
				}
			}
			if(nonChinese_Char_Flag){
				mmite = su_inner_m.erase(mmite);
			}
			else{
				InnerBoundMatch_s.insert(mmite->first);
				mmite++;
			}
		}
	}*/


	for(map<string, unsigned long>::iterator mmite = su_outer_m.begin(); mmite != su_outer_m.end(); ){
		if(mmite->second < MIN_THRESHOLD_FOR_NE_FREQ){
			mmite = su_outer_m.erase(mmite);
		}
		else if(mmite->first.length() < 2){
			mmite = su_outer_m.erase(mmite);
		}
		else{
			bool UnRegex_Symbol_Flag = false;
			vector<string> character_v;
			NLPOP::charseq_to_vector(mmite->first.c_str(), character_v);
			for(vector<string>::iterator vite = character_v.begin(); vite != character_v.end(); vite++){
				if(UnRegexSymbol_s.find(vite->data()) != UnRegexSymbol_s.end()){
					UnRegex_Symbol_Flag = true;
					break;
				}
			}
			if(UnRegex_Symbol_Flag){
				mmite = su_outer_m.erase(mmite);
			}
			else{
				OuterBoundMatch_s.insert(mmite->first);
				mmite++;
			}
		}
	}
		
		
		
		/*else{
			bool nonChinese_Char_Flag = false;
			vector<string> character_v;
			NLPOP::charseq_to_vector(mmite->first.c_str(), character_v);
			for(vector<string>::iterator vite = character_v.begin(); vite != character_v.end(); vite++){
				if(ChineseChar_s.find(vite->data()) == ChineseChar_s.end()){
					if(Number_Set.find(vite->data()) == Number_Set.end()){
						nonChinese_Char_Flag = true;
						break;
					}
				}
			}
			if(nonChinese_Char_Flag){
				mmite = su_outer_m.erase(mmite);
			}
			else{
				OuterBoundMatch_s.insert(mmite->first);
				mmite++;
			}
		}
	}*/

	return;

	multimap<unsigned long, string> su_inner_mm;
	multimap<unsigned long, string> su_outer_mm;
	for(map<string, unsigned long>::iterator mmite = su_inner_m.begin(); mmite != su_inner_m.end(); mmite = su_inner_m.erase(mmite)){
		su_inner_mm.insert(make_pair(mmite->second, mmite->first));
	}
	for(map<string, unsigned long>::iterator mmite = su_outer_m.begin(); mmite != su_outer_m.end(); mmite = su_outer_m.erase(mmite)){
		su_outer_mm.insert(make_pair(mmite->second, mmite->first));
	}
	size_t limit = (size_t)(su_inner_mm.size()*1);
	multimap<unsigned long, string>::iterator reservemite = su_inner_mm.begin();
	for(size_t i = 0; i < limit; i++){
		InnerBoundMatch_s.insert(reservemite->second);
		reservemite++;
	}

	limit = (size_t)(su_outer_mm.size()*1);
	reservemite = su_outer_mm.begin();
	for(size_t i = 0; i < limit; i++){
		OuterBoundMatch_s.insert(reservemite->second);
		reservemite++;
	}
}
void CRegex::Generate_START_Boundary_Dual_Match_Pair(vector<NE_Surround*>& NE_Surround_v, map<string, map<string, size_t>*>& DualMatchedStr_mm)
{
	vector<string> inner_n;
	vector<vector<string>> outer_v;
	//Load data
	Load_Inner_and_Outer_Vector(inner_n, outer_v, -1);


	set<string> InnerBoundMatch_s;
	set<string> OuterBoundMatch_s;

	Load_Frequent_Pattern_and_Filtering(InnerBoundMatch_s, OuterBoundMatch_s);

	vector<pair<string, string>*> outerpair_v;
	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = NE_Surround_v.begin(); ne_vite != NE_Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		set<size_t> START_s;
		set<size_t> END_s;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		ace_op::Get_Entity_Mention_extent_Map((*ne_vite)->entity_mention_v, START_s, END_s);
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			if(START_s.find(i) != START_s.end()){
				pair<string, string>* ploc_pair = new pair<string, string>;
				ploc_pair->first = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), 0, i);
				NECOM::Pretreatment_Sent_For_Named_Entity(ploc_pair->first, Number_set, English_set);

				ploc_pair->second = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), i, -1);
				NECOM::Pretreatment_Sent_For_Named_Entity(ploc_pair->second, Number_set, English_set);

				outerpair_v.push_back(ploc_pair);
			}
		}
	}

	for(vector<pair<string, string>*>::iterator vvite = outerpair_v.begin(); vvite != outerpair_v.end(); vvite++){
		vector<string> left_str_v;
		vector<string> right_str_v;
		set<string> leftmatch_s;
		set<string> rightmatch_s;
		pair<string, string>& outpair = **vvite;
		string l_matchedstr;
		string r_matchedstr;

		NLPOP::charseq_to_vector(outpair.first.c_str(), left_str_v);
		NLPOP::charseq_to_vector(outpair.second.c_str(), right_str_v);

		l_matchedstr = "";
		for(int i = left_str_v.size()-1; i >= 0; i--){
			l_matchedstr = left_str_v[i] + l_matchedstr;
			if(OuterBoundMatch_s.find(l_matchedstr) != OuterBoundMatch_s.end()){
				leftmatch_s.insert(l_matchedstr);
			}
		}
		r_matchedstr = "";
		for(size_t i = 0; i < right_str_v.size(); i++){
			r_matchedstr += right_str_v[i];
			if(InnerBoundMatch_s.find(r_matchedstr) != InnerBoundMatch_s.end()){
				rightmatch_s.insert(r_matchedstr);
			}
		}
		
		for(set<string>::iterator lsite = leftmatch_s.begin(); lsite != leftmatch_s.end(); lsite++){
			l_matchedstr = *lsite;	
			l_matchedstr = ".*" + l_matchedstr;

			for(set<string>::iterator rsite = rightmatch_s.begin(); rsite != rightmatch_s.end(); rsite++){
				r_matchedstr = *rsite;
				r_matchedstr += ".*";

				if(DualMatchedStr_mm.find(l_matchedstr) == DualMatchedStr_mm.end()){
					DualMatchedStr_mm[l_matchedstr];
					DualMatchedStr_mm[l_matchedstr] = new map<string, size_t>;
				}
				if(DualMatchedStr_mm[l_matchedstr]->find(r_matchedstr) == DualMatchedStr_mm[l_matchedstr]->end()){
					DualMatchedStr_mm[l_matchedstr]->insert(make_pair(r_matchedstr, 1));
				}
				else{
					(*DualMatchedStr_mm[l_matchedstr])[r_matchedstr]++;
				}
			}
		}
		delete *vvite;
	}
	outerpair_v.clear();
	//=============Out put Pattern and named entity reference
	string Pattern_Folder = NE_Pattern_Folder;
	Pattern_Folder += "STAR_Dual_Match.txt";
	ofstream out(Pattern_Folder.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	for(map<string, map<string, size_t>*>::iterator mmite = DualMatchedStr_mm.begin(); mmite != DualMatchedStr_mm.end(); mmite++){
		for(map<string, size_t>::iterator mite = mmite->second->begin(); mite != mmite->second->end(); mite++){
			out << mmite->first << '\t' << mite->first << '\t' << mite->second << endl;
		}
	}
	out.close();

}


//==================START Boundary Regex Pattern Initializtion
void CRegex::Named_Entity_END_Boundary_Regex_Pattern_Initialization(vector<NE_Surround*>& NE_Surround_v, size_t MaxPattern)
{
	if(!FreqPattern_Initilized_Flag){
		return;
	}
	map<string, map<string, size_t>*> DualMatchedStr_mm;
	vector<pair<string, string>*> DualMatchpair_v;

	Generate_END_Boundary_Dual_Match_Pair(NE_Surround_v, DualMatchedStr_mm);

	Instantiation_Dual_Match_Regex_Pattern_Erasing(MaxPattern, DualMatchedStr_mm);
}

void CRegex::Generate_END_Boundary_Dual_Match_Pair(vector<NE_Surround*>& NE_Surround_v, map<string, map<string, size_t>*>& DualMatchedStr_mm)
{
	vector<string> inner_n;
	vector<vector<string>> outer_v;
	//Load data
	Load_Inner_and_Outer_Vector(inner_n, outer_v, -1);
	
	set<string> InnerBoundMatch_s;
	set<string> OuterBoundMatch_s;

	Load_Frequent_Pattern_and_Filtering(InnerBoundMatch_s, OuterBoundMatch_s);

	vector<pair<string, string>*> outerpair_v;
	size_t CASID = 0;
	for(vector<NE_Surround*>::iterator ne_vite = NE_Surround_v.begin(); ne_vite != NE_Surround_v.end(); ne_vite++){
		size_t SENID = (*ne_vite)->SENID;
		vector<string> character_v;
		set<size_t> START_s;
		set<size_t> END_s;
		string& insurRef = (*ne_vite)->insur;
		NLPOP::charseq_to_vector(insurRef.c_str(), character_v);
		ace_op::Get_Entity_Mention_extent_Map((*ne_vite)->entity_mention_v, START_s, END_s);
		for(size_t i = 0; i < character_v.size(); i++){
			if(!strcmp(character_v[i].c_str(), "\n") || !strcmp(character_v[i].c_str(), " ")){
				continue;
			}
			if(END_s.find(i) != END_s.end()){
				pair<string, string>* ploc_pair = new pair<string, string>;
				ploc_pair->first = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), 0, i+1);
				NECOM::Pretreatment_Sent_For_Named_Entity(ploc_pair->first, Number_set, English_set);

				ploc_pair->second = Sentop::Get_Substr_by_Chinese_Character_Cnt(insurRef.c_str(), i+1, -1);
				NECOM::Pretreatment_Sent_For_Named_Entity(ploc_pair->second, Number_set, English_set);
				outerpair_v.push_back(ploc_pair);
			}
		}
	}
	for(vector<pair<string, string>*>::iterator vvite = outerpair_v.begin(); vvite != outerpair_v.end(); vvite++){
		vector<string> left_str_v;
		vector<string> right_str_v;
		set<string> leftmatch_s;
		set<string> rightmatch_s;
		pair<string, string>& outpair = **vvite;
		string l_matchedstr;
		string r_matchedstr;
		NLPOP::charseq_to_vector(outpair.first.c_str(), left_str_v);
		NLPOP::charseq_to_vector(outpair.second.c_str(), right_str_v);
		l_matchedstr = "";
		for(int i = left_str_v.size()-1; i >= 0; i--){
			l_matchedstr = left_str_v[i] + l_matchedstr;
			if(InnerBoundMatch_s.find(l_matchedstr) != InnerBoundMatch_s.end()){
				leftmatch_s.insert(l_matchedstr);
			}
		}
		r_matchedstr = "";
		for(size_t i = 0; i < right_str_v.size(); i++){
			r_matchedstr += right_str_v[i];
			if(OuterBoundMatch_s.find(r_matchedstr) != OuterBoundMatch_s.end()){
				rightmatch_s.insert(r_matchedstr);
			}
		}
		for(set<string>::iterator lsite = leftmatch_s.begin(); lsite != leftmatch_s.end(); lsite++){
			l_matchedstr = *lsite;
			l_matchedstr = ".*" + l_matchedstr;

			for(set<string>::iterator rsite = rightmatch_s.begin(); rsite != rightmatch_s.end(); rsite++){
				r_matchedstr = *rsite;
				r_matchedstr += ".*";

				if(DualMatchedStr_mm.find(l_matchedstr) == DualMatchedStr_mm.end()){
					DualMatchedStr_mm[l_matchedstr];
					DualMatchedStr_mm[l_matchedstr] = new map<string, size_t>;
				}
				if(DualMatchedStr_mm[l_matchedstr]->find(r_matchedstr) == DualMatchedStr_mm[l_matchedstr]->end()){
					DualMatchedStr_mm[l_matchedstr]->insert(make_pair(r_matchedstr, 1));
				}
				else{
					(*DualMatchedStr_mm[l_matchedstr])[r_matchedstr]++;
				}
			}
		}
		delete *vvite;
	}
	outerpair_v.clear();
	//=============Out put Pattern and named entity reference
	string Pattern_Folder = NE_Pattern_Folder;
	Pattern_Folder += "END_Dual_Match.txt";
	ofstream out(Pattern_Folder.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	for(map<string, map<string, size_t>*>::iterator mmite = DualMatchedStr_mm.begin(); mmite != DualMatchedStr_mm.end(); mmite++){
		for(map<string, size_t>::iterator mite = mmite->second->begin(); mite != mmite->second->end(); mite++){
			out << mmite->first << '\t' << mite->first << '\t' << mite->second << endl;
		}
	}
	out.close();
}




//------------------------For named Entity inner and outer pattern--------------------------------------------
void CRegex::Named_Entity_Inner_Regex_Pattern_Initialization(size_t MaxPattern)
{
	if(!FreqPattern_Initilized_Flag){
		return;
	}
	vector<string> inner_n;
	vector<vector<string>> outer_v;
	map<string, size_t> InnerMatchedStr_m;

	Load_Inner_and_Outer_Vector(inner_n, outer_v, 0);

	Gen_Regex_Named_Entity_Inner_Patterns(inner_n, InnerMatchedStr_m);

	set<string> FiledPattern_s;
	if(InnerMatchedStr_m.size() > MaxPattern){
		multimap<size_t, string> temp_map;
		for(map<string, size_t>::iterator mite = InnerMatchedStr_m.begin(); mite != InnerMatchedStr_m.end(); ){
			temp_map.insert(make_pair(mite->second, mite->first));
			mite = InnerMatchedStr_m.erase(mite);
		}
		size_t EraseCnt = temp_map.size() - MaxPattern;
		for(size_t i = 0; i < EraseCnt; i++){
			temp_map.erase(temp_map.begin());
		}
		for(multimap<size_t, string>::iterator mmite = temp_map.begin(); mmite != temp_map.end(); ){
			FiledPattern_s.insert(mmite->second);
			mmite = temp_map.erase(mmite);
		}
	}
	else{
		for(map<string, size_t>::iterator mite = InnerMatchedStr_m.begin(); mite != InnerMatchedStr_m.end(); ){
			FiledPattern_s.insert(mite->first);
			mite = InnerMatchedStr_m.erase(mite);
		}
	}
	size_t wregexID = 0;
	string loc_str;
	for(set<string>::iterator site = FiledPattern_s.begin(); site != FiledPattern_s.end(); site++){
		loc_str = "^" + *site + "$";
		NE_inner_wregex_m.insert(make_pair(wregexID++, SCONVERT::StringToWString(loc_str)));
	}
	InnerMatchedStr_m.clear();
}

void CRegex::Gen_Regex_Named_Entity_Inner_Patterns(vector<string>& inner_v, map<string, size_t>& InnerMatchedStr_m)
{
	bool Inner_Total_Match = false;
	if(inner_v.empty()){
		AppCall::Secretary_Message_Box("Empty data...: in CRegex...", MB_OK);
	}
	map<string, unsigned long> su_inner_m;
	string Pattern_Folder = NE_Pattern_Folder;
	Pattern_Folder += "Inner\\Workspace\\";
	m_CFPattern.Load_FreqPattern(Pattern_Folder + "SubFPSent_Info\\FreqPattern.dat", su_inner_m);

//==========Inner pattern geneating==========================
	set<string> EndMatch_s;
	set<string> TotalMatch_s;
	map<string, FREQPATTERNINFO> MulEntropy_m;

	for(map<string, unsigned long>::iterator mmite = su_inner_m.begin(); mmite != su_inner_m.end(); ){
		if(mmite->second < MIN_THRESHOLD_FOR_NE_FREQ){
			mmite = su_inner_m.erase(mmite);
		}
		else{
			EndMatch_s.insert(mmite->first);
			mmite++;
		}
	}
	for(vector<string>::iterator innervite = inner_v.begin(); innervite != inner_v.end(); innervite++){
		string i_str = innervite->data();//Test
		set<size_t> posit_s;
		vector<string> i_str_v;
		vector<bool> i_posit_v;
		set<string> PrixMatch_s;
		set<string> ProxMatch_s;
		string matchedstr;
		NLPOP::charseq_to_vector(innervite->c_str(), i_str_v);
		for(size_t i = 0; i < i_str_v.size(); i++){
			i_posit_v.push_back(false);
		}
		//==========tow side matched replace
		if(i_posit_v.empty()){
			continue;
		}
		if(i_posit_v.size() == 1){
			if(InnerMatchedStr_m.find(i_str) == InnerMatchedStr_m.end()){
				InnerMatchedStr_m.insert(make_pair(i_str, 1));
			}
			else{
				InnerMatchedStr_m[i_str]++;
			}
		}

		if(!Inner_Total_Match){
			matchedstr = "";
			for(size_t i = 0; i < i_str_v.size(); i++){
				matchedstr += i_str_v[i];
				if(EndMatch_s.find(matchedstr) != EndMatch_s.end()){
					PrixMatch_s.insert(matchedstr);
				}
			}
	
			matchedstr = "";
			for(int i = i_str_v.size()-1; i >= 0; i--){
				matchedstr = i_str_v[i] + matchedstr;
				if(EndMatch_s.find(matchedstr) != EndMatch_s.end()){
					ProxMatch_s.insert(matchedstr);
				}
			}
			for(set<string>::iterator isite = PrixMatch_s.begin(); isite != PrixMatch_s.end(); isite++){
				for(set<string>::iterator osite = ProxMatch_s.begin(); osite != ProxMatch_s.end(); osite++){
					size_t matchedsize = isite->size() + osite->size();
					matchedstr = "";
					if(matchedsize < innervite->length()){
						matchedstr = *isite + ".*" + *osite;
					}
					else if(matchedsize == innervite->length()){
						matchedstr = *isite + *osite;
					}
					else{
						continue;
					}
					if(InnerMatchedStr_m.find(matchedstr) == InnerMatchedStr_m.end()){
						InnerMatchedStr_m.insert(make_pair(matchedstr, 1));
					}
					else{
						InnerMatchedStr_m[matchedstr]++;
					}
				}
			}
		}
		else{
			multimap<string, pair<size_t, size_t>> MatchPosit_mm;// Generate substring in inner, those satisfy d_inner_mm;
			for(set<string>::iterator site = TotalMatch_s.begin(); site != TotalMatch_s.end(); site++){
				size_t firstp = innervite->find(*site);
				if(firstp != string::npos){
					size_t secondp = firstp + site->length();
					bool need_insert_flag = true;
					for(multimap<string, pair<size_t, size_t>>::iterator mpite = MatchPosit_mm.begin(); mpite != MatchPosit_mm.end(); ){
						if((firstp >= mpite->second.first) && (secondp <= mpite->second.second)){
							mpite = MatchPosit_mm.erase(mpite);
							continue;
						}
						else if((secondp >= mpite->second.first) && (secondp <= mpite->second.second)){
							if((MulEntropy_m.find(mpite->first) == MulEntropy_m.end()) || (MulEntropy_m.find(*site) == MulEntropy_m.end())){
								AppCall::Secretary_Message_Box("Error: in CRegex::Geny_Inner_Patterns()", MB_OK);
							}
							if(MulEntropy_m[mpite->first].ammount < MulEntropy_m[*site].ammount){
								mpite = MatchPosit_mm.erase(mpite);
								continue;
							}
						}
						else if((firstp <= mpite->second.first) && (secondp >= mpite->second.second)){
							need_insert_flag = false;
							break;
						}
						else if((mpite->second.second >= firstp) && (mpite->second.second <= secondp)){
							if((MulEntropy_m.find(mpite->first) == MulEntropy_m.end()) || (MulEntropy_m.find(*site) == MulEntropy_m.end())){
								AppCall::Secretary_Message_Box("Error: in CRegex::Gen_Regex_Named_Entity_Inner_and_outer_Patterns()", MB_OK);
							}
							if(MulEntropy_m[*site].ammount < MulEntropy_m[mpite->first].ammount){
								need_insert_flag = false;
								break;
							}
						}
						mpite++;
					}
					if(need_insert_flag){
						MatchPosit_mm.insert(make_pair(*site, make_pair(firstp, secondp)));
					}
				}//end of if(innervite->data().find(*site) != string::npos)
			}//end of for(multimap<double, string>::iterator mmite = d_inner_mm.begin();
			//----------------------------------
			for(multimap<string, pair<size_t, size_t>>::iterator mpite = MatchPosit_mm.begin(); mpite != MatchPosit_mm.end(); mpite++){
				for(size_t i = mpite->second.first; i <= mpite->second.second; i++){
					posit_s.insert(i);
				}
			}
		}
	}
//=============Out put Pattern and named entity reference
	Pattern_Folder = DATA_FOLDER;
	Pattern_Folder += "Inner\\InnerPatternRef.txt";
	ofstream out(Pattern_Folder.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	for(map<string, size_t>::iterator mite = InnerMatchedStr_m.begin(); mite != InnerMatchedStr_m.end(); mite++){
		out << mite->first << '\t' << mite->second << endl;
	}
	out.close();
}




//============================
void CRegex::Named_Entity_Outer_Regex_Pattern_Initialization(size_t MaxPattern)
{
	if(!FreqPattern_Initilized_Flag){
		return;
	}
	vector<string> inner_n;
	vector<vector<string>> outer_v;
	map<string, map<string, size_t>*> DualMatchedStr_mm;
	//Load data
	Load_Inner_and_Outer_Vector(inner_n, outer_v, -1);
	//Generate outer pattern
	
	vector<pair<string, string>*> outerpair_v;
	string left_str;
	string right_str;
	for(int i = 0; i < (int)outer_v.size(); i++){
		for(int j = 0; j < (int)outer_v[i].size()-1; j++ ){
			left_str = outer_v[i][j];
			pair<string, string>* ploc_pair = new pair<string, string>;
			ploc_pair->first = outer_v[i][j];
			ploc_pair->second = outer_v[i][j+1];
			outerpair_v.push_back(ploc_pair);
		}
	}
	outer_v.clear();

	Gen_Regex_Named_Entity_Outer_Patterns_Erasing(outerpair_v, DualMatchedStr_mm);

	Instantiation_Dual_Match_Regex_Pattern_Erasing(MaxPattern, DualMatchedStr_mm);

}
void CRegex::Instantiation_Dual_Match_Regex_Pattern_Erasing(size_t MaxPattern, map<string, map<string, size_t>*>& DualMatchedStr_mm)
{
	if(DualMatchedStr_mm.empty()){
		AppCall::Secretary_Message_Box("Empty data...: in CRegex...", MB_OK);
	}
	if(!DulRegex_map.empty()){
		AppCall::Secretary_Message_Box("Regex need Initialization...", MB_OK);
		Regex_Reset();
	}
	size_t PatternCnt = 0;
	vector<pair<string, string>*> Patterns_v;
	for(map<string, map<string, size_t>*>::iterator mmite = DualMatchedStr_mm.begin(); mmite != DualMatchedStr_mm.end(); mmite++){
		PatternCnt += mmite->second->size();
	}
	if((PatternCnt > MaxPattern) && (0 != MaxPattern)){
		multimap<size_t, pair<string, string>*> temp_map;
		for(map<string, map<string, size_t>*>::iterator mmite = DualMatchedStr_mm.begin(); mmite != DualMatchedStr_mm.end(); mmite = DualMatchedStr_mm.erase(mmite)){
			for(map<string, size_t>::iterator mite = mmite->second->begin();  mite != mmite->second->end(); mite = mmite->second->erase(mite)){
				pair<string, string>* ploc_pair = new pair<string, string>;
				ploc_pair->first = mmite->first;
				ploc_pair->second = mite->first;
				temp_map.insert(make_pair(mite->second, ploc_pair));
			}
			delete mmite->second;
		}
		DualMatchedStr_mm.clear();
		size_t EraseCnt = temp_map.size() - MaxPattern;
		for(size_t i = 0; i < EraseCnt; i++){
			delete temp_map.begin()->second;
			temp_map.erase(temp_map.begin());
		}
		for(multimap<size_t, pair<string, string>*>::iterator mmite = temp_map.begin(); mmite != temp_map.end(); mmite++){
			Patterns_v.push_back(mmite->second);//(make_pair(mmite->second->first, mmite->second->second));//
			//delete mmite->second;
		}
	}
	else{
		for(map<string, map<string, size_t>*>::iterator mmite = DualMatchedStr_mm.begin(); mmite != DualMatchedStr_mm.end(); ){
			for(map<string, size_t>::iterator mite = mmite->second->begin();  mite != mmite->second->end();  ){
				pair<string, string>* ploc_pair = new pair<string, string>;
				ploc_pair->first = mmite->first;
				ploc_pair->second = mite->first;
				Patterns_v.push_back(ploc_pair);// (make_pair(mmite->first, mite->first));//
				mite = mmite->second->erase(mite);
			}
			delete mmite->second;
			mmite = DualMatchedStr_mm.erase(mmite);
		}
		DualMatchedStr_mm.clear();
	}
	map<string, size_t> L_ID_m;
	map<string, size_t> R_ID_m;
	size_t L_ID = 0;
	size_t R_ID = 0;
	for(vector<pair<string, string>*>::iterator vite = Patterns_v.begin(); vite != Patterns_v.end(); vite++){
		pair<string, string>& loc_pair = **vite;
		if(L_ID_m.find(loc_pair.first) == L_ID_m.end()){
			L_ID_m.insert(make_pair(loc_pair.first, L_ID));
//			L_String2ID_m.insert(make_pair(L_ID, loc_pair.first));//:test
			//string locstr = " L_";
			//locstr += "^" + loc_pair.first + "$";
			//AppCall::Maxen_Responce_Message(locstr.c_str());
			L_wregex_m.insert(make_pair(L_ID, SCONVERT::StringToWString("^" + loc_pair.first + "$")));
			L_ID++;
		}	
		if(R_ID_m.find(loc_pair.second) == R_ID_m.end()){
			R_ID_m.insert(make_pair(loc_pair.second, R_ID));
//			R_String2ID_m.insert(make_pair(R_ID, loc_pair.second));//:test
			//string locstr = " L_";
			//locstr += "^" + loc_pair.first + "$";
			//AppCall::Maxen_Responce_Message(locstr.c_str());
			R_wregex_m.insert(make_pair(R_ID, SCONVERT::StringToWString("^" + loc_pair.second + "$")));
			R_ID++;
		}
		if(DulRegex_map.find(L_ID_m[loc_pair.first]) == DulRegex_map.end()){
			DulRegex_map[L_ID_m[loc_pair.first]];
			DulRegex_map[L_ID_m[loc_pair.first]] = new set<size_t>;
		}
		if(DulRegex_map[L_ID_m[loc_pair.first]]->find(R_ID_m[loc_pair.second]) == DulRegex_map[L_ID_m[loc_pair.first]]->end()){
			DulRegex_map[L_ID_m[loc_pair.first]]->insert(R_ID_m[loc_pair.second]);
		}
		delete *vite;
	}
	Patterns_v.clear(); 

	/*ostringstream ostrsteam;
	ostrsteam << "Pattern Pair: " << Patterns_v.size() << endl;
	ostrsteam << "L Pattern: " << L_wregex_m.size() << endl;
	ostrsteam << "R Pattern: " << R_wregex_m.size() << endl;
	AppCall::Secretary_Message_Box(ostrsteam.str(), MB_OK);*/

}


void CRegex::Gen_Regex_Named_Entity_Outer_Patterns_Erasing(vector<pair<string, string>*>& outerpair_v, map<string, map<string, size_t>*>& DualMatchedStr_mm)
{
	if(outerpair_v.empty()){
		AppCall::Secretary_Message_Box("Empty data...: in CRegex...", MB_OK);
	}

	set<string> InnerEndMatch_s;
	set<string> OuterEndMatch_s;

	Load_Frequent_Pattern_and_Filtering(InnerEndMatch_s, OuterEndMatch_s);

	for(vector<pair<string, string>*>::iterator vvite = outerpair_v.begin(); vvite != outerpair_v.end(); vvite++){
		vector<string> left_str_v;
		vector<string> right_str_v;
		set<string> leftmatch_s;
		set<string> rightmatch_s;
		pair<string, string>& outpair = **vvite;
		string l_matchedstr;
		string r_matchedstr;
		NLPOP::charseq_to_vector(outpair.first.c_str(), left_str_v);
		NLPOP::charseq_to_vector(outpair.second.c_str(), right_str_v);
		l_matchedstr = "";
		for(int i = left_str_v.size()-1; i >= 0; i--){
			l_matchedstr = left_str_v[i] + l_matchedstr;
			if(OuterEndMatch_s.find(l_matchedstr) != OuterEndMatch_s.end()){
				leftmatch_s.insert(l_matchedstr);
			}
		}
		r_matchedstr = "";
		for(size_t i = 0; i < right_str_v.size(); i++){
			r_matchedstr += right_str_v[i];
			if(OuterEndMatch_s.find(r_matchedstr) != OuterEndMatch_s.end()){
				rightmatch_s.insert(r_matchedstr);
			}
		}
		for(set<string>::iterator lsite = leftmatch_s.begin(); lsite != leftmatch_s.end(); lsite++){
			l_matchedstr = *lsite;
			if(l_matchedstr.length() < outpair.first.length()){
				l_matchedstr = ".*" + l_matchedstr;
			}
			for(set<string>::iterator rsite = rightmatch_s.begin(); rsite != rightmatch_s.end(); rsite++){
				r_matchedstr = *rsite;
				if(r_matchedstr.length() < outpair.second.length()){
					r_matchedstr += ".*";
				}
				if(DualMatchedStr_mm.find(l_matchedstr) == DualMatchedStr_mm.end()){
					DualMatchedStr_mm[l_matchedstr];
					DualMatchedStr_mm[l_matchedstr] = new map<string, size_t>;
				}
				if(DualMatchedStr_mm[l_matchedstr]->find(r_matchedstr) == DualMatchedStr_mm[l_matchedstr]->end()){
					DualMatchedStr_mm[l_matchedstr]->insert(make_pair(r_matchedstr, 1));
				}
				else{
					(*DualMatchedStr_mm[l_matchedstr])[r_matchedstr]++;
				}
			}
		}
		delete *vvite;
	}
	outerpair_v.clear();
	//=============Out put Pattern and named entity reference
	string Pattern_Folder = NE_Pattern_Folder;
	Pattern_Folder += "Outer\\OuterPatternRef.txt";
	ofstream out(Pattern_Folder.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);

	for(map<string, map<string, size_t>*>::iterator mmite = DualMatchedStr_mm.begin(); mmite != DualMatchedStr_mm.end(); mmite++){
		for(map<string, size_t>::iterator mite = mmite->second->begin(); mite != mmite->second->end(); mite++){
			out << mmite->first << '\t' << mite->first << '\t' << mite->second << endl;
		}
	}
	out.close();
}



//==================Init inner and outer pattern Data
void CRegex::Init_Inner_and_Outer_Pattern_Data_By_Surround(const char* pDataFolder, vector<NE_Surround*>& NE_Surround_v)
{
	//------load for number merging;
	string LexiconPath = DATA_FOLDER;
	LexiconPath += "Number_Set.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(LexiconPath))){
		AppCall::Secretary_Message_Box(LexiconPath.c_str(), MB_OK);
	}
	NLPOP::LoadWordsStringSet(LexiconPath, Number_set);

	LexiconPath = DATA_FOLDER;
	LexiconPath += "English_Set.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(LexiconPath))){
		AppCall::Secretary_Message_Box(LexiconPath.c_str(), MB_OK);
	}
	NLPOP::LoadWordsStringSet(LexiconPath, English_set);


	NE_Pattern_Folder = pDataFolder;

	vector<string> inner_v;
	vector<vector<string>> outer_v;
	Generate_Inner_and_Outer_Vector_By_Surround(NE_Surround_v, inner_v, outer_v);
	string workspace = NE_Pattern_Folder;

	Generate_Frequent_Pattern(inner_v, outer_v, workspace.c_str());

	string SaveFileName = pDataFolder;
	SaveFileName += "Inner\\Data\\Inner.dat";

	Write_Vector_Deque_List_To_File<vector<string>>(SaveFileName, inner_v, 0, '\n', "");

	SaveFileName = pDataFolder;
	SaveFileName += "Outer\\Data\\Outer.dat";
	
	ofstream out(SaveFileName.c_str());
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(size_t i = 0; i < outer_v.size(); i++){
		for(size_t j = 0; j < outer_v[i].size(); j++){
			out << outer_v[i][j] << " ";
		}
		out << endl;
	}
	out.close();

	FreqPattern_Initilized_Flag = true;
}

void CRegex::Generate_Inner_and_Outer_Vector_By_Surround(vector<NE_Surround*>& NE_Surround_v, vector<string>& inner_v, vector<vector<string>>& outer_v)
{
	if(NE_Surround_v.empty()){
		AppCall::Secretary_Message_Box("Empty data...: in CRegex...", MB_OK);
	}
	inner_v.clear();
	outer_v.clear();

	char sentencechar[MAX_SENTENCE];
	char sChar[3];
	sChar[2]=0;
	size_t sentlength;
	size_t sentpoist;
	string loc_str;

	for(vector<NE_Surround*>::iterator vite = NE_Surround_v.begin(); vite != NE_Surround_v.end(); vite++){
		for(vector<ACE_entity_mention>::iterator ivite = (*vite)->entity_mention_v.begin(); ivite != (*vite)->entity_mention_v.end(); ivite++){
			loc_str = NECOM::Pretreatment_Sent_For_Named_Entity(ivite->extent.charseq.c_str(), Number_set, English_set);
			inner_v.push_back(loc_str);
		}
	}
	for(vector<NE_Surround*>::iterator vite = NE_Surround_v.begin(); vite != NE_Surround_v.end(); vite++){
		set<size_t> namedposit_s;
		for(vector<ACE_entity_mention>::iterator ivite = (*vite)->entity_mention_v.begin(); ivite != (*vite)->entity_mention_v.end(); ivite++){
			for(size_t i = ivite->extent.START; i <= ivite->extent.END; i++){
				namedposit_s.insert(i);
			}
		}
		sentlength = (*vite)->insur.length();
		sentpoist = 0;
		strcpy_s(sentencechar, MAX_SENTENCE, "");
		vector<string> sentpattern_v;
		for(size_t i = 0; i <= sentlength; sentpoist++){
			sChar[0] = (*vite)->insur.c_str()[i++];
			sChar[1] = 0;	
			if(sChar[0] < 0 ){
				sChar[1]=(*vite)->insur.c_str()[i++];
			}
			if(namedposit_s.find(sentpoist) != namedposit_s.end()){
				if(strlen(sentencechar) != 0){
					loc_str = NECOM::Pretreatment_Sent_For_Named_Entity(sentencechar, Number_set, English_set);
					sentpattern_v.push_back(loc_str);
					strcpy_s(sentencechar, MAX_SENTENCE, "");
				}
			}
			else{
				strcat_s(sentencechar, MAX_SENTENCE, sChar);
			}
		}
		if(strlen(sentencechar) != 0){
			loc_str =  NECOM::Pretreatment_Sent_For_Named_Entity(sentencechar, Number_set, English_set);
			sentpattern_v.push_back(loc_str);
			strcpy_s(sentencechar, MAX_SENTENCE, "");
		}
		outer_v.push_back(sentpattern_v);
	}
}



void CRegex::Generate_Frequent_Pattern(vector<string>& inner_v, vector<vector<string>>& outer_v, const char* pDataFolder)
{
	string workspacestr;
	workspacestr =  pDataFolder;
	if(NLPOP::FolderExist(NLPOP::string2CString(workspacestr))){
		/*if(IDYES != AppCall::Secretary_Message_Box("目录已经存在，是否需要更新？", MB_YESNOCANCEL)){
			workspacestr = pDataFolder;
			workspacestr += "Inner\\Workspace\\";
			m_CFPattern.Load_FreqPattern(workspacestr + "SubFPSent_Info\\FreqPattern.dat", su_inner_m);
			workspacestr = pDataFolder;
			workspacestr += "Outer\\Workspace\\";
			m_CFPattern.Load_FreqPattern(workspacestr + "SubFPSent_Info\\FreqPattern.dat", su_outer_m);
			return;
		}*/
		NLPOP::DeleteDir(NLPOP::string2CString(workspacestr));
		_mkdir(workspacestr.c_str());
	}
	else{
		_mkdir(workspacestr.c_str());
	}
	
	string Service;
	vector<string> sParam_v;
	vector<LPVOID> pParam_v;

	workspacestr = pDataFolder;
	workspacestr += "Inner\\";
	_mkdir(workspacestr.c_str());
	
	Service = "频繁项集抽取"; 
	sParam_v.push_back(workspacestr);
	sParam_v.push_back(workspacestr);

	sParam_v[0] += "Data\\";
	sParam_v[1] += "Workspace\\";
	_mkdir(sParam_v[0].c_str());
	_mkdir(sParam_v[1].c_str());

	Write_Vector_Deque_List_To_File<vector<string>>(sParam_v[0]+"Inner_Pattern.txt", inner_v, 0, '\n', "");
	m_CFPattern.CFPattern_Port(Service, sParam_v, pParam_v);
	//m_CFPattern.Load_FreqPattern(sParam_v[1] + "SubFPSent_Info\\FreqPattern.dat", su_inner_m);

	//-----------outer
	workspacestr = pDataFolder;
	workspacestr += "Outer\\";
	_mkdir(workspacestr.c_str());

	sParam_v[0] = workspacestr;
	sParam_v[1] = workspacestr;

	sParam_v[0] += "Data\\";
	sParam_v[1] += "Workspace\\";
	_mkdir(sParam_v[0].c_str());
	_mkdir(sParam_v[1].c_str());

/*	for(size_t i = 0; i < outer_v.size(); i++){
		for(size_t j = 0; j < outer_v[i].size(); j++){
			pattern_s.insert(outer_v[i][j]);
		}
	}*/
	vector<string> singleouter_v;
	for(size_t i = 0; i < outer_v.size(); i++){
		for(size_t j = 0; j < outer_v[i].size(); j++){
			singleouter_v.push_back(outer_v[i][j]);
		}
	}
	Write_Vector_Deque_List_To_File<vector<string>>(sParam_v[0]+"outer_Pattern.txt", singleouter_v, 0, '\n', "");
	m_CFPattern.CFPattern_Port(Service, sParam_v, pParam_v);
}






bool CRegex::Named_Entity_Inner_Matched(const char* pcharseq, string& matchstr)
{
	size_t matchID;
	bool rtn_Flag = false;
	rtn_Flag = Named_Entity_Inner_Matched(pcharseq, matchID);
	if(rtn_Flag){
		ostringstream ostrsteam;
		ostrsteam << matchID;
		matchstr = ostrsteam.str();
	}
	return rtn_Flag;
}
bool CRegex::Named_Entity_Inner_Matched(const char* pcharseq, size_t& matchID)
{
	if(NE_inner_wregex_m.empty()){
		AppCall::Secretary_Message_Box("Regex is empty!: in CDetBound::Named_Entity_Inner_Matched()...", MB_OK);
	}
	string inputstr = pcharseq;
	wstring inwstring = SCONVERT::StringToWString(inputstr);
	boost::wsmatch what; 
	for(map<size_t, boost::wregex>::iterator regexite = NE_inner_wregex_m.begin(); regexite != NE_inner_wregex_m.end(); regexite++){
		if(regex_match(inwstring, what, regexite->second)){
			matchID = regexite->first;
			return true;
		}
	}
	return false;

}
bool CRegex::Regex_Pattern_Dual_Match_Poart(const char* Lpcharseq, const char* Rpcharseq, vector<string>& matchstr_v)
{
	//:test
	//string teststr;
	set<size_t> Lmatch_s;
	set<size_t> Rmatch_s;
	string inputstr = Lpcharseq;
	wstring inwstring = SCONVERT::StringToWString(inputstr);
	boost::wsmatch what; 
	for(map<size_t, boost::wregex>::iterator regexite = L_wregex_m.begin(); regexite != L_wregex_m.end(); regexite++){
		if(regex_match(inwstring, what, regexite->second)){
			Lmatch_s.insert(regexite->first);
		}
	}
	for(set<size_t>::iterator ssite = Lmatch_s.begin(); ssite != Lmatch_s.end(); ssite++){
		if(DulRegex_map.find(*ssite) == DulRegex_map.end()){
			continue;
		}
		for(set<size_t>::iterator site = DulRegex_map[*ssite]->begin(); site != DulRegex_map[*ssite]->end(); site++){
			if(R_wregex_m.find(*site) == R_wregex_m.end()){
				continue;
			}
			inputstr = Rpcharseq;
			inwstring = SCONVERT::StringToWString(inputstr);
			if(regex_match(inwstring, what, R_wregex_m[*site])){
				ostringstream ostrsteam;
				ostrsteam << *ssite << "_" << *site;
				matchstr_v.push_back(ostrsteam.str());
				//teststr = L_String2ID_m[*ssite] + "_" + R_String2ID_m[*site];
			}
		}
	}
	if(matchstr_v.empty()){
		return false;
	}
	else{
		return true;
	}
}

void CRegex::Named_Entity_Inner_Matched(const char* pcharseq, vector<string>& matchstr_v)
{
	set<size_t> matchID_s;
	string inputstr = pcharseq;
	wstring inwstring = SCONVERT::StringToWString(inputstr);
	boost::wsmatch what; 
	for(map<size_t, boost::wregex>::iterator regexite = NE_inner_wregex_m.begin(); regexite != NE_inner_wregex_m.end(); regexite++){
		if(regex_match(inwstring, what, regexite->second)){
			matchID_s.insert(regexite->first);
		}
	}
	for(set<size_t>::iterator site = matchID_s.begin(); site != matchID_s.end(); site++){
		ostringstream ostrsteam;
		ostrsteam << *site;
		matchstr_v.push_back(ostrsteam.str());
	}
}
void CRegex::Load_Inner_and_Outer_Vector(vector<string>& inner_v, vector<vector<string>>& outer_v, int Load_Model)
{
	string SaveFileName = NE_Pattern_Folder;
	SaveFileName += "Inner\\Data\\Inner.dat";

	if(Load_Model == 0 || Load_Model == -1){
		if(!NLPOP::FileExist(NLPOP::string2CString(SaveFileName))){
			AppCall::Secretary_Message_Box(SaveFileName, MB_OK);
		}
		Read_Line_InFile_To_Vector_Deque_List<vector<string>>(SaveFileName, inner_v, 0, '\n', "");
	}
	if(Load_Model == 1 || Load_Model == -1){
	SaveFileName = NE_Pattern_Folder;
	SaveFileName += "Outer\\Data\\Outer.dat";
	if(!NLPOP::FileExist(NLPOP::string2CString(SaveFileName))){
		AppCall::Secretary_Message_Box(SaveFileName, MB_OK);
	}
	char getlineBuf[MAX_SENTENCE];
	ifstream in(SaveFileName.c_str());
	if(in.bad())
			return;
		in.clear();
		in.seekg(0, ios::beg);
		while(EOF != in.peek()){
			in.getline(getlineBuf, MAX_SENTENCE,'\n');
			istringstream streamstr(getlineBuf);
			vector<string> loc_v;
			while(EOF != streamstr.peek()){
				streamstr.getline(getlineBuf, MAX_SENTENCE,' ');
				loc_v.push_back(getlineBuf);
			}
			outer_v.push_back(loc_v);
		}
		in.close();
	}
}

//------------------------For named entity name match---------------------------------
void  CRegex::Init_Head_Noun_Set(vector<NE_Surround*>& Surround_v)
{
	for(vector<NE_Surround*>::iterator nevite = Surround_v.begin(); nevite != Surround_v.end();nevite++){
		for(vector<ACE_entity_mention>::iterator hvite = (*nevite)->entity_mention_v.begin(); hvite != (*nevite)->entity_mention_v.end(); hvite++){
           Headnoun_s.insert(hvite->head.charseq);
		}
	}
}

void CRegex::Init_Regex_WordMor_MS(const char* wordsmspath)
{
	char sChar[3];
	sChar[2]=0;
	NLPOP::LoadWordData_AS_Map_string_MorphstrSet(wordsmspath, WordMor_ms);
	for(map<string, set<string>>::iterator msite = WordMor_ms.begin(); msite != WordMor_ms.end(); msite++){
		for(set<string>::iterator site = msite->second.begin(); site != msite->second.end();site++){
			if(site->find("nr2") != string::npos){
				sChar[0] = msite->first.c_str()[0];
				sChar[1] = 0;
				if(sChar[0] < 0){
					sChar[1] = msite->first.c_str()[1];
				}
				FirstName_s.insert(sChar);
			}
		}
	}
}
void CRegex::START_With_Surname(const char* strchar, bool START_Flag, vector<string>& outstr_v)
{
	char sChar[3];
	sChar[2]=0;
	sChar[0]=strchar[0];
	sChar[1]=0;	
	if(sChar[0] < 0 ){
		sChar[1]=strchar[1];
	}
	if(FirstName_s.find(sChar) != FirstName_s.end()){
		ostringstream ostrsteam;
		ostrsteam << "before_surname";
		outstr_v.push_back(ostrsteam.str());
	}
}
void CRegex::END_of_Head_Noun(const char* strchar, bool END_Flag, vector<string>& outstr_v)
{
	char sChar[3];
	sChar[2]=0;
	size_t strlength = strlen(strchar);
	string head;
	string locstr;
	vector<string> char_v;
	for(size_t i = 0; i < strlength; ){
		sChar[0]=strchar[i++];
		sChar[1]=0;	
		if(sChar[0] < 0 ){
			sChar[1]=strchar[i++];
		}
		char_v.push_back(sChar);
	}
	size_t k = 0;
	for(vector<string>::reverse_iterator rvite = char_v.rbegin();  rvite != char_v.rend(); rvite++){
		locstr = rvite->data() + head;
		if(Headnoun_s.find(head) != Headnoun_s.end()){
			head = locstr;
		}
	}
	if(0 == head.length()){
		return;
	}
	ostringstream ostrsteam;
	ostrsteam << "after_heaed";
	outstr_v.push_back(ostrsteam.str());
}

void CRegex::Possible_Left_adjacent_Max_Words_POS(const char* strchar, bool OuterFlag, vector<string>& outstr_v)
{
	char sChar[3];
	sChar[2]=0;
	size_t strlength = strlen(strchar);
	size_t k = 0;
	string tempstr;
	string maxWord;
	ostringstream ostrsteam;

	for(size_t i = 0; i < strlength; ){
		sChar[0]=strchar[i++];
		sChar[1]=0;	
		if(sChar[0] < 0 ){
			sChar[1]=strchar[i++];
		}
		tempstr += sChar;
		if(WordMor_ms.find(tempstr) != WordMor_ms.end()){
			maxWord = tempstr;
		}
	}
	if(0 == maxWord.length()){
		return;
	}
	set<string>& wmor_s = WordMor_ms[maxWord];
	for(set<string>::iterator site = wmor_s.begin(); site != wmor_s.end(); site++){
		ostrsteam.str("");
		ostrsteam << site->data() << "_L";
		outstr_v.push_back(ostrsteam.str());
	}
}
void CRegex::Possible_Right_adjacent_Max_Words_POS(const char* strchar, bool OuterFlag, vector<string>& outstr_v)
{
	char sChar[3];
	sChar[2]=0;
	size_t strlength = strlen(strchar);
	string tempstr;
	string maxWord;
	vector<string> char_v;
	for(size_t i = 0; i < strlength; ){
		sChar[0]=strchar[i++];
		sChar[1]=0;	
		if(sChar[0] < 0 ){
			sChar[1]=strchar[i++];
		}
		char_v.push_back(sChar);
	}
	size_t k = 0;
	for(vector<string>::reverse_iterator rvite = char_v.rbegin();  rvite != char_v.rend(); rvite++){
		tempstr = rvite->data() + tempstr;
		if(WordMor_ms.find(tempstr) != WordMor_ms.end()){
			maxWord = tempstr;
		}
	}
	if(0 == maxWord.length()){
		return;
	}
	set<string>& wmor_s = WordMor_ms[maxWord];

	ostringstream ostrsteam;
	for(set<string>::iterator site = wmor_s.begin(); site != wmor_s.end(); site++){
		ostrsteam.str("");
		ostrsteam << site->data() << "_R";
		outstr_v.push_back(ostrsteam.str());
	}
}
void CRegex::Adding_FreqPattern_to_Lexicon(const char* pDataFolder, set<string>& pmLexicon)
{
	Adding_FreqPattern_to_Lexicon(pDataFolder, pmLexicon, pmLexicon);
}
void CRegex::Adding_FreqPattern_to_Lexicon(const char* pDataFolder, set<string>& pmInner_s, set<string>& pmOuter_s)
{
	map<string, FREQPATTERNINFO> MulEntropy_m;
	map<string, unsigned long> su_inner_m;
	map<string, unsigned long> su_outer_m;

	string Pattern_Folder = NE_Pattern_Folder;
	Pattern_Folder += "Inner\\Workspace\\";

	if(!NLPOP::FileExist(NLPOP::string2CString(Pattern_Folder + "SubFPSent_Info\\FreqPattern.dat"))){
		AppCall::Secretary_Message_Box(PATH_WORDDB, MB_OK);
		return;
	}

	m_CFPattern.Load_FreqPattern(Pattern_Folder + "SubFPSent_Info\\FreqPattern.dat", su_inner_m);

	Pattern_Folder = NE_Pattern_Folder;
	Pattern_Folder += "Outer\\Workspace\\";
	
	m_CFPattern.Load_FreqPattern(Pattern_Folder + "SubFPSent_Info\\FreqPattern.dat", su_outer_m);

	for(map<string, unsigned long>::iterator mite = su_inner_m.begin(); mite != su_inner_m.end(); mite++){
		MulEntropy_m[mite->first];
		MulEntropy_m[mite->first].ammount = mite->second;
		if(su_outer_m.find(mite->first) != su_outer_m.end()){
			MulEntropy_m[mite->first].ammount += su_outer_m[mite->first];
			MulEntropy_m[mite->first].outer = 1.0*su_outer_m[mite->first]/MulEntropy_m[mite->first].ammount;
		}
		else{
			MulEntropy_m[mite->first].outer = 0;
		}
		MulEntropy_m[mite->first].inner = 1.0*mite->second/MulEntropy_m[mite->first].ammount;
	}

	for(map<string, unsigned long>::iterator mite = su_outer_m.begin(); mite != su_outer_m.end(); mite++){
		if(MulEntropy_m.find(mite->first) != MulEntropy_m.end()){
			continue;
		}
		MulEntropy_m[mite->first];
		MulEntropy_m[mite->first].inner = 0;
		MulEntropy_m[mite->first].outer = 1;
		MulEntropy_m[mite->first].ammount = mite->second;
	}
	multimap<double, string> d_inner_mm;
	multimap<double, string> d_outer_mm;

	for(map<string, FREQPATTERNINFO>::iterator meite = MulEntropy_m.begin(); meite != MulEntropy_m.end(); meite++){
		if(meite->second.inner > MIN_THRESHOLD_FOR_NE_PROB){
			d_inner_mm.insert(make_pair(meite->second.inner, meite->first));
			pmInner_s.insert(meite->first);
		}
		if(meite->second.outer > MIN_THRESHOLD_FOR_NE_PROB){
			d_outer_mm.insert(make_pair(meite->second.outer, meite->first));
			pmOuter_s.insert(meite->first);
		}
	}
}



