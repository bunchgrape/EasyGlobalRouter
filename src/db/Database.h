#pragma once
#include "global.h"

namespace db {
class Net;
class GCell;
class GRGrid;
struct Pin;
}  // namespace db

// FIXME:
#include "Net.h"
#include "GCellGrid.h"

namespace db {

class Database {
public:
    utils::logger* logger;
    string designName;

    int nNets;
    vector<Net*> nets;
    unordered_map<string, Net*> net_names;
    vector<vector<int>> nets_by_id;

    GRGrid grGrid;  // global routing grid

private:
    static const size_t _bufferCapacity = 128 * 1024;
    size_t _bufferSize = 0;
    char* _buffer = nullptr;

    Net* addNet(const string& name);

public:
    Database();
    ~Database();
    void clear();

    Net* getNet(const string& name);
    inline unsigned getNumNets() const { return nets.size(); }

public:
    /* defined in io/ */
    bool read(const std::string& netFile);
    bool read(const std::string& cellFile, const std::string& netFile);
    bool read(const std::string& blockFile, const std::string& netFile, const std::string& plFile);
    bool readCells(const std::string& file);
    bool readNets(const std::string& file);

    bool readBSNets(const std::string& file);
    bool readBSCell(const std::string& file);
    bool readBSPl(const std::string& file);

    bool readGRNets(const std::string& file);

    void write(ofstream& ofs);

};

} // namespace db