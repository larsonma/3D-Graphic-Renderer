/**
 * ViewContext.cpp - This is an implementation of a class for performing transformations.
 * Transformations are tracked in this class with the ability to undo them.
 * Author: larsonma@msoe.edu <Mitchell Larson>
 * Date: may 3 2018
 */

#include "ViewContext.h"

/* 
* This is a constructor for the ViewContext object. The ViewContext object requires that the reference
* point is specified so that 3D rendering can be accomplished. Additionally, the center of the screen
* and FOV must be specified
* Inputs:
*      x0 - x coordinate of reference point
*      y0 - y coordinate of reference point
*      z0 - z coordinate of reference point
*      x - x coordinate for the center of the screen
*      y - y coordinate for the center of the screen
*      zf - FOV value
* Outputs:
*      Pointer to ViewContext Object
*/
ViewContext::ViewContext(int x0, int y0, int z0, int x, int y, double zf){
    this->zf = zf;
    hdeg = vdeg = 0;

    p0 = new matrix(3,1);
    pref = new matrix(3,1);

    (*p0)[0][0] = x0;   (*pref)[0][0] = 0;
    (*p0)[1][0] = y0;   (*pref)[1][0] = 0;
    (*p0)[2][0] = z0;   (*pref)[2][0] = 0;

    createTransformMatricies();
    transformBasis();
    originToCenter(x,y);
}

/* 
 * This function handles destroying the ViewContext object, with its underyling data structures.
 * Inputs:
 *      gc - GraphicsContext object
 * Outputs:
 *      none
 */
ViewContext::~ViewContext(){
    delete changeBasisMatrix;
    delete toModelCoordinates;
    delete toDeviceCoordinates;
    delete translateToOrigin;
    delete translateFromOrigin;
    delete hOrbitMatrix;
    delete vOrbitMatrix;
}

/* 
 * This function converts the device coordinates observed on the screen into model coordinates. Essentially,
 * this undoes the transformations that have been applied to the screen so that the shape can be properly
 * created with its correct coordinates.
 * 
 * Inputs:
 *      shapeVerticies - matrix containing the verticies of the shape.
 * Outputs:
 *      matrix* - pointer to transformed matrix object
 */
matrix* ViewContext::deviceToModel(matrix* shapeVerticies){
    matrix* deviceCoordinates = new matrix(4,3);

    *deviceCoordinates = (*changeBasisMatrix * *shapeVerticies);

    return deviceCoordinates;
}

/* 
 * This function converts the model coorinates of a shape to the device coordinates that will be displayed
 * on the screen. This applies the transformation matrix to the shape.
 * 
 * Inputs:
 *      shapeVerticies - matrix containing the verticies of the shape.
 * Outputs:
 *      matrix* - pointer to transformed matrix object
 */
matrix* ViewContext::modelToDevice(matrix* shapeVerticies){
    matrix* deviceCoordinates = new matrix(4,3);

    *deviceCoordinates = (*hOrbitMatrix * *shapeVerticies);
    *deviceCoordinates = (*vOrbitMatrix * *deviceCoordinates);
    *deviceCoordinates = (*changeBasisMatrix * *deviceCoordinates);
    project(deviceCoordinates);
    *deviceCoordinates = *toDeviceCoordinates * *deviceCoordinates;

    return deviceCoordinates;
}

/**
 * This function acepts the 3D matrix and projects the points into 2D space. This changes the
 * x and y values, but does not change the z values.
 * 
 * Inputs:
 *      a - pointer to matrix on which to perform 2D projection
 * Outputs:
 *      none, but contents of a are changed
 */
void ViewContext::project(matrix* a){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 2; j++){
            (*a)[j][i] = (zf * (*a)[j][i]) / (std::abs((*a)[2][i]) + zf);
        }
    }
}

/* 
 * This function applies a scale to the exisiting transformation matrix, while simultaneously updating the 
 * inverse transformation matrix.
 * 
 * Inputs:
 *      a - scale factor in x direction
 *      b - scale factor in y direction
 * Outputs:
 *      none
 */
