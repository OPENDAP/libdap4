#include "config.h"
#include <string>
#include <sstream>
#include <memory>
        
#include "Array.h" 
#include "Byte.h"
#include "Float32.h"
#include "Float64.h"
#include "Grid.h"
#include "Int16.h"
#include "Int32.h"
#include "Sequence.h"
#include "Str.h"
#include "Structure.h"
#include "UInt16.h"
#include "UInt32.h"
#include "Url.h"
#include "D4Group.h"
#include "D4Dimensions.h"
        
#include "D4BaseTypeFactory.h" 
#include "D4ParserSax2.h"
#include "DDS.h" 
#include "DMR.h"
#include "XMLWriter.h"
    
#include "GNURegex.h"
        
#include "util.h"

using namespace std;
using namespace libdap;

int main() {

        D4BaseTypeFactory d4_factory;
        DMR *dmr = new DMR(&d4_factory, "test_grp_d4_dim");

        D4Group* root_grp = dmr->root();
        D4Dimensions *root_dims = root_grp->dims();
        auto d4_dim_unique = make_unique<D4Dimension>("dim", 2);
        root_dims->add_dim_nocopy(d4_dim_unique.release());
        auto g_ptr = make_unique<D4Group>("g");
        auto grp = g_ptr.get();
        auto d_int32 = new Int32("var");
        auto var = new Array("var",d_int32);
        auto var_d4_dim = root_dims->find_dim("dim");
        var->append_dim(var_d4_dim);
        grp->add_var_nocopy(var);
        root_grp->add_group_nocopy(g_ptr.release());
        delete d_int32;

        XMLWriter xml;
        dmr->print_dap4(xml);
        string dmr_src = string(xml.get_doc());
cerr<<"before string print" <<endl;
cerr<<dmr_src<<endl;



        DMR *dmr_2 = new DMR(*dmr);

        delete dmr;

        XMLWriter xml2;
        if(dmr_2) {
        dmr_2->print_dap4(xml2);
        string dmr_src2 = string(xml2.get_doc());
cerr<<"second one" <<endl;
cerr<<dmr_src2<<endl;

        delete dmr_2;
        }
        else 
cerr<<"the copied cache is null " <<endl;

    return 0;
}
