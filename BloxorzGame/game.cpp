#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include<GL/glu.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

struct point { //3D Point
    float x, y, z;
};

struct square {
    point leftFront, leftBack, rightBack, rightFront;

    square(point lf, point lb, point rb, point rf) {
        leftFront = lf;
        leftBack = lb;
        rightBack = rb;
        rightFront = rf;
    }
};

struct rectangle {
    point leftFrontBottom, rightFrontBottom, rightFrontTop, leftFrontTop; //Front of Rectangle
    point leftBackBottom, rightBackBottom, rightBackTop, leftBackTop; //Back of Rectangle
};

//Starting position of our rectangle
rectangle OurRect = {
    {1,0,2}, {2,0,2}, {2,2,2}, {1,2,2},
    {1,0,1}, {2,0,1}, {2,2,1}, {1,2,1}
};

int key_pressed = 0; //LEFT(-1) - RIGHT(1) - DOWN(2) - UP(-2)
float timer = 0;
int timerValue = 0;
point center;
rectangle TempRect;
bool playerWon = false, playerLost = false;
float recRed = 1.0f;
float recGreen = 0.5f;

int platform[8][12] = {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,1,2,1,1,0},
    {0,0,0,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};

int platRowCount = sizeof(platform) / sizeof(platform[0]);
int platColCount = sizeof(platform[0]) / sizeof(platform[0][0]);
int recRow = 2, recCol = 2, recSide = 0;

 void init()
{
    glClearColor(0.5, 0.0, 0.0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,640.0/480.0,1.0,500.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

//Draws a single square platform
void draw_square(float x, float y, float z, bool hole) {

    glPushMatrix();

        if(!hole)
            glColor3f(0.8f, 0.8f, 0.8f);
        else
            glColor3f(0, 0, 0);

        //Inner Square
        glBegin(GL_QUADS);
            glVertex3f(0.1f + x, y, 0.1f + z);
            glVertex3f(0.1f + x, y, 0.9f + z);
            glVertex3f(0.9f + x, y, 0.9f + z);
            glVertex3f(0.9f + x, y, 0.1f + z);
        glEnd();
    glPopMatrix();

    glPushMatrix();

        if(!hole)
            glColor3f(0, 0, 0);
        else
            glColor3f(1, 1, 1);

        //Outer Square
        glBegin(GL_QUADS);
            glVertex3f(0 + x, y, 0 + z);
            glVertex3f(0 + x, y, 1 + z);
            glVertex3f(0.1 + x, y, 1 + z);
            glVertex3f(0.1 + x, y, 0 + z);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(0 + x, y, 0 + z);
            glVertex3f(1 + x, y, 0 + z);
            glVertex3f(1 + x, y, 0.1 + z);
            glVertex3f(0 + x, y, 0.1 + z);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(0 + x, y, 1 + z);
            glVertex3f(1 + x, y, 1 + z);
            glVertex3f(1 + x, y, 0.9 + z);
            glVertex3f(0 + x, y, 0.9 + z);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(1 + x, y, 0 + z);
            glVertex3f(1 + x, y, 1 + z);
            glVertex3f(0.9 + x, y, 1 + z);
            glVertex3f(0.9 + x, y, 0 + z);
        glEnd();

    glPopMatrix();
}

//Draws the rectangle (player)
void draw_rectangle(rectangle OurRect) {
    glPushMatrix();

        glColor3f(recRed, recGreen, 0);

        //Bottom
        glBegin(GL_QUADS);
            glVertex3f(OurRect.leftBackBottom.x, OurRect.leftBackBottom.y, OurRect.leftBackBottom.z);
            glVertex3f(OurRect.rightBackBottom.x, OurRect.rightBackBottom.y, OurRect.rightBackBottom.z);
            glVertex3f(OurRect.rightFrontBottom.x, OurRect.rightFrontBottom.y, OurRect.rightFrontBottom.z);
            glVertex3f(OurRect.leftFrontBottom.x, OurRect.leftFrontBottom.y, OurRect.leftFrontBottom.z);
        glEnd();

        //Top
        glBegin(GL_QUADS);
            glVertex3f(OurRect.leftBackTop.x, OurRect.leftBackTop.y, OurRect.leftBackTop.z);
            glVertex3f(OurRect.rightBackTop.x, OurRect.rightBackTop.y, OurRect.rightBackTop.z);
            glVertex3f(OurRect.rightFrontTop.x, OurRect.rightFrontTop.y, OurRect.rightFrontTop.z);
            glVertex3f(OurRect.leftFrontTop.x, OurRect.leftFrontTop.y, OurRect.leftFrontTop.z);
        glEnd();

        //Front
        glBegin(GL_QUADS);
            glVertex3f(OurRect.leftFrontBottom.x, OurRect.leftFrontBottom.y, OurRect.leftFrontBottom.z);
            glVertex3f(OurRect.rightFrontBottom.x, OurRect.rightFrontBottom.y, OurRect.rightFrontBottom.z);
            glVertex3f(OurRect.rightFrontTop.x, OurRect.rightFrontTop.y, OurRect.rightFrontTop.z);
            glVertex3f(OurRect.leftFrontTop.x, OurRect.leftFrontTop.y, OurRect.leftFrontTop.z);
        glEnd();

        //Back
        glBegin(GL_QUADS);
            glVertex3f(OurRect.leftBackBottom.x, OurRect.leftBackBottom.y, OurRect.leftBackBottom.z);
            glVertex3f(OurRect.rightBackBottom.x, OurRect.rightBackBottom.y, OurRect.rightBackBottom.z);
            glVertex3f(OurRect.rightBackTop.x, OurRect.rightBackTop.y, OurRect.rightBackTop.z);
            glVertex3f(OurRect.leftBackTop.x, OurRect.leftBackTop.y, OurRect.leftBackTop.z);
        glEnd();

        //Left
        glBegin(GL_QUADS);
            glVertex3f(OurRect.leftBackBottom.x, OurRect.leftBackBottom.y, OurRect.leftBackBottom.z);
            glVertex3f(OurRect.leftBackTop.x, OurRect.leftBackTop.y, OurRect.leftBackTop.z);
            glVertex3f(OurRect.leftFrontTop.x, OurRect.leftFrontTop.y, OurRect.leftFrontTop.z);
            glVertex3f(OurRect.leftFrontBottom.x, OurRect.leftFrontBottom.y, OurRect.leftFrontBottom.z);
        glEnd();

        //Right
        glBegin(GL_QUADS);
            glVertex3f(OurRect.rightBackBottom.x, OurRect.rightBackBottom.y, OurRect.rightBackBottom.z);
            glVertex3f(OurRect.rightBackTop.x, OurRect.rightBackTop.y, OurRect.rightBackTop.z);
            glVertex3f(OurRect.rightFrontTop.x, OurRect.rightFrontTop.y, OurRect.rightFrontTop.z);
            glVertex3f(OurRect.rightFrontBottom.x, OurRect.rightFrontBottom.y, OurRect.rightFrontBottom.z);
        glEnd();

    glPopMatrix();
}

//Draws the game platform
void draw_platform() {

    for(int i = 0; i < platRowCount; i++) {
        for(int j = 0; j < platColCount; j++) {
            if(platform[i][j] == 1) {
                draw_square(j - 1, 0, i - 1, false);
            }
            else if(platform[i][j] == 2) {
                draw_square(j - 1, 0, i - 1, true);
            }
        }
    }
}

//Function to check if the rectangle is on the exit
bool playerOnExit() {

    if(recRow == 5 && recCol == 8 && recSide == 0)
        return true;

    return false;
}

//Function to check if the rectangle is out of bounds
bool playerOutOfBounds() {

    if(platform[recRow][recCol] == 0)
        return true;

    switch(recSide) {
        case -1:
            if(platform[recRow][recCol - 1] == 0)
                return true;
            break;
        case 1:
            if(platform[recRow][recCol + 1] == 0)
                return true;
            break;
        case -2:
            if(platform[recRow - 1][recCol] == 0)
                return true;
            break;
        case 2:
            if(platform[recRow + 1][recCol] == 0)
                return true;
            break;
        default:
            break;
    }


    return false;
}

//Comments are left in for testing and for how I came up with the logic
//In the ending I realized the rectanglePosition array wasn't necessary

/*
This function basically envisions the rectangle moving on an "array" platform
The variable recSide accounts for the rectangle when it has dropped on one side
If the rectangle is standing straight then recSide is 0
When the rectangle falls one a side, recSide changes to a value to indicate what side the rectangle is facing
*/
bool moveRectanglePosition(string movePos) {

    //rectanglePosition[recRow][recCol] = 0;

    if(recSide == 0) {
        if(movePos == "LEFT") {
            //rectanglePosition[recRow][recCol - 1] = 1;
            //rectanglePosition[recRow][recCol - 2] = 1;
            recSide = -1;
            recCol--;
        }
        if(movePos == "RIGHT") {
            //rectanglePosition[recRow][recCol + 1] = 1;
            //rectanglePosition[recRow][recCol + 2] = 1;
            recSide = 1;
            recCol++;
        }
        if(movePos == "FORWARD") {
            //rectanglePosition[recRow + 1][recCol] = 1;
            //rectanglePosition[recRow + 2][recCol] = 1;
            recSide = 2;
            recRow++;
        }
        if(movePos == "BACK") {
            //rectanglePosition[recRow - 1][recCol] = 1;
            //rectanglePosition[recRow - 2][recCol] = 1;
            recSide = -2;
            recRow--;
        }
    }
    else if(recSide == -1) {
        //rectanglePosition[recRow][recCol - 1] = 0;
        if(movePos == "LEFT") {
            //rectanglePosition[recRow][recCol - 2] = 2;
            recSide = 0;
            recCol -= 2;
        }
        if(movePos == "RIGHT") {
            //rectanglePosition[recRow][recCol + 1] = 2;
            recSide = 0;
            recCol += 1;
        }
        if(movePos == "FORWARD") {
            //rectanglePosition[recRow + 1][recCol] = 1;
            //rectanglePosition[recRow + 1][recCol - 1] = 1;
            recRow += 1;
        }
        if(movePos == "BACK") {
            //rectanglePosition[recRow - 1][recCol] = 1;
            //rectanglePosition[recRow - 1][recCol - 1] = 1;
            recRow -= 1;
        }
    }
    else if(recSide == 1) {
        //rectanglePosition[recRow][recCol + 1] = 0;
        if(movePos == "LEFT") {
            //rectanglePosition[recRow][recCol - 1] = 2;
            recSide = 0;
            recCol -= 1;
        }
        if(movePos == "RIGHT") {
            //rectanglePosition[recRow][recCol + 2] = 2;
            recSide = 0;
            recCol += 2;
        }
        if(movePos == "FORWARD") {
            //rectanglePosition[recRow + 1][recCol] = 1;
            //rectanglePosition[recRow + 1][recCol + 1] = 1;
            recRow += 1;
        }
        if(movePos == "BACK") {
            //rectanglePosition[recRow - 1][recCol] = 1;
            //rectanglePosition[recRow - 1][recCol + 1] = 1;
            recRow -= 1;
        }
    }
    else if(recSide == -2) {
        //rectanglePosition[recRow][recCol] = 0;
        //rectanglePosition[recRow - 1][recCol] = 0;
        if(movePos == "LEFT") {
            //rectanglePosition[recRow][recCol - 1] = 1;
            //rectanglePosition[recRow - 1][recCol - 1] = 1;
            recCol -= 1;
        }
        if(movePos == "RIGHT") {
            //rectanglePosition[recRow][recCol + 1] = 1;
            //rectanglePosition[recRow - 1][recCol + 1] = 1;
            recCol += 1;
        }
        if(movePos == "FORWARD") {
            //rectanglePosition[recRow + 1][recCol] = 2;
            recSide = 0;
            recRow += 1;
        }
        if(movePos == "BACK") {
            //rectanglePosition[recRow - 2][recCol] = 2;
            recSide = 0;
            recRow -= 2;
        }
    }
    else if(recSide == 2) {
        //rectanglePosition[recRow][recCol] = 0;
        //rectanglePosition[recRow + 1][recCol] = 0;
        if(movePos == "LEFT") {
            //rectanglePosition[recRow][recCol - 1] = 1;
            //rectanglePosition[recRow - 1][recCol - 1] = 1;
            recCol -= 1;
        }
        if(movePos == "RIGHT") {
            //rectanglePosition[recRow][recCol + 1] = 1;
            //rectanglePosition[recRow - 1][recCol + 1] = 1;
            recCol += 1;
        }
        if(movePos == "FORWARD") {
            //rectanglePosition[recRow + 2][recCol] = 2;
            recSide = 0;
            recRow += 2;
        }
        if(movePos == "BACK") {
            //rectanglePosition[recRow - 1][recCol] = 2;
            recSide = 0;
            recRow -= 1;
        }
    }

    /*
    //TESTING

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 12; j++) {
            cout << rectanglePosition[i][j] << " ";
        }
        cout << endl;
    }
    cout << recSide;
    cout << endl;

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 12; j++) {
            cout << platform[i][j] << " ";
        }
        cout << endl;
    }
    cout <<endl;

    cout << "RecRow: " << recRow << endl << "RecCol: " << recCol << endl;

    if(playerOutOfBounds())
        cout << "Out" << endl;
    */
    return false;
}

/*
This function returns the point we want to rotate.
It gets the minimum point in order to go left and the maximum point to go right
The same goes for a rotation of forwards or backwards
*/
point CalculateRotationPoint(int key, rectangle OurRect) {

    point left, right, backwards, forwards;
    left.x = std::min({OurRect.leftBackBottom.x, OurRect.leftBackTop.x, OurRect.leftFrontBottom.x, OurRect.leftFrontTop.x, OurRect.rightBackBottom.x, OurRect.rightBackTop.x, OurRect.rightFrontBottom.x, OurRect.rightFrontTop.x});
    left.y = 0;
    left.z = OurRect.leftBackBottom.z;

    right.x = std::max({OurRect.leftBackBottom.x, OurRect.leftBackTop.x, OurRect.leftFrontBottom.x, OurRect.leftFrontTop.x, OurRect.rightBackBottom.x, OurRect.rightBackTop.x, OurRect.rightFrontBottom.x, OurRect.rightFrontTop.x});
    right.y = 0;
    right.z = OurRect.leftBackBottom.z;

    forwards.x = OurRect.leftBackBottom.x;
    forwards.y = 0;
    forwards.z = std::max({OurRect.leftBackBottom.z, OurRect.leftBackTop.z, OurRect.leftFrontBottom.z, OurRect.leftFrontTop.z, OurRect.rightBackBottom.z, OurRect.rightBackTop.z, OurRect.rightFrontBottom.z, OurRect.rightFrontTop.z});

    backwards.x = OurRect.leftBackBottom.x;
    backwards.y = 0;
    backwards.z = std::min({OurRect.leftBackBottom.z, OurRect.leftBackTop.z, OurRect.leftFrontBottom.z, OurRect.leftFrontTop.z, OurRect.rightBackBottom.z, OurRect.rightBackTop.z, OurRect.rightFrontBottom.z, OurRect.rightFrontTop.z});

    bool bounds = false;

    if(key == -1) {
        moveRectanglePosition("LEFT");

        return left;
    }
    if(key == 1) {
        moveRectanglePosition("RIGHT");

        return right;
    }
    if(key == -2) {
        moveRectanglePosition("BACK");

        return backwards;
    }
    if(key == 2) {
        moveRectanglePosition("FORWARD");

        return forwards;
    }

    if(key == 0) return {0.0, 0.0, 0.0};
}

//Apply mathematical formula for rotation around a point
rectangle ExecuteRotation(rectangle OurRect, float beta, point rotationPoint) {

    rectangle temp;

    if(key_pressed == -1 || key_pressed == 1) {
        temp.leftBackBottom.x = rotationPoint.x + cos(beta) * (OurRect.leftBackBottom.x - rotationPoint.x) + sin(beta) * (OurRect.leftBackBottom.y - rotationPoint.y);
        temp.leftBackBottom.y = rotationPoint.y - sin(beta) * (OurRect.leftBackBottom.x - rotationPoint.x) + cos(beta) * (OurRect.leftBackBottom.y - rotationPoint.y);
        temp.leftBackBottom.z = OurRect.leftBackBottom.z;

        temp.leftBackTop.x = rotationPoint.x + cos(beta) * (OurRect.leftBackTop.x - rotationPoint.x) + sin(beta) * (OurRect.leftBackTop.y - rotationPoint.y);
        temp.leftBackTop.y = rotationPoint.y - sin(beta) * (OurRect.leftBackTop.x - rotationPoint.x) + cos(beta) * (OurRect.leftBackTop.y - rotationPoint.y);
        temp.leftBackTop.z = OurRect.leftBackTop.z;

        temp.leftFrontBottom.x = rotationPoint.x + cos(beta) * (OurRect.leftFrontBottom.x - rotationPoint.x) + sin(beta) * (OurRect.leftFrontBottom.y - rotationPoint.y);
        temp.leftFrontBottom.y = rotationPoint.y - sin(beta) * (OurRect.leftFrontBottom.x - rotationPoint.x) + cos(beta) * (OurRect.leftFrontBottom.y - rotationPoint.y);
        temp.leftFrontBottom.z = OurRect.leftFrontBottom.z;

        temp.leftFrontTop.x = rotationPoint.x + cos(beta) * (OurRect.leftFrontTop.x - rotationPoint.x) + sin(beta) * (OurRect.leftFrontTop.y - rotationPoint.y);
        temp.leftFrontTop.y = rotationPoint.y - sin(beta) * (OurRect.leftFrontTop.x - rotationPoint.x) + cos(beta) * (OurRect.leftFrontTop.y - rotationPoint.y);
        temp.leftFrontTop.z = OurRect.leftFrontTop.z;

        temp.rightBackBottom.x = rotationPoint.x + cos(beta) * (OurRect.rightBackBottom.x - rotationPoint.x) + sin(beta) * (OurRect.rightBackBottom.y - rotationPoint.y);
        temp.rightBackBottom.y = rotationPoint.y - sin(beta) * (OurRect.rightBackBottom.x - rotationPoint.x) + cos(beta) * (OurRect.rightBackBottom.y - rotationPoint.y);
        temp.rightBackBottom.z = OurRect.rightBackBottom.z;

        temp.rightBackTop.x = rotationPoint.x + cos(beta) * (OurRect.rightBackTop.x - rotationPoint.x) + sin(beta) * (OurRect.rightBackTop.y - rotationPoint.y);
        temp.rightBackTop.y = rotationPoint.y - sin(beta) * (OurRect.rightBackTop.x - rotationPoint.x) + cos(beta) * (OurRect.rightBackTop.y - rotationPoint.y);
        temp.rightBackTop.z = OurRect.rightBackTop.z;

        temp.rightFrontBottom.x = rotationPoint.x + cos(beta) * (OurRect.rightFrontBottom.x - rotationPoint.x) + sin(beta) * (OurRect.rightFrontBottom.y - rotationPoint.y);
        temp.rightFrontBottom.y = rotationPoint.y - sin(beta) * (OurRect.rightFrontBottom.x - rotationPoint.x) + cos(beta) * (OurRect.rightFrontBottom.y - rotationPoint.y);
        temp.rightFrontBottom.z = OurRect.rightFrontBottom.z;

        temp.rightFrontTop.x = rotationPoint.x + cos(beta) * (OurRect.rightFrontTop.x - rotationPoint.x) + sin(beta) * (OurRect.rightFrontTop.y - rotationPoint.y);
        temp.rightFrontTop.y = rotationPoint.y - sin(beta) * (OurRect.rightFrontTop.x - rotationPoint.x) + cos(beta) * (OurRect.rightFrontTop.y - rotationPoint.y);
        temp.rightFrontTop.z = OurRect.rightFrontTop.z;
    }

    if(key_pressed == -2 || key_pressed == 2) {
        temp.leftBackBottom.z = rotationPoint.z + cos(beta) * (OurRect.leftBackBottom.z - rotationPoint.z) + sin(beta) * (OurRect.leftBackBottom.y - rotationPoint.y);
        temp.leftBackBottom.y = rotationPoint.y - sin(beta) * (OurRect.leftBackBottom.z - rotationPoint.z) + cos(beta) * (OurRect.leftBackBottom.y - rotationPoint.y);
        temp.leftBackBottom.x = OurRect.leftBackBottom.x;

        temp.leftBackTop.z = rotationPoint.z + cos(beta) * (OurRect.leftBackTop.z - rotationPoint.z) + sin(beta) * (OurRect.leftBackTop.y - rotationPoint.y);
        temp.leftBackTop.y = rotationPoint.y - sin(beta) * (OurRect.leftBackTop.z - rotationPoint.z) + cos(beta) * (OurRect.leftBackTop.y - rotationPoint.y);
        temp.leftBackTop.x = OurRect.leftBackTop.x;

        temp.leftFrontBottom.z = rotationPoint.z + cos(beta) * (OurRect.leftFrontBottom.z - rotationPoint.z) + sin(beta) * (OurRect.leftFrontBottom.y - rotationPoint.y);
        temp.leftFrontBottom.y = rotationPoint.y - sin(beta) * (OurRect.leftFrontBottom.z - rotationPoint.z) + cos(beta) * (OurRect.leftFrontBottom.y - rotationPoint.y);
        temp.leftFrontBottom.x = OurRect.leftFrontBottom.x;

        temp.leftFrontTop.z = rotationPoint.z + cos(beta) * (OurRect.leftFrontTop.z - rotationPoint.z) + sin(beta) * (OurRect.leftFrontTop.y - rotationPoint.y);
        temp.leftFrontTop.y = rotationPoint.y - sin(beta) * (OurRect.leftFrontTop.z - rotationPoint.z) + cos(beta) * (OurRect.leftFrontTop.y - rotationPoint.y);
        temp.leftFrontTop.x = OurRect.leftFrontTop.x;

        temp.rightBackBottom.z = rotationPoint.z + cos(beta) * (OurRect.rightBackBottom.z - rotationPoint.z) + sin(beta) * (OurRect.rightBackBottom.y - rotationPoint.y);
        temp.rightBackBottom.y = rotationPoint.y - sin(beta) * (OurRect.rightBackBottom.z - rotationPoint.z) + cos(beta) * (OurRect.rightBackBottom.y - rotationPoint.y);
        temp.rightBackBottom.x = OurRect.rightBackBottom.x;

        temp.rightBackTop.z = rotationPoint.z + cos(beta) * (OurRect.rightBackTop.z - rotationPoint.z) + sin(beta) * (OurRect.rightBackTop.y - rotationPoint.y);
        temp.rightBackTop.y = rotationPoint.y - sin(beta) * (OurRect.rightBackTop.z - rotationPoint.z) + cos(beta) * (OurRect.rightBackTop.y - rotationPoint.y);
        temp.rightBackTop.x = OurRect.rightBackTop.x;

        temp.rightFrontBottom.z = rotationPoint.z + cos(beta) * (OurRect.rightFrontBottom.z - rotationPoint.z) + sin(beta) * (OurRect.rightFrontBottom.y - rotationPoint.y);
        temp.rightFrontBottom.y = rotationPoint.y - sin(beta) * (OurRect.rightFrontBottom.z - rotationPoint.z) + cos(beta) * (OurRect.rightFrontBottom.y - rotationPoint.y);
        temp.rightFrontBottom.x = OurRect.rightFrontBottom.x;

        temp.rightFrontTop.z = rotationPoint.z + cos(beta) * (OurRect.rightFrontTop.z - rotationPoint.z) + sin(beta) * (OurRect.rightFrontTop.y - rotationPoint.y);
        temp.rightFrontTop.y = rotationPoint.y - sin(beta) * (OurRect.rightFrontTop.z - rotationPoint.z) + cos(beta) * (OurRect.rightFrontTop.y - rotationPoint.y);
        temp.rightFrontTop.x = OurRect.rightFrontTop.x;
    }

    return temp;
}

//Draws the game
void draw_level() {

    draw_platform();

    if(key_pressed == 0) {
        draw_rectangle(OurRect);
    }
    if(key_pressed != 0) {
        if(key_pressed == 2) timerValue = 1;
        else if(key_pressed == -2) timerValue = -1;
        else timerValue = key_pressed;
        if(timer == 0) {
            TempRect = OurRect;
            center = CalculateRotationPoint(key_pressed, OurRect);
            OurRect = ExecuteRotation(OurRect, (float)timerValue * M_PI/2, center);
            timer = (float)timerValue * M_PI/360;
        }
        else if (timer != 0) {
            TempRect = ExecuteRotation(TempRect, (float)timerValue * M_PI/360, center);
            draw_rectangle(TempRect);
            timer += (float)timerValue * M_PI/360;
        }
        if (timer >= M_PI/2 || timer <= -M_PI/2) {
            timer = 0;
            key_pressed = 0;
            timerValue = 0;
        }
    }

    if(playerOutOfBounds()) {
        playerLost = true;
    }

    if(playerOnExit()) {
        playerWon = true;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(-7, -1, -15);
    glRotatef(45, 1, 1, 0);

    draw_level();
}

int main(int argc, char* args[])
{
    SDL_Window* window = SDL_CreateWindow("My SDL OpenGL project",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          640, 480,
                          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glcontext);

    int loop = 1;
    SDL_Event event;
    init();
    while (loop==1)
    {
        while (SDL_PollEvent(&event))
        {
            if(playerWon) {
                cout << "Player Won!" << endl;
                cout << "Enter 'SPACE' to start again" << endl;
                recRed = 0.0f;
                recGreen = 1.0f;
            }
            else if(playerLost) {
                cout << "Player Went Out Of Bounds!" << endl;
                cout << "Enter 'SPACE' to start again" << endl;
                recRed = 0.0f;
                recGreen = 0.0f;
            }
            switch(event.type)
            {
                case SDL_QUIT:
                loop = 0;
                break;
                case SDL_KEYDOWN:
                    if(timer == 0 && !(playerLost || playerWon)) {
                        if (event.key.keysym.sym==SDLK_LEFT)
                            key_pressed = -1;
                        if(event.key.keysym.sym==SDLK_RIGHT)
                            key_pressed = 1;
                        if(event.key.keysym.sym==SDLK_UP)
                            key_pressed = -2;
                        if(event.key.keysym.sym==SDLK_DOWN)
                            key_pressed = 2;
                        break;
                    }
                    if(playerWon || playerLost) {
                        if (event.key.keysym.sym==SDLK_SPACE) {
                            OurRect = {
                                {1,0,2}, {2,0,2}, {2,2,2}, {1,2,2},
                                {1,0,1}, {2,0,1}, {2,2,1}, {1,2,1}};
                            key_pressed = 0;
                            timer = 0;
                            timerValue = 0;
                            recRow = 2;
                            recCol = 2;
                            recSide = 0;
                            playerWon = false;
                            playerLost = false;
                            recRed = 1.0f;
                            recGreen = 0.5f;
                        }
                }
            }
        }
        display();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
