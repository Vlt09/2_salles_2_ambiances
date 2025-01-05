#define GLFW_INCLUDE_NONE
#include "include/Camera.hpp"
#include "include/Renderer.hpp"
#include "include/Sphere.hpp"
#include "include/Quad.hpp"
#include "include/Box.hpp"
#include "include/Room.hpp"
#include "include/Skybox.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/glm.hpp>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/getTime.hpp>
int window_width = 800;
int window_height = 800;

float last_xpos = 400;
float last_ypos = 400;
bool first_mouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float xMax = 10.f;
float xMin = -34.f;
float zMax = 10.f;
float zMin = -10.f;

bool firstRoomFlag = true;

Camera camera{};
std::vector<glimac::BBox3f> bboxVector;

void updateDeltaTime()
{
    float currentFrame = glimac::getTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

bool checkCollision(const glm::vec3 &position)
{

    if ((position.x < -10.f && position.x > -12.f) && (position.z >= 2.f || position.z <= -2.f))
    {
        return true;
    }

    if ((position.x < -32.f || position.x > 10.f))
    {
        return true;
    }

    if (position.z < -12.f || position.z > 12.f)
    {
        return true;
    }
    if (position.z > -2. && position.z < 2.)
    {
        if (position.x < -32.f || position.x > 10.f)
        {
            return true;
        }
    }

    return false;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    glm::vec3 nextPos;
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            nextPos = camera.nextMoveUp(1.);
            if (!checkCollision(nextPos))
            {
                camera.moveUp(1.);
            }
            break;
        case GLFW_KEY_A:
            nextPos = camera.nextMoveLeft(1.f);
            if (!checkCollision(nextPos))
            {
                camera.moveLeft(1.);
            }
            break;
        case GLFW_KEY_S:
            nextPos = camera.nextMoveUp(-1.f);
            if (!checkCollision(nextPos))
            {
                camera.moveUp(-1.);
            }
            break;
        case GLFW_KEY_D:
            nextPos = camera.nextMoveLeft(-1.f);
            if (!checkCollision(nextPos))
            {
                camera.moveLeft(-1.);
            }
            break;
        case GLFW_KEY_E:
            camera.rotateLeft(-15.);
            break;
        case GLFW_KEY_Q:
            camera.rotateLeft(15.f);
            break;
        }
    }
}

static void mouse_button_callback(GLFWwindow * /*window*/, int /*button*/, int /*action*/, int /*mods*/)
{
}

static void scroll_callback(GLFWwindow * /*window*/, double /*xoffset*/, double /*yoffset*/)
{
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (first_mouse)
    {
        last_xpos = xpos;
        last_ypos = ypos;
        first_mouse = false;
    }

    camera.rotateLeft(xpos - last_xpos);
    camera.rotateUp(last_ypos - ypos);

    last_xpos = xpos;
    last_ypos = ypos;
}

static void size_callback(GLFWwindow * /*window*/, int width, int height)
{
    window_width = width;
    window_height = height;
}

int main(int argc, char *argv[])
{
    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }

    /* Create a window and its OpenGL context */
    // #ifdef __APPLE__
    //     /* We need to explicitly ask for a 3.3 context on Mac */
    //     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // #endif
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "2 salles 2 ambiances", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Intialize glad (loads the OpenGL functions) */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    glimac::FilePath applicationPath(argv[0]);

    glEnable(GL_DEPTH_TEST);
    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);

    glm::mat4 proj_matrix = glm::perspective(glm::radians(70.f), (float)window_width / window_height, 0.1f, 100.f);
    auto viewMatrix = camera.getViewMatrix();

    auto firstRoom_light_posY = camera.cameraPosition().y + 10.f;

    Renderer renderer(proj_matrix, viewMatrix);
    Skybox skybox(applicationPath);
    Room fr, sr;

    /* Init first Box*/
    fr.initFirstRoom(applicationPath.dirPath() + "src/shaders/3D.vs.glsl",
                     applicationPath.dirPath() + "src/shaders/directionallight.fs.glsl",
                     camera.cameraPosition(), bboxVector, applicationPath);

    fr.translateSpotLight(glm::vec3(camera.cameraPosition().x, firstRoom_light_posY, camera.cameraPosition().z), 0);
    fr.setSpotLightDirection(camera.cameraPosition(), 0);
    fr.translateSpotLight(glm::vec3(camera.cameraPosition().x, firstRoom_light_posY + 5.f, camera.cameraPosition().z), 1);

    fr.translateSpotLight(glm::vec3(camera.cameraPosition().x, firstRoom_light_posY, camera.cameraPosition().z - 5.f), 1);
    fr.setSpotLightDirection(glm::vec3(camera.cameraPosition().x, 0., camera.cameraPosition().z - 5.f), 1);

    fr.setGlobalLightPos(glm::vec3(camera.cameraPosition().x, firstRoom_light_posY, camera.cameraPosition().z));

    /* Init second Box */
    auto cPos = camera.cameraPosition();
    auto shift = glm::vec3(cPos.x - 24.f, cPos.y, cPos.z);
    sr.initSecondRoom(applicationPath.dirPath() + "src/shaders/3D.vs.glsl",
                      applicationPath.dirPath() + "src/shaders/second_room.fs.glsl",
                      shift, applicationPath, bboxVector);

    glm::vec3 border = glm::vec3(cPos.x - 12, cPos.y, cPos.z);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        renderer.setViewMatrix(camera.getViewMatrix());

        glClearColor(1.f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.renderSkybox(skybox);
        renderer.renderFirstRoom(fr, camera.cameraPosition(), border);
        renderer.renderSecondRoom(sr, camera.cameraPosition(), border);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
        updateDeltaTime();
    }
    fr.clearBuffers();
    sr.clearBuffers();

    glfwTerminate();
    return 0;
}