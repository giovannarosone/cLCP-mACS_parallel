#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "Writer.h"
#include "Config.h"
#include "Types.h"

namespace multi_acs {

/*
 *	 LCP file Writer 
 */

LCPWriter::LCPWriter(const MultiACSParameters& params, FILE* lcp_file) {

	this->lcp_file = lcp_file;
	lcp_counter = 0;
	BUFFER_SIZE = params.global_buffer_size;

	try {

		buffer = new SequenceLength[BUFFER_SIZE];

	} catch (std::bad_alloc& execObj) {

		buffer = nullptr;
        std::cerr << "ERROR: LCPWriter bad_alloc Exception, " << execObj.what() << std::endl;
        exit(EXIT_FAILURE);

	}

}

LCPWriter::~LCPWriter() {
	if (buffer != NULL) {
		delete[] buffer;
		buffer = NULL;
	}
}

void LCPWriter::writeLCPValue(SequenceLength lcp_value) {
	if(lcp_counter == BUFFER_SIZE) {
		fwrite(buffer, sizeof(SequenceLength), BUFFER_SIZE, lcp_file);
		lcp_counter = 0;
	}
	buffer[lcp_counter] = lcp_value;
	++lcp_counter;
}

void LCPWriter::flushLCPValues() {
	if(lcp_counter) {
		fwrite(buffer, sizeof(SequenceLength), lcp_counter, lcp_file);
		lcp_counter = 0;
	}
}


/*
 *	 D file Writer 
 */

DWriter::DWriter(const MultiACSParameters& params, FILE* d_file) {

	this->d_file = d_file;
	d_counter = 0;
	BUFFER_SIZE = params.global_buffer_size;

	try {

		buffer = new SequenceLength[BUFFER_SIZE];

	} catch (std::bad_alloc& execObj) {

		buffer = nullptr;
        std::cerr << "ERROR: DWriter bad_alloc Exception, " << execObj.what() << std::endl;
        exit(EXIT_FAILURE);

	}

}

DWriter::~DWriter() {
	if (buffer != NULL) {
		delete[] buffer;
		buffer = NULL;
	}
}

void DWriter::writeDValue(SequenceLength d_value) {
	if(d_counter == BUFFER_SIZE) {
		fwrite(buffer, sizeof(SequenceLength), BUFFER_SIZE, d_file);
		d_counter = 0;
	}
	buffer[d_counter] = d_value;
	++d_counter;
}

void DWriter::writeLCPInterval(stackedLCPInterval lcp_interval,
		LetterNumber last_pos) {
	LetterNumber to_write = lcp_interval.pos - last_pos + (last_pos ? 0 : 1);
	if(d_counter + to_write > BUFFER_SIZE) {
		LetterNumber remaining = BUFFER_SIZE - d_counter;
		to_write -= remaining;
		for(LetterNumber i = 0; i < remaining; ++i)
			buffer[d_counter++] = 0;
		fwrite(buffer, sizeof(SequenceLength), d_counter, d_file);
		d_counter = 0;
		while(to_write > 0) {
			if(to_write < BUFFER_SIZE) {
				for(LetterNumber i = 0; i < to_write - 1; ++i)
					buffer[d_counter++] = 0;
				buffer[d_counter++] = lcp_interval.lcp + 1;
				to_write = 0;
			}
			else {
				for(LetterNumber i = 0; i < BUFFER_SIZE; ++i)
					buffer[i] = 0;
				fwrite(buffer, sizeof(SequenceLength), BUFFER_SIZE, d_file);
				to_write -= BUFFER_SIZE;
			}
		}

	}
	else {
		for(LetterNumber i = 0; i < to_write - 1; ++i)
			buffer[d_counter++] = 0;
		buffer[d_counter++] = lcp_interval.lcp + 1;
	}
}

void DWriter::fillWithZeros(LetterNumber zeros_num) {
	if(d_counter + zeros_num > BUFFER_SIZE) {
		LetterNumber remaining = BUFFER_SIZE - d_counter;
		zeros_num -= remaining;
		for(LetterNumber i = 0; i < remaining; ++i)
			buffer[d_counter++] = 0;
		fwrite(buffer, sizeof(SequenceLength), d_counter, d_file);
		d_counter = 0;
		while(zeros_num > 0) {
			if(zeros_num < BUFFER_SIZE) {
				for(LetterNumber i = 0; i < zeros_num; ++i)
					buffer[d_counter++] = 0;
				zeros_num = 0;
			}
			else {
				for(LetterNumber i = 0; i < BUFFER_SIZE; ++i)
					buffer[i] = 0;
				fwrite(buffer, sizeof(SequenceLength), BUFFER_SIZE, d_file);
				zeros_num -= BUFFER_SIZE;
			}
		}
	}
	else {
		for(LetterNumber i = 0; i < zeros_num; ++i)
			buffer[d_counter++] = 0;
	}
}

void DWriter::flushDValues() {
	if(d_counter) {
		fwrite(buffer, sizeof(SequenceLength), d_counter, d_file);
		d_counter = 0;
	}
}

} /* namespace multi_acs */
