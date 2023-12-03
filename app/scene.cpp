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

SceneNode::SceneNode()
{
    static uint32_t globalId = 0;
    m_id = globalId++;
    if (!initializeOpenGLFunctions())
    {
        throw std::runtime_error("Could not initialize OpenGL functions!");
    }
}

void SceneNode::render()
{
    GLint shaderProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, m_pose.data());
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

std::optional<QVector3D> Scene::findDepth(int u, int v)
{

    std::set<std::tuple<int, int>> visited;
    std::queue<std::tuple<int, int>> pixels;
    pixels.emplace(u, v);

    while (!pixels.empty())
    {
        auto pixel = pixels.front();
        pixels.pop();
        visited.insert(pixel);

        float depth = getDepth(std::get<0>(pixel), std::get<1>(pixel));
        if (depth > 0.0)
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
                std::tuple<int, int> newPixel(std::get<0>(pixel) + i,
                                              std::get<1>(pixel) + j);
                if (!visited.contains(newPixel))
                    pixels.push(std::move(newPixel));
            }
        }
    }

    return std::nullopt;
}
