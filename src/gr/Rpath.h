#include "global.h"
#include "db/Database.h"

namespace gr {

class Rpath {
public:
    vector<vector<int>> demH;
    vector<vector<int>> demV;
    db::GRGrid grGrid;
    vector<db::Net*> net_queue;

public:
};

}