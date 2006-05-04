/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-10-21
 * Description : kipi dcraw process 
 * 
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// C++ includes.

#include <string>
#include <list>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <csignal>

// C Ansi includes.

extern "C"
{
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <jpeglib.h>
#include <tiffio.h>
#include <png.h>
}

struct DCParams
{
    bool        identify;
    bool        fourColor;
    bool        cameraWB;
    bool        documentMode;
    bool        quick;
    bool        jpeg;
    bool        tiff;
    bool        png;

    int         quality;
    int         fd[2];

    std::string outFile;
    std::string inFile;
    std::string directory;
    std::string red;
    std::string blue;
    std::string brightness;
  
    pid_t       pid;
} dcParams;

void sighandler(int)
{
    if (dcParams.pid) 
    {
        std::cerr << "killing child" << std::endl;
        kill(dcParams.pid, SIGKILL);
    }
    exit(EXIT_FAILURE);
}

void forkDcraw()
{
    struct sigaction sa;
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &sighandler;
    sigaction (SIGTERM, &sa, NULL);
    
    /* create pipe */
    pipe(dcParams.fd);

    /* fork dcraw child process */
    dcParams.pid = fork();

    if (dcParams.pid == -1) 
    {
        std::cerr << "Failed to fork child process" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (dcParams.pid == (pid_t) 0) 
    {
        /* child process. close the read end of file descriptor  */
        close (dcParams.fd[0]);
        
        /* Connect the write end of the pipe to standard output */
        if (dcParams.identify)
            dup2 (dcParams.fd[1], STDERR_FILENO);
        else
            dup2 (dcParams.fd[1], STDOUT_FILENO);

        if (!dcParams.directory.empty())
            chdir(dcParams.directory.c_str());
        
        /* Replace the child process with the dcraw program */

        std::string name = "dcraw";

        const char** argl = new const char*[12];
        argl[0] = "dcraw";
        argl[1] = "-c";
        int index = 2;

        if (dcParams.identify)
            argl[index++] = "-i";
        else 
        {
            if (dcParams.fourColor)
                argl[index++] = "-f";
        
            if (dcParams.cameraWB)
                argl[index++] = "-w";
            
            if (dcParams.documentMode)
                argl[index++] = "-d";
            
            if (dcParams.quick)
                argl[index++] = "-h";

            argl[index++] = "-b";
            argl[index++] = dcParams.brightness.c_str();
            argl[index++] = "-r";
            argl[index++] = dcParams.red.c_str();
            argl[index++] = "-l";
            argl[index++] = dcParams.blue.c_str();
        }

        argl[index++]  = dcParams.inFile.c_str();
        argl[index] = 0;

        if (execvp("dcraw", (char* const*) argl) == -1) 
        {
            std::cerr << "Failed to start dcraw process" << std::endl;
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else 
    {
        /* parent process. close write end of file descriptor */
        close (dcParams.fd[1]);
    }
}

void printUsage()
{
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    dcParams.identify     = false;
    dcParams.fourColor    = false;
    dcParams.cameraWB     = false;
    dcParams.documentMode = false;
    dcParams.quick        = false;
    dcParams.brightness   = "1.0";
    dcParams.red          = "1.0";
    dcParams.blue         = "1.0";
    dcParams.jpeg         = false;
    dcParams.tiff         = false;
    dcParams.png          = false;
    dcParams.quality      = 100;        // JPEG quality
    dcParams.pid          = 0;

    int c;

    while ((c = getopt(argc, argv, "ifwdhg:b:r:l:o:F:Q:D:")) != -1) 
    {
        switch (c) 
        {
            case 'i': 
            {
                dcParams.identify = true;
                break;
            }
                
            case 'f': 
            {
                dcParams.fourColor = true;
                break;
            }
    
            case 'w': 
            {
                dcParams.cameraWB = true;
                break;
            }
    
            case 'd': 
            {
                dcParams.documentMode = true;
                break;
            }
    
            case 'h':   
            {
                dcParams.quick = true;
                break;
            }
            
            case 'b': 
            {
                dcParams.brightness = optarg;
                break;
            }
    
            case 'r': 
            {
                dcParams.red = optarg;
                break;
            }
    
            case 'l': 
            {
                dcParams.blue = optarg;
                break;
            }
    
            case 'o': 
            {
                dcParams.outFile = optarg;
                break;
            }
    
            case 'F': 
            {
                std::string format(optarg);
                if (format == "JPEG" || format == "JPG")
                    dcParams.jpeg = true;
                else if (format == "TIFF" || format == "TIF")
                    dcParams.tiff = true;
                else if (format == "PNG")
                    dcParams.png = true;
                else if (format != "PPM")
                {
                    std::cerr << "Exiting: wrong format: " << format << std::endl;
                    printUsage();
                }
                break;
            }
    
            case 'Q': 
            {
                dcParams.quality = atoi(optarg);
                break;
            }            
    
            case 'D': 
            {
                dcParams.directory = optarg;
                break;
            }
                
            default: 
            {
                std::cerr << "Exiting: Unknown option: " << c  << std::endl;
                printUsage();
                break;
            }
        }
    }

    if (optind < argc) 
    {
        dcParams.inFile = argv[optind];
    }

    if (dcParams.inFile.empty()) 
    {
        std::cerr << "No input file specified" << std::endl;
        printUsage();
    }

    if (dcParams.outFile.empty() && !dcParams.identify) 
    {
        std::cerr << "No output file specified" << std::endl;
        printUsage();
    }
        
    forkDcraw();

    if (dcParams.identify) 
    {
        FILE* stream = fdopen(dcParams.fd[0], "r");
        int c;
        
        while ((c = fgetc(stream)) != EOF) 
        {
            printf("%c",(unsigned char)c);
        }
        fclose(stream);

        int status;
        ::waitpid( dcParams.pid, &status, 0);

        return WEXITSTATUS(status);
    }
    
    FILE* stream = fdopen(dcParams.fd[0], "r");

    char s[256];

    fgets(s, 256, stream);
    s[2] = 0;
    if (!(strcmp(s,"P6") == 0)) 
    {
        std::cerr << "Invalid ppm file read from pipe" << std::endl;
        exit(EXIT_FAILURE);
    }

    unsigned char *imgData = 0;
    int            width, height,scale;

    bool done = false;

    while (!done) 
    {
        if (fgets(s, 256, stream) == 0) 
        {
            std::cerr << "Failed to read from pipe" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (s[0] != 0) 
        {
            done = true;

            sscanf(s, "%i %i", &width, &height);
            if (width > 32767 || height > 32767) 
            {
                std::cerr << "Image size too large" << std::endl;
                exit(EXIT_FAILURE);
            }

            fgets(s, 256, stream);
            sscanf(s, "%i", &scale);

            imgData = new unsigned char[width*height*3];
            fread(imgData, width*height*3, 1, stream);
        }
    }

    if (!imgData) 
    {
        std::cerr << "Failed to read ppm data from pipe" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (dcParams.jpeg)
    {
        // Write to JPEG file
        
        FILE* f = 0;
        f = fopen(dcParams.outFile.c_str(), "wb");

        if (!f) 
        {
            std::cerr << "Failed to open JPEG file for writing"
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr       jerr;

        int         row_stride;
        JSAMPROW    row_pointer[1];

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, f);
        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, dcParams.quality, TRUE);
        jpeg_start_compress(&cinfo, TRUE);
        row_stride = cinfo.image_width * 3;
        
        while (cinfo.next_scanline < cinfo.image_height)
        {
            row_pointer[0] = imgData + (cinfo.next_scanline * row_stride);
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }
        
        jpeg_finish_compress(&cinfo);
        fclose(f);
    }
    else if (dcParams.png)
    {
        // write to PNG file

        FILE* f = 0;
        f = fopen(dcParams.outFile.c_str(), "wb");

        if (!f) 
        {
            std::cerr << "Failed to open PNG file for writing"
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        png_color_8 sig_bit;
        png_bytep   row_ptr;
        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop info_ptr  = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, f);
        png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
                     PNG_COLOR_TYPE_RGB,        PNG_INTERLACE_NONE, 
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        sig_bit.red   = 8;
        sig_bit.green = 8;
        sig_bit.blue  = 8;
        sig_bit.alpha = 8;
        png_set_sBIT(png_ptr, info_ptr, &sig_bit);
        png_set_compression_level(png_ptr, 9);
        png_write_info(png_ptr, info_ptr);
        png_set_shift(png_ptr, &sig_bit);
        png_set_packing(png_ptr);
        unsigned char* ptr  = imgData;

        for (int y = 0; y < height; y++)
        {
            row_ptr = (png_bytep) ptr;
            png_write_rows(png_ptr, &row_ptr, 1);
            ptr += (width * 3);
        }

        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
        png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
        fclose(f);
    }
    else if (dcParams.tiff)
    {
        // Write to tiff file

        TIFF               *tif=0;
        unsigned char      *data;
        int                 y;
        int                 w;
        
        tif = TIFFOpen(dcParams.outFile.c_str(), "w");

        if (!tif) 
        {
            std::cerr << "Failed to open TIFF file for writing"
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,  width);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
        TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_ADOBE_DEFLATE);
        {
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
            w = TIFFScanlineSize(tif);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,
                            TIFFDefaultStripSize(tif, 0));
            for (y = 0; y < height; y++)
            {
                data = imgData + (y * width * 3);
                TIFFWriteScanline(tif, data, y, 0);
            }
        }

        TIFFClose(tif);
    }
    else 
    {
        // Write to PPM file
        
        FILE* f = fopen(dcParams.outFile.c_str(), "wb");
        if (!f) 
        {
            std::cerr << "Failed to open ppm file for writing"
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        fprintf(f, "P6\n%d %d\n255\n", width, height);
        fwrite(imgData, 1, width*height*3, f);
        fclose(f);
    }
    
    delete [] imgData;

    ::waitpid(dcParams.pid, 0, 0);

    return EXIT_SUCCESS;
}
