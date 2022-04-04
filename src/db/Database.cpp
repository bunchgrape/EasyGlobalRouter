#include "Database.h"

using namespace db;

/***** Database *****/
Database::Database() {
    _buffer = new char[_bufferCapacity];
}

Database::~Database() {
    delete[] _buffer;
    _buffer = nullptr;
    // for regions.push_back(new Region("default"));
} //END MODULE

//---------------------------------------------------------------------

Net* Database::addNet(const string& name) {
    // Net* net = getNet(name);
    // if (net) {
    //     printlog("Net re-defined: %s", name.c_str());
    //     return net;
    // }
    Net* net = new Net(name);
    net_names[name] = net;
    nets.push_back(net);
    return net;
} //END MODULE

//---------------------------------------------------------------------

Net* Database::getNet(const string& name) {
    unordered_map<string, Net*>::iterator mi = net_names.find(name);
    if (mi == net_names.end()) {
        return nullptr;
    }
    return mi->second;
} //END MODULE

//---------------------------------------------------------------------