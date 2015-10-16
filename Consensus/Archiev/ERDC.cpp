#include "stdafx.h"
//#include "ERDC.h"
#include "RCCom.h"
#include "ConvertUTF.h"



CERDC::CERDC()
{	
	CaseCheck_Flag = false;
	POS_Flag = false;
	Enitity_Subtype_Flag = false;

	//string savepath = DATA_FOLDER;
	//savepath += "CRDC\\";
	//savepath += "Englishstr_POS.txt";
	//map<string, vector<string>> RelationPOS_m;
	//Read_English_Relation_Case_POS(savepath.c_str(), RelationPOS_m);
}
CERDC::~CERDC()
{
}
//-------------------------------<Generate Training Cases>
void CERDC::Generate_English_Training_Cases(const char* infopath, vector<Relation_Case>& Relation_Case_v, vector<pair<me_context_type, me_outcome_type>>& Maxentraining_v)
{
	ostringstream ostrsteam;
	RelationContext loc_Context;
	string relation_mention_str;
	string first_entity_mention_str;
	string second_entity_mention_str;
	string first_head_str;
	string second_head_str;


	size_t missing_relation_mention = 0;
	Relation_Case_v.reserve(200000);

	//ostrsteam << "Total English Relation Cases: " << Relation_Case_v.size() << endl;
	//AppCall::Secretary_Message_Box(ostrsteam.str(), MB_OK);

	string savepath = DATA_FOLDER;
	savepath += "CRDC\\";
	savepath += "Englishstr_POS.txt";
	map<string, vector<string>> RelationPOS_m;
	Read_English_Relation_Case_POS(savepath.c_str(), RelationPOS_m);

	vector<string> SentForPOS_v;
	for(vector<Relation_Case>::iterator vite = Relation_Case_v.begin(); vite != Relation_Case_v.end(); vite++){
		//if(!strcmp(vite->SUBTYPE.c_str(), NEGETIVE) || !strcmp(vite->TYPE.c_str(), NEGETIVE)){
		//	continue;
		//}
		if(CaseCheck_Flag){
			if(!ace_op::Check_Relation_Cases(*vite)){
				missing_relation_mention++;
				continue;
			}
		}
		map<string, unsigned long> WordsCnt_map;
		me_context_type loc_RCase_v;
		//-----dual types constriant;
		if(POS_Flag){
			ostrsteam.str("");
			ostrsteam << "_" << vite->first_entity.Entity_TYPE << "_" << vite->sencond_entity.Entity_TYPE << "_";
			loc_RCase_v.push_back(make_pair(ostrsteam.str(), (float)1.0));

			Sentop::Extract_ACE_entities_Pair_Context(*vite, loc_Context);
			Delet_0AH_in_string(loc_Context);
			//SentForPOS_v.push_back(loc_Context.L_str);
			//SentForPOS_v.push_back(loc_Context.M_str);
			//SentForPOS_v.push_back(loc_Context.R_str);
			//continue;
			if(loc_Context.L_str.length() != 0){
				if(RelationPOS_m.find(loc_Context.L_str.c_str()) == RelationPOS_m.end()){
					AppCall::Secretary_Message_Box("Failed to get POS Tag", MB_OK);
					missing_relation_mention++;
					continue;
				}
				else if(!RelationPOS_m[loc_Context.L_str].empty()){
					WordsCnt_map.insert(make_pair(RelationPOS_m[loc_Context.L_str][RelationPOS_m[loc_Context.L_str].size()-1] + "__l", 1));
				}
				else{
					//AppCall::Secretary_Message_Box("Failed to get POS Tag", MB_OK);
				}
			}
			if(loc_Context.M_str.length() != 0){
				if(RelationPOS_m.find(loc_Context.M_str.c_str()) == RelationPOS_m.end()){
					AppCall::Secretary_Message_Box("Failed to get POS Tag", MB_OK);
					missing_relation_mention++;
					continue;
				}
				else if(!RelationPOS_m[loc_Context.M_str].empty()){
					WordsCnt_map.insert(make_pair("m__" + RelationPOS_m[loc_Context.M_str][0], 1));
					WordsCnt_map.insert(make_pair(RelationPOS_m[loc_Context.M_str][RelationPOS_m[loc_Context.M_str].size()-1] + "__m", 1));
				}
				else{
					//AppCall::Secretary_Message_Box("Failed to get POS Tag", MB_OK);
				}
			}
			if(loc_Context.R_str.length() != 0){
				if(RelationPOS_m.find(loc_Context.R_str.c_str()) == RelationPOS_m.end()){
					AppCall::Secretary_Message_Box("Failed to get POS Tag", MB_OK);
					missing_relation_mention++;
					continue;
				}
				else if(!RelationPOS_m[loc_Context.R_str].empty()){
					WordsCnt_map.insert(make_pair("r__" + RelationPOS_m[loc_Context.R_str][0], 1));
				}
				else{
					//AppCall::Secretary_Message_Box("Failed to get POS Tag", MB_OK);
				}
			}
			//RCCOM::Adjacent_Words_POS_Feature_Extracting(m_CSegmter, loc_Context.L_str.c_str(), WordsCnt_map, "", "__l");
			//RCCOM::Adjacent_Words_POS_Feature_Extracting(m_CSegmter, loc_Context.M_str.c_str(), WordsCnt_map, "m__", "__m");
			//RCCOM::Adjacent_Words_POS_Feature_Extracting(m_CSegmter, loc_Context.R_str.c_str(), WordsCnt_map, "r__", "");
		}
		
		for(map<string, unsigned long>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
			loc_RCase_v.push_back(make_pair(mite->first, (float)1.0));
		}

		Maxentraining_v.push_back(make_pair(loc_RCase_v, TYPE_Flag?vite->TYPE:vite->SUBTYPE));
	}
	Relation_Case_v.clear();

	if(CaseCheck_Flag){
		ostrsteam.str("");
		ostrsteam << "Missing relation mention: " << missing_relation_mention;
		AppCall::Secretary_Message_Box(ostrsteam.str(), MB_OK);
	}
	//savepath = DATA_FOLDER;
	//savepath += "CRDC\\";
	//savepath += "Englishstr.txt";
	//Write_Vector_Deque_List_To_File<vector<string>>(savepath.c_str(), SentForPOS_v, 0, '\n', "");
}


