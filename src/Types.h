#ifndef TYPES_H_
#define TYPES_H_

#include <vector>
#include <cassert>
#include "Config.h"


namespace multi_acs {

enum InputFileFormat : int {
	BCR = 0,
	EGSA = 1 // not supported
};

typedef unsigned char uchar; 	// 1 Byte
typedef unsigned short ushort; 	// 2 Byte
typedef unsigned int uint; 		// 2-4 Byte
typedef unsigned long ulong; 	// 4-8 Byte

// Type to represent: Symbol of the sequences alphabet (Typically {A, C, G, T, $})
typedef uchar AlphabetSymbol;

// Type to represent: Dimension of the sequences alphabet
typedef AlphabetSymbol AlphabetSize;

// Type to represent: Length of a sequence / LCP value
#if SEQ_LEN==0
typedef uchar SequenceLength;
#elif SEQ_LEN==1
typedef ushort SequenceLength;
#else
typedef uint SequenceLength;
#endif

// Type to represent: Number of sequences
#if SEQ_NUM==0
typedef uint SequenceNumber;
#else
typedef ulong SequenceNumber;
#endif

// Type to represent: Number of characters in EBWT (position)
#if CHAR_NUM==0
typedef uint LetterNumber;
#else
typedef ulong LetterNumber;
#endif

// Type to represent: Value of distance between sequences
typedef double DistanceValue;


// Element of the mACS input file (LCP array + ID array of the global collection)    
#if defined(__linux__) && (__GNUG__)           // compiled on linux with GNU GCC

    #pragma pack(1)
    typedef struct {

        SequenceLength lcp;     // position in the sequence, so it goes from 0 a length read
        SequenceNumber id;      // number of the sequence ("colour")

    } ElementTypeLCPda;

#elif defined(__APPLE__) && defined(__clang__) // compiled on Darwin (macOS) with LLVM clang

    typedef struct __attribute__((packed)) {    
        SequenceLength lcp;     // position in the sequence, so it goes from 0 a length read
        SequenceNumber id;      // number of the sequence ("colour")
    } ElementTypeLCPda;
    
#endif


struct ElementTypeLCPda2 {
    SequenceLength lcp;     // position in the sequence, so it goes from 0 a length read
    SequenceNumber id;      // number of the sequence ("colour")
};
        

// Element of the stack used in D generation
struct stackedLCPInterval {
	LetterNumber pos;
	SequenceLength lcp;
};


typedef struct  {
    SequenceLength lcpCurrent;
    LetterNumber posCurrent;
} structD;


const AlphabetSymbol C_MaxAlphabetSize(static_cast<AlphabetSize>(-1));
const SequenceNumber C_MaxSequenceNumber(static_cast<SequenceNumber>(-1));
const SequenceLength C_MaxSequenceLength(static_cast<SequenceLength>(-1));
const LetterNumber C_MaxLetterNumber(static_cast<LetterNumber>(-1));

    
} /* namespace multi_acs */

#endif /* TYPES_H_ */
