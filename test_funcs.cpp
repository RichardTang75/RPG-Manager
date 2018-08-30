//
//  test_funcs.cpp
//  tacticsclone
//
//  Created by asdfuiop on 7/28/18.
//  Copyright © 2018 asdfuiop. All rights reserved.
//

#include <vector>
#include <string>
#include <unordered_map>
#include "lodepng.h"
#include <fstream>


void array_img(std::vector<std::vector<float>>& array, std::vector<unsigned char>& img,
               const int& rows, const int& cols, float& min, float& max)
{
    //grayscale
    float range = max-min;
    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            float normalized =(array[col][row]-min)/range;
            int blue = 230-(normalized)*60; //70+35
            int red=55 + 25*normalized; //55+25
            int green=33 + 17*normalized; //33+17
            int gray = 241 - 241 * normalized;
            img[4 * row*cols + 4 * col] = gray;
            img[4 * row*cols + 4 * col + 1] = gray;
            img[4 * row*cols + 4 * col + 2] = gray;
            img[4 * row*cols + 4 * col + 3] = 255;
        }
    }
}

void checkthis(std::vector<std::vector<float>>& in, int rows, int cols, float& min, float & max)
{
    ///set up the base
    //union it. set constraints on the other terrains.  (only add the initial if inside where you want)
    std::string filename = "ayoo.png";
    std::vector<unsigned char> img(4 * rows*cols);
    //vectormap map = set_to_vectormap(in, rows, cols);
    array_img(in, img, rows, cols, min, max);
    unsigned error = lodepng::encode(filename.c_str(), img, rows, cols);
    //    if (error)
    //    {
    //        std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << "\n";
    //    }
}

void write_polygon_index_map(std::unordered_map<float, std::unordered_map<int, std::vector<int>>> into)
{
    std::ofstream debug_text("debug.txt");
    for (auto & pair : into)
    {
        debug_text << pair.first << ":\n";
        for (auto & second_pair : pair.second)
        {
            debug_text << second_pair.first << ":\t";
            for (int & index :  second_pair.second)
            {
                debug_text << index << " ";
            }
            debug_text <<"\n";
        }
        debug_text << "\n";
    }

}
