#ifndef READER_H_
#define READER_H_

#include "Config.h"
#include "Types.h"
#include "MultiACSParameters.h"

namespace multi_acs {

/*
 *	 BCR File Reader 
 */

class BCRReader {
public:
	BCRReader(const MultiACSParameters& params, FILE* bcr_file);
	~BCRReader();
	bool readBCRStruct(ElementTypeLCPda &bcr_struct);

private:
	FILE* bcr_file = nullptr;
	ElementTypeLCPda *buffer = nullptr;
	unsigned int BUFFER_SIZE;
	LetterNumber struct_counter;
	LetterNumber last_num_read;

//	ElementTypeLCPda buffer[BUFFER_SIZE];
//	LetterNumber struct_counter = BUFFER_SIZE;
//	LetterNumber last_num_read = BUFFER_SIZE;
};


/*
 *	 LCP File Reader 
 */

class LCPReader {
public:
	LCPReader(const MultiACSParameters& params, FILE* lcp_file);
	~LCPReader();
	bool readLCPValue(SequenceLength &value);

private:
	FILE* lcp_file = nullptr;
	SequenceLength *buffer = nullptr;
	unsigned int BUFFER_SIZE; 
	LetterNumber lcp_counter;
	LetterNumber last_num_read;
};

    
/*
 *	 D File Reader PAIRS
 */

class DReader {
public:
	DReader(const MultiACSParameters& params, FILE* d_file);
	~DReader();
	bool readDPair(structD &value);

private:
	FILE* d_file = nullptr;
	structD *buffer = nullptr;
	unsigned int BUFFER_SIZE; 
	LetterNumber d_counter;
	LetterNumber last_num_read;
};
    
    
/*
 *	 D File Reader PAIRS EXTENDED 
 */

/*
	class DReader {
	    public:
	        DReader(FILE* d_file);
	        bool readDValue(SequenceLength &value);
	        
	    private:
	        FILE* d_file = nullptr;
	        SequenceLength buffer[BUFFER_SIZE];
	        LetterNumber d_counter = BUFFER_SIZE;
	        LetterNumber last_num_read = BUFFER_SIZE;
	};
*/
    
} /* namespace multi_acs */

#endif /* READER_H_ */
