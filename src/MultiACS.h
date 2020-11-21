#ifndef MULTIACS_H_
#define MULTIACS_H_

#include "MultiACSParameters.h"
#include "Types.h"
#include <vector>

namespace multi_acs {

	const char C_MultiACS_ClassName[]{ "MultiACS" };

class MultiACS {
public:
	MultiACS(const MultiACSParameters& params);
	void computeACS();
private:
	const MultiACSParameters& params;

	void computeColumnScore(std::vector<std::vector<LetterNumber>> &score, SequenceNumber ref_id, LetterNumber N, FILE* threadOutput_file);

};

} /* namespace multi_acs */

#endif /* MULTIACS_H_ */
