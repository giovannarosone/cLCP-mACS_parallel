#include "Config.h"
#include "Types.h"
#include "Tools.h"
#include "MultiACSParameters.h"
#include "MultiACS.h"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <cstdlib>
using namespace std;
using namespace multi_acs;


const char C_ApplicationName[]{"cLCP-mACS"};


void printConfiguration(const MultiACSParameters params) {
	cout << "------- CONFIGS RESUME --------\n";
	cout << "> TYPES SIZES\n";
	cout << "Sequences Length: " << sizeof(SequenceLength) << " Byte\n";
	cout << "Sequences Number: " << sizeof(SequenceNumber) << " Byte\n";
	cout << "Letter Number: " << sizeof(LetterNumber) << " Byte\n";
//	cout << "> BUFFER SIZE (n. elements): " << BUFFER_SIZE << '\n';
	cout << "> BUFFER SIZE (n. elements): " << params.global_buffer_size << '\n';

	if(BUILD_UCLCP == 1)
		cout << "> BUILD Upper cLCP!\n";
	if(BUILD_LCLCP == 1)
		cout << "> BUILD Lower cLCP!\n";
	if(BUILD_CLCP == 1)
		cout << "> BUILD cLCP!\n";
	if(DELETE_D == 1)
		cout << "> DELETE D ARRAYS AFTER COMPLETION!\n";
	if(DELETE_PARTIAL_LCP == 1)
		cout << "> DELETE SINGLE LCP ARRAYS AFTER COMPLETION!\n";
	cout << "----- END CONFIGS RESUME ------\n" << endl;
}


void printParameters(const MultiACSParameters params) {
	cout << "-------- PARAMS RESUME --------\n";
	cout << "Collection file path: "
			<< params.collection_file_name << '\n';
    
	cout << "Input File Format: ";
	if(params.input_file_format == InputFileFormat::BCR)
		cout << "BCR format\n";
	else
		cout << "EGSA format\n";
    
	if(!params.doDGeneration)
		cout << "SKIP D Arrays Generation Step (files .d NEEDED)\n";
    if(!params.doGenerateExtendedD)
        cout << "GENERATE D Extended Arrays (files .d NEEDED)\n";
	if(!params.doFlushIntermediateFiles)
		cout << "DELETE Intermediate Files {i}.d and {i}.lcp after computing i-vs-all\n";
	if(params.verbose)
		cout << "***WITH VERBOSE REPORTING***\n";
	cout << "------ END PARAMS RESUME ------\n" << endl;
}


void printUsage() {
	cout << "Usage: [-h] [-v] [-d] [-E] [-f] [-Q BUFFER_SIZE] [-I input_format] target_collection\n\n";
	cout << "\th:\thelp message\n\n";
	cout << "\tv:\tverbose output\n\n";
	//cout << "\tp:\tskip preprocessing step (file .info NEEDED containing alphabet size, number of sequence, number of letters)\n\n";
	cout << "\td:\tskip D arrays generation step (files .d NEEDED)\n\n";
    cout << "\tE:\tgenerate D Extended arrays\n\n";
	cout << "\tf:\tdelete intermediate files {i}.d and {i}.lcp after computing i-vs-all\n\n";
    cout << "\tQ:\tset BUFFER_SIZE (default " << DEFAULT_BUFFER_SIZE << ")\n\n";
    cout << "\tI:\tinput format\n";
	cout <<	"\t\t- 0 BCR file format (LCP + DA)\n";
	cout <<	"\t\t- 1 EGSA file format (LCP + DA + SA + BWT)\n";
    //cout << "\tN:\tspecifies the number of concurrent openmp threads\n\n";

	cout << endl;
}


int main(int argc, char* argv[]) {

    bool verbose = false;
	bool doDGeneration = true;
    bool doGenerateExtendedD = false;
	bool doFlushIntermediateFiles = false;
	uint global_buffer_size = DEFAULT_BUFFER_SIZE;	// Config.h
	int input_file_format = InputFileFormat::BCR;	// Types.h, BCR:0 or EGSA:1

	string collection_file_name;

	/*
	#if defined(__APPLE__) && defined(__clang__)	
		cout << "ElementTypeLCPda (packed): " << sizeof(ElementTypeLCPda) << endl;
		cout << "ElementTypeLCPda: " << sizeof(ElementTypeLCPda2) << endl;
	#endif
	*/
		
	int o;
	while((o = getopt(argc, argv, "vdEfQ:I:h")) != -1) {
		switch(o) {
            case 'v':
                verbose = true;
                break;

            /*
			case 'p':
				doPreprocessing = false;
				break;
			*/

			case 'd':
				doDGeneration = false;
				break;
            case 'E':
                doGenerateExtendedD = true;
                break;
			case 'f':
				doFlushIntermediateFiles = true;
				break;

			/*
            case 'L':
                option_print_clcp = true;			// substituted by #define OPTION_PRINT_CLCP in Config.h
                break;
            */

			/*
            case 'S':
                option_print_score_matrix = true;	// substituted by #define OPTION_PRINT_SCOREM in Config.h
                break;
            */


            case 'Q':
				global_buffer_size = atoi(optarg);	// not used, BUFFER_SIZE is set as a macro in Config.h
				cout << "global_buffer_size set to: " << global_buffer_size << endl;
				if (global_buffer_size <= 0) {
					cerr << "Invalid BUFFER SIZE: " << global_buffer_size << endl;
					exit(EXIT_FAILURE);
				}
				break;

			case 'I':
				input_file_format = atoi(optarg);
				if (input_file_format == 1) {
					cout << "GESA input format to be implemented. Bye.";
					exit(EXIT_FAILURE);
				}
				break;
			case 'h':
			default:
				printUsage();
				exit(EXIT_FAILURE);
		}
	}

    
	if(optind == argc - 1) {
		collection_file_name = string(argv[optind++]);
	} else {
		printUsage();

		ostringstream err_message;
		err_message << "Missing Target Collection file name";
		Error::stopWithError(C_ApplicationName, __func__, err_message.str());
	}

	MultiACSParameters params(	verbose,
								doDGeneration,
					            doGenerateExtendedD,
								doFlushIntermediateFiles,
								global_buffer_size,
								input_file_format,
								collection_file_name);

	printConfiguration(params);
	printParameters(params);

	MultiACS acs(params);
   	acs.computeACS();

    cout << "Bye!\n";

	return EXIT_SUCCESS;

}