void ViewContext::scale(double a, double b){
    matrix scale(4,4);
    matrix undoScale(4,4);

    scale[0][0] = a;
    scale[1][1] = b;
    scale[2][2] = 1;
    scale[3][3] = 1;

    undoScale[0][0] = 1/a;
    undoScale[1][1] = 1/b;
    undoScale[2][2] = 1;
    undoScale[3][3] = 1;

    *toDeviceCoordinates = *translateFromOrigin * scale * *translateToOrigin * *toDeviceCoordinates;
    *toModelCoordinates = *toModelCoordinates * *translateFromOrigin * undoScale * *translateToOrigin;
}

/* 
 * This function applies a rotate matrix to the exisiting transformation matrix, while simultaneously updating the 
 * inverse transformation matrix.
 * 
 * Inputs:
 *      deg - degrees to rotate
 * Outputs:
 *      none
 */
void ViewContext::rotate(double theta_deg){
    matrix rotate(4,4);
    matrix undoRotate(4,4);

    double theta = theta_deg * (PI/180.0);

    rotate[0][0] = std::cos(theta);
    rotate[0][1] = -1 * std::sin(theta);
    rotate[1][0] = std::sin(theta);
    rotate[1][1] = std::cos(theta);
    rotate[2][2] = 1;
    rotate[3][3] = 1;

    undoRotate[0][0] = std::cos(-theta);
    undoRotate[0][1] = -1 * std::sin(-theta);
    undoRotate[1][0] = std::sin(-theta);
    undoRotate[1][1] = std::cos(-theta);
    undoRotate[2][2] = 1;
    undoRotate[3][3] = 1;

    *toDeviceCoordinates = (*translateFromOrigin * rotate * *translateToOrigin * *toDeviceCoordinates);
    *toModelCoordinates = (*toModelCoordinates * *translateFromOrigin * undoRotate * *translateToOrigin);  
}

/**
 * Method for performing a horizontal orbit around the y-axis in 3D space.
 * 
 * Inputs:
 *      degrees - amount to orbit by since last orbit (relative, not absolute)
 * Outputs:
 *      none
 */
void ViewContext::hOrbit(double degrees){
    matrix rotate = matrix::identity(4);

    double theta = degrees * (PI/180.0);
    hdeg += theta;

    rotate[0][0] = std::cos(hdeg);
    rotate[0][2] = 1 * std::sin(hdeg);
    rotate[2][0] = -1 * std::sin(hdeg);
    rotate[2][2] = 1 * std::cos(hdeg);

    *hOrbitMatrix = rotate;
}

/**
 * Method for performing a vertical orbit in 3D space.
 * 
 * Inputs:
 *      degrees - amount to orbit by since last orbit (relative, not absolute)
 * Outputs:
 *      none
 */
void ViewContext::vOrbit(double degrees){
    matrix rotate = matrix::identity(4);

    double theta = degrees * (PI/180.0);
    vdeg += theta;

    matrix yToPo(3,1);
    yToPo[0][0] = (*p0)[0][0];
    yToPo[1][0] = 0;
    yToPo[2][0] = (*p0)[2][0];

    matrix y(3,1);
    y[1][0] = 1.0;

    matrix axisOfRot(3,1);
    axisOfRot = matrix(crossProduct3X1(&yToPo,&y));

    //rotate to z axis
    matrix rotateToZ = matrix::identity(4);
    rotateToZ[0][0] = axisOfRot[2][0] / std::sqrt(std::pow(axisOfRot[0][0],2) + std::pow(axisOfRot[2][0],2));
    rotateToZ[0][2] = -1 * axisOfRot[0][0] / std::sqrt(std::pow(axisOfRot[0][0],2) + std::pow(axisOfRot[2][0],2));
    rotateToZ[2][0] = axisOfRot[0][0] / std::sqrt(std::pow(axisOfRot[0][0],2) + std::pow(axisOfRot[2][0],2));
    rotateToZ[2][2] = axisOfRot[2][0] / std::sqrt(std::pow(axisOfRot[0][0],2) + std::pow(axisOfRot[2][0],2));

    //rotate from z axis
    matrix rotateFromZ = matrix::identity(4);
    rotateFromZ[0][0] = axisOfRot[2][0] / std::sqrt(std::pow(axisOfRot[0][0],2) + std::pow(axisOfRot[2][0],2));
    rotateFromZ[0][2] = axisOfRot[0][0] / std::sqrt(std::pow(axisOfRot[0][0],2) + std::pow(axisOfRot[2][0],2));
    rotateFromZ[2][0] = axisOfRot[0][0] / std::sqrt(std::pow(axisOfRot[0][0],2) + std::pow(axisOfRot[2][0],2));
    rotateFromZ[2][2] = -1 * axisOfRot[2][0] / std::sqrt(std::pow(axisOfRot[0][0],2) + std::pow(axisOfRot[2][0],2));


    //rotate points around z axis
    matrix rotateZ = matrix::identity(4);
    rotateZ[0][0] = std::cos(vdeg);
    rotateZ[0][1] = -1 * std::sin(vdeg);
    rotateZ[1][0] = std::sin(vdeg);
    rotateZ[1][1] = std::cos(vdeg);

    matrix zOrbit(4,4);
    zOrbit = rotateToZ * rotateZ * rotateFromZ;

    *vOrbitMatrix = zOrbit;
}

