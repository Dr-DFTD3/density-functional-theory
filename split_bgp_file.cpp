#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#define SOFT "splitGatheredPoscars"
#define EXE  "sgp"
#define VERS "0.3"



// check format of a supposed structure file
int is_gp_file( char* fname, int verb );

// split the file into smaller chunks of POSCARS
int split_file( char* inFile, char* outFile, int nstructs, int verb );

// split the file into smaller chunks of POSCARS
int scan_file( char* inFile, char* outFile, int verb, std::string id );

// count the total number of unique structues in the file
int count_structures( char* inFile,int verb );

// convert a std::string object to a char*
char* str2char(std::string str);

// split a string into words based on a delimeter
std::vector<std::string> split_string(std::string list, char delim);


void show_help()
{
	fprintf(stderr,"  %s v %s\n",SOFT,VERS);
	fprintf(stderr,"  Utility to split a file containing multiple POSCAR/CONTCAR\n");
	fprintf(stderr,"  files from an USPEX calculation.\n" );
	fprintf(stderr,"USAGE: %s [OPTIONS] ... [FILE]\n",EXE);
	fprintf(stderr,"options...\n");
	fprintf(stderr,"   -i|--id    [STR]    grab a specific structure id(s)\n");
	fprintf(stderr,"                         format   = 1,2,5,100,4 ...\n");
	fprintf(stderr,"                         default  == get all structues in file\n");
	fprintf(stderr,"   -n|--name  [STR]    define the base name of the structues to be written\n");
	fprintf(stderr,"                         format   = <basename>-[id].vasp\n");
	fprintf(stderr,"                         default  == structures-*.vasp\n");
	fprintf(stderr,"   -p|--print [INT]    total number of structures to print\n");
	fprintf(stderr,"                       from the file, takes the first 'n' found\n");
	fprintf(stderr,"                         default  == all \n");
	fprintf(stderr,"   -v|-vv|-vvv         amount of info printed to the screen\n");
	fprintf(stderr,"                         default  = silent\n");
	fprintf(stderr,"                           -v     == startup/end messages\n");    
	fprintf(stderr,"                           -vv    == + new structures found\n"); 
	fprintf(stderr,"                           -vvv   == + intermediate messages\n");
	fprintf(stderr,"                           -vvvv  == a lot of information\n");
	fprintf(stderr,"extras...\n");
	fprintf(stderr,"       --debug        extensive status messages, equivalent to -vvvv \n");
	fprintf(stderr,"    -h|--help         this message\n");
	fprintf(stderr,"    -u|--usage        brief list of parmaters and arguments\n");
	fprintf(stderr,"       --version      display version and author info then exit\n\n");
	fprintf(stderr,"examples....\n");
	fprintf(stderr,"%s BESTgatheredPOSCARS \n",EXE);
	fprintf(stderr,"%s uspex_structures -i 1,2,8,10 \n",EXE);
	fprintf(stderr,"%s -n sns -p 10 BESTgatheredPOSCARS \n",EXE);

	exit(EXIT_SUCCESS);
}


void show_version()
{
    fprintf(stdout,"(MSL coreutils) %s v. %s\n",EXE,VERS );
    fprintf(stdout,"Copyright (C) 2016 Materials Simulation Laboratory\n" );
    fprintf(stdout,"This is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n\n" );
    fprintf(stdout,"Written by: Joseph M. Gonzalez\n");
    exit(EXIT_SUCCESS);
}


void show_usage()
{
    fprintf(stdout,"(MSL coreutils) %s v. %s\n",EXE,VERS );
    fprintf(stdout,"USAGE: %s [OPTIONS] filename\n",EXE );
    fprintf(stdout,"OPTIONS: [-n output base name] [-p #structures]\n" );
    fprintf(stdout,"         [-v|-vv|-vvv verbosity] [-h help] [--version]\n" );
    exit(EXIT_SUCCESS);
}



