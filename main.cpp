#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
// #include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <termios.h>

using namespace std;

#define HEIGHT 800
#define WIDTH 600

int fbfd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
char *fbp = 0;
int x = 0, y = 0;
long int location = 0;

int redPixelMatrix[WIDTH][HEIGHT];
int greenPixelMatrix[WIDTH][HEIGHT];
int bluePixelMatrix[WIDTH][HEIGHT];

<<<<<<< HEAD
long int location = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

=======
>>>>>>> 59e095e63ea5a4a5dd7748b739ce05dd16c40aca
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

void printMatrixToFrameBuffer() {
    //display merge center
    // Menulis ke layar tengah file
    for (y = vinfo.yres/2 - WIDTH/2; y < WIDTH + vinfo.yres/2 - WIDTH/2; y++) {
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
    }
}

void drawWhitePoint(int x1, int y1) {
    redPixelMatrix[x1][y1] = 255;
    greenPixelMatrix[x1][y1] = 255;
    bluePixelMatrix[x1][y1] = 255;
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

<<<<<<< HEAD
void DrawToScreen(){
    /* prosedure yang menggambar ke layar dari matriks RGB (harusnya rata tengah)*/
    int fbfd = 0;
    long int screensize = 0;
    char *fbp = 0;
    long int location = 0;
    int x , y;
=======
int main() {
    clearMatrix();
    //Gambar trapesium
    drawWhiteLine(50, 250, 70, 270);
    drawWhiteLine(50, 250, 50, 200);
    drawWhiteLine(50, 200, 70, 180);
    drawWhiteLine(70, 180, 70, 270);

    //Gambar circle
    drawSemiCircle(50, 225, 25);    

    //Gambar arena, tapi gambarnya ancur karena bug yg gua ceritain tadi
    drawWhiteLine(0, 0, 0, 400);
    drawWhiteLine(0, 400, 300, 400);
    drawWhiteLine(300, 400, 300, 0);
    drawWhiteLine(300, 0, 0, 0);

    eraseWithBlackBox(0,0,100,100);
    //pusat lingkaran
    int xp = 150;
    int yp = 275;
    char KeyPressed;
    
    
    // Open the file for reading and writing framebuffer
>>>>>>> 59e095e63ea5a4a5dd7748b739ce05dd16c40aca
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

<<<<<<< HEAD
    //display merge center
    // Menulis ke layar tengah file
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
    munmap(fbp, screensize);
    close(fbfd);
}

int main() {
    printf("masuk\n");
	
    
    
    
    
    

    clearMatrix();
    printf("masuk clearMatrix\n");
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
    printf("masu erase black box\n");

    DrawToScreen();
    // Open the file for reading and writing framebuffer

    //int i=HEIGHT + vinfo.yres/2 - HEIGHT/2;
    //printf("%d\n",i);

    /*
    int count = 0;
    do{
        i--;       
        count++;
        for (y = vinfo.yres/2 - HEIGHT/2; y < HEIGHT + vinfo.yres/2 - HEIGHT/2; y++){
            //printf("%d\n",y);
            for (x = vinfo.xres/2 - WIDTH/2; x < WIDTH + vinfo.xres/2 - WIDTH/2; x++) {
                location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
                //printf("location: %ld\n",location);
                //printf("%d %d\n",y,i);
                //usleep(400000);
                if(y>i-1){
                    *(fbp + location) = 0;      // Some blue
                    *(fbp + location + 1) = 0;   // A little green
                    *(fbp + location + 2) = 0;    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency
                }else if (vinfo.bits_per_pixel == 32){ 
                //4byte
                    *(fbp + location) = arraypixel[2][(y - vinfo.yres/2 + HEIGHT/2 + count) % HEIGHT +1][x - vinfo.xres/2 + WIDTH/2];        // Some blue
                    *(fbp + location + 1) = arraypixel[1][(y - vinfo.yres/2 + HEIGHT/2 + count) % HEIGHT +1][x - vinfo.xres/2 + WIDTH/2];     // A little green
                    *(fbp + location + 2) = arraypixel[0][(y - vinfo.yres/2 + HEIGHT/2 + count) % HEIGHT +1][x - vinfo.xres/2 + WIDTH/2];    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency
                //location += 4;
                }
                else  { //assume 16bpp
                    int b = 0;
                    int g = 100;     // A little green
                    int r = 0;    // A lot of red
                    unsigned short int t = r<<11 | g << 5 | b;
                    *((unsigned short int*)(fbp + location)) = t;
                }
=======
    do {
        while (!detectKeyStroke()) {
            //do nothing
        }
        KeyPressed = getchar();
        switch (KeyPressed) {
            case 'D': {
                drawCircle(yp,xp,25);
                drawWhiteLine(yp,xp+25,yp-25,xp+50);
                drawWhiteLine(yp-25,xp,yp-50,xp+25);
                drawWhiteLine(yp-25,xp+50,yp-50,xp+25);            
                break;
>>>>>>> 59e095e63ea5a4a5dd7748b739ce05dd16c40aca
            }
            case 'S': {
                drawCircle(yp,xp,25);
                drawWhiteLine(yp-15,xp+20,yp-50,xp+20);
                drawWhiteLine(yp-15,xp-20,yp-50,xp-20);
                drawWhiteLine(yp-50,xp+20,yp-50,xp-20);
                break;
            }
            case 'A': {
                drawCircle(yp,xp,25);
                drawWhiteLine(yp,xp-25,yp-25,xp-50);
                drawWhiteLine(yp-25,xp,yp-50,xp-25);
                drawWhiteLine(yp-25,xp-50,yp-50,xp-25);
                break;
            } 
        }
    } while (KeyPressed!='C');

    printMatrixToFrameBuffer();

    

    return 0;
}