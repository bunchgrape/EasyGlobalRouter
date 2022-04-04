#pragma once

namespace db {

struct Pin
{   
    Pin() {}
    Pin(int pos_x, int pos_y)
        : pos_x(pos_x), pos_y(pos_y) {}
    int pos_x = 0;
    int pos_y = 0;
};

class Net {
private:
    string _name;
    int _id = -1;

public:
    vector<Pin*> Pins;

    Net(const std::string& name = "") : _name(name) { }
    ~Net();

    void set_id(const int i) { _id = i; }
    void set_name(const string& name) { _name = name; }

    const std::string& name() const { return _name; }
    const int id() const { return _id; }

    void addPin(Pin* pin);

    friend ostream& operator<<(ostream& os, const Net& c) {
        return os << c._name << "\n";
    }
};

}
