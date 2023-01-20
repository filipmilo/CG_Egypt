/**
* Autor: Nedeljko Tesanovic
* Namjena: Demonstracija upotrebe sablonskog projekta za ucitavanje i prikaz modela, 3D transformacije, perspektivne projekcije i klase za bafere
* Original file info
 * @file main.cpp
 * @author Jovan Ivosevic
 * @brief Base project for Computer Graphics course
 * @version 0.1
 * @date 2022-10-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "shader.hpp"
#include "model.hpp" //Klasa za ucitavanje modela
#include "renderable.hpp" //Klasa za bafere
#include "camera.hpp"

int WindowWidth = 800;
int WindowHeight = 800;
const std::string WindowTitle = "Egypt At Night";
const float TargetFPS = 60.0f;
const float TargetFrameTime = 1.0f / TargetFPS;

// carpet movement
float carpetX = 3.5;
float carpetZ = -4.0;

struct Input {
    bool MoveLeft;
    bool MoveRight;
    bool MoveUp;
    bool MoveDown;
    bool LookLeft;
    bool LookRight;
    bool LookUp;
    bool LookDown;
};

struct EngineState {
    Input* mInput;
    Camera* mCamera;
    float mDT;
};
 
const float intensityMap[5][2] = {{0.7, 1.8} , {0.35, 0.44}, {0.22, 0.20}, {0.14, 0.07}, {0.09, 0.032}};

/*
static void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
    switch (key) {
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
    }
}
*/

static void
ErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

static void
FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, width, height);
}

static void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);
    Input* UserInput = State->mInput;
    bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
    switch (key) {
    case GLFW_KEY_A: UserInput->MoveLeft = IsDown; break;
    case GLFW_KEY_D: UserInput->MoveRight = IsDown; break;
    case GLFW_KEY_W: UserInput->MoveUp = IsDown; break;
    case GLFW_KEY_S: UserInput->MoveDown = IsDown; break;

    case GLFW_KEY_RIGHT: UserInput->LookLeft = IsDown; break;
    case GLFW_KEY_LEFT: UserInput->LookRight = IsDown; break;
    case GLFW_KEY_UP: UserInput->LookUp = IsDown; break;
    case GLFW_KEY_DOWN: UserInput->LookDown = IsDown; break;

    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
    }
}

static void
HandleInput(EngineState* state) {
    Input* UserInput = state->mInput;
    Camera* FPSCamera = state->mCamera;
    if (UserInput->MoveLeft) FPSCamera->Move(-1.0f, 0.0f, state->mDT);
    if (UserInput->MoveRight) FPSCamera->Move(1.0f, 0.0f, state->mDT);
    if (UserInput->MoveDown) FPSCamera->Move(0.0f, -1.0f, state->mDT);
    if (UserInput->MoveUp) FPSCamera->Move(0.0f, 1.0f, state->mDT);

    if (UserInput->LookLeft) FPSCamera->Rotate(1.0f, 0.0f, state->mDT);
    if (UserInput->LookRight) FPSCamera->Rotate(-1.0f, 0.0f, state->mDT);
    if (UserInput->LookDown) FPSCamera->Rotate(0.0f, -1.0f, state->mDT);
    if (UserInput->LookUp) FPSCamera->Rotate(0.0f, 1.0f, state->mDT);

}


void handleKeys(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_I))
    {
        std::cout << "I was pressed" << std::endl;
        carpetZ += 0.05;
    }
    else if (glfwGetKey(window, GLFW_KEY_K))
    {
        std::cout << "K was pressed" << std::endl;
        carpetZ -= 0.05;
    }
    else if (glfwGetKey(window, GLFW_KEY_J))
    {
        std::cout << "J was pressed" << std::endl;
        carpetX += 0.05;
    }
    else if (glfwGetKey(window, GLFW_KEY_L))
    {
        std::cout << "L was pressed" << std::endl;
        carpetX -= 0.05;
    }
}

