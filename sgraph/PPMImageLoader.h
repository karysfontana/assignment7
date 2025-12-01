#ifndef __PPM_IMAGELOADER_H_
#define __PPM_IMAGELOADER_H_

#include "ImageLoader.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

/**
 * @brief This class is used to load an image in the ASCII PPM format 
 * 
 */
class PPMImageLoader: public ImageLoader {

    public:
        PPMImageLoader() {

        }

        void load(string filename) {
            ifstream fp;
            string str;
            int i,j;
            int r,g,b;
            int factor;
            fp.open(filename.c_str());
            if (!fp.is_open())
                throw std::invalid_argument("File not found!");
            std::stringstream input;
            std::string line;
            while (std::getline(fp,line)) {
                if ((line.length()>0) && (line[0]!='#')) {
                    input << line << endl;
                }
            }
            fp.close();
            input >> str;
            input >> width >> height;
            input >> factor;
            image = new GLubyte[3*width*height];
            for (i=0;i<height;i++)
            {
                for (j=0;j<width;j++)
                {
                    input >> r >> g >> b;
                    image[3*((height-1-i)*width+j)] = r;
                    image[3*((height-1-i)*width+j)+1] = g;
                    image[3*((height-1-i)*width+j)+2] = b;
                }
            }
        }
    
};

#endif