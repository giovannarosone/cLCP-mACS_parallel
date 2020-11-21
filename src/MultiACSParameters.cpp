#include "MultiACSParameters.h"
#include "Types.h"

namespace multi_acs {

MultiACSParameters::MultiACSParameters(const bool verbose, const bool doDGeneration, const bool doGenerateExtendedD, const bool doFlushIntermediateFiles, const uint global_buffer_size, const int input_file_format, const std::string collection_file_name)
: verbose(verbose),
  doDGeneration(doDGeneration),
  doGenerateExtendedD(doGenerateExtendedD),
  doFlushIntermediateFiles(doFlushIntermediateFiles),
  global_buffer_size(global_buffer_size),
  input_file_format(input_file_format),
  collection_file_name(collection_file_name) {
}

} /* namespace multi_acs */
