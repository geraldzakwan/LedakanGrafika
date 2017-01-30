#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <termios.h>

using namespace std;

#define HEIGHT 800
#define WIDTH 600

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;

int redPixelMatrix[WIDTH][HEIGHT];
int greenPixelMatrix[WIDTH][HEIGHT];
int bluePixelMatrix[WIDTH][HEIGHT];

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

void printMatrix() { // print ke stdio
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

void drawRedPoint(int x1,int y1){
    redPixelMatrix[x1][y1] = 255;
    greenPixelMatrix[x1][y1] = 0;
    bluePixelMatrix[x1][y1] = 0;   
}


void floodFill(int x,int y,int redBatas,int greenBatas,int blueBatas,int redColor,int greenColor,int blueColor){
    if((x>=0 && x<HEIGHT) && (y>=0 && y<WIDTH)){
        if(!((redPixelMatrix[x][y]==redBatas && greenPixelMatrix[x][y]==greenBatas && bluePixelMatrix[x][y]==blueBatas) || 
            (redPixelMatrix[x][y]==redColor && greenPixelMatrix[x][y]==greenColor && bluePixelMatrix[x][y]==blueColor))){
            redPixelMatrix[x][y] = redColor;
            greenPixelMatrix[x][y] = greenColor;
            bluePixelMatrix[x][y] = blueColor;
            floodFill(x,y+1,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
            floodFill(x+1,y,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
            floodFill(x,y-1,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
            floodFill(x-1,y,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
        }
    }
}

void drawSemiCircle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawWhitePoint(x0 - x, y0 + y);
        drawWhitePoint(x0 - y, y0 + x); 
        drawWhitePoint(x0 - y, y0 - x);
        drawWhitePoint(x0 - x, y0 - y);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void drawCircle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawWhitePoint(x0 - x, y0 + y);
        drawWhitePoint(x0 - y, y0 + x); 
        drawWhitePoint(x0 - y, y0 - x);
        drawWhitePoint(x0 - x, y0 - y);
        drawWhitePoint(x0 + x, y0 + y);
        drawWhitePoint(x0 + y, y0 + x); 
        drawWhitePoint(x0 + y, y0 - x);
        drawWhitePoint(x0 + x, y0 - y);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void eraseWithBlackBox(int x1, int y1, int x2, int y2) {
    int x,y;
    for (x=x1; x<=x2; x++) {
        for (y=y1; y<=y2; y++) {
            redPixelMatrix[x][y] = 0;
            greenPixelMatrix[x][y] = 0;
            bluePixelMatrix[x][y] = 0;
        }
    }    
}

void drawWhiteLine(int x1, int y1, int x2, int y2) {
	//Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;
    deltaX = abs(deltaX);
    deltaY = abs(deltaY);

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

void drawRedLine(int x1, int y1, int x2, int y2) {
    //Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;
    deltaX = abs(deltaX);
    deltaY = abs(deltaY);

    int x = x1;
    int y = y1;

    drawRedPoint(x,y);

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
 
            drawRedPoint(x, y);
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
 
            drawRedPoint(x, y);
        }
    }
}

int detectKeyStroke() {
    static bool flag = false;
    static const int STDIN = 0;

    if (!flag) {
        //Memakai termios untuk mematikan line buffering
        struct termios T;

        tcgetattr(STDIN, &T);
        T.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &T);
        setbuf(stdin, NULL);
        flag = true;
    }

    int NByte;
    ioctl(STDIN, FIONREAD, &NByte);  // STDIN = 0
    
    return NByte;
}

void DrawToScreen(){
    /* prosedure yang menggambar ke layar dari matriks RGB (harusnya rata tengah)*/
    long int location = 0;
    int x , y;
    printf("before loop\n");
    for (y = vinfo.yres/2 - WIDTH/2; y < WIDTH + vinfo.yres/2 - WIDTH/2; y++)
        for (x = vinfo.xres/2 - HEIGHT/2; x < HEIGHT + vinfo.xres/2 - HEIGHT/2; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            //printf("location: %ld\n",location);
            if (vinfo.bits_per_pixel == 32) { 
                //4byte
                    *(fbp + location) = bluePixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];        // Some blue
                    *(fbp + location + 1) = greenPixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];     // A little green
                    *(fbp + location + 2) = redPixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency
            //location += 4;
            } else  { //assume 16bpp
                int b = 0;
                int g = 100;     // A little green
                int r = 0;    // A lot of red
                unsigned short int t = r<<11 | g << 5 | b;
                *((unsigned short int*)(fbp + location)) = t;
            }
        }
    printf("after loop\n");    
}

void drawExplosion(int x,int y){
    //x = 70
    // bentuk bintang ada 8 garis sesuai dengan parameter titik pusat (x,y)
    int pointx1 = x-20, pointy1 =y+20;
    int pointx3 = x+30, pointy3 =y+30;
    int pointx5 = x+20, pointy5 =y-20;
    int pointx7 = x-30, pointy7 =y-30;

    int pointx2 = x, pointy2 = y+15;
    int pointx4 = x+10, pointy4 = y;
    int pointx6 = x, pointy6 = y-10;
    int pointx8 = x-15, pointy8 = y;

    //gambar ledakan
    drawRedLine(pointx1,pointy1,pointx2,pointy2);
    drawRedLine(pointx2,pointy2,pointx3,pointy3);
    drawRedLine(pointx3,pointy3,pointx4,pointy4);
    drawRedLine(pointx4,pointy4,pointx5,pointy5);
    drawRedLine(pointx5,pointy5,pointx6,pointy6);
    drawRedLine(pointx6,pointy6,pointx7,pointy7);
    drawRedLine(pointx7,pointy7,pointx8,pointy8);
    drawRedLine(pointx8,pointy8,pointx1,pointy1);
    
    //warnain 
    floodFill(x,y,255,0,0,255,255,0);
}


int main() {
    //printf("masuk\n");
    clearMatrix();
    printf("masuk clearMatrix\n");
    
    int fbfd = 0;
    long int screensize = 0;
   
    
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    //printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }
    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }
    //printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // mendapat screensize layar monitor
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    //printf("screensize %ld\n",screensize);

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0);
    //printf("The framebuffer device was mapped to memory successfully.\n");
    //display merge center
    // Menulis ke layar tengah file
    //Gambar trapesium
    drawWhiteLine(50, 250, 70, 270);
    drawWhiteLine(50, 250, 50, 200);
    drawWhiteLine(50, 200, 70, 180);
    drawWhiteLine(70, 180, 70, 270);
    printf("masuk gambar trapesium\n");

    //Gambar circle
    drawSemiCircle(50, 225, 25);    
    printf("masuk gambar semicircle\n");

    //Gambar arena, tapi gambarnya ancur karena bug yg gua ceritain tadi
    drawWhiteLine(0, 0, 0, 400);
    drawWhiteLine(0, 400, 300, 400);
    drawWhiteLine(300, 400, 300, 0);
    drawWhiteLine(300, 0, 0, 0);
    printf("masuk gambar arena\n");

    //eraseWithBlackBox(0,0,100,100);
    printf("masuk erase black box\n");
    int xp = 150;
    int yp = 275;
    char KeyPressed;

    //gambar meledak
    drawExplosion(70,100);

    DrawToScreen();


    munmap(fbp, screensize);
    close(fbfd);  
    return 0;
}