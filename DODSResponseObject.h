#ifndef  DODSResponseObject_h_
#define DODSResponseObject_h_ 1

#include <string>

using std::string ;

/** @brief Abstract base class representing a specific set of information
 * for a DODS dataset, such as DAS, DDS, DODS, DDX.
 */
class DODSResponseObject
{
public:
    DODSResponseObject() {} ;
    virtual ~DODSResponseObject() {} ;
};

#endif //DODSResponseObject_h_

