#include <stdio.h>
#include "pico/stdlib.h"
#include "cam.h"
#include <math.h>

int main()
{
    stdio_init_all();
    init_camera_pins();

    while (true) {
        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        int com = findLine(IMAGESIZEY/2); // calculate the position of the center of the line
    }
}
