#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

constexpr size_t imageWidth = 25;
constexpr size_t imageHeight = 6;
constexpr size_t numPixels = imageWidth * imageHeight;

void makePBM(int image[], string fname) {
    ofstream out(fname);

    out << "P1" << '\n';
    out << imageWidth << " " << imageHeight << '\n';
    for (size_t c=0; c < imageHeight; ++c) {
        for (size_t r=0; r < imageWidth; ++ r)
            out << image[c * imageWidth + r] << ' ';
        out << '\n';
    }   
}

int main() {
    ifstream in("input");

    string line;
    getline(in, line);

    int numLayers = line.size() / numPixels;
    int image[numLayers][numPixels];

    // read image
    for (size_t i=0; i < line.size(); ++i) {
        int layer = i / numPixels;
        int pixel = i % numPixels;
            
        image[layer][pixel] = line[i] - '0';
    }

    int flattened[numPixels];

    for (size_t i=0; i < numPixels; ++i) {
        int color = 0;
        for (size_t l=0; l < numLayers; ++l)
           if (image[l][i] != 2) {
                color = image[l][i];
                break;
           }
        flattened[i] = color;
    }

    makePBM(flattened, "out.pbm");

    return 0;
}
