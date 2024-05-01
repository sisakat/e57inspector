#include "panorama.h"

#include <algorithm>
#include <e57inspector/E57Reader.h>
#include <iostream>
#include <stdexcept>
#include <utility>

static const int BUFFER_SIZE = 10000;

Panorama::Panorama(std::string filename) : m_filename(std::move(filename)) {}

std::array<float, 3> sphericalToCartesian(const std::array<float, 3> rtp)
{
    const float r = rtp[0];
    const float sinTheta = std::sin(rtp[1]);
    const float sinPhi = std::sin(rtp[2]);
    const float cosTheta = std::cos(rtp[1]);
    const float cosPhi = std::cos(rtp[2]);
    return std::array<float, 3>{r * cosTheta * cosPhi, r * cosTheta * sinPhi,
                                r * sinTheta};
}

E57Data3DPtr findData3DByGuid(const E57Reader& reader, const std::string& guid)
{
    for (const auto& data3d : reader.root()->data3D())
    {
        const std::string data3DGuid = data3d->getString("guid");
        if (data3DGuid == guid)
        {
            return data3d;
        }
    }
    return nullptr;
}

PanoramaImage Panorama::createPanorama(const std::string& data3dGuid) const
{
    PanoramaImage result;
    const auto& reader = std::make_unique<E57Reader>(m_filename);

    E57Data3DPtr data3DPtr = findData3DByGuid(*reader, data3dGuid);
    if (!data3DPtr)
    {
        throw std::runtime_error("Could not find Data3D with specified GUID.");
    }

    auto indexBoundsIt = std::find_if(
        data3DPtr->children().begin(), data3DPtr->children().end(),
        [](const auto& children) { return children->name() == "indexBounds"; });
    if (indexBoundsIt == data3DPtr->children().end())
    {
        throw std::runtime_error("Could not find index bounds in Data3D.");
    }

    const auto& indexBounds = *indexBoundsIt;
    result.height = indexBounds->getInteger("rowMaximum");
    result.width = indexBounds->getInteger("columnMaximum");
    result.data.resize(result.height * result.width * 4, 0);

    auto dataInfo = reader->dataInfo(data3DPtr->data().at("points"));

    auto hasAttribute = [&dataInfo](const std::string& name)
    {
        return std::any_of(dataInfo.begin(), dataInfo.end(),
                           [&name](const auto& info)
                           { return info.identifier == name; });
    };

    auto dataReader = reader->dataReader(data3DPtr->data().at("points"));

    bool hasColor = false;
    bool hasIntensity = false;
    std::vector<std::array<float, 3>> rgb(0);
    if (hasAttribute("colorRed") && hasAttribute("colorGreen") &&
        hasAttribute("colorBlue"))
    {
        rgb.resize(BUFFER_SIZE);
        hasColor = true;
        dataReader.bindBuffer("colorRed", (float*)&rgb[0][0], rgb.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("colorGreen", (float*)&rgb[0][1], rgb.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("colorBlue", (float*)&rgb[0][2], rgb.size(),
                              3 * sizeof(float));
    }

    std::vector<float> intensity(0);
    if (hasAttribute("intensity"))
    {
        intensity.resize(BUFFER_SIZE);
        hasIntensity = true;
        dataReader.bindBuffer("intensity", (float*)&intensity[0],
                              intensity.size());
    }

    bool hasColumnIndex = false;
    std::vector<uint32_t> columnIndex(0);
    if (hasAttribute("columnIndex"))
    {
        hasColumnIndex = true;
        columnIndex.resize(BUFFER_SIZE);
        dataReader.bindBuffer("columnIndex", (uint32_t*)&columnIndex[0],
                              columnIndex.size());
    }

    bool hasRowIndex = false;
    std::vector<uint32_t> rowIndex(0);
    if (hasAttribute("rowIndex"))
    {
        hasRowIndex = true;
        rowIndex.resize(BUFFER_SIZE);
        dataReader.bindBuffer("rowIndex", (uint32_t*)&rowIndex[0],
                              rowIndex.size());
    }

    if (!hasRowIndex || !hasColumnIndex)
    {
        throw std::runtime_error("Data3D has no row index or column index.");
    }

    if (!hasColor && !hasIntensity)
    {
        throw std::runtime_error("Data3D has no color or intensity.");
    }

    struct Data
    {
        std::vector<float> intensity;
        std::vector<std::array<float, 4>> rgba;
        std::vector<uint32_t> rowIndex;
        std::vector<uint32_t> columnIndex;
    };

    uint64_t count;
    Data data;
    while ((count = dataReader.read()) > 0)
    {
        for (size_t i = 0; i < count; ++i)
        {
            if (hasColor)
            {
                data.rgba.push_back({rgb[i][0] / 255.0f, rgb[i][1] / 255.0f,
                                     rgb[i][2] / 255.0f, 1.0f});
            }
            else if (hasIntensity)
            {
                data.intensity.push_back(intensity[i]);
            }

            data.rowIndex.push_back(rowIndex[i]);
            data.columnIndex.push_back(columnIndex[i]);
        }
    }

    // map intensity between [0;1]
    if (!data.intensity.empty())
    {
        float minIntensity =
            *std::min_element(data.intensity.begin(), data.intensity.end());
        float maxIntensity =
            *std::max_element(data.intensity.begin(), data.intensity.end());
        std::for_each(data.intensity.begin(), data.intensity.end(),
                      [minIntensity, maxIntensity](auto& intensity)
                      {
                          intensity -= minIntensity;
                          intensity /= (maxIntensity - minIntensity);
                      });
    }

    // create final image
    for (int i = 0; i < data.columnIndex.size(); ++i)
    {
        if (data.rowIndex[i] < result.height && data.rowIndex[i] >= 0 &&
            data.columnIndex[i] < result.width && data.columnIndex[i] >= 0)
        {
            auto row = result.height - data.rowIndex[i] - 1;
            auto col = data.columnIndex[i];
            if (hasColor)
            {
                result.data[(row * result.width + col) * 4 + 0] =
                    static_cast<uint8_t>(data.rgba[i][0] * 255);
                result.data[(row * result.width + col) * 4 + 1] =
                    static_cast<uint8_t>(data.rgba[i][1] * 255);
                result.data[(row * result.width + col) * 4 + 2] =
                    static_cast<uint8_t>(data.rgba[i][2] * 255);
            }
            else if (hasIntensity)
            {
                result.data[(row * result.width + col) * 4 + 0] =
                    static_cast<uint8_t>(data.intensity[i] * 255);
                result.data[(row * result.width + col) * 4 + 1] =
                    static_cast<uint8_t>(data.intensity[i] * 255);
                result.data[(row * result.width + col) * 4 + 2] =
                    static_cast<uint8_t>(data.intensity[i] * 255);
            }
        }
    }

    return result;
}