/* 
 * This function applies a translation matrix to the exisiting transformation matrix, while simultaneously updating the 
 * inverse transformation matrix.
 * 
 * Inputs:
 *      x - translation in the x direction
 *      y - translation in the y direction
 * Outputs:
 *      none
 */
void ViewContext::translate(int x, int y){
    matrix translate = matrix::identity(4);
    matrix undoTranslate = matrix::identity(4);

    translate[0][3] = x;
    translate[1][3] = y;

    undoTranslate[0][3] = -x;
    undoTranslate[1][3] = -y;

    *toDeviceCoordinates = (*translateFromOrigin * translate * *translateToOrigin * *toDeviceCoordinates);
    *toModelCoordinates = (*toModelCoordinates * *translateFromOrigin * undoTranslate * *translateToOrigin);
}

/* 
 * This function resets the transformation matricies so that the view is in its original form
 * 
 * Inputs:
 *      none
 * Outputs:
 *      none
 */
void ViewContext::reset(){
    resetTransformMatricies();
    (*toDeviceCoordinates) = *translateFromOrigin * *toDeviceCoordinates;
}

/**
 * Interface method for changing the field of view.
 * 
 * Inputs:
 *      fov - amount to change the FOV by
 * Outputs:
 *      none
 */
void ViewContext::adjustFOV(double fov){
    zf += fov;
}

/**
 * This is a private method for transforming to a new basis for 3D rendering.
 * 
 * Input:
 *      none
 * Output:
 *      none
 */
void ViewContext::transformBasis(){
    matrix N(3,1);  matrix V(3,1);  matrix L(3,1);
    N[0][0] = (*p0)[0][0];    N[1][0] = (*p0)[1][0];    N[2][0] = (*p0)[2][0];
    V[1][0] = 1;

    L = matrix(crossProduct3X1(&V,&N));

    matrix l(normalize(&L));
    matrix n(normalize(&N));
    matrix m(crossProduct3X1(&n,&l));

    (*changeBasisMatrix)[0][0] = l[0][0];
    (*changeBasisMatrix)[0][1] = l[1][0];
    (*changeBasisMatrix)[0][2] = l[2][0];
    (*changeBasisMatrix)[0][3] = -1 * dot(&l,p0);

    (*changeBasisMatrix)[1][0] = m[0][0];
    (*changeBasisMatrix)[1][1] = m[1][0];
    (*changeBasisMatrix)[1][2] = m[2][0];
    (*changeBasisMatrix)[1][3] = -1 * dot(&m,p0);

    (*changeBasisMatrix)[2][0] = n[0][0];
    (*changeBasisMatrix)[2][1] = n[1][0];
    (*changeBasisMatrix)[2][2] = n[2][0];
    (*changeBasisMatrix)[2][3] = -1 * dot(&n,p0);

    (*changeBasisMatrix)[3][0] = 0;
    (*changeBasisMatrix)[3][1] = 0;
    (*changeBasisMatrix)[3][2] = 0;
    (*changeBasisMatrix)[3][3] = 1;
}

