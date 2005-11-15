#ifndef  DODSResponseObject_h_
#define DODSResponseObject_h_ 1

#include <string>

using std::string ;

/** @brief Abstract base class representing a specific set of information
    for a DODS dataset (currently DAS and DDS).
    
    This class' sole purpose is to serve as a base class for DAS and DDS.
    Code in the OPeNDAP/HOA Back End Server (BES) uses this to pass pointers
    to DAS and DDS objects from one place to another. 
    
    \note The DDX and 'DODS' objects don't exist. The DDX response is generated
    using a DDS object. Optionally the information from a DAS can be merged
    into the DDS beforehand. The 'DODS' object is generated from a DDS once
    data values are read into it.
 */
class DODSResponseObject
{
public:
    DODSResponseObject() {} ;
    virtual ~DODSResponseObject() {} ;
};

#endif //DODSResponseObject_h_

