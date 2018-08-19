/**
 * main.cpp - This is test driver for the Shapes classes
 * Author: larsonma@msoe.edu <Mitchell Larson>
 * Date: april 19 2018
 */


#include <iostream>
#include <unistd.h>
#include <fstream>
#include <fenv.h>

#include "MyDrawing.h"
#include "ViewContext.h"
#include "x11context.h"
#include "Triangle.h"
#include "Image.h"

static GraphicsContext* gc;
static ViewContext* vc;

static void initialize();
static void demo();

/* 
 * This is a driver for testing the Shapes functionality
 * 
 * Parameters:
 * 	none
 * 
 * Returns:
 *  0 if successful
 */
int main(){

    initialize();
    demo();

    delete gc;
    delete vc;
    return 0;
}

static void initialize(){
    gc = new X11Context(800,600,GraphicsContext::BLACK);
    vc = new ViewContext(50,50,0,gc->getWindowWidth()/2,gc->getWindowWidth()/2,1000);
}

static void demo(){
    gc->setColor(GraphicsContext::WHITE);

    MyDrawing md(vc);

    gc->runLoop(&md);
}
