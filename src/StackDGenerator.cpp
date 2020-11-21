#include "StackDGenerator.h"
#include "MultiACSParameters.h"
#include "Config.h"
#include "Types.h"
#include "Tools.h"
#include "Reader.h"
#include "Writer.h"
#include "malloc_count/malloc_count.h"
#include <iostream>
#include <sstream>
#include <list>
#include <algorithm>
#include <string>
#include <cstdio>
#include <new>
using namespace std;

namespace multi_acs {


StackDGenerator::StackDGenerator(const MultiACSParameters& params,
	const SequenceNumber seq_num, const LetterNumber N) :
				params(params), seq_num(seq_num), N(N) {

}


void StackDGenerator::checkForDFiles() {

	for (SequenceNumber i = 0; i < seq_num; ++i) {

		FileName d_file_name(params.collection_file_name, i , C_DynBlockFileExt);
		FILE* d_file = fopen(d_file_name.c_str(), "rb");
		if (d_file == nullptr) {
			ostringstream err_message;
			err_message << "Couldn't open file " << d_file_name.str();
			Error::stopWithError(C_StackDGenerator_ClassName, __func__, err_message.str());
		}
		if (params.verbose) {
			cout << d_file_name.str() << " FOUND" << endl;
		}
		fclose(d_file);

		FileName lcp_file_name(params.collection_file_name, i, C_LcpFileExt);
		FILE* lcp_file = fopen(lcp_file_name.c_str(), "rb");
		if (lcp_file == nullptr) {
			ostringstream err_message;
			err_message << "Couldn't open file " << lcp_file_name.str();
			Error::stopWithError(C_StackDGenerator_ClassName, __func__, err_message.str());
		}
		if (params.verbose) {
			cout << lcp_file_name.str() << " FOUND" << endl;
		}
		fclose(lcp_file);
	}
	cout << endl;
}


void StackDGenerator::generateSingleD(SequenceNumber ref_id, FILE* thread_output_file, int thread_id, stringstream &ss) {
    
	switch(params.input_file_format) {
		case InputFileFormat::BCR: 
            generateSingleDfromBCR(ref_id, thread_output_file, thread_id, ss);
			break;
        /*
		case InputFileFormat::EGSA:	
            ostringstream err_message;
            err_message << "GESA format to be implemented.";
            Error::stopWithError(C_StackDGenerator_ClassName, __func__, err_message.str());
        */
		default: 	
            ostringstream err_message;
			err_message << "Unrecognized file format";
			Error::stopWithError(C_StackDGenerator_ClassName, __func__, err_message.str());
	}
    
}


// -----------------------------------------------------------------------------

void StackDGenerator::generateSingleDfromBCR(SequenceNumber refID, FILE* thread_output_file, int thread_id, stringstream &ss) 
{
    SequenceNumber nSeq = this->seq_num;
    LetterNumber lengthBWT = this->N;

    /******************************* CODICE GIOVANNA *************************/
    
    /*                                           */
    /*  START TIMING: D Array Pairs generation   */
    /*                                           */

    FileName        bcr_file_name(params.collection_file_name, C_BCRFileExt);
    string          fileInput = string(params.collection_file_name);
    LetterNumber    numcharEle = 0;
    ElementTypeLCPda *bufferEle = nullptr;
    FILE             *InFileLCPDa = NULL;
    unsigned int    BUFFER_SIZE;


    // Allocating memory for reading LCP+DA file
    try {

        BUFFER_SIZE = params.global_buffer_size;
        bufferEle = new ElementTypeLCPda[BUFFER_SIZE];

    } catch (std::bad_alloc& execObj) {
  
        cerr << "ERROR: StackDGenerator bad_alloc Exception, " << execObj.what() << endl;
        exit(EXIT_FAILURE);
  
    }


    // Open the .lcpDA file
    InFileLCPDa = fopen(bcr_file_name.c_str(), "rb");
    if (! InFileLCPDa) {

        ss << "thread=" << thread_id << ": ERROR opening " << bcr_file_name.c_str() << endl;
        fprintf(thread_output_file, "%s", ss.str().c_str());
        fflush(thread_output_file);
        exit (EXIT_FAILURE);

    }

    
    // Setting up the output .d file where to write the D pairs
    string fnDarray = fileInput + std::to_string(refID) + multi_acs::C_DynBlockFilePairs + "\0";

    FILE *InFileDarray = fopen(fnDarray.c_str(), "wb");
    if (! InFileDarray) {

        ss << "thread=" << thread_id << ": build_arrayD: Error opening " << fnDarray << "." << endl;
        fprintf(thread_output_file, "%s", ss.str().c_str());
        fflush(thread_output_file);        
        exit (EXIT_FAILURE);

    }

    
    // Setting up the output .lcp file where to write the LCP values
    string fnSingleLCP = fileInput + std::to_string(refID) +".lcp\0";
    FILE *OutFileSingleLCP = fopen(fnSingleLCP.c_str(), "wb");

    if (! OutFileSingleLCP) {

        ss << "thread=" << thread_id << ": build_arrayD: Error opening " << fnSingleLCP << "." << endl;
        fprintf(thread_output_file, "%s", ss.str().c_str());
        fflush(thread_output_file);        
        exit (EXIT_FAILURE);

    }


    //
    // D PAIRS computation starts here
    //

    structD eleStructD;
    std::vector <structD> vectStructD;

    //Read the first nSeq elements: they are associated to the suffixes containing only the end-marker.
    //There exists an element for each sequence
    //lcp values are 0    
    
    if (nSeq < BUFFER_SIZE) {

        numcharEle = fread(bufferEle, sizeof(ElementTypeLCPda), nSeq+1, InFileLCPDa);

        #ifdef DEBUG_STACKD
            ss << "thread=" << thread_id << ": ";
            ss << "(nSeq < BUFFER_SIZE) numcharEle=" << numcharEle << ", nSeq+1=" << nSeq+1 << endl;
            fprintf(thread_output_file, "%s", ss.str().c_str());
            fflush(thread_output_file);
            ss.str("");
        #endif

        assert(numcharEle == nSeq+1);

        #ifdef DEBUG_STACKD
            ss << "(ciclo FOR) thread=" << thread_id << ": ";
            for (LetterNumber j=0; j < numcharEle; j++) {
                    ss << "( "  << bufferEle[j].id <<" , "  << bufferEle[j].lcp << ")" <<  endl;
                    // std::cerr <<"( "  << bufferEle[j].dd <<" , "  << bufferEle[j].lcp << ")" <<  std::endl;
            }
            fprintf(thread_output_file, "%s", ss.str().c_str());
            fflush(thread_output_file);
            ss.str("");
        #endif
    } else { // END OF: (nSeq < BUFFER_SIZE)

        #ifdef DEBUG_STACKD
            ss << "(nSeq >= BUFFER_SIZE) thread=" << thread_id << ": ";
            ss << nSeq+1 << "<" << BUFFER_SIZE  <<  endl;
            fprintf(thread_output_file, "%s", ss.str().c_str());
            fflush(thread_output_file);
            ss.str("");
        #endif

        assert(nSeq+1 < BUFFER_SIZE);
    }
    
    //The LCP value for the suffix starting with $ is 0 by definition!
    SequenceLength val=0;
    fwrite(&val,sizeof(SequenceLength),1,OutFileSingleLCP);

    #ifdef DEBUG_STACKD
        ss << "thread=" << thread_id << ": ";
        ss << "LCP_"<< refID << "[" << 0 << "] = "  << (int)val << std::endl;
        fprintf(thread_output_file, "%s", ss.str().c_str());
        fflush(thread_output_file);
        ss.str("");
    #endif

    
    SequenceNumber IdSeqNow=0;
    SequenceLength LcpNow=0, minLcpValue=0;
    LetterNumber posMinLCP=0;
    SequenceNumber IdSeqPrev=0;
    SequenceLength LcpPrev=0;
    LetterNumber numberEleRead=0;
    LetterNumber posInLCParray=0;
    bool setPrev;
    bool setNow;
    
    //Read the first element does not associated to the suffixes containing only the end-marker.
    //For intance, associated to the first suffix A$.
    //lcp value is 0
    
    IdSeqNow = bufferEle[numcharEle-1].id;
    LcpNow = bufferEle[numcharEle-1].lcp;

    LcpPrev = LcpNow;   //it is the LCP value of the first element (no $-suffix), possibly 0
    IdSeqPrev = IdSeqNow; //it is the ID seq of the first element (no $-suffix)

    setPrev = isRef (IdSeqPrev, refID);
    setNow = isRef (IdSeqNow, refID);    //Note that in this step: IdSeqPrev = IdSeqNow;
    
    #ifdef DEBUG_STACKD
        ss << "thread=" << thread_id << ": ";
        ss << "( "  << IdSeqNow <<" , "  << LcpNow << ")" <<  std::endl;
        fprintf(thread_output_file, "%s", ss.str().c_str());
        fflush(thread_output_file);
        ss.str("");
    #endif
    
    if (setPrev == 0)  { //it is idseq == refID // TODO
        
        fwrite(&LcpPrev,sizeof(SequenceLength),1,OutFileSingleLCP);

        #ifdef DEBUG_STACKD
            ss << "thread=" << thread_id << ": ";
            ss << "LCP_"<< refID << "[" << 0 << "] = "  << (int)val << std::endl;
            fprintf(thread_output_file, "%s", ss.str().c_str());
            fflush(thread_output_file);
            ss.str("");
        #endif
    }
    
    eleStructD.posCurrent = nSeq;
    eleStructD.lcpCurrent = LcpNow;
    vectStructD.push_back(eleStructD);

    LetterNumber leftToReadObj = lengthBWT - nSeq - 1;         //nObjInCycle;
    
    //The position of the first no $-symbol
    numberEleRead = numcharEle-1;
    posInLCParray = numcharEle-1;
    
    #ifdef DEBUG_STACKD
        ss << "thread=" << thread_id << ": ";
        ss << "FIRST (no-$ suffix)\n isSetNow " << (int)setNow << " IdSeqNow= "  << IdSeqNow <<"\tLcpNow= "  << (int)LcpNow <<"\tposInLCParray= "  << posInLCParray <<"\tnumberEleRead= "  << numberEleRead <<  std::endl;
        fprintf(thread_output_file, "%s", ss.str().c_str());
        fflush(thread_output_file);
        ss.str("");
    #endif
    
    SequenceLength minLCPinColoredBlock=0;
    
    while ((numcharEle!=0) && (leftToReadObj > 0)) 
    {
        if (leftToReadObj >0) {
            if (leftToReadObj < BUFFER_SIZE) {

                #ifdef DEBUG_STACKD
                    cout << "(leftToReadObj < BUFFER_SIZE): BEFORE read: numcharEle=" << numcharEle << ", leftToReadObj=" << leftToReadObj << "." << endl;
                #endif

                numcharEle = fread(bufferEle, sizeof(ElementTypeLCPda), leftToReadObj, InFileLCPDa);
                assert(numcharEle == leftToReadObj);

                #ifdef DEBUG_STACKD
                    cout << "(leftToReadObj < BUFFER_SIZE): AFTER read: numcharEle=" << numcharEle << ", leftToReadObj=" << leftToReadObj << "." << endl;
                #endif

            } else {

                #ifdef DEBUG_STACKD
                    cout << "(leftToReadObj >= BUFFER_SIZE): BEFORE read: numcharEle=" << numcharEle << ", leftToReadObj=" << leftToReadObj << "." << endl;
                #endif

                numcharEle = fread(bufferEle, sizeof(ElementTypeLCPda), BUFFER_SIZE, InFileLCPDa);
                assert(numcharEle == BUFFER_SIZE);

                #ifdef DEBUG_STACKD
                    cout << "(leftToReadObj >= BUFFER_SIZE): AFTER read: numcharEle=" << numcharEle << ", leftToReadObj=" << leftToReadObj << "." << endl;
                #endif

            }
        } // END OF: if (leftToReadObj >0) 
        

        //For each element in the buffer
        for (LetterNumber obj=0; obj < numcharEle; obj++) 
        {
            #ifdef DEBUG_STACKD
                ss << "thread=" << thread_id << ": ***Reading - obj= "  << obj <<  endl;
                fprintf(thread_output_file, "%s", ss.str().c_str());
                fflush(thread_output_file);
                ss.str("");
            #endif

            IdSeqNow = bufferEle[obj].id;
            LcpNow = bufferEle[obj].lcp;            
            setNow = isRef (IdSeqNow, refID);

            #ifdef DEBUG_STACKD
                ss << "thread=" << thread_id << ": ";
                ss << obj+nSeq+1 <<" --> ( "  << IdSeqNow <<" , "  << LcpNow << ")" <<  std::endl;
                fprintf(thread_output_file, "%s", ss.str().c_str());
                fflush(thread_output_file);
                ss.str("");
            #endif

            if (setNow == 0)  { //it is idseq == refID
                if (setNow == setPrev) {  //Two consecutive suffixes belonging to refID
                    fwrite(&LcpNow,sizeof(SequenceLength),1,OutFileSingleLCP);

                    #ifdef DEBUG_STACKD
                        ss << "thread=" << thread_id << ": ";
                        ss << "LCP_"<< refID << "[" << 0 << "] = "  << (int)LcpNow << std::endl;
                        fprintf(thread_output_file, "%s", ss.str().c_str());
                        fflush(thread_output_file);
                        ss.str("");
                    #endif

                } else { 

                    if (LcpNow < minLcpValue) { //The previous suffix is not refID and LcpNow < minLcpValue
                        fwrite(&LcpNow,sizeof(SequenceLength),1,OutFileSingleLCP);

                        #ifdef DEBUG_STACKD
                            ss << "thread=" << thread_id << ": ";
                            ss << "LCP_"<< refID << "[" << 0 << "] = "  << (int)LcpNow << std::endl;
                            fprintf(thread_output_file, "%s", ss.str().c_str());
                            fflush(thread_output_file);
                            ss.str("");
                            //std::cerr <<"LCP_"<< refID << "[" << 0 << "] = "  << (int)LcpNow << std::endl;
                        #endif
                    } else {  //The previous suffix is not refID and LcpNow >= minLcpValue

                        fwrite(&minLcpValue,sizeof(SequenceLength),1,OutFileSingleLCP);

                        #ifdef DEBUG_STACKD
                            ss << "thread=" << thread_id << ": ";
                            ss << "LCP_"<< refID << "[" << 0 << "] = "  << (int)minLcpValue << std::endl;
                            fprintf(thread_output_file, "%s", ss.str().c_str());
                            fflush(thread_output_file);
                            ss.str("");
                            //std::cerr <<"LCP_"<< refID << "[" << 0 << "] = "  << (int)minLcpValue << std::endl;
                        #endif
                    } // END OF: if (lcpNow < minLcpValue)
                } // END OF: if (setNow == setPrev) 
            } // END OF: if (setNow == 0)

            posInLCParray++;
            numberEleRead++;
                
            #ifdef DEBUG_STACKD
                ss << "thread=" << thread_id << ": ";
                ss << "isSetNow= "  << (int)setNow << " IdSeqNow= "  << IdSeqNow <<"\tLcpNow= "  << (int)LcpNow <<"\tposInLCParray= "  << posInLCParray <<"\tnumberEleRead= "  << numberEleRead <<  std::endl;
                fprintf(thread_output_file, "%s", ss.str().c_str());
                fflush(thread_output_file);
                ss.str("");
            #endif
                
            structD ele;
                
            if (LcpNow > 0) {
                if (setNow == setPrev) {
                        
                    #ifdef DEBUG_STACKD
                        ss << "thread=" << thread_id << ": ";
                        ss << "\t Same color" <<  std::endl;
                        fprintf(thread_output_file, "%s", ss.str().c_str());
                        fflush(thread_output_file);
                        ss.str("");
                    #endif
                        
                    if (LcpNow > minLCPinColoredBlock) {  //in monocromatic block minLCPinColoredBlock=0

                        if (LcpNow < minLcpValue) {
                            //we have to reset the minLcpValue, the previous elements can be ignored
                            //the stack must be emptied
                            vectStructD.resize(0);
                            //vectStructD.shrink_to_fit();
                            
                            //set the minimum again in the last position (we are in monocromatic block)
                            //the last read element
                            numberEleRead = posInLCParray;
                            minLcpValue = LcpNow;
                            posMinLCP = numberEleRead;
                        } else {

                            //Avoid lcp-values duplicated in the stack
                            if  (vectStructD.size() > 0) {   //In the stack we have at least an element, we check duplicates
                                if (LcpNow != vectStructD[vectStructD.size()-1].posCurrent) {
                                    eleStructD.posCurrent = posInLCParray;
                                    eleStructD.lcpCurrent = LcpNow;
                                    vectStructD.push_back(eleStructD);
                                } else {
                                        numberEleRead--;   //we do not consider this suffix
                                } // END OF: if (LcpNow != vectStructD[vectStructD.size()-1].posCurrent))
                            } // END OF if (vectStructD.size() > 0)
                        } // END OF(LcpNow < minLcpValue)

                    } else {  // if (LcpNow > minLCPinColoredBlock)

                        //we have to reset the minLcpValue, the previous elements can be ignored
                        //the stack must be emptied
                        vectStructD.resize(0);
                            
                        //set the minimum again in the last position (we are in monocromatic block)
                        //the last read element

                        numberEleRead = posInLCParray;
                        minLCPinColoredBlock = LcpNow;  

                        minLcpValue = LcpNow;
                        posMinLCP = numberEleRead;
                        eleStructD.posCurrent = posInLCParray;
                        eleStructD.lcpCurrent = LcpNow;
                        vectStructD.push_back(eleStructD);                            
                    } // END: if (LcpNow > minLCPinColoredBlock)
                        
                } else {  // if (setNow == setPrev)  //First element associated to a different sequence (become colored block)

                    //Read the element in the stack (old monocromatic block)

                    #ifdef DEBUG_STACKD
                        ss << "thread=" << thread_id << ": ";
                        ss << "\t Different color" <<  std::endl;
                        fprintf(thread_output_file, "%s", ss.str().c_str());
                        fflush(thread_output_file);
                        ss.str("");
                    #endif

                    minLcpValue = LcpNow;
                    posMinLCP = numberEleRead-1;
                    
                    ele=vectStructD[vectStructD.size()-1];

                    if ( (ele.lcpCurrent < LcpNow)  ){
                        minLcpValue = LcpNow;
                        posMinLCP = numberEleRead-1; //Assume that the minLCP is in the last position such that I have the same color

                        #ifdef DEBUG_STACKD
                            ss << "thread=" << thread_id << ": ";
                            ss << "\tSet minLcpValue= "  << minLcpValue <<"\tposInLCParray= "  << posInLCParray <<  std::endl;
                            fprintf(thread_output_file, "%s", ss.str().c_str());
                            fflush(thread_output_file);
                            ss.str("");
                        #endif
                    }

                    #ifdef DEBUG_STACKD
                        ss << "thread=" << thread_id << ": ";
                        ss << "\tvectStructD.size()= "  << vectStructD.size() <<  std::endl;
                        fprintf(thread_output_file, "%s", ss.str().c_str());
                        fflush(thread_output_file);
                        ss.str("");
                    #endif
                    
                    bool ok=0;
                    //Empty the stack
                    LetterNumber dim =vectStructD.size();
                    for (LetterNumber i=0; i < dim; i++) {

                        LetterNumber pos = dim-i-1;
                        ele=vectStructD[pos];

                        #ifdef DEBUG_STACKD
                            ss << "thread=" << thread_id << ": ";
                            ss << "\t\t Svuota stack i= "  << pos << " ele.lcpCurrent= " << ele.lcpCurrent << " numberEleRead-1= " << numberEleRead-1 << std::endl;
                            fprintf(thread_output_file, "%s", ss.str().c_str());
                            fflush(thread_output_file);
                            ss.str("");
                        #endif
                            
                        if (ele.lcpCurrent < minLcpValue) {
                            ok=1;
                            
                            structD eleDarray;
                            eleDarray.posCurrent = numberEleRead-1;
                            eleDarray.lcpCurrent = minLcpValue;
                            arrayD_ext.push_back(eleDarray);

                            minLcpValue = ele.lcpCurrent;
                            posMinLCP = numberEleRead-1;

                            #if (DEBUGMODE == 1)
                                ss << "thread=" << thread_id << ": ";
                                ss << "\t\t-->D["  << numberEleRead-1 << "]=" << minLcpValue << std::endl << "\t\tSet minLcpValue= "  << minLcpValue <<"\tposInLCParray= "  << posInLCParray <<  std::endl;
                                fprintf(thread_output_file, "%s", ss.str().c_str());
                                fflush(thread_output_file);
                                ss.str("");
                            #endif
                        }

                        numberEleRead--;
                    }  //end for


                    //TO DO - STORE IN FILE
                    dim =arrayD_ext.size();

                    for (LetterNumber i=0; i < dim; i++) {
                        LetterNumber pos = dim-i-1;
                        eleStructD = arrayD_ext[pos];

                        #ifdef DEBUG_STACKD
                            ss << "thread=" << thread_id << ": ";
                            ss << "\t\t---->D["  << eleStructD.posCurrent << "]=" << eleStructD.lcpCurrent << std::endl << "\t\tSet minLcpValue= "  << minLcpValue << "\tposInLCParray= "  << posInLCParray << endl;
                            fprintf(thread_output_file, "%s", ss.str().c_str());
                            fflush(thread_output_file);
                            ss.str("");
                        #endif
                    }

                    //sort arrayD_ext for posCurrent
                    quickSort(arrayD_ext);

                    for (LetterNumber i=0; i < dim; i++) {
                        //std::cerr <<"\t\t*****>D["  << arrayD_ext[i].posCurrent << "]=" << arrayD_ext[i].lcpCurrent << std::endl;
                        fwrite(&arrayD_ext[i], sizeof(structD), 1, InFileDarray);
                    }

                    arrayD_ext.resize(0);
                        

                    if ((minLCPinColoredBlock < LcpNow) && (ok == 0) ){

                        structD eleDarray;
                        eleDarray.posCurrent = numberEleRead;
                        eleDarray.lcpCurrent = LcpNow;
                        arrayD_ext.push_back(eleDarray);
                            
                        minLcpValue = ele.lcpCurrent;
                        posMinLCP = numberEleRead;

                        #if (DEBUGMODE == 1)
                            ss << "thread=" << thread_id << ": ";
                            ss << "\t\t-->D["  << numberEleRead-1 << "]=" << LcpNow << std::endl << "\t\tSet minLcpValue= "  << minLcpValue <<"\tposInLCParray= "  << posInLCParray <<  std::endl;
                            fprintf(thread_output_file, "%s", ss.str().c_str());
                            fflush(thread_output_file);
                            ss.str("");
                        #endif
                    } // END OF: if ((minLCPinColoredBlock < LcpNow) && (ok == 0) )
                        
                    vectStructD.resize(0);
                    //vectStructD.shrink_to_fit();
                    minLCPinColoredBlock = LcpNow;   //from now on, we must consider only LCP values greater than the minLCPinColoredBlock
                    eleStructD.posCurrent = posInLCParray;
                    eleStructD.lcpCurrent = LcpNow;
                    vectStructD.push_back(eleStructD);
                        
                    //set the minimum again in the last position (the first position of the different color)
                    //the last read element
                    numberEleRead = posInLCParray;
                    minLcpValue = LcpNow;
                    IdSeqPrev = IdSeqNow;
                    setPrev = setNow;
                    posMinLCP = numberEleRead-1;
                        
                }  //end if (IdSeqPrev != IdSeqNow)

            } else {      //if (lcpNow > 0)

                //we have to reset the minLcpValue, the previous elements can be ignored
                //the stack must be emptied
                vectStructD.resize(0);
                //vectStructD.shrink_to_fit();
                
                LcpPrev = LcpNow;   //it is the LCP value of the first element (another segment BWT), possibly 0
                IdSeqPrev = IdSeqNow; //it is the ID seq of the first element (another segment BWT)
                setPrev = setNow;
                
                //set the minimum again in the last position (we are in monocromatic block)
                //the last read element
                numberEleRead = posInLCParray;
                minLcpValue = LcpNow;
                posMinLCP = numberEleRead;
                minLCPinColoredBlock = 0;
                eleStructD.posCurrent = posInLCParray;
                eleStructD.lcpCurrent = LcpNow;
                vectStructD.push_back(eleStructD);
                
                #ifdef DEBUG_STACKD
                    ss << "thread=" << thread_id << ": ";
                    ss << "Change symbol idSeqNow="  << IdSeqNow << " setNow= " << setNow << std::endl;
                    fprintf(thread_output_file, "%s", ss.str().c_str());
                    fflush(thread_output_file);
                    ss.str("");
                #endif
                
            } // END OF: if (lcpNow > 0)      
        }   // END OF: for (LetterNumber obj=0; obj < numcharEle; obj++)
        
        leftToReadObj -= numcharEle;
    }  // END: while ((numcharEle!=0) && (leftToReadObj > 0))

    
    fclose(InFileDarray);    
    fclose(OutFileSingleLCP);
    fclose(InFileLCPDa);   
    

    // De allocating allocated memory.
    delete[] bufferEle;
    bufferEle = NULL;

}

// -----------------------------------------------------------------------------
} /* namespace multi_acs */
