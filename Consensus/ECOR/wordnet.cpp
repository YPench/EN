#include "StdAfx.h"
#include "wordnet.h"

static struct {
    char *option;		/* user's search request */
    int search;			/* search to pass findtheinfo() */
    int pos;			/* part-of-speech to pass findtheinfo() */
    int helpmsgidx;		/* index into help message table */
    char *label;		/* text for search header message */
} *optptr, optlist[] = {
    { "-synsa", SIMPTR,	ADJ, 0, "Similarity" },
    { "-antsa", ANTPTR,	ADJ, 1, "Antonyms" },
    { "-perta", PERTPTR, ADJ, 0, "Pertainyms" },
    { "-attra", ATTRIBUTE, ADJ, 2, "Attributes" },
    { "-domna", CLASSIFICATION, ADJ, 3, "Domain" },
    { "-domta", CLASS, ADJ, 4, "Domain Terms" },
    { "-famla", FREQ, ADJ, 5, "Familiarity" },
    { "-grepa", WNGREP, ADJ, 6, "Grep" },

    { "-synsn", HYPERPTR, NOUN, 0, "Synonyms/Hypernyms (Ordered by Estimated Frequency)" },
    { "-antsn", ANTPTR,	NOUN, 2, "Antonyms" },
    { "-coorn", COORDS, NOUN, 3, "Coordinate Terms (sisters)" },
    { "-hypen", -HYPERPTR, NOUN, 4, "Synonyms/Hypernyms (Ordered by Estimated Frequency)" },
    { "-hypon", HYPOPTR, NOUN, 5, "Hyponyms" },
    { "-treen", -HYPOPTR, NOUN, 6, "Hyponyms" },
    { "-holon", HOLONYM, NOUN, 7, "Holonyms" },
    { "-sprtn", ISPARTPTR, NOUN, 7, "Part Holonyms" },
    { "-smemn", ISMEMBERPTR, NOUN, 7, "Member Holonyms" },
    { "-ssubn", ISSTUFFPTR, NOUN, 7, "Substance Holonyms" },
    { "-hholn",	-HHOLONYM, NOUN, 8, "Holonyms" },
    { "-meron", MERONYM, NOUN, 9, "Meronyms" },
    { "-subsn", HASSTUFFPTR, NOUN, 9, "Substance Meronyms" },
    { "-partn", HASPARTPTR, NOUN, 9, "Part Meronyms" },
    { "-membn", HASMEMBERPTR, NOUN, 9, "Member Meronyms" },
    { "-hmern", -HMERONYM, NOUN, 10, "Meronyms" },
    { "-nomnn", DERIVATION, NOUN, 11, "Derived Forms" },
    { "-derin", DERIVATION, NOUN, 11, "Derived Forms" },
    { "-domnn", CLASSIFICATION, NOUN, 13, "Domain" },
    { "-domtn", CLASS, NOUN, 14, "Domain Terms" },
    { "-attrn", ATTRIBUTE, NOUN, 12, "Attributes" },
    { "-famln", FREQ, NOUN, 15, "Familiarity" },
    { "-grepn", WNGREP, NOUN, 16, "Grep" },

    { "-synsv", HYPERPTR, VERB, 0, "Synonyms/Hypernyms (Ordered by Estimated Frequency)" },
    { "-simsv", RELATIVES, VERB, 1, "Synonyms (Grouped by Similarity of Meaning)" },
    { "-antsv", ANTPTR, VERB, 2, "Antonyms" },
    { "-coorv", COORDS, VERB, 3, "Coordinate Terms (sisters)" },
    { "-hypev", -HYPERPTR, VERB, 4, "Synonyms/Hypernyms (Ordered by Estimated Frequency)" },
    { "-hypov", HYPOPTR, VERB, 5, "Troponyms (hyponyms)" },
    { "-treev", -HYPOPTR, VERB, 5, "Troponyms (hyponyms)" },
    { "-tropv", -HYPOPTR, VERB, 5, "Troponyms (hyponyms)" },
    { "-entav", ENTAILPTR, VERB, 6, "Entailment" },
    { "-causv", CAUSETO, VERB, 7, "\'Cause To\'" },
    { "-nomnv", DERIVATION, VERB, 8, "Derived Forms" },
    { "-deriv", DERIVATION, VERB, 8, "Derived Forms" },
    { "-domnv", CLASSIFICATION, VERB, 10, "Domain" },
    { "-domtv", CLASS, VERB, 11, "Domain Terms" },
    { "-framv", FRAMES, VERB, 9, "Sample Sentences" },
    { "-famlv", FREQ, VERB, 12, "Familiarity" },
    { "-grepv", WNGREP, VERB, 13, "Grep" },

    { "-synsr", SYNS, ADV, 0, "Synonyms" },
    { "-antsr", ANTPTR, ADV, 1, "Antonyms" },
    { "-pertr", PERTPTR, ADV, 0, "Pertainyms" },
    { "-domnr", CLASSIFICATION, ADV, 2, "Domain" },
    { "-domtr", CLASS, ADV, 3, "Domain Terms" },
    { "-famlr", FREQ, ADV, 4, "Familiarity" },
    { "-grepr", WNGREP, ADV, 5, "Grep" },

    { "-over", OVERVIEW, ALL_POS, -1, "Overview" },
    { NULL, 0, 0, 0, NULL }
};