void makeCarpet(Shader &Basic, Renderable &cube, glm::mat4 m, float carpetY)
{
    Basic.SetUniform3f("uColor", glm::vec3(0.78, 0.78, 0.78));

    m = glm::translate(glm::mat4(1.0f), glm::vec3(carpetX, carpetY, carpetZ));
    m = glm::scale(m, glm::vec3(1.0, 0.1, 0.5));
    Basic.SetModel(m);
    cube.Render();

    Basic.SetUniform3f("uColor", glm::vec3(1.0, 0.0, 0.0));

    m = glm::translate(glm::mat4(1.0f), glm::vec3(carpetX + 0.4, carpetY, carpetZ));
    m = glm::scale(m, glm::vec3(1.0, 0.1, 0.5));
    Basic.SetModel(m);
    cube.Render();

    Basic.SetUniform3f("uColor", glm::vec3(0.0, 1.0, 0.0));

    m = glm::translate(glm::mat4(1.0f), glm::vec3(carpetX, carpetY, carpetZ + 0.2));
    m = glm::scale(m, glm::vec3(1.0, 0.1, 0.5));
    Basic.SetModel(m);
    cube.Render();


    Basic.SetUniform3f("uColor", glm::vec3(0.0, 0.0, 1.0));

    m = glm::translate(glm::mat4(1.0f), glm::vec3(carpetX + 0.4, carpetY, carpetZ + 0.2));
    m = glm::scale(m, glm::vec3(1.0, 0.1, 0.5));
    Basic.SetModel(m);
    cube.Render();

}

