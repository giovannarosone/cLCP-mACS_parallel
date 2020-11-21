#include "Reader.h"
#include "Config.h"
#include "Types.h"
#include <iostream>
#include <new>
using namespace std;

namespace multi_acs {


/*
 *	 BCR File Reader 
 */

BCRReader::BCRReader(const MultiACSParameters& params, FILE* bcr_file) {
	
	this->bcr_file = bcr_file;
	BUFFER_SIZE = params.global_buffer_size;
	struct_counter = BUFFER_SIZE;
	last_num_read = BUFFER_SIZE;

	try {

		buffer = new ElementTypeLCPda[BUFFER_SIZE];

	} catch (std::bad_alloc& execObj) {

		buffer = nullptr;
        cerr << "ERROR: BCRReader bad_alloc Exception, " << execObj.what() << endl;
        exit(EXIT_FAILURE);

	}

}

BCRReader::~BCRReader() {
	if (buffer != NULL) {
		delete[] buffer;
		buffer = NULL;
	}
}

bool BCRReader::readBCRStruct(ElementTypeLCPda& bcr_struct) {

	LetterNumber num_read = 0;
	if(struct_counter == last_num_read) {
		num_read = fread(buffer, sizeof(ElementTypeLCPda), BUFFER_SIZE, bcr_file);
		if(num_read == 0) {
			return false;
		} else {
			last_num_read = num_read;
		}
		struct_counter = 0;
	}
	bcr_struct = buffer[struct_counter++];
	return true;
}



/*
 *	 LCP File Reader 
 */

LCPReader::LCPReader(const MultiACSParameters& params, FILE* lcp_file) {
	
	this->lcp_file = lcp_file;
	BUFFER_SIZE = params.global_buffer_size;
	lcp_counter = BUFFER_SIZE;
	last_num_read = BUFFER_SIZE;

	try {

		buffer = new SequenceLength[BUFFER_SIZE];

	} catch (std::bad_alloc& execObj) {

		buffer = nullptr;
        cerr << "ERROR: LCPReader bad_alloc Exception, " << execObj.what() << endl;
        exit(EXIT_FAILURE);

	}
}

LCPReader::~LCPReader() {
	if (buffer != NULL) {
		delete[] buffer;
		buffer = NULL;
	}
}

bool LCPReader::readLCPValue(SequenceLength& value) {
	LetterNumber num_read = 0;
	if(lcp_counter == last_num_read) {
		num_read = fread(buffer, sizeof(SequenceLength), BUFFER_SIZE, lcp_file);
		if(num_read == 0) {
			return false;
		} else {
			last_num_read = num_read;
		}
		lcp_counter = 0;
	}
	value = buffer[lcp_counter++];
	return true;
}


/*
 *	 D File Reader PAIRS 
 */
       
DReader::DReader(const MultiACSParameters& params, FILE* d_file) {
	
	this->d_file = d_file;
	BUFFER_SIZE = params.global_buffer_size;
	d_counter = BUFFER_SIZE;
	last_num_read = BUFFER_SIZE;

	try {

		buffer = new structD[BUFFER_SIZE];

	} catch (std::bad_alloc& execObj) {

		buffer = nullptr;
        cerr << "ERROR: DReader bad_alloc Exception, " << execObj.what() << endl;
        exit(EXIT_FAILURE);

	}
}

DReader::~DReader() {
	if (buffer != NULL) {
		delete[] buffer;
		buffer = NULL;
	}
}

bool DReader::readDPair(structD& current_d_pair) {

    LetterNumber num_read = 0;

    if(d_counter == last_num_read) {
        num_read = fread(buffer, sizeof(structD), BUFFER_SIZE, d_file);
        if(num_read == 0) {
            return false;
        } else {
            last_num_read = num_read;
        }
        
        d_counter = 0;
        
    }
    
    current_d_pair = buffer[d_counter++];
    return true;
}


/*
 *	 D File Reader EXTENDED 
 */

/*
	DReader::DReader(FILE* d_file) :
				d_file(d_file) {

	}
*/

/*
	bool DReader::readDValue(SequenceLength& value) {
		LetterNumber num_read = 0;
		if(d_counter == last_num_read) {
			num_read = fread(buffer, sizeof(SequenceLength), BUFFER_SIZE, d_file);
			if(num_read == 0) {
				return false;
			} else {
				last_num_read = num_read;
			}
			d_counter = 0;
		}
		value = buffer[d_counter++];
		return true;
	}
*/

} /* namespace multi_acs */
