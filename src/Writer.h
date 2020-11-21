#ifndef WRITER_H_
#define WRITER_H_

#include "Config.h"
#include "Types.h"
#include "MultiACSParameters.h"

namespace multi_acs {

/*
 *	 LCP file Writer 
 */

class LCPWriter {
public:
	LCPWriter(const MultiACSParameters& params, FILE* lcp_file);
	~LCPWriter();
	void writeLCPValue(SequenceLength lcp_value);
	void flushLCPValues();

private:
	FILE* lcp_file = nullptr;
	unsigned int BUFFER_SIZE;
	SequenceLength *buffer = nullptr;
	LetterNumber lcp_counter; // = 0;
};


/*
 *	 D file Writer 
 */

class DWriter {
public:
	DWriter(const MultiACSParameters& params, FILE* d_file);
	~DWriter();
	void writeDValue(SequenceLength d_value);
	void writeLCPInterval(stackedLCPInterval lcp_interval, LetterNumber last_pos);
	void fillWithZeros(LetterNumber zeros_num);
	void flushDValues();

private:
	FILE* d_file = nullptr;
	unsigned int BUFFER_SIZE;
	SequenceLength *buffer = nullptr;
	LetterNumber d_counter; // = 0;
};

} /* namespace multi_acs */

#endif /* WRITER_H_ */
