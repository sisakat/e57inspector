#ifndef E57INSPECTOR_PANORAMA_H
#define E57INSPECTOR_PANORAMA_H

#include <string>
#include <vector>
#include <cstdint>

struct PanoramaImage
{
    uint32_t width;
    uint32_t height;
    std::vector<uint8_t> data;
};

class Panorama
{
public:
    /**
     * @param filename Path to an E57 file.
     */
    explicit Panorama(std::string filename);

    /**
     * Creates a panorama image from the scan data.
     * If the required attributes and fields are not present, a runtime
     * exception is thrown.
     * @param data3dGuid GUID of the scan that the panorama image should be
     * created from.
     * @return A panorama image as RGBA8888 with width and height.
     */
    [[nodiscard]] PanoramaImage
    createPanorama(const std::string& data3dGuid) const;

private:
    std::string m_filename;
};

#endif // E57INSPECTOR_PANORAMA_H
