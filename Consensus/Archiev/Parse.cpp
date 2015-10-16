#include "stdafx.h"
#include "Parse.h"
#include "ConvertUTF.h"
#include "Charniak\\ECArgs.h"
#include "Charniak\\Term.h"
#include "Charniak\\headFinder.h"
#include "Charniak\\UnitRules.h"

#define MAX_PARATER 32
char* argv[MAX_PARATER];

CParse::CParse(CSegmter& ref_Segmter): m_CSegmter(ref_Segmter)
{	

	//Read_Parsed_Tree("E:\\JavaSpace\\Data\\TreeBank.txt");
	//Read_Parsed_Tree("E:\\JavaSpace\\Data\\output\\Parsed_File.txt");
}
CParse::~CParse()
{
}

void CParse::Read_Parsed_Tree(const char* c_treepath)
{
	vector<string> parameter_v;
	parameter_v.push_back("pSgT");
	parameter_v.push_back("-LCh");
	parameter_v.push_back("F:\\YPench\\Charniak\\DATA\\CH\\");

	for(size_t i = 0; i < MAX_PARATER; i++){
		argv[i] = new char[256];
	}
	for(size_t i = 0; i < parameter_v.size(); i++){
		strcpy_s(argv[i], 256, parameter_v[i].c_str());
	}

	ECArgs args( parameter_v.size(), argv );
	//assert(args.nargs() == 1);
	ECString path(args.arg(0));
	cerr << "At start of pHsgt" << endl;

	for(int n = 0 ; n < MAXNUMNTS ; n++){
		numTerm[n] = 0;
	}
	Term::init( path );
	if(args.isset('L')){
		Term::Language = args.value('L');
	}
	readHeadInfo(path);

	int sentenceCount = 0;
	ECString s1lex("^^");
	ECString s1nm("S1");
	int s1Int = Term::get(s1nm)->toInt();
	UnitRules ur;
	ur.init();

	ifstream in(c_treepath);
	istream& scin(in);
	while(scin){
		if(sentenceCount%10000 == 0){
			cerr << sentenceCount << endl;
		}
		InputTree  parse;
		scin >> parse;
		if(!scin){
			break;
		}
		int len = parse.length();
		if(len == 0){
			break;
		}
		else if(len == -1){
			continue;
		}
		pParse_v.push_back(&parse);
    }
	for(size_t i = 0; i < MAX_PARATER; i++){
		delete argv[i];
	}
	Term::Delete_Terms();
}