struct {
    char *templat;		/* template for generic search message */
    char *option;		/* text for help message */
    char *helpstr;	
} searchstr[] = {		/* index by search type type */
    { NULL, NULL, NULL },
    { "-ants%c",	"-ants{n|v|a|r}",	"\t\tAntonyms", },
    { "-hype%c",	"-hype{n|v}",	"\t\tHypernyms", },
    { "-hypo%c, -tree%c",	"-hypo{n|v}, -tree{n|v}",
	  "\tHyponyms & Hyponym Tree", },
    { "-enta%c",	"-entav\t",	"\t\tVerb Entailment", },
    { "-syns%c",	"-syns{n|v|a|r}",	"\t\tSynonyms (ordered by estimated frequency)", },
    { "-smem%c",	"-smemn\t",	"\t\tMember of Holonyms", },
    { "-ssub%c",	"-ssubn\t",	"\t\tSubstance of Holonyms", },
    { "-sprt%c",	"-sprtn\t",	"\t\tPart of Holonyms", },
    { "-memb%c",	"-membn\t",	"\t\tHas Member Meronyms", },
    { "-subs%c",	"-subsn\t",	"\t\tHas Substance Meronyms", },
    { "-part%c",	"-partn\t",	"\t\tHas Part Meronyms", },
    { "-mero%c",	"-meron\t",	"\t\tAll Meronyms", },
    { "-holo%c",	"-holon\t",	"\t\tAll Holonyms", },
    { "-caus%c",	"-causv\t",	"\t\tCause to", },
    { NULL, NULL, NULL },	/* PPLPTR - no specific search */
    { NULL, NULL, NULL },	/* SEEALSOPTR - no specific search */
    { "-pert%c",	"-pert{a|r}",	"\t\tPertainyms", },
    { "-attr%c",	"-attr{n|a}",	"\t\tAttributes", },
    { NULL, NULL, NULL },	/* verb groups - no specific pointer */
    { "-deri%c",        "-deri{n|v}",   "\t\tDerived Forms",},
    { "-domn%c", "-domn{n|v|a|r}", "\t\tDomain" },
    { "-domt%c", "-domt{n|v|a|r}", "\t\tDomain Terms" }, 
    { NULL, NULL, NULL },	/* SYNS - taken care of with SIMPTR */
    { "-faml%c",	"-faml{n|v|a|r}",	"\t\tFamiliarity & Polysemy Count", },
    { "-fram%c",	"-framv\t",	"\t\tVerb Frames", },
    { "-coor%c",	"-coor{n|v}",	"\t\tCoordinate Terms (sisters)", },
    { "-sims%c",	"-simsv\t",	"\t\tSynonyms (grouped by similarity of meaning)", },
    { "-hmer%c",	"-hmern\t",	"\t\tHierarchical Meronyms", },
    { "-hhol%c", 	"-hholn\t",	"\t\tHierarchical Holonyms" },
    { "-grep%c",	"-grep{n|v|a|r}", "\t\tList of Compound Words" },
    { "-over", "-over\t", "\t\tOverview of Senses" },
};

