/**
 * ViewContext.h - This is an interfece for the ViewContext class performing transformations.
 * Transformations are tracked in this class with the ability to undo them.
 * Author: larsonma@msoe.edu <Mitchell Larson>
 * Date: may 3 2018
 */

#ifndef VIEWCONTEXT_H
#define VIEWCONTEXT_H

#include <cmath>

#include "matrix.h"
#include "Shape.h"

#define PI 3.14159265359

class Shape;

class ViewContext {
    public:

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
        ViewContext(int x0, int y0, int z0, int x, int y, double zf);

        /* 
        * This function handles destroying the ViewContext object, with its underyling data structures.
        * Inputs:
        *      gc - GraphicsContext object
        * Outputs:
        *      none
        */        
        ~ViewContext();

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
        matrix* deviceToModel(matrix*);

        /* 
        * This function converts the model coorinates of a shape to the device coordinates that will be displayed
        * on the screen. This applies the transformation matrix to the shape.
        * 
        * Inputs:
        *      shapeVerticies - matrix containing the verticies of the shape.
        * Outputs:
        *      matrix* - pointer to transformed matrix object
        */
        matrix* modelToDevice(matrix*);

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
        void scale(double a, double b);

        /* 
        * This function applies a rotate matrix to the exisiting transformation matrix, while simultaneously updating the 
        * inverse transformation matrix.
        * 
        * Inputs:
        *      deg - degrees to rotate
        * Outputs:
        *      none
        */
        void rotate(double theta);

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
        void translate(int x, int y);

        /* 
        * This function resets the transformation matricies so that the view is in its original form
        * 
        * Inputs:
        *      none
        * Outputs:
        *      none
        */
        void reset();

        /**
         * This function acepts the 3D matrix and projects the points into 2D space. This changes the
         * x and y values, but does not change the z values.
         * 
         * Inputs:
         *      a - pointer to matrix on which to perform 2D projection
         * Outputs:
         *      none, but contents of a are changed
         */
        void project(matrix* a);

        /**
         * Interface method for changing the field of view.
         * 
         * Inputs:
         *      fov - amount to change the FOV by
         * Outputs:
         *      none
         */
        void adjustFOV(double fov);

        /**
         * Method for performing a horizontal orbit around the y-axis in 3D space.
         * 
         * Inputs:
         *      degrees - amount to orbit by since last orbit (relative, not absolute)
         * Outputs:
         *      none
         */
        void hOrbit(double degrees);

        /**
         * Method for performing a vertical orbit in 3D space.
         * 
         * Inputs:
         *      degrees - amount to orbit by since last orbit (relative, not absolute)
         * Outputs:
         *      none
         */
        void vOrbit(double degrees);

    private:
        matrix* toModelCoordinates;
        matrix* toDeviceCoordinates;

        matrix* changeBasisMatrix;

        matrix* hOrbitMatrix;
        matrix* vOrbitMatrix;

        matrix* translateToOrigin;
        matrix* translateFromOrigin;

        matrix* p0;
        matrix* pref;
        double zf;

        double mi;
        double li;

        double hdeg;
        double vdeg;

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
        matrix crossProduct3X1(matrix* a, matrix* b);

        /**
         * This is a private helper method for normalizing a matrix.
         * 
         * Input:
         *      a - pointer to matrix to be normalized.
         * Output:
         *      resulting normalized matrix
         */
        matrix normalize(matrix* a);

        /**
         * Private helper method for determining the dot product of 2 3X1 matricies
         * 
         * Input:
         *      a - pointer to matrix on the left
         *      b - pointer to matrix on the right
         * Output:
         *      result of dot product
         */
        double dot(matrix*a, matrix* b);

        /**
         * Determines the magnitude of a 3X1 matrix.
         * 
         * Input:
         *  a - pointer to matrix
         * Output:
         *      magnitude of matrix
         */
        double magnitude(matrix* a);

        /**
         * This is a private method for transforming to a new basis for 3D rendering.
         * 
         * Input:
         *      none
         * Output:
         *      none
         */
        void transformBasis();

        /**
         * This is a private helper method for creating the necessary transformation matricies.
         * 
         * Input:
         *      none
         * Output:
         *      none
         */
        void createTransformMatricies();

        /**
         * This is a private helper function for reseting the transform matricies so that all objects
         * appear in their original form.
         * 
         * Input:
         *      none
         * Output:
         *      none
         */
        void resetTransformMatricies();

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
        void originToCenter(double x, double y);
};

#endif