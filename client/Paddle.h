#ifndef PADDLE_H
#define PADDLE_H
#include <atomic>

class Paddle {
private:
    const int _x;
    int _y;

public:
    Paddle(int x, int y) 
        : _x(x), _y(y)
    {}

    const int SIZEX = 10;
    const int SIZEY = 50;
    const int VEL = 5;
    
    int x() { return _x; }
    int y() { return _y; }
    void y(int y) { _y = y; }
    void up() { _y -= VEL; }
    void down() { _y += VEL; }
};

#endif