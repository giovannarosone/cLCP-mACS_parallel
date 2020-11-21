#include "MultiACS.h"
#include "StackDGenerator.h"
#include "Types.h"
#include "Tools.h"
#include "Reader.h"
#include "Writer.h"
#include "malloc_count/malloc_count.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
#include <omp.h>
using namespace std;

namespace multi_acs {

	MultiACS::MultiACS(const MultiACSParameters& params)
		: params(params) {
	}

	void MultiACS::computeACS() {

		AlphabetSize sigma;
		SequenceNumber m;
		LetterNumber N;

        clock_t start_1, start_2;
        time_t start_wc_1, start_wc_2;
        double elapsed_1, elapsed_wc_1;     // timimg the entire ACS calculation
        double elapsed_2, elapsed_wc_2;     // timing only the distance-computation code using the score matrix

            
        /*  Open collection.info */
		FileName info_file_name(params.collection_file_name, C_InfoFileExt);  // class Filename: Tools.h, C_InfoFileExt: Config.h
		FILE* info_file = fopen(info_file_name.c_str(), "rb"); // NOTICE: it's binary
		if (info_file == nullptr) {
			ostringstream err_message;
			err_message << "Couldn't open file " << info_file_name.str();
			Error::stopWithError(C_MultiACS_ClassName, __func__, err_message.str());
		}


        /*  Read from .info */        
		fread(&N, sizeof(LetterNumber), 1, info_file);      // N: eBWT length         (LetterNumber in Types.h, but selected in Config.h)
        fread(&m, sizeof(SequenceNumber), 1, info_file);    // m: number of Sequences (SequenceNumber as above)
        fread(&sigma, sizeof(AlphabetSize), 1, info_file);  // sigma: alphabet size   (AlphabetSize as above)
		fclose(info_file);

		cout << "------- COLLECTION INFO -------\n";
		cout << "Alphabet Size: " << (uint) sigma << '\n';
		cout << "Sequences Number: " << (SequenceNumber) m << '\n';
		cout << "Letters Number: " << (LetterNumber) N << '\n';
		cout << "----- END COLLECTION INFO -----\n" << endl;


        /*  Open .len (contains the lengths of sequences) */
        FileName len_file_name(params.collection_file_name, C_LengthFileExt);
        FILE* len_file = fopen(len_file_name.c_str(), "rb");
        if (len_file == nullptr) {
            ostringstream err_message;
            err_message << "Couldn't open file " << len_file_name.str();
            Error::stopWithError(C_MultiACS_ClassName, __func__, err_message.str());
        }
        

        /*  Read from .len: seq_lengths[i] contains the lenght of the i-th sequence in the collection */        
        vector <SequenceLength> seq_lengths(m);
        fread(&seq_lengths[0], sizeof(SequenceLength), m, len_file);
        fclose(len_file);


        /* Start ACS computation */
        start_1 = clock();
        start_wc_1 = time(NULL);

        vector< vector<LetterNumber> > score(m, vector<LetterNumber>(m, 0));
        cout << "ACS Computation START\n";
        cout << "Current memory: " << malloc_count_current() << " Bytes" << endl;


/* -------------------------------------------------------------------------------- */
/* ---------------------------- BEGIN OF PARALLEL SECTION ------------------------- */
/* -------------------------------------------------------------------------------- */

//#pragma omp parallel default(none) shared(seq_lengths, score, N, m, sigma, params)
#pragma omp parallel shared(seq_lengths, score, N, m, sigma, params)
{
        clock_t thread_start;
        time_t  thread_start_wc;
        double  thread_elapsed;
        double  thread_elapsed_wc;

        int     thread_id = omp_get_thread_num();
        FILE*   thread_output_file = NULL;
        stringstream ss;


        /* Each thread write its own thread_id.out.txt output file */
        ss << params.collection_file_name << "_thread_" << thread_id << ".out.txt";
        thread_output_file = fopen(ss.str().c_str(), "w");
        if (thread_output_file == nullptr) {
            ostringstream err_message;
            err_message << "Couldn't open file " << ss.str();
            Error::stopWithError(C_MultiACS_ClassName, __func__, err_message.str());
        }
        ss.str("");
    
    
        /* Initialize StackDGenerator for computation of D arrays */
        StackDGenerator* d_generator = new StackDGenerator(params, m, N);


        /* Sequences are equally distributed to available threads, each thread performing computing column scores[][i] */
        #pragma omp for schedule(static)
		for (SequenceNumber i = 0; i < m; ++i) {
        
            /*  ---------------------------------------------  */
            /*  1. Computation of {i}.d, {i}.d_pairs, {i}.lcp  */
            /*  ---------------------------------------------  */
            
            thread_start = clock();
            thread_start_wc = time(NULL);

            if (params.doDGeneration) {

                (*d_generator).generateSingleD(i, thread_output_file, thread_id, ss);   // uses BUFFERSIZEcLCP*sizeof(ElementTypeLCPda) bytes

            } else {

                ss << "thread=" << thread_id << ": SKIPPED generateSingleD(" << i << "), seq_length=" << seq_lengths[i] << endl;
                fprintf(thread_output_file, "%s", ss.str().c_str());
                fflush(thread_output_file);
                ss.str("");

                /* Do nothing */

            }

            thread_elapsed = (clock()-thread_start)/(double)(CLOCKS_PER_SEC);
            thread_elapsed_wc = difftime(time(NULL),thread_start_wc);
            
            ss  << "thread=" << thread_id << ": computing generateSingleD(" << i << "), seq_length=" << seq_lengths[i] << " took: "
                << "elapsed_time=" << thread_elapsed << ", wall_clock_time=" << thread_elapsed_wc << endl;

            fprintf(thread_output_file, "%s", ss.str().c_str());
            fflush(thread_output_file);
            ss.str("");            
            
            /*  ---------------------------------  */
            /*  2. Computation of i-vs-all scores  */
            /*  ---------------------------------  */
  
            thread_start = clock();
            thread_start_wc = time(NULL);

    	    computeColumnScore(score, i, N, thread_output_file);

            thread_elapsed = (clock()-thread_start)/(double)(CLOCKS_PER_SEC);
            thread_elapsed_wc = difftime(time(NULL),thread_start_wc);

            ss  << "thread=" << thread_id << ": computing computeColumnScore(" << i << ") took: "            
                << "elapsed_time=" << thread_elapsed << ", wall_clock_time=" << thread_elapsed_wc << endl;

            fprintf(thread_output_file, "%s", ss.str().c_str());
            fflush(thread_output_file);
            ss.str("");

		} // END OF: for(SequenceNumber=)
    
        if (thread_output_file)
            fclose(thread_output_file);

        delete d_generator;

} // END OF #pragma
        
/* ------------------------------------------------------------------------------ */
/* ---------------------------- END OF PARALLEL SECTION ------------------------- */
/* ------------------------------------------------------------------------------ */

        /*  Having computed all the columns of the score matrix, we proceed to compute the actual ACS distances */
        FileName distance_file_name(params.collection_file_name, C_DistanceFileExt);
        FILE* distance_file = fopen(distance_file_name.c_str(), "w");
        if (distance_file == nullptr) {
            ostringstream err_message;
            err_message << "Couldn't open file " << distance_file_name.str();
            Error::stopWithError(C_MultiACS_ClassName, __func__, err_message.str());
        }
        
        SequenceLength s1, s2;
        DistanceValue sumS1, sumS2;
        DistanceValue distance = 0;

        #ifdef OPTION_PRINT_SCOREM
            cout << endl << "PAIRWISE SCORES + ACS values:" << endl;
        #endif

        cout << "\tDistances Computation START" << endl;
        start_2 = clock();
        start_wc_2 = time(NULL);

        for (SequenceNumber r = 0; r < m; ++r) {
            
            SequenceLength n_r = seq_lengths[r];
            for (SequenceNumber t = r + 1; t < m; ++t) { // triangular matrix

                if (r != t) { // ?

                    SequenceLength n_t = seq_lengths[t];

                    //            distance = (n_x - 1)*(log(n_r - 1)/base)/(2*score_x[r])
                    //                    - (log(n_x - 1)/base)/(n_x)
                    //                    + (n_r - 1)*(log(n_x - 1)/base)/(2*score_r[r])
                    //                    - (log(n_r - 1)/base)/(n_r);

                    s1 = n_t - 1;
                    s2 = n_r - 1;

                    sumS1 = score[t][r];
                    sumS2 = score[r][t];
                    
                    #ifdef OPTION_PRINT_SCOREM
                        cout << "\t|Seq" << t << "|=" << n_t - 1 <<
                        "\t|Seq" << r << "|=" << n_r - 1 << '\n';
                        cout << "\tscore(" << t << "," << r << ")="
                        << sumS1 <<
                        "\tscore(" << r << "," << t << ")="
                        << sumS2 << endl;
                    #endif
                    
                    distance = ((log10(s1) / (sumS2 / s2)) - ((2.0*log10(s2)) / (s2 + 1)) + (log10(s2) / (sumS1 / s1)) - ((2.0*log10(s1)) / (s1 + 1)))*0.5;
                    
                    #ifdef OPTION_PRINT_SCOREM
                        cout << "\t> ACS(" << r << "," << t << ")="
                        << distance << endl;
                    #endif
                    
                    fprintf(distance_file, "%f\t", distance);

                } else {

                    /* DEBUGGING:
                        cout << "r=" << r << ", t=" << t << " and r==t is TRUE" << endl;
                    */

                    fprintf(distance_file, "0\t");

                }
            }
        }
        
        fclose(distance_file);

        elapsed_2 = (clock()-start_2)/(double)(CLOCKS_PER_SEC);
        elapsed_wc_2 = difftime(time(NULL),start_wc_2);

        cout << "\tDistances Computation END: elapsed_time=" << elapsed_2 << ", wall_clock_time=" << elapsed_wc_2 << endl;

        elapsed_1 = (clock()-start_1)/(double)(CLOCKS_PER_SEC);
        elapsed_wc_1 = difftime(time(NULL),start_wc_1);
        
        cout << "ACS Computation END\n";        
        cout << "Current memory: " << malloc_count_current() << " Bytes" << endl << endl;
        cout << "### ACS Computation Reporting ###\n";
        cout << "Elapsed time: " << elapsed_1 << " secs\n";
        cout << "Wall Clock time: " << elapsed_wc_1 << " secs\n";
        cout << "Peak memory: " << malloc_count_peak() << " Bytes\n";
        cout << "#################################\n" << endl;

	}