void CERDC::Read_English_Relation_Case_POS(const char* cinputpath, map<string, vector<string>>& RelationPOS_m)
{
	char SentenceBuf[MAX_SENTENCE];
	vector<string> POSLine_v;
	//string sentstr;
	
	Read_Line_InFile_To_Vector_Deque_List<vector<string>>(cinputpath, POSLine_v, 0, '\n', false);

	for(vector<string>::iterator vite = POSLine_v.begin(); vite != POSLine_v.end(); vite++){
		istringstream sstream(vite->data());
		sstream.getline(SentenceBuf, MAX_SENTENCE, '\t');
		if(RelationPOS_m.find(SentenceBuf) != RelationPOS_m.end()){
			continue;
		}
		vector<string>& refsent_v = RelationPOS_m[SentenceBuf];

		while(sstream.peek() != EOF){
			sstream.getline(SentenceBuf, MAX_SENTENCE, '\t'); //get word
			sstream.getline(SentenceBuf, MAX_SENTENCE, '\t'); //get POS
			if(strlen(SentenceBuf) == 0){
				continue;
			}
			refsent_v.push_back(SentenceBuf);
		}
	}
}

void CERDC::Generate_Positive_And_Negetive_ACE_Relation_Cases(ACE_Corpus& m_ACE_Corpus, vector<Relation_Case>& Relation_Case_v)
{	
	list<Relation_Case> totall_Case_l;

	//Generate_Negative_Training_Case_Using_Antity(m_ACE_Corpus, totall_Case_l);

	Adding_Positive_Training_Case_in_Relation_Mention(m_ACE_Corpus, totall_Case_l);

	for(list<Relation_Case>::iterator lite = totall_Case_l.begin(); lite != totall_Case_l.end(); ){
		Relation_Case_v.push_back(*lite);
		lite = totall_Case_l.erase(lite);
	}
}
void CERDC::Generate_Negative_Training_Case_Using_Antity(ACE_Corpus& m_ACE_Corpus, list<Relation_Case>& Relation_Case_l)
{
	map<string, ACE_entity>& ACE_Entity_Info_map = m_ACE_Corpus.ACE_Entity_Info_map;
	deque<ACE_relation>& ACE_Relation_Info_d = m_ACE_Corpus.ACE_Relation_Info_d;
	map<string, ACE_sgm>& ACE_sgm_mmap = m_ACE_Corpus.ACE_sgm_mmap;
	
	Extract_Sentence_with_Two_Named_Entities(m_ACE_Corpus, Relation_Case_l);

	Delete_Sentence_Contianing_Positive_Relation_Case(Relation_Case_l, ACE_Relation_Info_d);

}
//-----------------------------------Extract Sentence with Two Named Entities
void CERDC::Extract_Sentence_with_Two_Named_Entities(ACE_Corpus& ACE_Corpus, list<Relation_Case>& Relation_Case_l)
{
	map<string, ACE_entity>& ACE_Entity_Info_map = ACE_Corpus.ACE_Entity_Info_map;
	map<string, ACE_sgm>& ACE_sgm_mmap = ACE_Corpus.ACE_sgm_mmap;
	map<string, list<pair<string, pair<size_t,size_t>>>> ACE_DocSentence_map;//<DOCID, list_pair: <Sentence, (START, ENd);>
	map<string, vector<pair<ACE_extent, ACE_entity_mention*>>> EntityMentionInfo_map;//Save each entity mention info in doc.
	//------------------------------------------------------------------------
	ace_op::Segment_ACE_English_sgm_to_Sentence(ACE_sgm_mmap, ACE_DocSentence_map);
	//------------------------------------------------------------------------
	//Extract each entity mention info in doc.
	for(map<string, ACE_entity>::iterator mite = ACE_Entity_Info_map.begin(); mite != ACE_Entity_Info_map.end(); mite++){
		for(vector<ACE_entity_mention>::iterator vite = mite->second.entity_mention_v.begin(); vite != mite->second.entity_mention_v.end(); vite++){
			if(EntityMentionInfo_map.find(vite->DOCID) == EntityMentionInfo_map.end()){
				EntityMentionInfo_map[vite->DOCID];
			}
			EntityMentionInfo_map[vite->DOCID].push_back(make_pair(vite->extent, &(*vite)));
		}
	}
	//------------------------------------------------------------------------
	//<DOCID, list_pair: <Sentence, <START, ENd> >
	ACE_entity_mention* First_Enitity;
	pair<size_t, size_t> FirstMentionExtent;
	
	for(map<string, list<pair<string, pair<size_t,size_t>>>>::iterator mite = ACE_DocSentence_map.begin(); mite != ACE_DocSentence_map.end(); mite++){

		for(list<pair<string, pair<size_t,size_t>>>::iterator sgmlite = mite->second.begin(); sgmlite != mite->second.end(); sgmlite++, SENID++){
			vector<pair<ACE_extent, ACE_entity_mention*>>& entityref_v = EntityMentionInfo_map[mite->first];
			for(vector<pair<ACE_extent, ACE_entity_mention*>>::iterator fvist = entityref_v.begin(); fvist != entityref_v.end(); fvist++){
				//Check wether the entity set is accordance with original doc;
				if(strcmp(fvist->first.charseq.c_str(), Sentop::Get_Substr_by_Chinese_Character_Cnt(ACE_sgm_mmap[mite->first].DOC.c_str(), fvist->first.START, fvist->first.END + 1 - fvist->first.START).c_str())){
					ostringstream ostream;
					ostream << "文档：" << mite->first << endl << "位置：" << fvist->first.START<< endl<< "命名实体：" << fvist->first.charseq << endl;
					ostream << "文档字符：" << Sentop::Get_Substr_by_Chinese_Character_Cnt(ACE_sgm_mmap[mite->first].DOC.c_str(), fvist->first.START, fvist->first.END + 1 - fvist->first.START) << endl;
					if(IDYES != AppCall::Secretary_Message_Box(ostream.str(), MB_YESNOCANCEL)){
						return;
					}
					continue;
				}
				if((sgmlite->second.first  <= fvist->first.START) && (sgmlite->second.second >= fvist->first.END)){
					First_Enitity = fvist->second;
					FirstMentionExtent.first = fvist->first.START;
					FirstMentionExtent.second = fvist->first.END;
					vector<pair<ACE_extent, ACE_entity_mention*>>::iterator svist = entityref_v.begin();
					
					for(; svist != entityref_v.end(); svist++){
						if((sgmlite->second.first <= svist->first.START) && (sgmlite->second.second >= svist->first.END)){ 
							if((FirstMentionExtent.first != svist->first.START) || (FirstMentionExtent.second != svist->first.END)){

								Relation_Case loc_Relation_Case;
								loc_Relation_Case.SENID = SENID;
								loc_Relation_Case.DOCID = mite->first;
								loc_Relation_Case.TYPE = NEGETIVE;
								loc_Relation_Case.SUBTYPE = NEGETIVE;
								loc_Relation_Case.relatin_mention.DOCID = mite->first;
								loc_Relation_Case.relatin_mention.ID = "";
								loc_Relation_Case.relatin_mention.LEXICALCONDITION = "";
								loc_Relation_Case.relatin_mention.extent.charseq = sgmlite->first;
								loc_Relation_Case.relatin_mention.extent.START = sgmlite->second.first;
								loc_Relation_Case.relatin_mention.extent.END = sgmlite->second.second;
								
								loc_Relation_Case.first_entity = *First_Enitity;
								loc_Relation_Case.sencond_entity = *(svist->second);
								
								Relation_Case_l.push_back(loc_Relation_Case);
							}
						}
					}//svist
				}
			}//fvist
		}//for(list<pair<
	}//for(map<string,
}
void CERDC::Delete_Sentence_Contianing_Positive_Relation_Case(list<Relation_Case>& Relation_Case_l, deque<ACE_relation>& ACE_Relation_Info_d)
{
	bool Erase_Flag;
	for(list<Relation_Case>::iterator lite = Relation_Case_l.begin(); lite != Relation_Case_l.end(); ){
		Erase_Flag = false;
		for(deque<ACE_relation>::iterator dite = ACE_Relation_Info_d.begin(); dite != ACE_Relation_Info_d.end(); dite++){

			for(vector<ACE_relation_mention>::iterator vite =  dite->relation_mention_v.begin(); vite !=  dite->relation_mention_v.end(); vite++){
				if(vite->ID.find(lite->DOCID) == -1){
					continue;
				}
				ACE_extent Arg_1, Arg_2;
				for(multimap<string, ACE_relation_mention_arg>::iterator mmite = vite->arg_mention_mmap.begin(); mmite != vite->arg_mention_mmap.end(); mmite++){
					if(!strcmp(mmite->second.ROLE.c_str(), "Arg-1")){
						Arg_1 = mmite->second.extent;
					}
					if(!strcmp(mmite->second.ROLE.c_str(), "Arg-2")){
						Arg_2 = mmite->second.extent;
					}
				}
				if((Arg_1.START == lite->first_entity.extent.START) && (Arg_1.END == lite->first_entity.extent.END) && 
					(Arg_2.START == lite->sencond_entity.extent.START) && (Arg_2.END == lite->sencond_entity.extent.END)){
					lite = Relation_Case_l.erase(lite);
					Erase_Flag = true;
					break;
				}
			}
			if(Erase_Flag){
				break;
			}
		}
		if(!Erase_Flag){
			lite++;
		}
	}
}

