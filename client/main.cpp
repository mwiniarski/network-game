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
#include "Paddle.h"

using namespace std;

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

        thread recvThread(&StateSender::receive, &ss, p2);
        recvThread.detach();
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

    DIR dir() { return direction; }

    Paddle *p1, *p2;
    Ball *ball;
    StateSender ss;

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
    static uint32_t counter = 0;
    GameBoard *gb = (GameBoard *)obj;
    static int py[2] = {gb->p2->y(), gb->p2->y()};
    //cout << "py: " << py << " p2: " << gb->p2->y() << endl;;
    // Move paddle 1
    switch(gb->dir())
    {
    case GameBoard::UP:
        gb->p1->up();
        break;
    case GameBoard::DOWN:
        gb->p1->down();
        break;
    case GameBoard::NONE:
        break;
    }

    // Move paddle 2
    StateSender::updt update = gb->ss.getUpdate();
    if (update.updated)
    {
        py[0] = py[1];
        py[1] = update.value;
        gb->p2->y(update.value);
    }
    else if (py[1] != py[0])
    {   
        py[1] > py[0] ? gb->p2->down() : gb->p2->up();
    }

    // Update paddle 1 state
    if (counter % 2 == 0)
        gb->ss.send(gb->p1->y());
    
    // ball
    Ball *b = gb->ball;

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
    //connect();
    //return 1;

    Fl_Window win(400, 400);
    GameBoard pb1(0, 0, 400, 400, 0);
    win.show();

    Fl::add_timeout(0.01, gameLoop, &pb1);
    return(Fl::run());
}