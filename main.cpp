#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
// #include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

using namespace std;

#define HEIGHT 600
#define WIDTH 800

int redPixelMatrix[WIDTH][HEIGHT];
int greenPixelMatrix[WIDTH][HEIGHT];
int bluePixelMatrix[WIDTH][HEIGHT];

long int location = 0;

void clearMatrix() {
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            redPixelMatrix[i][j] = 0;
            greenPixelMatrix[i][j] = 0;
            bluePixelMatrix[i][j] = 0;
        }
    }
}

void printMatrix() { // change to frame buffer
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            if (bluePixelMatrix[i][j] != 0) {
                cout << 0;
            } else {
                cout << '-';
            }
        }
        cout << endl;
    }
}

void drawWhitePoint(int x1, int y1) {
    redPixelMatrix[x1][y1] = 255;
    greenPixelMatrix[x1][y1] = 255;
    bluePixelMatrix[x1][y1] = 255;
}

void drawWhiteLine(int x1, int y1, int x2, int y2) {
    int deltaX = abs(x2 - x1);
    int deltaY = abs(y2 - y1);
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;

    int x = x1;
    int y = y1;

    drawWhitePoint(x,y);

    if (deltaX >= deltaY) {
        int error = 2 * deltaY - deltaX;

        while (x != x2) {
            if ((error >= 0) && (error || (ix > 0)))
            {
                error -= deltaX;
                y += iy;
            }
 
            error += deltaY;
            x += ix;
 
            drawWhitePoint(x, y);
        }
    } else {
        int error = 2 * deltaX - deltaY;

        while (y != y2)
        {
            if ((error >= 0) && (error || (iy > 0)))
            {
                error -= deltaY;
                x += ix;
            }
 
            error += deltaX;
            y += iy;
 
            drawWhitePoint(x, y);
        }
    }
}

int main() {
    clearMatrix();
    drawWhiteLine(1, 1, 5, 1);
    printMatrix();
    cout << endl;

    drawWhiteLine(4, 7, 10, 15);
    printMatrix();
    cout << endl;

    clearMatrix();

    drawWhiteLine(1, 2, 3, 4);
    printMatrix();
    cout << endl;
    return 0;
}