void CERDC::Adding_Positive_Training_Case_in_Relation_Mention(ACE_Corpus& m_ACE_Corpus, list<Relation_Case>& Relation_Case_l)
{
	deque<ACE_relation>& ACE_Relation_Info_d = m_ACE_Corpus.ACE_Relation_Info_d;
	map<string, ACE_entity>& ACE_Entity_Info_map = m_ACE_Corpus.ACE_Entity_Info_map;

	map<string, ACE_entity_mention> EntityMention_map;
	RCCOM::Get_Entity_Mention_extent_Map(ACE_Entity_Info_map, EntityMention_map);
	size_t SENID = 0;
	for(size_t i = 0; i < ACE_Relation_Info_d.size(); i++){
		for(vector<ACE_relation_mention>::iterator rmvite = ACE_Relation_Info_d[i].relation_mention_v.begin();  rmvite != ACE_Relation_Info_d[i].relation_mention_v.end(); rmvite++){
			Relation_Case loc_case;
			loc_case.SENID = SENID++;
			loc_case.DOCID = rmvite->DOCID;
			loc_case.TYPE = ACE_Relation_Info_d[i].TYPE;
			loc_case.SUBTYPE = ACE_Relation_Info_d[i].SUBTYPE;
			loc_case.relatin_mention = *rmvite;
			for(multimap<string, ACE_relation_mention_arg>::iterator emvite = rmvite->arg_mention_mmap.begin(); emvite != rmvite->arg_mention_mmap.end(); emvite++){
				if(!strcmp("Arg-1", emvite->first.c_str())){
					if(EntityMention_map.find(emvite->second.REFID) != EntityMention_map.end()){
						loc_case.first_entity = EntityMention_map[emvite->second.REFID];
					}
				}
				if(!strcmp("Arg-2", emvite->first.c_str())){
					if(EntityMention_map.find(emvite->second.REFID) != EntityMention_map.end()){
						loc_case.sencond_entity = EntityMention_map[emvite->second.REFID];
					}
				}
			}
			Relation_Case_l.push_back(loc_case);
		}
	}
}