int main(int nargs, char** argv)
{

	std::vector<std::string> idList;
	std::string ids;

	int   splitAll =1;
	int   errs     =0;
	int   found    =0;
	int   nstructs =0;
	int   verb     =0; 
	char* inFile   =NULL;
	char* outFile  =NULL;


	if (nargs==1) show_usage();


	for (int ii = 1; ii < nargs; ii++) {
        if (argv[ii][0]=='-' && argv[ii][1]!='-' ) {
            char* option =argv[ii];
            for (int l=1;l<strlen(option);l++) {
                if (option[l]=='h') show_help( );
                else if (option[l]=='i') {ids = argv[ii+1];splitAll =0;}
                else if (option[l]=='n') outFile = argv[ii+1]; 
                else if (option[l]=='p') nstructs = atoi( argv[ii+1] );   
                else if (option[l]=='u') show_usage( );
                else if (option[l]=='v') verb++;
            }
        } else if (argv[ii][0]=='-' && argv[ii][1]=='-') {
			argv[ii]++;argv[ii]++;
			if (strcmp(argv[ii],"help")==0) show_help( );
			else if (strcmp(argv[ii],"debug")==0) verb =3;
			else if (strcmp(argv[ii],"ids")==0) {ids = argv[ii+1];splitAll =0;}
			else if (strcmp(argv[ii],"name")==0) outFile = argv[ii+1];
			else if (strcmp(argv[ii],"print")==0) nstructs = atoi( argv[ii+1] );
			else if (strcmp(argv[ii],"usage")==0) show_usage( );
			else if (strcmp(argv[ii],"version")==0) show_version( );
        }
    }

    // search argument list to determine the file name
    for (int i=1; i<nargs; i++)
    {
    	if ( is_gp_file( argv[i],verb ) )
    	{
    		inFile = argv[i];
    		found =1;
    		break;
    	}
    }

    if (!found) 
    {
    	fprintf(stderr,"No structure  file has been found, try again\n" );
    	return EXIT_FAILURE;
    }


	if (verb>0) fprintf(stderr,"scanning \"%s\" for poscars now\n",inFile );    

    if (splitAll)
    {
    	errs = split_file( inFile,outFile,nstructs,verb );
    	if (errs) return EXIT_FAILURE;
    	if (verb>0) fprintf(stderr,"done!\n" );
    }
    else
    {
    	idList = split_string( ids,',' );
    	if (verb>1) 
		{
			fprintf(stderr,"id list: " );
			for (int i=0;i<idList.size();i++)
			{
				fprintf(stderr,"%s ",idList[i].c_str() );
			}
			fprintf(stderr,"\n" );
		}

    	for (int i=0;i<idList.size();i++)
    	{
			errs += scan_file( inFile,outFile,verb,idList[i] );
    	}

    	
    	if (errs) return EXIT_FAILURE;
    	if (verb>0) fprintf(stderr,"done!\n" );
    } 

	return EXIT_SUCCESS;
}

int is_gp_file( char* fname,int verb )
{
    int found =0;

    std::ifstream data(fname);
    std::string line, arg1;
    std::istringstream iss;

    if (!data.is_open()) 
    {
        // fprintf(stderr," could not open \"%s\" for reading!\n",fname);
        return found;
    }

    std::getline( data,line );
    iss.clear(); iss.str( line );
	iss >> arg1;
	char* nl = str2char( arg1 );
	if (nl[0] =='E' && nl[1] == 'A') {

		if ( isdigit( nl[2] ) ) 
		{
			if (verb>3) fprintf(stderr,"\"%s\" is a BGP style file\n",fname );
			data.close();
			return 1;
		}

	}

	data.close();
	return 0;

}


