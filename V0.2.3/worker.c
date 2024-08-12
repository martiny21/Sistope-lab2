#include "fworker.h"

int main(int argc, char *argv[]){
    int f = atoi(argv[1]);   
    float p = atof(argv[2]); 
    // p = valor de saturacion, u = umbral de binarizacion
    fprintf(stderr,"Worker\n");
    while(1){
        
        size_t numPixels;
        read(STDIN_FILENO,&numPixels,sizeof(size_t));
        if (numPixels == 0){
            fprintf(stderr,"Worker: Terminando\n");
            break;
        }

        RGBPixel *Buffer = (RGBPixel*)malloc(sizeof(RGBPixel)*numPixels);
        read(STDIN_FILENO,Buffer,sizeof(RGBPixel)*numPixels);

        RGBPixel *NewPixels = NULL;
        switch (f)
        {
            case 1:
                
                NewPixels = saturate_pixels(Buffer,numPixels,p);
                break;
            case 2:
                NewPixels = saturate_pixels(Buffer,numPixels,p);
                NewPixels = grayScale_pixels(Buffer,numPixels);
                break;
            case 3:
                float u = atof(argv[3]);
                NewPixels = saturate_pixels(Buffer,numPixels,p);
                NewPixels = grayScale_pixels(Buffer,numPixels);
                NewPixels = binarize_pixels(Buffer,numPixels,u);
                break;
        
            default:
                break;
        }
        write(STDOUT_FILENO,NewPixels,sizeof(RGBPixel)*numPixels);
        free(NewPixels);
        free(Buffer);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    return 0;
}