void CERDC::Delet_0AH_in_string(RelationContext& pm_context)
{
	Delet_0AH_in_string(pm_context.L_str);
	Delet_0AH_in_string(pm_context.M_str);
	Delet_0AH_in_string(pm_context.R_str);
}

void CERDC::Delet_0AH_in_string(string &pmstr)
{
	char sentencechar[MAX_SENTENCE];
	char sChar[3];
	sChar[2]=0;

	strcpy_s(sentencechar, MAX_SENTENCE, "");
	for(size_t Cnt = 0; Cnt < pmstr.length(); ){
		sChar[0] = pmstr.c_str()[Cnt++];
		sChar[1] = 0;	
		if(sChar[0] < 0 ){
			sChar[1]=pmstr.c_str()[Cnt++];
		}
		if(!strcmp(sChar, "\n")){
			continue;
		}
		if(!strcmp(sChar, "\t")){
			continue;
		}
		else{
			strcat_s(sentencechar, MAX_SENTENCE, sChar);
		}
	}
	pmstr = sentencechar;

}

void CERDC::For_English_Segmentation()
{
	string pathstr = "F:\\testset\\";
	vector<string> FilePath_v;
	vector<string> FileClass_v;
	NLPOP::Get_Specified_Files(pathstr, FilePath_v, "");
	set<string> fileclass_s;
	set<string> term_types_s;
	map<unsigned long, map<unsigned long, unsigned long>> WordsFreq_mm;
	map<string, unsigned long> Term2ID_map;
	unsigned long iterater = 0;

	vector<Relation_Case> Relation_Case_v;
//	Initiate_Words_set(Relation_Case_v);

	for(unsigned int i = 0; i < FilePath_v.size(); i++)
	{
		string fileclass = FilePath_v[i].substr(0, FilePath_v[i].rfind("\\"));
		fileclass = fileclass.substr(fileclass.rfind("\\")+1, fileclass.length());
		FileClass_v.push_back(fileclass);
		fileclass_s.insert(fileclass);
		WordsFreq_mm[i];

		deque<string> TotalLine_d;
		string inputstr;

		Read_Line_InFile_To_Vector_Deque_List<deque<string>>(FilePath_v[i], TotalLine_d, 0, '\n', false);
		
		string contentstr = "";
		for(unsigned int j = 0; j < TotalLine_d.size(); j++)
		{
			wstring wstr;
			wstr = SCONVERT::FromUtf8(TotalLine_d[j]);
			char* pchar = SCONVERT::narrow(wstr.c_str());
			string str(pchar);

			contentstr+= str;
		}
		TotalLine_d.clear();

		map<string, unsigned long> WordsCnt_map;
		vector<pair<string, unsigned long>> loc_RCase_v;
//		m_CSegmter.Omni_words_feature_Extracting(contentstr.c_str(), m_WordsSet, WordsCnt_map, "", "");
		
		for(map<string, unsigned long>::iterator mite = WordsCnt_map.begin(); mite != WordsCnt_map.end(); mite++){
			if(mite->first.find(';') != string::npos){
				continue;
			}
			loc_RCase_v.push_back(make_pair(mite->first, mite->second));
		}

//		Training_map.insert(make_pair(fileclass, loc_RCase_v));
	}
//	RCCOM::Output_Extract_ACE_Relation_Words_Context_For_Maxent("F:\\Training.txt", Training_map);

	MAXEN::Generate_Training_Matrix_3_Gram("F:\\Training.txt", "F:\\Matrix.txt", "F:\\Class.txt");
}