void CParse::Generate_Segmented_Relation_Mention_Case(const char* c_relaitoncasepath)
{
	vector<Relation_Case> Relation_Case_v;
	ace_op::Load_Relation_Case(c_relaitoncasepath, Relation_Case_v);

	

	map<size_t,vector<ACE_extent>> SegRelation_map;
	map<string, vector<pair<ACE_extent, ACE_entity_mention*>>> EntityMentionInfo_map;

	//Generate entity mention map;
	for(vector<Relation_Case>::iterator rvite = Relation_Case_v.begin(); rvite != Relation_Case_v.end(); rvite++){
		if(EntityMentionInfo_map.find(rvite->first_entity.DOCID) == EntityMentionInfo_map.end()){
			EntityMentionInfo_map[rvite->first_entity.DOCID];
		}
		else{
			EntityMentionInfo_map[rvite->first_entity.DOCID].push_back(make_pair(rvite->first_entity.extent, &rvite->first_entity));
		}
		if(EntityMentionInfo_map.find(rvite->sencond_entity.DOCID) == EntityMentionInfo_map.end()){
			EntityMentionInfo_map[rvite->sencond_entity.DOCID];
		}
		else{
			EntityMentionInfo_map[rvite->sencond_entity.DOCID].push_back(make_pair(rvite->sencond_entity.extent, &rvite->sencond_entity));
		}
	}
	
	//Collect relation mention enity extend
	for(vector<Relation_Case>::iterator rvite = Relation_Case_v.begin(); rvite != Relation_Case_v.end(); rvite++){
		if(SegRelation_map.find(rvite->SENID) != SegRelation_map.end()){
			continue;
		}
		SegRelation_map[rvite->SENID];
		for(vector<pair<ACE_extent, ACE_entity_mention*>>::iterator vite = EntityMentionInfo_map[rvite->DOCID].begin(); vite != EntityMentionInfo_map[rvite->DOCID].end(); vite++){
			if((rvite->relatin_mention.extent.START <= vite->first.START) && (rvite->relatin_mention.extent.END >=  vite->first.END)){
				bool Unique_Flag = true;
				for(vector<ACE_extent>::iterator evite = SegRelation_map[rvite->SENID].begin();  evite != SegRelation_map[rvite->SENID].end(); evite++){
					if((evite->START == vite->first.START) && (evite->END == vite->first.END)){
						Unique_Flag = false;
						break;
					}
				}
				if(Unique_Flag){
					SegRelation_map[rvite->SENID].push_back(vite->first);
				}
			}
		}
	}

	//-------------------------------------------------------------------------------
	size_t currentposit;
	string currentstr;
	char sChar[3];
	sChar[2]=0;
	//bool FirstCutpoint_Flag;
	//bool SecondCutpoint_Flag;
	//size_t FirstCutpoint;
	//size_t SecondCutpoint;
	size_t LastCutpoint;
	bool Entity_Mention_Flag;
	CLAUSEPOS loc_Seg;
	
	//map<size_t, vector<size_t>> SegRelationCutpoint_map;
	map<size_t,vector<string>> SegRelationstr_map;
	for(vector<Relation_Case>::iterator rvite = Relation_Case_v.begin(); rvite != Relation_Case_v.end(); rvite++){
		//if(SegRelationCutpoint_map.find(rvite->SENID) != SegRelationCutpoint_map.end()){
		//	continue;
		//}
		//SegRelationCutpoint_map[rvite->SENID];
		if(SegRelationstr_map.find(rvite->SENID) != SegRelationstr_map.end()){
			continue;
		}
		SegRelationstr_map[rvite->SENID];
		//FirstCutpoint_Flag = false;
		//SecondCutpoint_Flag = false;
		ACE_extent& refrmextent = rvite->relatin_mention.extent;
		set<size_t> cutpoint_s;
		for(vector<ACE_extent>::iterator evite = SegRelation_map[rvite->SENID].begin();  evite != SegRelation_map[rvite->SENID].end(); evite++){
			cutpoint_s.insert(evite->START);
			cutpoint_s.insert(evite->END+1);
		}
		//for(set<size_t>::iterator site = cutpoint_s.begin(); site != cutpoint_s.end(); site++){
		//	SegRelationCutpoint_map[rvite->SENID].push_back(*site);
		//}
		currentposit = refrmextent.START;
		LastCutpoint = refrmextent.START;
		currentstr = "";
		for(size_t i = 0; i < refrmextent.charseq.length(); ){
			if(cutpoint_s.find(currentposit) != cutpoint_s.end()){
				if(currentstr.length() != 0){
					Entity_Mention_Flag = false;
						//SecondCutpoint = currentposit;
					for(vector<ACE_extent>::iterator evite = SegRelation_map[rvite->SENID].begin();  evite != SegRelation_map[rvite->SENID].end(); evite++){
						if((LastCutpoint == evite->START) && (currentposit-1 == evite->END)){
							ace_op::Delet_0AH_and_20H_in_string(currentstr);
							SegRelationstr_map[rvite->SENID].push_back(currentstr);
							Entity_Mention_Flag = true;
							break;
						}
					}
					if(!Entity_Mention_Flag){
						ace_op::Delet_0AH_and_20H_in_string(currentstr);
						m_CSegmter.ICTCLAS_Segmention_Port(currentstr.c_str(), loc_Seg);
						for(vector<string>::iterator vite = loc_Seg.Clause.begin(); vite != loc_Seg.Clause.end(); vite++){
							SegRelationstr_map[rvite->SENID].push_back(vite->data());
						}
						loc_Seg.Clause.clear();
						loc_Seg.POS.clear();	
					}
					currentstr = "";
					LastCutpoint = currentposit;
				}
			}
			sChar[0] = refrmextent.charseq.c_str()[i++];
			sChar[1] = 0;
			if(sChar[0] < 0 ){
				sChar[1] = refrmextent.charseq.c_str()[i++];
			}
			currentstr += sChar;
			currentposit++;
		}
		if(currentstr.length() != 0){
			Entity_Mention_Flag = false;
				//SecondCutpoint = currentposit;
			for(vector<ACE_extent>::iterator evite = SegRelation_map[rvite->SENID].begin();  evite != SegRelation_map[rvite->SENID].end(); evite++){
				if((LastCutpoint == evite->START) && (currentposit-1 == evite->END)){
					ace_op::Delet_0AH_and_20H_in_string(currentstr);
					SegRelationstr_map[rvite->SENID].push_back(currentstr);
					Entity_Mention_Flag = true;
					break;
				}
			}
			if(!Entity_Mention_Flag){
				ace_op::Delet_0AH_and_20H_in_string(currentstr);
				m_CSegmter.ICTCLAS_Segmention_Port(currentstr.c_str(), loc_Seg);
				for(vector<string>::iterator vite = loc_Seg.Clause.begin(); vite != loc_Seg.Clause.end(); vite++){
					SegRelationstr_map[rvite->SENID].push_back(vite->data());
				}
				loc_Seg.Clause.clear();
				loc_Seg.POS.clear();	
			}
			currentstr = "";
			LastCutpoint = currentposit;
		}
		vector<string>& refviewseg = SegRelationstr_map[rvite->SENID];
	}

	string savapath = DATA_FOLDER;
	savapath += "CRDC\\";
	savapath += "parsefile.txt";
	Output_For_Charniak_PARSE(savapath.c_str(), SegRelationstr_map);
}

