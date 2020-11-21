#ifndef MULTIACSPARAMETERS_H_
#define MULTIACSPARAMETERS_H_

#include "Types.h"
#include <string>

namespace multi_acs {

class MultiACSParameters {
public:
	MultiACSParameters(
			const bool verbose,
			const bool doDGeneration,
            const bool doGenerateExtendedD,
			const bool doFlushIntermediateFiles,
			const uint global_buffer_size,
			const int input_file_format,
//            const int omp_nthreads,
			const std::string collection_file_name);
	
	const bool verbose;
    const bool doDGeneration;
    const bool doGenerateExtendedD;
	const bool doFlushIntermediateFiles;
	const uint global_buffer_size;
	const int input_file_format;
//    const int omp_nthreads;
	const std::string collection_file_name;

};

} /* namespace multi_acs */

#endif /* MULTIACSPARAMETERS_H_ */
