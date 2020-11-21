#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

namespace multi_acs {

// Type dimension for Sequences Length
/* USE: 0 - below 255 (uint8_t/unsigned char)
 *		1 - between 256 and 65.536 (uint16_t/unsigned short)
 *		2 - between 65.536 and 4.294.967.296 (uint32_t/unsigned int)
 */
#define SEQ_LEN 2

// Type dimension for Number of Sequences
/* USE: 0 - below 4.294.967.296 (uint32_t/unsigned int)
 *		1 - otherwise (uint64_t/unsigned long)
 */
#define SEQ_NUM 0

// Type dimension for Number of Character/Letter
/* USE: 0 - below 4.294.967.296 sequences (uint32_t/unsigned int)
 *		1 - otherwise (uint64_t/unsigned long)
 */
//#define CHAR_NUM 1
#define CHAR_NUM 1

// Explicit construction of the Upper cLCP?
/* USE: 0 - NO
 *		1 - YES
 */
#define BUILD_UCLCP 0

// Explicit construction of the Lower cLCP?
/* USE: 0 - NO
 *		1 - YES
 */
#define BUILD_LCLCP 0

// Explicit construction of the cLCP?
/* USE: 0 - NO
 *		1 - YES
 */
#define BUILD_CLCP 0

// Delete D arrays after ACS computation?
/* USE: 0 - NO
 *		1 - YES
 */
#define DELETE_D 0

// Delete single-sequence LCP arrays after ACS computation?
/* USE: 0 - NO
 *		1 - YES
 */
#define DELETE_PARTIAL_LCP 0

// Dimension (number of elements) of Reading/Writing Buffers
#define DEFAULT_BUFFER_SIZE  149488
//#define BUFFER_SIZE  400000

// Placeholder character
#define PLACEHOLDER_CHAR '#'

// Sentinel character
#define TERMINATE_CHAR '$'
    
// File Extensions
//const std::string C_BCRFileExt{".bcr"};
const std::string C_BCRFileExt{".lcpDa"};
const std::string C_GESAFileExt{".gesa"};
const std::string C_LcpFileExt{".lcp"};
const std::string C_IdFileExt{".id"};
const std::string C_DynBlockFilePairs{".d_pairs"};
const std::string C_DynBlockFileExt{".d"};
const std::string C_CLcpFileExt{".clcp"};
const std::string C_DistanceFileExt{".acs"};
const std::string C_LengthFileExt{ ".len" };
const std::string C_InfoFileExt{ ".info" };

// Debugging flags
//#define OPTION_PRINT_CLCP	1
//#define OPTION_PRINT_SCOREM	1

} /* namespace multi_acs */

#endif /* CONFIG_H_ */