	void MultiACS::computeColumnScore(vector<vector<LetterNumber>> &score, SequenceNumber ref_id, LetterNumber N, FILE* thread_output_file) {

        SequenceLength alpha;       // minimum LCP value since beginning of current t-interval (ref_id=t)
        SequenceLength zeta;        // maximum D value since beginning of current t-interval
        LetterNumber x;
        bool first;                 // true if lcp_t = -1 (first suffix)

        ElementTypeLCPda bcr_struct;           // current lcpDA pair
        SequenceLength lcp_t_value; // current lcp_t value
        SequenceLength d_t_value;   // current d_t value

        structD d_pair;             // current pair (lcpCurrent, posCurrent) in D_t
        LetterNumber j;             // eBWT running index

        stringstream ss;            // OpenMP related
        int thread_id;

        /*
            // TIMING
            time_t start,end;
            double dif;
            time (&start);
        */

        /* Get my thread_id (OpenMP) */
        thread_id = omp_get_thread_num();

        /* Open .lcpDA (or .bcr) file */
        FileName bcr_file_name(params.collection_file_name, multi_acs::C_BCRFileExt);
		FILE* bcr_file = fopen(bcr_file_name.c_str(), "rb");
		if (bcr_file == nullptr) {
			ostringstream err_message;
			err_message << "Couldn't open file " << bcr_file_name.str();
			Error::stopWithError(C_MultiACS_ClassName, __func__, err_message.str());
		}

		BCRReader bcr_reader(params, bcr_file);

		if(params.verbose) {
            ss << "thread=" << thread_id << ": ";
			ss << "Reading from " << bcr_file_name.str() << endl;
            fprintf(thread_output_file, "%s", ss.str().c_str());
            ss.str("");

		}
   
        /* Open {ref_id}.d file containing array D_t (t=ref_id)   */
        FileName d_file_name(params.collection_file_name, ref_id, C_DynBlockFileExt);
        FileName d_pairs_file_name(params.collection_file_name, ref_id, C_DynBlockFilePairs);
		FILE* d_file = fopen(d_pairs_file_name.c_str(), "rb");
		if (d_file == nullptr) {
			ostringstream err_message;
			err_message << "Couldn't open file " << d_file_name.str();
			Error::stopWithError(C_MultiACS_ClassName, __func__, err_message.str());
		}

        DReader d_reader(params, d_file);
        if (params.verbose) {
            ss << "thread=" << thread_id << ": Reading from " << d_file_name.str() << endl;
            fprintf(thread_output_file, "%s", ss.str().c_str());
            ss.str("");
        }

        /* Open {ref_id}.lcp file containing lcp_t (t=ref_id) */
		FileName lcp_file_name = FileName(params.collection_file_name, ref_id, C_LcpFileExt);
		FILE* lcp_file = fopen(lcp_file_name.c_str(), "rb");
		if (lcp_file == nullptr) {
			ostringstream err_message;
			err_message << "Couldn't open file " << lcp_file_name.str();
			Error::stopWithError(C_MultiACS_ClassName, __func__, err_message.str());
		}

		LCPReader lcp_reader(params, lcp_file);
        if (params.verbose) {
            ss << "thread=" << thread_id << ": Reading from " << lcp_file_name.str() << endl;
            fprintf(thread_output_file, "%s", ss.str().c_str());
            ss.str("");
        }

        /* Prints cLCP colmns */
        #ifdef OPTION_PRINT_CLCP // command line: -L (Score matrix)
            ss << "thread=" << thread_id << ": cLCP[][" << ref_id << "] " << endl;
            fprintf(thread_output_file, "%s", ss.str().c_str());
            ss.str("");
        #endif


        // -----------------------------------------------------------------
        // 1. read lcpDA until position d_pair.posCurrent - 1, d_t_value = 0
        // -----------------------------------------------------------------
        if (! lcp_reader.readLCPValue(lcp_t_value)) {
            cerr << "ERROR: lcp_t file is empty!" << endl;
            exit(EXIT_FAILURE);        }

        if (! d_reader.readDPair(d_pair)) {
            cerr << "ERROR: D_t file is empty!" << endl;
            exit(EXIT_FAILURE);
        }

        first = true;
        alpha = C_MaxSequenceLength;
        zeta = 0;
        d_t_value = 0;
        
        for (j=0 ; j < d_pair.posCurrent; j++) {
                  
            // read next (lcp, da) pair from BCR
            bcr_reader.readBCRStruct(bcr_struct);
            if (bcr_struct.id != ref_id) {  // inside a ref_id-interval

                // updates the minimum lcp values since beginning of a ref_id-interval
                if (bcr_struct.lcp < alpha)
                    alpha = bcr_struct.lcp;

                // first = (h_t == 1), we are looking at the first ref_id-colored suffix, therefore alpha=+inf.                
                if (first) {

                    first = false;
                    score[bcr_struct.id][ref_id] += zeta;
                    
                    #ifdef OPTION_PRINT_CLCP
                        //cout << (int) zeta << endl;
                        fprintf(thread_output_file, "%d\n", zeta);
                    #endif
                    
                } else if (alpha > lcp_t_value) { // lcp_t_value is LCP(t1, t2) with t=ref_id (color)

                    score[bcr_struct.id][ref_id] += alpha;
                    
                    #ifdef OPTION_PRINT_CLCP
                        //cout << (int) alpha << endl;
                        fprintf(thread_output_file, "%d\n", alpha);
                    #endif
                    
                } else {
                    x = max(max(alpha, zeta), lcp_t_value);
                    score[bcr_struct.id][ref_id] += max(max(alpha, zeta), lcp_t_value);
                    
                    #ifdef OPTION_PRINT_CLCP
/*                        #if CHAR_NUM==0
                         fprintf(thread_output_file, "%d\n", x);
                        #else
                         fprintf(thread_output_file, "%lu\n", x);                    
                        #endif*/
                    #endif
                    
                }
                
            } else {                        // starts a new ref_id-interval
                
                alpha = C_MaxSequenceLength;    // alpha=+inf;
                zeta = 0;                       // zeta=0;
                
                #ifdef OPTION_PRINT_CLCP
                    //cout << "-" << endl;
                    fprintf(thread_output_file, "-\n");
                #endif
                
                if (! lcp_reader.readLCPValue(lcp_t_value)) {

                    lcp_t_value = 0;
                    if(first) 
                        first = false;
                }
                
            }
        } // END OF: for (j = 0; j < d_pair.posCurrent; j++, k++)


        // ---------------------------------------------------------------
        // 2. j = d_pair.posCurrent, d_t_value = d_pair.lcpCurrent + 1
        // ---------------------------------------------------------------
        d_t_value = d_pair.lcpCurrent + 1;
        bcr_reader.readBCRStruct(bcr_struct);

        if (bcr_struct.id != ref_id) {
            if (bcr_struct.lcp < alpha)
                alpha = bcr_struct.lcp;
            
            if ((d_t_value > 0) && (d_t_value - 1 > zeta))
                zeta = d_t_value - 1;
            
            if (first) { // TODO: togliere
                first = false;
                score[bcr_struct.id][ref_id] += zeta;
                
                #ifdef OPTION_PRINT_CLCP
                    //cout << (int) zeta << endl;
                    fprintf(thread_output_file, "%d\n", zeta);
                #endif
                
            } else if (alpha > lcp_t_value) {
                score[bcr_struct.id][ref_id] += alpha;
                
                #ifdef OPTION_PRINT_CLCP
                    //cout << (int) alpha << endl;
                    fprintf(thread_output_file, "%d\n", alpha);
                #endif
                
            } else {
                x = max(max(alpha, zeta), lcp_t_value);
                score[bcr_struct.id][ref_id] += max(max(alpha, zeta), lcp_t_value);
                
                #ifdef OPTION_PRINT_CLCP
/*                        #if CHAR_NUM==0
                         fprintf(thread_output_file, "%d\n", x);
                        #else
                         fprintf(thread_output_file, "%lu\n", x);                       
                        #endif*/
                #endif
                
            }
            
        } else { 
            
            alpha = C_MaxSequenceLength; // dgreco: alpha=+inf; zeta=0
            zeta = 0;

            if (!lcp_reader.readLCPValue(lcp_t_value)) {
                lcp_t_value = 0;
                if(first) first = false;
            }
            
        } // END OF: if (bcr_struct.id != ref_id) 

        
        
        // ------------------------ MIDDLE d_pairs ---------------------------


        j = j + 1;
        while (d_reader.readDPair(d_pair)) {

            // -----------------------------------------------------------------
            // 1. read lcpDA until position d_pair.posCurrent - 1, d_t_value = 0
            // -----------------------------------------------------------------
            d_t_value = 0;
            
            for ( ; j < d_pair.posCurrent; j++) {
                
                bcr_reader.readBCRStruct(bcr_struct);
                if (bcr_struct.id != ref_id) {
                    if (bcr_struct.lcp < alpha)
                        alpha = bcr_struct.lcp;
                    
                    if (first) {
                        first = false;
                        score[bcr_struct.id][ref_id] += zeta;
                        
                        #ifdef OPTION_PRINT_CLCP
                            //cout << (int) zeta << endl;
                            fprintf(thread_output_file, "%d\n", zeta);
                        #endif
                        
                    } else if (alpha > lcp_t_value) {
                        score[bcr_struct.id][ref_id] += alpha;
                        
                        #ifdef OPTION_PRINT_CLCP
                            //cout << (int) alpha << endl;
                            fprintf(thread_output_file, "%d\n", alpha);
                        #endif
                        

                    } else {
                        x = max(max(alpha, zeta), lcp_t_value);
                        score[bcr_struct.id][ref_id] += x;
                        
                        #ifdef OPTION_PRINT_CLCP
/*                            #if CHAR_NUM==0
                             fprintf(thread_output_file, "%d\n", x);
                            #else
                             fprintf(thread_output_file, "%lu\n", x);                        
                            #endif*/
                        #endif
                        
                    }
                    
                } else { 
                    
                    alpha = C_MaxSequenceLength;
                    zeta = 0;
                    
                    #ifdef OPTION_PRINT_CLCP
                        //cout << "-" << endl;
                        fprintf(thread_output_file, "-\n");
                    #endif
                    
                    if (!lcp_reader.readLCPValue(lcp_t_value)) {
                        lcp_t_value = 0;
                        if(first) first = false;
                    }
                    
                }
            } // for (j = 0; j < d_pair.posCurrent; j++, k++)
            
            
            // ---------------------------------------------------------------
            // 2. j = d_pair.posCurrent, d_t_value = d_pair.lcpCurrent + 1
            // ---------------------------------------------------------------
            d_t_value = d_pair.lcpCurrent + 1;
            bcr_reader.readBCRStruct(bcr_struct);
            
            if (bcr_struct.id != ref_id) {
                if (bcr_struct.lcp < alpha)
                    alpha = bcr_struct.lcp;
                
                if ((d_t_value > 0) && (d_t_value - 1 > zeta))
                    zeta = d_t_value - 1;

                if (first) { // TODO: togliere
                    first = false;
                    score[bcr_struct.id][ref_id] += zeta;
                    
                     #ifdef OPTION_PRINT_CLCP
                         //cout << (int) zeta << endl;
                         fprintf(thread_output_file, "%d\n", zeta);
                     #endif
                    
                }
                else if (alpha > lcp_t_value) {
                    score[bcr_struct.id][ref_id] += alpha;
                    
                     #ifdef OPTION_PRINT_CLCP
                         //cout << (int) alpha << endl;
                         fprintf(thread_output_file, "%d\n", alpha);
                     #endif
                    
                }
                else {
                    x = max(max(alpha, zeta), lcp_t_value);
                    score[bcr_struct.id][ref_id] += x;
                    
                     #ifdef OPTION_PRINT_CLCP
                         //cout << (int) x << endl;
                       /* #if CHAR_NUM==0
                         fprintf(thread_output_file, "%d\n", x);
                        #else
                         fprintf(thread_output_file, "%lu\n", x);                         
                        #endif*/
                     #endif
                    
                }
                
            } else { // dgreco: inizio di un nuovo X-interval
                
                alpha = C_MaxSequenceLength; // dgreco: alpha=+inf; zeta=0
                zeta = 0;
                
                 #ifdef OPTION_PRINT_CLCP
                     //cout << "-" << endl;
                     fprintf(thread_output_file, "-\n");
                 #endif
                 
                 if (!lcp_reader.readLCPValue(lcp_t_value)) {
                 lcp_t_value = 0;
                 if(first) first = false;
                 }
                
            } // if (bcr_struct.id != ref_id)
            
            j = j + 1;
        } // while (d_reader.readDPair(d_pair))
        
        
        // -------------------------- NO d_pairs --------------------------
        // No more D_t pairs to read, therefore d_t_value_t = 0 from now on
        // -------------------------- NO d_pairs --------------------------
        for ( ; j < N; j++) {
            
            d_t_value = 0;
            bcr_reader.readBCRStruct(bcr_struct);

            if (bcr_struct.id != ref_id) {
                if (bcr_struct.lcp < alpha)
                    alpha = bcr_struct.lcp;
                
                if (first) { // TODO: togliere?
                    first = false;
                    score[bcr_struct.id][ref_id] += zeta;
                    
                     #ifdef OPTION_PRINT_CLCP
                         //cout << (int) zeta << endl;
                         fprintf(thread_output_file, "%d\n", zeta);
                     #endif
                    
                }
                else if (alpha > lcp_t_value) {
                    score[bcr_struct.id][ref_id] += alpha;
                    
                     #ifdef OPTION_PRINT_CLCP
                         //cout << (int) alpha << endl;
                         fprintf(thread_output_file, "%d\n", alpha);
                     #endif
                    
                }
                else {
                    x = max(max(alpha, zeta), lcp_t_value);
                    score[bcr_struct.id][ref_id] += x;
                    
                     #ifdef OPTION_PRINT_CLCP
                        /*#if CHAR_NUM==0
                         fprintf(thread_output_file, "%d\n", x);
                        #else
                         fprintf(thread_output_file, "%lu\n", x);                         
                        #endif*/
                     #endif
                    
                }
                
            } else { // dgreco: inizio di un nuovo X-interval
                
                alpha = C_MaxSequenceLength; // dgreco: alpha=+inf; zeta=0
                zeta = 0;
                
                 #ifdef OPTION_PRINT_CLCP
                     //cout << "-" << endl;
                     fprintf(thread_output_file, "-\n");
                 #endif
                 
                 if (!lcp_reader.readLCPValue(lcp_t_value)) {

                    lcp_t_value = 0;
                    if(first) 
                        first = false;
                 }
                
            } // if (bcr_struct.id != ref_id) {
        }
        
        fclose(bcr_file);
        fclose(d_file);
        fclose(lcp_file);


        
        if (params.doFlushIntermediateFiles) {  // remove d_file_name and lcp_file_name
            
            if (params.verbose) {
                ss << "thread=" << thread_id << ": Deleting " << lcp_file_name.str() << " and " << d_file_name.str() << "..\n" << endl;
                fprintf(thread_output_file, "%s", ss.str().c_str());
                ss.str("");
            }
            
            std::remove(lcp_file_name.c_str());
            std::remove(d_pairs_file_name.c_str());
            if (params.doGenerateExtendedD)
                std::remove(d_file_name.c_str());
        
        }

        /*

        // TIMING
            time (&end);
            dif = difftime (end,start);
            ss << "\tthread=" << thread_id << ": ";
            ss << "computeColumnScore computation took " << dif << " seconds." << endl;
            fprintf(thread_output_file, "%s", ss.str().c_str());
            fflush(thread_output_file);
            ss.str("");

        */
	
    }

} /* namespace multi_acs */