#include "scene.h"

Scene::Scene() : m_bufferCache(1024)
{

}

void Scene::render()
{
    for (auto& child : m_nodes) {
        child->render();
    }
}

SceneNode::SceneNode()
{
    static uint32_t globalId = 0;
    m_id = globalId++;
    if (!initializeOpenGLFunctions()) {
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
