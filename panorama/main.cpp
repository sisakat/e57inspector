#include "panorama.h"
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

void printHelp(const std::string& exePath)
{
    std::cout << "Usage: " << exePath
              << " E57_FILE DATA3D_GUID OUTPUT_IMAGE.JPG" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printHelp(argv[0]);
        return 1;
    }

    std::string filename(argv[1]);
    std::string data3DGuid(argv[2]);
    std::string outputFilename(argv[3]);

    try
    {
        Panorama panorama(filename);
        auto panoramaImage = panorama.createPanorama(data3DGuid);

        stbi_write_jpg(outputFilename.c_str(),
                       static_cast<int>(panoramaImage.width),
                       static_cast<int>(panoramaImage.height), 4,
                       panoramaImage.data.data(), 95);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 4;
    }

    return 0;
}