/**
 * MyDrawing.cpp - Implementation of event handler for sceen events
 * Author: larsonma@msoe.edu <Mitchell Larson>
 * Date: may 3 2018
 */

#include "MyDrawing.h"
#include "gcontext.h"
#include "Triangle.h"

#include <iostream>
#include <string>

const std::string filename = "image.txt";

/* 
 * This is a constructor for a MyDrawing object
 * Inputs:
 *      vc - ViewContext object for applying transformations.
 */
MyDrawing::MyDrawing(ViewContext* vc){
    this->vc = vc;
    mouseState = Mouse::RELEASED;
    color = GraphicsContext::WHITE;
    image = new Image();
    x0 = y0 = 0;
    return;
}

/* 
 * This is a Destructor for a MyDrawing object
 * Inputs:
 *      none
 */
MyDrawing::~MyDrawing(){
    delete image;
}

/* 
 * This function handles the exposure event. When this happens, the image on the screen is 
 * erased and then redrawn.
 * Inputs:
 *      gc - GraphicsContext object
 * Outputs:
 *      none
 */
void MyDrawing::paint(GraphicsContext* gc){
    gc->clear();
    image->draw(gc,vc);
}

/* 
 * This function handles the button down event. When this happens, a pixel is added to the 
 * screen and the state of the drawing process is updated.
 * Inputs:
 *      gc - GraphicsContext object
 *      button - integer key for the button pressed
 *      x - x coordinate of button press
 *      y - y coordinate of button press
 * Outputs:
 *      none
 */
void MyDrawing::mouseButtonDown(GraphicsContext* gc, unsigned int button, int x, int y){
    x0 = x;
    y0 = y;
    mouseState = Mouse::CLICKED;
}

/* 
 * This function handles the button up event. When this happens, the drawing state is checked and
 * a shape is drawn if complete.
 * Inputs:
 *      gc - GraphicsContext object
 *      button - integer key for the button pressed
 *      x - x coordinate of button press
 *      y - y coordinate of button press
 * Outputs:
 *      none
 */
void MyDrawing::mouseButtonUp(GraphicsContext* gc, unsigned int button, int x, int y){
    mouseState = Mouse::RELEASED;
}

/* 
* This function handles the mouse move event. When this happens, if the mouse is being
* pressed down, orbits of the shapes are performed in 3D. 
* Inputs:
*      gc - GraphicsContext object
*      x - x coordinate of button press
*      y - y coordinate of button press
* Outputs:
*      none
*/
void MyDrawing::mouseMove(GraphicsContext* gc, int x, int y){
    if(mouseState == Mouse::CLICKED){
        int deltaX = x - x0;
        int deltaY = y - y0;

        if(deltaX > orbitSensitivity){
            vc->hOrbit(orbitAmount);
            image->draw(gc,vc);
            x0 = x;
        }else if(deltaX < -orbitSensitivity){
            vc->hOrbit(-orbitAmount);
            image->draw(gc,vc);
            x0 = x;
        }

        if(deltaY > orbitSensitivity){
            vc->vOrbit(orbitAmount);
            image->draw(gc,vc);
            y0 = y;
        }else if(deltaY < -orbitSensitivity){
            vc->vOrbit(-orbitAmount);
            image->draw(gc,vc);
            y0 = y;
        }
    }
}

/* 
 * This function handles the key down alert. This involves switching on the keycode and
 * performing the appropriate actions, which can be viewed by displaying the help menu.
 * Inputs:
 *      gc - GraphicsContext object
 *      keycode - integer value for key that was pressed
 * Outputs:
 *      none
 */
void MyDrawing::keyDown(GraphicsContext* gc, unsigned int keycode){
    switch(keycode){
        case 'l':
        case 'L':
            loadFromFile();
            image->draw(gc,vc);
            break;
        case 65361:
            vc->translate(-20,0);
            image->draw(gc,vc);
            break;
        case 65362:
            vc->translate(0,-20);
            image->draw(gc,vc);
            break;
        case 65363:
            vc->translate(20,0);
            image->draw(gc,vc);
            break;
        case 65364:
            vc->translate(0,20);
            image->draw(gc,vc);
            break;
        case '=':
            vc->scale(2,2);
            image->draw(gc,vc);
            break;
        case '-':
            vc->scale(0.5,0.5);
            image->draw(gc,vc);
            break;
        case ',':
            vc->rotate(-10);
            image->draw(gc,vc);
            break;
        case '.':
            vc->rotate(10);
            image->draw(gc,vc);
            break;
        case 'r':
            vc->reset();
            image->draw(gc,vc);
            break;
        case 'z':
            vc->adjustFOV(10);
            image->draw(gc,vc);
            break;
        case 'x':
            vc->adjustFOV(-10);
            image->draw(gc,vc);
            break;
        default:
            printHelp();
    }
}

/* 
 * This is a helper function for saving an image to file. This function requres no inputs as it
 * uses the file scoped state variables.
 * Inputs:
 *      none
 * Outputs:
 *      none
 */
void MyDrawing::saveToFile(){
    std::ofstream myfile;
    myfile.open(filename);
    image->out(myfile);
    myfile.close();
}

/* 
 * This is a helper function for loading an image to file. This function requres no inputs as it
 * uses the file scoped state variables.
 * Inputs:
 *      none
 * Outputs:
 *      none
 */
void MyDrawing::loadFromFile(){
    std::ifstream myfile;
    std::string file = "./resources/cube.stl";
    myfile.open(file);

    if(file.substr(file.size()-3).compare("stl")==0){
        image = Image::readSTLFile(myfile);

        //add axis
        image->add(new Triangle(0,0,0,100,0,0,0,0,0,124,252,0));
        image->add(new Triangle(0,0,0,0,100,0,0,0,0,255,20,147));
        image->add(new Triangle(0,0,0,0,0,100,0,0,0,0,0,255));
    }else if(file.substr(file.size()-3).compare(".txt")==0){
        image = Image::in(myfile);
    }
    myfile.close();
}

/* 
 * This is a helper function for printing the help menu.
 * Inputs:
 *      none
 * Outputs:
 *      none
 */
void MyDrawing::printHelp(){
    std::cout << "Usage:\n"
                 "\tloading to file:\n"
                 "\t\tl - load cube.stl from file\n"
                 "\tImage Transformations:\n"
                 "\t\tup - translate up\tdown - translate down\n"
                 "\t\tleft - translate left\tright - translate right\n"
                 "\t\t= - scale by 2\t- - scale by 0.5\n"
                 "\t\t. - rotate by 10 deg\t, - rotate by -10 deg\n"
                 "\t\tr - reset transformations\n"
                 "\t\tMouse Drag:\n"
                 "\t\t\tDrag left - rotate clockwise around y axis\n"
                 "\t\t\tDrag right - rotate counter clockwise around y axis\n"
                 "\t\t\tDrag up - vertical orbit up\tDrag down - vertical orbit down\n"
                 "\t\tz - increase FOV\tx - decrease FOV\n" << std::endl;
}