Cwordnet::Cwordnet()
{
	//Init_WordNet();
	//do_is_defined("apple");
	//search_port("horse");
}

Cwordnet::~Cwordnet()
{


}
bool Cwordnet::IS_Prototype(const char* chword)
{
	if(strlen(chword) > 255){
		AppCall::Secretary_Message_Box("Out of Range in: Cwordnet::IS_Prototype()");
	}
	char charword[256];
	strcpy_s(charword, 256, chword);
	char* cpout = morphstr(charword, 1);
	if(cpout == NULL){
		return true;
	}
	if(strcmp(chword, cpout)){
		return false;
	}
	return true;
}

bool Cwordnet::Get_Prototype(const char* chword, char* coutput, size_t buf_size)
{
	if(strlen(chword) > 255){
		AppCall::Secretary_Message_Box("Out of Range in: Cwordnet::IS_Prototype()");
	}
	strcpy_s(coutput, buf_size, "");
	char charword[256];
	strcpy_s(charword, 256, chword);
	char* cpout = morphstr(charword, 1);
	if(cpout == NULL){
		return false;
	}
	strcpy_s(coutput, buf_size, cpout);
	if(strcmp(chword, coutput)){
		return true;
	}
	return true;
}

bool Cwordnet::Init_WordNet()
{
    if (wninit()){
		return false;
    }
	return true;
}
void Cwordnet::search_port(const char* cword, char cpos)
{

	int argc = 3;
	char *argv[32];
	for(int i = 0; i < 5; i++){
		argv[i] = new char[256];
	}
	strcpy_s(argv[0], 256, "wn");
	strcpy_s(argv[1], 256, cword);
	strcpy_s(argv[2], 256, "-s");
	if('N' == cpos){
		strcpy_s(argv[3], 256, "-synsn");
	}
	else if('V' == cpos){
		strcpy_s(argv[3], 256, "-synsv");
	}
	else{
		return;
	}
	strcpy_s(argv[4], 256, "");

    searchwn(argc, argv);

	for(int i = 0; i < 5; i++){
		delete argv[i];
	}
}

int Cwordnet::searchwn(int ac, char *av[])
{
    int i, j = 1, pos;
    int whichsense = ALLSENSES, help = 0;
    int errcount = 0, outsenses = 0;
    //char tmpbuf[256];		/* buffer for constuction error messages */
    
    if (ac == 2)		/* print available searches for word */
	exit(do_is_defined(av[1]));

    /* Parse command line options once and set flags */

    dflag = fileinfoflag = offsetflag = wnsnsflag = 0;

    for(i = 1; i < ac; i++) {
		if(!strcmp("-g",av[i]))
			dflag++;
		else if (!strcmp("-h",av[i]))
			help++;
	//	else if (!strcmp("-l", av[i]))
	//	    printlicense();
		else if (!strncmp("-n", av[i], 2) && strncmp("-nomn", av[i] ,5))
			whichsense = atoi(av[i] + 2);
		else if (!strcmp("-a", av[i]))
			fileinfoflag = 1;
		else if (!strcmp("-o", av[i]))
			offsetflag = 1;
		else if (!strcmp("-s", av[i]))
			wnsnsflag = 1;
    }

    /* Replace spaces with underscores before looking in database */

    strtolower(strsubst(av[1], ' ', '_'));

    /* Look at each option in turn.  If it's not a command line option
       (which was processed earlier), perform the search requested. */

    while(*(av[++j])) {
		if(!cmdopt(av[j])) {	/* not a command line option */
			if((i = getoptidx(av[j])) != -1) {
				optptr = &optlist[i];
				/* print help text before search output */
				if (help && optptr->helpmsgidx >= 0){
					printf("%s\n", helptext[optptr->pos][optptr->helpmsgidx]);
				}
				if (optptr->pos == ALL_POS){
					for (pos = 1; pos <= NUMPARTS; pos++){
						outsenses += do_search(av[1], pos, optptr->search, whichsense, optptr->label);
					}
				}
				else{
					outsenses += do_search(av[1], optptr->pos, optptr->search, whichsense, optptr->label);
				}
			}
			else {
				//sprintf(tmpbuf, "wn: invalid search option: %s\n", av[j]);
				//display_message(tmpbuf);
				errcount++;
			}
		}
    }
    return(errcount ? -errcount : outsenses);
}
int Cwordnet::getoptidx(char *searchtype)
{
    int i;

	for (i = 0; optlist[i].option; i++){
		if (!strcmp(optlist[i].option, searchtype)){
			return(i);
		}
	}

    return(-1);
}
int Cwordnet::cmdopt(char *str)
{
    if (!strcmp("-g", str) ||
	!strcmp("-h", str) ||
	!strcmp("-o", str) ||
	!strcmp("-l", str) ||
	!strcmp("-a", str) ||
	!strcmp("-s", str) ||
	(!strncmp("-n", str, 2) && strncmp("-nomn", str,5))){
		return (1);
	}
    else{
		return(0);
	}
}
int Cwordnet::do_search(char *searchword, int pos, int search, int whichsense, char *label)
{
    int totsenses = 0;
    char *morphword, *outbuf;

    outbuf = findtheinfo(searchword, pos, search, whichsense);
    totsenses += wnresults.printcnt;
	//if (strlen(outbuf) > 0){
	//	printf("\n%s of %s %s\n%s", label, partnames[pos], searchword, outbuf);
	//}
    if (morphword = morphstr(searchword, pos))
	do {
	    outbuf = findtheinfo(morphword, pos, search, whichsense);
	    totsenses += wnresults.printcnt;
		//if (strlen(outbuf) > 0){ 
		//	printf("\n%s of %s %s\n%s", label, partnames[pos], morphword, outbuf);
		//}
	} while (morphword = morphstr(NULL, pos));

    return(totsenses);
}

