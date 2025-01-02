#define GLFW_INCLUDE_NONE
#include "include/Camera.hpp"
#include "include/Renderer.hpp"
#include "include/Sphere.hpp"
#include "include/Quad.hpp"
#include "include/Room.hpp"
#include "include/FirstRoom.hpp"

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

Camera camera{};

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            camera.moveUp(1.);
            break;
        case GLFW_KEY_A:
            camera.moveLeft(1.);
            break;
        case GLFW_KEY_S:
            camera.moveUp(-1.);
            break;
        case GLFW_KEY_D:
            camera.moveLeft(-1.);
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
    // Room room;
    FirstRoom fr, sr;

    /* Init first Room*/
    // fr.initFirstRoom(applicationPath.dirPath() + "src/shaders/3D.vs.glsl",
    //                  applicationPath.dirPath() + "src/shaders/directionallight.fs.glsl",
    //                  "/home/valentin/m2/opengl/2_salles_2_ambiances/src/assets/MC-Torch/model/obj/Torch.obj",
    //                  "/home/valentin/m2/opengl/2_salles_2_ambiances/src/assets/MC-Torch/model/obj/Torch.mtl",
    //                  camera.cameraPosition());
    // fr.translateSpotLight(glm::vec3(camera.cameraPosition().x, firstRoom_light_posY, camera.cameraPosition().z), 0);
    // fr.setSpotLightDirection(camera.cameraPosition(), 0);
    // // fr.translateSpotLight(glm::vec3(camera.cameraPosition().x, firstRoom_light_posY + 5.f, camera.cameraPosition().z), 1);

    // fr.translateSpotLight(glm::vec3(camera.cameraPosition().x, firstRoom_light_posY, camera.cameraPosition().z - 5.f), 1);
    // fr.setSpotLightDirection(glm::vec3(camera.cameraPosition().x, 0., camera.cameraPosition().z - 5.f), 1);

    // fr.setGlobalLightPos(glm::vec3(camera.cameraPosition().x, firstRoom_light_posY, camera.cameraPosition().z));

    /* Init second Room */
    sr.initSecondRoom(applicationPath.dirPath() + "src/shaders/3D.vs.glsl",
                      applicationPath.dirPath() + "src/shaders/second_room.fs.glsl",
                      camera.cameraPosition(), applicationPath);

    auto t = camera.cameraPosition();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        renderer.setViewMatrix(camera.getViewMatrix());

        glClearColor(1.f, 0.5f, 0.5f, 1.f);
        // glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));
        // glUniformMatrix4fv(mv_loc, 1, GL_FALSE, glm::value_ptr(mv_matrix));
        // glUniformMatrix4fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_matrix));

        // renderer.render(sphere, uniformVariable);
        // renderer.render(bounds, room.getUniformVariable());

        // renderer.renderFirstRoom(fr);
        renderer.renderSecondRoom(sr, camera.cameraPosition());

        // glBindVertexArray(sphere.getMeshBuffer()->vao);
        // glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        // GLenum err = glGetError();
        // if (err != GL_NO_ERROR)
        // {
        //     std::cerr << "OpenGL error: " << err << std::endl;
        // }
        // glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}