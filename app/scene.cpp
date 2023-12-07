#include "scene.h"

#include <queue>
#include <set>
#include <tuple>

Scene::Scene() : m_bufferCache(1024) {}

void Scene::render()
{
    for (auto& child : m_nodes)
    {
        child->render();
    }
}

SceneNode::SceneNode(SceneNode* parent) : m_parent{parent}
{
    static uint32_t globalId = 0;
    m_id = globalId++;
    if (!initializeOpenGLFunctions())
    {
        throw std::runtime_error("Could not initialize OpenGL functions!");
    }
    m_pose.setToIdentity();
}

void SceneNode::render()
{
    scene()->shader()->setUniformMat4("model", pose().data());
}

float Scene::getDepth(int u, int v)
{
    GLfloat depth = 0.0;
    glReadPixels(u, v, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    if ((depth > 0.0) && (depth < 1.0))
    {
        return depth;
    }
    return -1;
}

std::optional<QVector3D> Scene::findDepth(int u, int v, int viewportX,
                                          int viewportY, int viewportWidth,
                                          int viewportHeight)
{

    std::set<std::tuple<int, int>> visited;
    std::queue<std::tuple<int, int>> pixels;
    pixels.emplace(u, v);

    float depth = getDepth(u, v);
    if (depth > 0.0)
    {
        return QVector3D{static_cast<float>(u), static_cast<float>(v), depth};
    }

    std::vector<float> depthBuffer(viewportWidth * viewportHeight);
    glReadPixels(viewportX, viewportY, viewportWidth, viewportHeight,
                 GL_DEPTH_COMPONENT, GL_FLOAT, depthBuffer.data());

    while (!pixels.empty())
    {
        auto pixel = pixels.front();
        pixels.pop();

        depth = depthBuffer.at(std::get<1>(pixel) * viewportWidth +
                               std::get<0>(pixel));
        if (depth > 0.0f && depth < 1.0f)
        {
            return QVector3D{static_cast<float>(std::get<0>(pixel)),
                             static_cast<float>(std::get<1>(pixel)), depth};
        }

        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (i == 0 && j == 0)
                    continue;
                int newU = std::get<0>(pixel) + i;
                int newV = std::get<1>(pixel) + j;
                std::tuple<int, int> newPixel(newU, newV);
                if (!visited.contains(newPixel) && newU >= 0 &&
                    newU < viewportWidth && newV >= 0 && newV < viewportHeight)
                {
                    visited.insert(newPixel);
                    pixels.push(std::move(newPixel));
                }
            }
        }
    }

    return std::nullopt;
}
