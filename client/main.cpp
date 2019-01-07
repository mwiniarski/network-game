#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>
#include <iostream>
#include <string>
#include <math.h>
#include <time.h>

#include "network.h"

using namespace std;

class Paddle {
private:
    const int _x;
    int _y;

public:
    Paddle(int x, int y) 
        : _x(x), _y(y)
    {}

    int x() { return _x; }
    int y() { return _y; }
    void up() { _y -= 5; }
    void down() { _y += 5; }

    const int SIZEX = 10;
    const int SIZEY = 50;
};

struct Ball {
    int _xVel;
    int _yVel;
    int _x;
    int _y;
    const int SIZE = 10;

    Ball(int xVel, int yVel, int x, int y) 
        :_xVel(xVel), _yVel(yVel), _x(x), _y(y)
    {}
};

class GameBoard : public Fl_Box
{
public:
    enum DIR {NONE, UP, DOWN };

    GameBoard(int x, int y, int w, int h, int playerNum)
        : Fl_Box(x, y, w, h), 
          p1(new Paddle(0, 50)), 
          p2(new Paddle(w - 10, 150)),
          ball(new Ball(4, 4, 160, 200))
    {
        box(FL_FLAT_BOX);
        color(FL_BLACK);

        if (playerNum != 0) {
            std::swap(p1, p2);
        }
    }

    int handle(int p) 
    {
        switch(p) {
            case FL_KEYDOWN:
                if(Fl::event_key() == FL_Up)
                {
                    if (direction == NONE)
                    {
                        direction = UP;          
                    }
                }
                if(Fl::event_key() == FL_Down)
                {
                    if (direction == NONE)
                    {
                        direction = DOWN;
                    }
                }
                break;                    
            case FL_KEYUP:
                direction = NONE;
            break; 
        }

        return 1;
    }

    void up() { p1->up(); }
    void down() { p1->down(); }
    DIR dir() { return direction; }

    Paddle *p1, *p2;
    Ball *ball;

private:
    void draw() {
        Fl_Box::draw();
        fl_rectf(p1->x(), p1->y(), p1->SIZEX, p2->SIZEY, FL_GREEN);
        fl_rectf(p2->x(), p2->y(), p2->SIZEX, p2->SIZEY, FL_GREEN);
        fl_rectf(ball->_x, ball->_y, ball->SIZE, ball->SIZE, FL_RED);
    }

    DIR direction = NONE;
};

void countVelocity(int &vel1, int &vel2)
{
    srand(time(NULL));
    int r = rand() % 3 - 1;
    int k = rand() % 2;
    if (k)
        vel1 += r;
    else 
        vel2 += -r;

    vel1 = min(vel1, 6);
    vel1 = max(vel1, -6);
    vel2 = min(vel2, 6);
    vel2 = max(vel2, -6);
} 

void gameLoop(void * obj)
{
    GameBoard *gb = (GameBoard *)obj;

    // Move paddle
    switch(gb->dir())
    {
    case GameBoard::UP:
        gb->up();
        break;
    case GameBoard::DOWN:
        gb->down();
        break;
    case GameBoard::NONE:
        break;
    } 

    // ball
    Ball *b = gb->ball;
    static int counter = 0;

    // borders
    if (b->_y + b->SIZE >= gb->h() || b->_y <= 0)
        b->_yVel = -b->_yVel;
    if (b->_x + b->SIZE >= gb->w() || b->_x <= 0)
        b->_xVel = -b->_xVel;
    
    // paddle
    Paddle *p1 = gb->p1, *p2 = gb->p2;

    if (counter > 10) {
        if (b->_x <= p1->SIZEX + p1->x() && b->_y <= p1->SIZEY + p1->y() && b->_y >= p1->y())
        {
            b->_xVel = abs(b->_xVel);
            countVelocity(b->_xVel, b->_yVel);
            counter = 0;
        }
            
        if (b->_x + b->SIZE >= p2->x() && b->_y <= p2->SIZEY + p2->y() && b->_y >= p2->y())
        {
            b->_xVel = -abs(b->_xVel);
            countVelocity(b->_xVel, b->_yVel);
            counter = 0;
        }
    }

    // move
    b->_x += b->_xVel;
    b->_y += b->_yVel;
    
    gb->redraw();
    counter++;

    Fl::repeat_timeout(0.02, gameLoop, obj);
}

int main() 
{
    connect();
    return 1;
    
    Fl_Window win(400, 400);
    GameBoard pb1(0, 0, 400, 400, 0);
    win.show();

    Fl::add_timeout(0.01, gameLoop, &pb1);
    return(Fl::run());
}