int Cwordnet::do_is_defined(char *searchword)
{
    int i, found = 0;
    unsigned int search;
    char *morphword;

    if (searchword[0] == '-') {
		//AppCall::Secretary_Message_Box("wn: invalid search word\n");
	return(-1);
    }
    
    /* Print all valid searches for word in all parts of speech */

    strtolower(strsubst(searchword, ' ', '_'));
	    
    for (i = 1; i <= NUMPARTS; i++) {
	if ((search = is_defined(searchword, i)) != 0) {
	    printsearches(searchword, i, search);
	    found = 1;
	} else{
		//AppCall::Secretary_Message_Box("\nNo information available for %s %s\n");
	}

	if ((morphword = morphstr(searchword, i)) != NULL)
	    do {
		if ((search = is_defined(morphword, i)) != 0) {
		    printsearches(morphword, i, search);
		    found = 1;
		} else{
		   // AppCall::Secretary_Message_Box("\nNo information available for %s %s\n");
		}
	    } while ((morphword = morphstr(NULL, i)) != NULL );
    }
    return(found);
}
void Cwordnet::printsearches(char *word, int dbase, unsigned long search)
{
    int j;

    printf("\nInformation available for %s %s\n", partnames[dbase], word);
    for (j = 1; j <= MAXSEARCH; j++)
	if ((search & bit(j)) && searchstr[j].option) {
	    printf("\t");
	    printf(searchstr[j].templat,
		   partchars[dbase], partchars[dbase]);
	    printf(searchstr[j].helpstr);
	    printf("\n");
	}
}

void Cwordnet::wn()
{
	/*display_message = error_message;
	int argc = 3;
	char *argv[32];
	for(int i = 0; i < argc; i++){
		argv[i] = new char[256];
	}
	strcpy_s(argv[0], 256, "wn");
	strcpy_s(argv[1], 256, "books");
	strcpy_s(argv[2], 256, "-over");
	argc--;

    if (argc < 2) {
	printusage();
	exit(-1);
    } else if (argc == 2 && !strcmp("-l", argv[1])) {
	printlicense();
	exit(-1);
    }

    if (wninit()) {
	display_message("wn: Fatal error - cannot open WordNet database\n");
	exit (-1);
    }
    exit(searchwn(argc, argv));*/
}

