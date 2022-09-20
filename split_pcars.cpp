#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cstring>
#define SOFT "splitPoscars"
#define EXE "spcars"
#define VERS "0.2"

// split the file into smaller chunks of POSCARS
int split_file( char* inFile, char* outFile, int nstructs, int verb );

// count the total number of unique structues in the file
int count_structures( char* inFile );

// convert a std::string object to a char*
char* str2char(std::string str);

void show_help()
{
	fprintf(stderr,"  %s v %s\n",SOFT,VERS);
	fprintf(stderr,"  Utility to split a file containing multiple POSCAR/CONTCAR\n");
	fprintf(stderr,"  files from an USPEX calculation.\n" );
	fprintf(stderr,"   ** File name assumed to be last argument...\n\n");
	fprintf(stderr,"USAGE: %s [OPTIONS] [FILE]\n",EXE);
	fprintf(stderr,"options...\n");
	fprintf(stderr,"   -n|--name  [STR]    define the base name of the structues to be written\n");
	fprintf(stderr,"                         format   = <basename>-[id].poscar\n");
	fprintf(stderr,"                         default == structures-*.poscar\n");
	fprintf(stderr,"   -p|--print [INT]    total number of structures to print\n");
	fprintf(stderr,"                       from the file, takes the first 'n' found\n");
	fprintf(stderr,"                         default == all \n");
	fprintf(stderr,"   -v|-vv|-vvv         amount of info printed to the screen\n");
	fprintf(stderr,"                         default = silent\n");
	fprintf(stderr,"                           -v   == startup/end messages\n");    
	fprintf(stderr,"                           -vv  == + new structures found\n"); 
	fprintf(stderr,"                           -vvv == + intermediate messages\n");
	fprintf(stderr,"extras...\n");
	fprintf(stderr,"       --debug        extensive status messages and computed values, equivalent to -vvv \n");
	fprintf(stderr,"    -h|--help         this message\n");
	fprintf(stderr,"    -u|--usage        brief list of parmaters and arguments\n");
	fprintf(stderr,"       --version      display version and author info then exit\n\n");
	fprintf(stderr,"examples....\n");
	fprintf(stderr,"%s BESTgatheredPOSCARS \n",EXE);
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
                else if (option[l]=='n') outFile = argv[ii+1]; 
                else if (option[l]=='p') nstructs = atoi( argv[ii+1] );   
                else if (option[l]=='u') show_usage( );
                else if (option[l]=='v') verb++;
            }
        } else if (argv[ii][0]=='-' && argv[ii][1]=='-') {
			argv[ii]++;argv[ii]++;
			if (strcmp(argv[ii],"help")==0) show_help( );
			else if (strcmp(argv[ii],"debug")==0) verb =3;
			else if (strcmp(argv[ii],"name")==0) outFile = argv[ii+1];
			else if (strcmp(argv[ii],"print")==0) nstructs = atoi( argv[ii+1] );
			else if (strcmp(argv[ii],"usage")==0) show_usage( );
			else if (strcmp(argv[ii],"version")==0) show_version( );
        }
    }

    inFile = argv[nargs-1];

	int errs = split_file( inFile,outFile,nstructs,verb );

	if (errs) return EXIT_FAILURE;

	if (verb>0) fprintf(stderr,"done!\n" );

	return EXIT_SUCCESS;
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

    if (verb>0) fprintf(stderr,"scanning \"%s\" for unique poscars now\n",inFile );

    char* nl;
    char* nxt;
    int   nLines =0;

	char* fnm = new char[200];

	// grab the first line
	std::getline( data,line );
	int rstructs =1;

	// if no structure count was provided, assume the user
	// wants to split the entire file
	if (!nstructs) nstructs = count_structures( inFile );

	if (verb>2) fprintf(stderr,"counted \"%i\" in the file \"%s\"\n",nstructs,inFile );



	// loop over the file to search 
	// for new structs
	while (1) {
		iss.clear(); iss.str( line );
		iss >> arg1;
		nl = str2char( arg1 );
		if (nl[0] =='E' && nl[1] == 'A') {
			nl++;nl++; // trim the "EA" and keep the structure ID
			if (verb>1) fprintf(stderr,"*found a new structure: S-%s\n",nl );
			if (outFile==NULL) sprintf( fnm,"structure-%s.poscar",nl );
			else sprintf( fnm,"%s-%s.poscar",outFile,nl );
			
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

char* str2char(std::string str)
{
    char *arg = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), arg);
    arg[str.size()] = '\0';
    return arg;
}

int count_structures( char* inFile )
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
	return ns;
}

