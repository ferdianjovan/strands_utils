#include "dataimage.h"

DataImage::DataImage()
{

}


DataImage::DataImage(char *filename)
{
    IplImage* img = 0;


    printf("Cargando Imagen (%s)\n", filename);    img = cvLoadImage(filename, -1);

    if(!img) printf("no se pudo cargar la imagen");
    else     printf("Imagen Cargada\n");


    printf("Copiando Imagen Cv a Dmt\n");
    pxl = (unsigned char *)malloc(img->imageSize);

    assert(pxl != NULL);    memcpy(pxl, img->imageData, img->imageSize);
    printf("bits pp = %d\n",img->depth);


    /* 3) DECODE the Data Matrix barcode from the copied image */
    // creo una nuova immagine grande come la roi !  // WIDTH E HEIGTH DEVONO ESSERE MULTIPLI DI 4 !!!!!!!!!!

    CvRect rect = cvGetImageROI(img);   CvRect rectOriginal = cvGetImageROI(img);


    rect.width = ((rect.width) / 4) * 4; // approssima al multiplo inferiore
    rect.height = ((rect.height) / 4) * 4;


    cvSetImageROI(img, rect);

    img2 = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, 1);

    cvConvertImage(img, img2, 0);
    //cvCopy(img, img2);

    cvSetImageROI(img, rectOriginal);


    //imgd = dmtxImageCreate(pxl, img->width, img->height, DmtxPack8bppK);
    imgd = dmtxImageCreate((unsigned  char *)img2->imageData, rect.width, rect.height, DmtxPack8bppK);

    assert(imgd != NULL);

    cvReleaseImage(&img);
    // Hasta aqui cargo la imagen
}



void DataImage::decodificarImagen()
{
    char tcha[20];
    DmtxDecode     *dec;
    DmtxRegion     *reg;
    DmtxMessage    *msg;

    if(!cvSaveImage("JaipulImage.jpg", img2))   printf("No se pudo guardar la imagen\n");

    dmtxImageSetProp(imgd, DmtxPropImageFlip, DmtxFlipNone);



    dec = dmtxDecodeCreate(imgd, 1);
    assert(dec != NULL);
    //printf("7\n");


    reg = dmtxRegionFindNext(dec, NULL);
    if(reg != NULL)
    {
        //printf("8\n");
        msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
        if(msg != NULL)
        {


            //std::stringstream ss;
            sprintf(tcha,"%s",msg->output);
            std_msgs::String ss;
            ss.data = tcha;
            //dtmtxmsg.data
            imginfo = ss;

            //ROS_INFO("%s", dtmtxmsg.data.c_str());


            printf("Resultado:\n %s \n",ss.data.c_str());
            dmtxMessageDestroy(&msg);
        }
        else
        {
            printf("No hay Mensaje\n");
        }
        dmtxRegionDestroy(&reg);
    }
    //printf("9\n");

    cvReleaseImage(&img2);
    dmtxDecodeDestroy(&dec);
    dmtxImageDestroy(&imgd);
    free(pxl);
}