/**
 * This is a private helper method for creating the necessary transformation matricies.
 * 
 * Input:
 *      none
 * Output:
 *      none
 */
void ViewContext::createTransformMatricies(){
    toModelCoordinates = new matrix(4,4);
    toDeviceCoordinates = new matrix(4,4);
    changeBasisMatrix = new matrix(4,4);
    translateToOrigin = new matrix(4,4);
    translateFromOrigin = new matrix(4,4);
    hOrbitMatrix = new matrix(4,4);
    vOrbitMatrix = new matrix(4,4);

    resetTransformMatricies();
}

/**
 * This is a private helper function for reseting the transform matricies so that all objects
 * appear in their original form.
 * 
 * Input:
 *      none
 * Output:
 *      none
 */
void ViewContext::resetTransformMatricies(){
    toModelCoordinates->clear();
    toDeviceCoordinates->clear();
    hOrbitMatrix->clear();
    vOrbitMatrix->clear();

    for(int i = 0; i < 4; i++){
        (*toModelCoordinates)[i][i] = 1.0;
        (*toDeviceCoordinates)[i][i] = 1.0;
        (*hOrbitMatrix)[i][i] = 1.0;
        (*vOrbitMatrix)[i][i] = 1.0;
    }
}

/**
 * This is a private helper function for moving the objects from the window origin to the middle
 * of the screen.
 * 
 * Input:
 *      x - x coordinate for the middle of the screen.
 *      y - y coordinate for the middle of the screen.
 * Output:
 *      none
 */
void ViewContext::originToCenter(double x, double y){
    (*translateToOrigin)[0][3] = -x;
    (*translateToOrigin)[1][3] = -y;
    (*translateFromOrigin)[0][3] = x;
    (*translateFromOrigin)[1][3] = y;

    for(int i = 0; i < 4; i++){
        (*translateFromOrigin)[i][i] = 1.0;
        (*translateToOrigin)[i][i] = 1.0;
    }

    (*toDeviceCoordinates) = *translateFromOrigin * *toDeviceCoordinates;
}

/**
 * This is a private helper method for perfoming the cross product of 2 3X1
 * matricies.
 * 
 * Inputs:
 *      a - pointer to the matrix on the left
 *      b - pointer to the matrix on the right
 * Outputs:
 *      resulting matrix from cross product.
 */
matrix ViewContext::crossProduct3X1(matrix* a, matrix* b){
    matrix result = matrix(3,1);

    result[0][0] = ((*a)[1][0] * (*b)[2][0]) - ((*a)[2][0] * (*b)[1][0]);
    result[1][0] = -(((*a)[0][0] * (*b)[2][0]) - ((*a)[2][0] * (*b)[0][0]));
    result[2][0] = ((*a)[0][0] * (*b)[1][0]) - ((*a)[1][0] * (*b)[0][0]);

    return result;
}

/**
 * Determines the magnitude of a 3X1 matrix.
 * 
 * Input:
 *  a - pointer to matrix
 * Output:
 *      magnitude of matrix
 */
double ViewContext::magnitude(matrix* a){
    return std::sqrt(std::pow((*a)[0][0],2) + std::pow((*a)[1][0],2) + std::pow((*a)[2][0],2));
}

/**
 * This is a private helper method for normalizing a matrix.
 * 
 * Input:
 *      a - pointer to matrix to be normalized.
 * Output:
 *      resulting normalized matrix
 */
matrix ViewContext::normalize(matrix* a){
    double norm = magnitude(a);

    matrix result = matrix(3,1);
    result[0][0] = (*a)[0][0] / norm;
    result[1][0] = (*a)[1][0] / norm;
    result[2][0] = (*a)[2][0] / norm;

    return result;
}

/**
 * Private helper method for determining the dot product of 2 3X1 matricies
 * 
 * Input:
 *      a - pointer to matrix on the left
 *      b - pointer to matrix on the right
 * Output:
 *      result of dot product
 */
double ViewContext::dot(matrix* a, matrix* b){
    double result = (*a)[0][0] * (*b)[0][0] + (*a)[1][0] * (*b)[1][0] + (*a)[2][0] * (*b)[2][0];
    return result;
}
