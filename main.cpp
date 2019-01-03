#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <iostream>
#include <string>
using namespace std;


class PlayerBox : public Fl_Box
{
    Fl_Boxtype _type = FL_FLAT_BOX;
    Fl_Color _color = FL_GREEN;
    string _name;

public:
    static int count;

    PlayerBox(int x, int y, int w, int h, string name)
        : Fl_Box(x, y, w, h), _name(name)
    {
        box(_type);
        color(_color);
    }

    void up() {
        Fl_Box::x(x() - 5);
    }

    int handle(int p) {
        
        int e = Fl_Box::handle(p);
        //cout << p << endl;
        
        switch(p) {
            case FL_KEYDOWN:
                cout << _name << endl;
                cout << Fl::event_button() << endl;
                up(); 

                return 1;
            break; 
        }

        return 1;
    }

private:
    void draw() {
        Fl_Box::draw();
    }
};

int PlayerBox::count = 0;

int main() {
     Fl_Window win(400, 400);
     PlayerBox pb1(20, 40, 15, 50, "Player 1");
     PlayerBox pb2(win.w() - 35, 140, 15, 50, "Player 2");
     win.show();
     return(Fl::run());
}

//===============================
//
// FLTK drawing example showing simple line drawing animation
// erco 03/22/07
//
// #include <FL/Fl.H>
// #include <FL/Fl_Window.H>
// #include <FL/Fl_Box.H>
// #include <FL/fl_draw.H>

// class MyTimer : public Fl_Box {
//     void draw() {
     
//         Fl_Box::draw();

//     }
// public:
//     // CONSTRUCTOR
//     MyTimer(int X,int Y,int W,int H) : Fl_Box(X,Y,W,H) {
//         box(FL_FLAT_BOX);
//         color(FL_GREEN);
//     }
// };
// // MAIN
// int main() {
//      Fl_Window win(220, 220);
//      MyTimer tim(10, 10, win.w()-20, win.h()-20);
//      win.show();
//      return(Fl::run());
// }