void CParse::Output_For_Charniak_PARSE(const char* csavepath, map<size_t,vector<string>>& SegRelationstr_map)
{
	//wofstream out;
	//locale oldLocale1 = out.imbue(locale(std::locale("chs"), "", LC_CTYPE));
	//out.open(csavepath);
	ofstream out(csavepath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	
	for(map<size_t,vector<string>>::iterator mvite = SegRelationstr_map.begin(); mvite != SegRelationstr_map.end(); mvite++){
		out << "<s> ";
		for(vector<string>::iterator vite = mvite->second.begin(); vite != mvite->second.end(); vite++){
			out << vite->data() << " ";
		}
		out << "</s>" << endl;
	}
	out.close();

	//out.imbue(oldLocale1);
}

void CParse::Generate_Parsing_Sentence(const char* parspath, vector<Relation_Case>& Relation_Case_v)
{
	CLAUSEPOS loc_Seg;
	pACE_extent pFrontExtent, pBackExtent;
	RelationContext loc_Context;
	ofstream out(parspath);
	if(out.bad())
		return;
	out.clear();
	out.seekp(0, ios::beg);
	for(vector<Relation_Case>::iterator rvite = Relation_Case_v.begin(); rvite != Relation_Case_v.end(); rvite++){
		Sentop::Extract_ACE_entities_Pair_Context(*rvite, loc_Context);
		ace_op::Delet_0AH_and_20H_in_string(loc_Context);
		if(rvite->first_entity.extent.START <= rvite->sencond_entity.extent.START){
			pFrontExtent = &(rvite->first_entity.extent);
			pBackExtent = &(rvite->sencond_entity.extent);
		}
		else{
			pFrontExtent = &(rvite->sencond_entity.extent);
			pBackExtent = &(rvite->first_entity.extent);
		}
		if(0 != loc_Context.L_str.length()){
			loc_Seg.Clause.clear();
			loc_Seg.POS.clear();
			m_CSegmter.ICTCLAS_Segmention_Port(loc_Context.L_str.c_str(), loc_Seg);
			for(vector<string>::iterator vite = loc_Seg.Clause.begin(); vite != loc_Seg.Clause.end(); vite++){
				out << vite->data() << ' ';
			}
		}
		if(pFrontExtent->END <= pBackExtent->START){
			out << ace_op::Delet_0AH_and_20H_in_string(pFrontExtent->charseq.c_str()) << ' ';
			if(0 != loc_Context.M_str.length()){
				loc_Seg.Clause.clear();
				loc_Seg.POS.clear();
				m_CSegmter.ICTCLAS_Segmention_Port(loc_Context.M_str.c_str(), loc_Seg);
				for(vector<string>::iterator vite = loc_Seg.Clause.begin(); vite != loc_Seg.Clause.end(); vite++){
					out << vite->data() << ' ';
				}
			}
			out << ace_op::Delet_0AH_and_20H_in_string(pBackExtent->charseq.c_str()) << ' ';
		}
		else if(pFrontExtent->END <= pBackExtent->END){
			out << ace_op::Delet_0AH_and_20H_in_string(pFrontExtent->charseq.c_str()) << ' ';
			string locstr = Sentop::Get_Substr_by_Chinese_Character_Cnt_With_RelPosit(rvite->relatin_mention.extent.charseq.c_str(), rvite->relatin_mention.extent.START, pFrontExtent->END, pBackExtent->END-pFrontExtent->END+1);
			out <<  ace_op::Delet_0AH_and_20H_in_string(locstr.c_str()) << ' ';
		}
		else if(pFrontExtent->END > pBackExtent->END){
			out << ace_op::Delet_0AH_and_20H_in_string(pBackExtent->charseq.c_str()) << ' ';
			string locstr = Sentop::Get_Substr_by_Chinese_Character_Cnt_With_RelPosit(rvite->relatin_mention.extent.charseq.c_str(), rvite->relatin_mention.extent.START, pBackExtent->END, pFrontExtent->END-pBackExtent->END+1);
			out <<  ace_op::Delet_0AH_and_20H_in_string(locstr.c_str()) << ' ';
		}
		else if(true){
			ostringstream ostream;
			ostream << "First Entity: " << pFrontExtent->charseq << "; Second Entity: " << pBackExtent->charseq;
			AppCall::Secretary_Message_Box(ostream.str(), MB_OK);
		}
		if(0 != loc_Context.R_str.length()){
			loc_Seg.Clause.clear();
			loc_Seg.POS.clear();
			m_CSegmter.ICTCLAS_Segmention_Port(loc_Context.R_str.c_str(), loc_Seg);
			for(vector<string>::iterator vite = loc_Seg.Clause.begin(); vite != loc_Seg.Clause.end(); vite++){
				out << vite->data() << ' ';
			}
		}
		out << endl;
	}

	
	out.close();


}



