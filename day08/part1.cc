#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

constexpr size_t imageWidth = 25;
constexpr size_t imageHeight = 6;
constexpr size_t numPixels = imageWidth * imageHeight;

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

    // find layer with fewest 0 digits
    int minZeros = numPixels;
    int minLayer;

    for (size_t l=0; l < numLayers; ++l) {
        int zeros = 0;
        for (size_t i=0; i < numPixels; ++i)
            if (image[l][i] == 0)
                ++zeros;
        if (zeros < minZeros) {
            minZeros = zeros;
            minLayer = l;
        }
    }

    // find number of 1 digits multiplied by num of 2 digits
    int numOnes = 0;
    int numTwos = 0;

    for (size_t i=0; i < numPixels; ++i) {
       int num = image[minLayer][i]; 
       if (num == 1)
           ++numOnes;
       if (num == 2)
           ++numTwos;
    }

    cout << (numOnes * numTwos) << '\n';

    return 0;
}
