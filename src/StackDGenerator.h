#ifndef STACKDGENERATOR_H_
#define STACKDGENERATOR_H_

#include "MultiACSParameters.h"
#include "Types.h"
#include "Config.h"
#include <algorithm>
#include <sstream>

// #define DEBUG_STACKD 0
// #define BUFFERSIZEcLCP 1048576    NOT USED
// #define BUFFERSIZEcLCP 4000000    NOT USED
// #define Darray_intMem 0           NOT USED
// #define BuildSingleLCP 1          NOT USED


namespace multi_acs {

const char C_StackDGenerator_ClassName[]{"StackDGeneretor"};
    
class StackDGenerator {
public:
    StackDGenerator(const MultiACSParameters& params, const SequenceNumber seq_num, const LetterNumber N);
    void generateSingleD(SequenceNumber ref_id, FILE* threadOutput_file, int thread_id, std::stringstream &ss);
    
private:
    const MultiACSParameters& params;
    const SequenceNumber seq_num;
    const LetterNumber N;
    std::vector<structD> arrayD_ext;

    void generateSingleDfromBCR(SequenceNumber ref_id, FILE* threadOutput_file, int thread_id, std::stringstream &ss);
    void checkForDFiles();
    
    bool isRef (SequenceNumber idseq, SequenceNumber refID) {
        return (idseq == refID ? 0 : 1);
    }
    
    static bool cmpSortEl (structD a, structD b) {
        return (a.posCurrent == b.posCurrent ? a.lcpCurrent < b.lcpCurrent : a.posCurrent < b.posCurrent);
    }
    
    void quickSort(std::vector< structD > &v) {
        sort(v.begin(),v.end(),cmpSortEl);
    }
};

} /* namespace multi_acs */

#endif /* STACKDGENERATOR_H_ */
