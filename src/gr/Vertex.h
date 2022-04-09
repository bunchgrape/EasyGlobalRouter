#include "global.h"
#include "db/Database.h"

namespace gr {

struct DirectEnum
{
    enum DirectType 
    {
        UP = 0, 
        DOWN = 1, 
        LEFT = 2,
        RIGHT = 3,
    };

    int x_off[4] = {0, 0, -1, 1};
    int y_off[4] = {1, -1, 0, 0}; 
};

struct Point {
    Point() {}
    Point(int x, int y)
        : x_(x), y_(y) {}
    int x_ = 0;
    int y_ = 0;

    bool operator==(Point a) const{
        if(a.x_ == x_ && a.y_ == y_) return true;
        else return false;
    }

    int operator-(Point b) const{
        return (abs(x_ - b.x_) + abs(y_ - b.y_));
    }
};

const DirectEnum Direction;

class Vertex {
public:
    int cost;
    Point pos;
    std::shared_ptr<Vertex> prev;

    int direction;

    Vertex(int c, Point v, const std::shared_ptr<Vertex> &p, int d) : 
                cost(c), pos(v), prev(p), direction(d) {}
};

}