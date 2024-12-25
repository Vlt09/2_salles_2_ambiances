#define GLFW_INCLUDE_NONE
#include "include/Camera.hpp"
#include "include/Renderer.hpp"
#include "include/Sphere.hpp"
#include "include/Quad.hpp"
#include "include/Room.h"
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
    glimac::Program program = glimac::loadProgram(applicationPath.dirPath() + "src/shaders/3D.vs.glsl",
                                                  applicationPath.dirPath() + "src/shaders/normal.fs.glsl");

    program.use();

    auto mvp_loc = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    auto mv_loc = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    auto normal_loc = glGetUniformLocation(program.getGLId(), "uNormalMatrix");

    glEnable(GL_DEPTH_TEST);
    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);

    auto mv_matrix = glm::translate(glm::mat4(1), glm::vec3(0., 0., -5.));
    glm::mat4 proj_matrix = glm::perspective(glm::radians(70.f), (float)window_width / window_height, 0.1f, 100.f);
    auto normal_matrix = glm::transpose(glm::inverse(mv_matrix));
    auto viewMatrix = camera.getViewMatrix();

    Room::UniformMatrix uniformMatrix;

    uniformMatrix.uMVPMatrix = mvp_loc;
    uniformMatrix.uMVMatrix = mv_loc;
    uniformMatrix.uNormalMatrix = normal_loc;

    Renderer renderer(proj_matrix, viewMatrix);
    Room room;
    room.constructRoom(camera.cameraPosition());
    Sphere sphere(1, 32, 16);
    Quad quad(5, 5);

    auto bounds = room.getBounds();
    // bounds.rotateModel(90.f, glm::vec3(1.f, 0.f, 0.f));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        renderer.setViewMatrix(camera.getViewMatrix());

        glClearColor(1.f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix * mv_matrix));
        // glUniformMatrix4fv(mv_loc, 1, GL_FALSE, glm::value_ptr(mv_matrix));
        // glUniformMatrix4fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_matrix));

        renderer.render(bounds, uniformMatrix, mv_matrix);

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