static void
DrawFloor(unsigned vao, const Shader& shader, unsigned diffuse, unsigned specular) {
    glUseProgram(shader.GetId());
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular);
    float Size = 4.0f;
    for (int i = -2; i < 4; ++i) {
        for (int j = -2; j < 4; ++j) {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(i * Size, 0.0f, j * Size));
            Model = glm::scale(Model, glm::vec3(Size, 0.1f, Size));
            shader.SetModel(Model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

int main() {
    GLFWwindow* Window = 0;
    if (!glfwInit()) {
        std::cerr << "Failed to init glfw" << std::endl;
        return -1;
    }
    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(ErrorCallback);

    Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), 0, 0);
    if (!Window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);
    
    EngineState State = { 0 };
    Camera FPSCamera;
    Input UserInput = { 0 };
    State.mCamera = &FPSCamera;
    State.mInput = &UserInput;
    glfwSetWindowUserPointer(Window, &State);
    

    glfwSetKeyCallback(Window, KeyCallback);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
    

    GLenum GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        std::cerr << "Failed to init glew: " << glewGetErrorString(GlewError) << std::endl;
        glfwTerminate();
        return -1;
    }

    

    Shader BasicShader("shaders/basic_old.vert", "shaders/basic.frag");
    Shader ColorShader("shaders/color.vert", "shaders/color.frag");
    Shader PhongShaderMaterialTexture("shaders/basic.vert", "shaders/phong_material_texture.frag");

    //Setup Phong shader
    glUseProgram(PhongShaderMaterialTexture.GetId());
    
    PhongShaderMaterialTexture.SetUniform3f("uDirLight.Direction", glm::vec3(20.0, 25.5, 10.0));
    PhongShaderMaterialTexture.SetUniform3f("uDirLight.Ka", glm::vec3(0.75294f, 0.75294f, 0.75294f));
    PhongShaderMaterialTexture.SetUniform3f("uDirLight.Kd", glm::vec3(0.5f, 0.5f, 0.5f));
    PhongShaderMaterialTexture.SetUniform3f("uDirLight.Ks", glm::vec3(1.0f));

    PhongShaderMaterialTexture.SetUniform3f("uPointLights[0].Ka", glm::vec3(1.0f, 0.843f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uPointLights[0].Kd", glm::vec3(0.0f, 0.5f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uPointLights[0].Ks", glm::vec3(1.0f));
    PhongShaderMaterialTexture.SetUniform1f("uPointLights[0].Kc", 1.0f);
    PhongShaderMaterialTexture.SetUniform1f("uPointLights[0].Kl", intensityMap[0][0]);
    PhongShaderMaterialTexture.SetUniform1f("uPointLights[0].Kq", intensityMap[0][1]);

    PhongShaderMaterialTexture.SetUniform3f("uPointLights[1].Ka", glm::vec3(1.0f, 0.843f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uPointLights[1].Kd", glm::vec3(0.0f, 0.5f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uPointLights[1].Ks", glm::vec3(1.0f));
    PhongShaderMaterialTexture.SetUniform1f("uPointLights[1].Kc", 1.0f);
    PhongShaderMaterialTexture.SetUniform1f("uPointLights[1].Kl", intensityMap[4][0]);
    PhongShaderMaterialTexture.SetUniform1f("uPointLights[1].Kq", intensityMap[4][1]);

    //glm::vec3(0.0f, 3.5f, -2.0f)
    //glm::vec3(20.0, 25.5, 10.0)
    //glm::vec3(20.0, 20.5, 10.0)

    //glm::vec3(1.0, 15.5, 5.0)
    //glm::vec3(1.0,-5.0, -1.0)
    PhongShaderMaterialTexture.SetUniform3f("uSpotlight.Position", glm::vec3(20.0, 20.5, 10.0));
    PhongShaderMaterialTexture.SetUniform3f("uSpotlight.Direction", -glm::vec3(20.0, 20.5, 10.0));
    PhongShaderMaterialTexture.SetUniform3f("uSpotlight.Ka", glm::vec3(1.0f, 0.0f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uSpotlight.Kd", glm::vec3(0.5f, 0.0f, 0.0f));
    PhongShaderMaterialTexture.SetUniform3f("uSpotlight.Ks", glm::vec3(1.0f));
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.Kc", 1.0f);
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.Kl", 0.0014f);
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.Kq", 0.000007f);
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.InnerCutOff", glm::cos(glm::radians(2.5f)));
    PhongShaderMaterialTexture.SetUniform1f("uSpotlight.OuterCutOff", glm::cos(glm::radians(2.5f)));
    // NOTE(Jovan): Diminishes the light's diffuse component by half, tinting it slightly red
    PhongShaderMaterialTexture.SetUniform1i("uMaterial.Kd", 0);
    // NOTE(Jovan): Makes the object really shiny
    PhongShaderMaterialTexture.SetUniform1i("uMaterial.Ks", 1);
    PhongShaderMaterialTexture.SetUniform1f("uMaterial.Shininess", 128.0f);
    glUseProgram(0);

    
    //Ucitavanje modela
    Model Jap("spider/spider.obj");
    if (!Jap.Load())
    {
        std::cout << "Failed to load model!\n";
        glfwTerminate();
        return -1;
    }

    float cubeVertices[] = 
    {
        -0.2, -0.2, -0.2,       0.0, 0.0, 0.0,
        +0.2, -0.2, -0.2,       0.0, 0.0, 0.0,
        -0.2, -0.2, +0.2,       0.0, 0.0, 0.0,
        +0.2, -0.2, +0.2,       0.0, 0.0, 0.0,

        -0.2, +0.2, -0.2,       0.0, 0.0, 0.0,
        +0.2, +0.2, -0.2,       0.0, 0.0, 0.0,
        -0.2, +0.2, +0.2,       0.0, 0.0, 0.0,
        +0.2, +0.2, +0.2,       0.0, 0.0, 0.0,
    };

    float funkyCubeVertices[] = //Sarena kocka
    {
        -0.2, -0.2, -0.2,       0.0, 0.0, 1.0,
        +0.2, -0.2, -0.2,       0.0, 1.0, 0.0,
        -0.2, -0.2, +0.2,       0.0, 0.5, 1.0,
        +0.2, -0.2, +0.2,       0.5, 0.0, 0.0,

        -0.0, +0.2, -0.0,       1.0, 0.0, 1.0,
        +0.0, +0.2, -0.0,       1.0, 1.0, 0.0,
        -0.0, +0.2, +0.0,       1.0, 1.0, 1.0,
        +0.0, +0.2, +0.0,       0.0, 0.0, 0.0,
    };

    unsigned int cubeIndices[] = {
        3, 6, 2,
        3, 7, 6,

        1, 7, 3,
        1, 5, 7,

        4, 6, 7,
        4, 7, 5,

        0, 4, 1,
        1, 4, 5,

        0, 2, 6,
        0, 6, 4,

        0, 1, 3,
        0, 3, 2
        
    }; //Indeksi za formiranje kocke

    //Pravljenje Renderable objekta (Generise VAO i VBO pri konstrukciji)
    Renderable cube(cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices));
    Renderable funkyCube(funkyCubeVertices, sizeof(funkyCubeVertices), cubeIndices, sizeof(cubeIndices));

    unsigned CubeDiffuseTexture = Texture::LoadImageToTexture("res/sand.jpg");
    unsigned CubeSpecularTexture = Texture::LoadImageToTexture("res/sand_spec.jpg");
    unsigned PyramidDiffuseTexture = Texture::LoadImageToTexture("res/pyramid.png");
    unsigned CarpetTexture = Texture::LoadImageToTexture("res/carpet.png");
    unsigned MoonTexture = Texture::LoadImageToTexture("res/moon.jpg");

    std::vector<float> CubeVertices = {
        // X     Y     Z     NX    NY    NZ    U     V    FRONT SIDE
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // R U
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
                                                        // LEFT SIDE
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // RIGHT SIDE
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // BOTTOM SIDE
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // TOP SIDE
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // BACK SIDE
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // L D
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // R U
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
    };

    unsigned CubeVAO;
    glGenVertexArrays(1, &CubeVAO);
    glBindVertexArray(CubeVAO);
    unsigned CubeVBO;
    glGenBuffers(1, &CubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, CubeVertices.size() * sizeof(float), CubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::vector<float> PyramidVertices = {
        // X     Y     Z     NX    NY    NZ    U     V    FRONT SIDE
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // R U
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
                                                        // LEFT SIDE
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.0f,  0.5f, -0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.0f,  0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.0f,  0.5f, -0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // RIGHT SIDE
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
         0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
         0.0f,  0.5f, -0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
         0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // BOTTOM SIDE
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // TOP SIDE
        -0.0f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // L D
         0.0f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.0f,  0.5f, -0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
         0.0f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
         0.0f,  0.5f, -0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.0f,  0.5f, -0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // BACK SIDE
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // L D
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
         0.0f,  0.5f, -0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
        -0.0f,  0.5f, -0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // R U
         0.0f,  0.5f, -0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
    };

    unsigned PyramidVAO;
    glGenVertexArrays(1, &PyramidVAO);
    glBindVertexArray(PyramidVAO);
    unsigned PyramidVBO;
    glGenBuffers(1, &PyramidVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, PyramidVertices.size() * sizeof(float), PyramidVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Matrica modela (transformacije)
    glm::mat4 m(1.0f);
    //mat4(A) generise matricu 4x4 sa A na glavnoj dijagonali

    //Matrica pogleda (kamere)
    glm::mat4 v = glm::lookAt(glm::vec3(0.0, 1.0, -5.5), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 View = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());

    //lookAt(pozicija kamere, tacka u koju kamera gleda, kako je kamera rotirana po osi definisanoj sa prethodne dvije tacke)
    
    //Matrica projekcije
    glm::mat4 p = glm::perspective(glm::radians(90.0f), (float)WindowWidth / WindowHeight, 0.1f, 100.0f);
    //perspective(FOV ugao, aspect ratio prozora, prednja odsjecna ravan i zadnja odsjecna ravan)
    
    float angle = 0;//Ugao za rotiranje aviona
    float movement = 0;//Offset za pomijeranje aviona

    float carpetY = 0.55;
    float capetYOffset = 0.002;
    float offsetMultiplier = 1;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glClearColor(0.074, 0.094, 0.384, 1.0);


    float cameraOffset = 0.0;

    float FrameStartTime = glfwGetTime();
    float FrameEndTime = glfwGetTime();
    float dt = FrameEndTime - FrameStartTime;

    int pointLightIntensity1 = 0;
    int pointLightIntensity2 = 4;
    int point1Direction = 1;
    int point2Direction = -1;
    int counter = 0;

    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        HandleInput(&State);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        FrameStartTime = glfwGetTime();
        p = glm::perspective(glm::radians(90.0f), (float)WindowWidth / WindowHeight, 0.1f, 100.0f);
        View = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());

        glUseProgram(PhongShaderMaterialTexture.GetId());
        PhongShaderMaterialTexture.SetProjection(p);
        PhongShaderMaterialTexture.SetView(View);
        PhongShaderMaterialTexture.SetUniform3f("uViewPos", FPSCamera.GetPosition());

        glm::vec3 PointLightPosition(-3.0f, 12.0f, -3.5f);
        glm::vec3 PointLight2Position(7.5f, 5.0f, 0.0f);
        
        PhongShaderMaterialTexture.SetUniform3f("uPointLights[0].Position", PointLightPosition);
        PhongShaderMaterialTexture.SetUniform3f("uPointLights[1].Position", PointLight2Position);

        PhongShaderMaterialTexture.SetUniform1f("uPointLights[0].Kl", intensityMap[pointLightIntensity1][0]);
        PhongShaderMaterialTexture.SetUniform1f("uPointLights[0].Kq", intensityMap[pointLightIntensity1][1]);
        
        PhongShaderMaterialTexture.SetUniform1f("uPointLights[1].Kl", intensityMap[pointLightIntensity2][0]);
        PhongShaderMaterialTexture.SetUniform1f("uPointLights[1].Kq", intensityMap[pointLightIntensity2][1]);


        

        if (counter == 25) {
            
            if (pointLightIntensity1 == 0)
                point1Direction = 1;
            else if (pointLightIntensity1 == 4)
                point1Direction = -1;

            if (pointLightIntensity2 == 0)
                point2Direction = 1;
            else if (pointLightIntensity2 == 4)
                point2Direction = -1;

            pointLightIntensity1 += point1Direction;
            pointLightIntensity2 += point2Direction;

            counter = 0;
        }
        
        counter++;


        //carpet
        glm::mat4 ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(carpetX, carpetY, carpetZ));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.5, 0.01, 3.0));
        PhongShaderMaterialTexture.SetModel(ModelMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, CarpetTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //moon
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(20.0, 25.5, 10.0));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4.0f));
        PhongShaderMaterialTexture.SetModel(ModelMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, MoonTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(45.0f), glm::vec3(1.0, 1.0, 1.0));
        PhongShaderMaterialTexture.SetModel(ModelMatrix);

        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //big pyramid
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-3.0f, 5.0f, -3.5f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(10.0f));
        PhongShaderMaterialTexture.SetModel(ModelMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PyramidDiffuseTexture);
        glBindVertexArray(PyramidVAO);
        glDrawArrays(GL_TRIANGLES, 0, PyramidVertices.size() / 8);
        
        //smol pyramid
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(7.5f, 1.5f, 0.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4.0f));
        PhongShaderMaterialTexture.SetModel(ModelMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PyramidDiffuseTexture);
        glBindVertexArray(PyramidVAO);
        glDrawArrays(GL_TRIANGLES, 0, PyramidVertices.size() / 8);

        //spooder
        m = glm::translate(glm::mat4(1.0f), glm::vec3(3.0, 0.0, 7.0)); //Pomjeranje za XYZ (cetvrta kolona matrice)
        m = glm::scale(m, glm::vec3(0.05, 0.05, 0.05)); //Skaliranje objekta po XYZ (dijagonala matrice. Ne stavljati 0!)
        PhongShaderMaterialTexture.SetModel(m);
        Jap.Render();

        DrawFloor(CubeVAO, PhongShaderMaterialTexture, CubeDiffuseTexture, CubeSpecularTexture);
        
        glUseProgram(ColorShader.GetId());
        ColorShader.SetProjection(p);
        ColorShader.SetView(View);
        
        //pointlight source
        m = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 12.0f, -3.5f));
        ColorShader.SetUniform3f("uColor", glm::vec3(1.0f, 0.843f, 0.0f));
        ColorShader.SetModel(m);
        cube.Render();

        m = glm::translate(glm::mat4(1.0f), glm::vec3(7.5f, 5.0f, 0.0f));
        ColorShader.SetUniform3f("uColor", glm::vec3(1.0f, 0.843f, 0.0f));
        ColorShader.SetModel(m);
        cube.Render();

        m = glm::translate(glm::mat4(1.0f), glm::vec3(20.0, 20.5, 10.0));
        ColorShader.SetUniform3f("uColor", glm::vec3(1.0f, 0.2f, 0.0f));
        ColorShader.SetModel(m);
        cube.Render();


        carpetY += capetYOffset * offsetMultiplier;
        if (carpetY + capetYOffset > 0.55 && offsetMultiplier == 1)
            offsetMultiplier = -1;
        else if (carpetY + capetYOffset < 0.5 && offsetMultiplier == -1)
            offsetMultiplier = 1;

        handleKeys(Window);

        glUseProgram(0);
        glfwSwapBuffers(Window);

        FrameEndTime = glfwGetTime();
        dt = FrameEndTime - FrameStartTime;
        if (dt < TargetFPS) {
            int DeltaMS = (int)((TargetFrameTime - dt) * 1e3f);
            std::this_thread::sleep_for(std::chrono::milliseconds(DeltaMS));
            FrameEndTime = glfwGetTime();
        }
        dt = FrameEndTime - FrameStartTime;
        State.mDT = FrameEndTime - FrameStartTime;
    }

    glfwTerminate();
    return 0;
}