int scan_file( char* inFile, char* outFile, int verb, std::string id )
{

    std::ifstream data(inFile);
    std::string line,line2;
    std::istringstream iss;
    std::string arg1;

    char* nl;
    char* nxt;

	char* fnm = new char[200];

	// loop over the file to search 
	// for new structs
	while (std::getline(data,line )) {
		iss.clear(); iss.str( line );
		iss >> arg1;
		nl = str2char( arg1 );
		if (nl[0] =='E' && nl[1] == 'A') 
		{

			nl++;nl++; // trim the "EA" but keep the structure ID
			if (verb>3) fprintf(stderr,"*found a new structure: S-%s\n",nl );

			std::string snum = nl;
			if (snum == id)
			{
				if (verb>1) fprintf(stderr,"Found structure: %s\n",snum.c_str() );
			
				if (outFile==NULL) sprintf( fnm,"structure-%s.vasp",nl );
				else sprintf( fnm,"%s-%s.vasp",outFile,nl );

				FILE* fptr = fopen( fnm,"w" );
				fprintf(fptr,"%s\n",line.c_str() );
				while (std::getline( data,line2 )) {
					iss.clear(); iss.str( line2 );
					iss >> arg1;
					nxt = str2char( arg1 );
					if (nxt[0] =='E' && nxt[1] == 'A') {
						line = line2;
						break;
					} else {
						fprintf(fptr,"%s\n",line2.c_str() );
					}
				}
				fclose( fptr );
			}
			if (verb>1) fprintf(stderr,"WARNING: structure \"%s\" not found!\n",snum.c_str() );
		}
	}

    data.close();

    return 0;
}


int split_file( char* inFile, char* outFile, int nstructs, int verb )
{

    std::ifstream data(inFile);
    std::string line,line2;
    std::istringstream iss;
    std::string arg1;

    if (!data.is_open()) {
    	fprintf(stderr,"Error: Cannot open \"%s\" for reading\n",inFile );
    	return 1;
    }

    if (verb>1) fprintf(stderr,"scanning \"%s\" for unique poscars now\n",inFile );

    char* nl;
    char* nxt;
    int   nLines =0;

	char* fnm = new char[200];

	// grab the first line
	std::getline( data,line );
	int rstructs =1;

	// if no structure count was provided, assume the user
	// wants to split the entire file
	if (!nstructs) nstructs = count_structures( inFile,verb );

	// loop over the file to search 
	// for new structs
	while (1) {
		iss.clear(); iss.str( line );
		iss >> arg1;
		nl = str2char( arg1 );
		if (nl[0] =='E' && nl[1] == 'A') {
			nl++;nl++; // trim the "EA" and keep the structure ID
			if (verb>3) fprintf(stderr,"*found a new structure: S-%s\n",nl );
			if (outFile==NULL) sprintf( fnm,"structure-%s.vasp",nl );
			else sprintf( fnm,"%s-%s.vasp",outFile,nl );
			
			FILE* fptr = fopen( fnm,"w" );
			fprintf(fptr,"%s\n",line.c_str() );
			while (std::getline( data,line2 )) {
				iss.clear(); iss.str( line2 );
				iss >> arg1;
				nxt = str2char( arg1 );
				if (nxt[0] =='E' && nxt[1] == 'A') {
					line = line2;
					fprintf(fptr,"\n");
					break;
				} else {
					fprintf(fptr,"%s\n",line2.c_str() );
				}
			}
			fclose( fptr );
		}
		rstructs++;
		if (rstructs>nstructs) break;
	}

    data.close();

    return 0;
}

int count_structures( char* inFile, int verb )
{

	int ns =0;
	std::ifstream data(inFile);
	std::string line;
	std::istringstream iss;
	std::string arg1;

	char* nl;

	// loop over the file to search 
	// unique structs
	while ( std::getline( data,line ) ) {
		iss.clear(); iss.str( line );
		iss >> arg1;
		nl = str2char( arg1 );
		if (nl[0] =='E' && nl[1] == 'A') ns++;
	}

	if (verb>3) fprintf(stderr,"Found %i structures in your file \"%s\"\n",ns,inFile );
	return ns;
}

std::vector<std::string> 
split_string(std::string list, char delim)
{
	// int value;
    std::vector<std::string> internal;
    std::stringstream ss(list);
	
    std::string token;
    while (std::getline(ss, token, delim)) 
	{
		internal.push_back( token );
	}

 //    while (std::getline(ss, token, delim)) 
	// {
	// 	std::stringstream stream( token );
	// 	stream >> value;
	// 	internal.push_back(value);
	// }
    return internal;
}

char* str2char(std::string str)
{
    char *arg = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), arg);
    arg[str.size()] = '\0';
    return arg;
}



