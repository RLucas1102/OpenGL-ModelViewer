#define GLFW_INCLUDE_NONE // Ensures no header conflicts; GLFW and GLAD can interfere if swapped
#include <glad/glad.h> // Extension loader library
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ShaderLoader.h>
#include <modelLoader/model.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// Callbacks
void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* MyWindow, int key, int scancode, int action, int mods); // Make local to this file
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Light properties
glm::vec3 lightPos = glm::vec3(0, 1, -4);

glm::vec3 plightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
glm::vec3 plightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 plightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
float constant = 1.0f;
float linear = 0.7f;
float quadratic = 1.8f;

glm::vec3 dirLightDir = glm::vec3(0.0f, -1.0f, -1.0f);
glm::vec3 dirLightAmb = glm::vec3(0.1f, 0.1f, 0.1f);
glm::vec3 dirLightDif = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 dirLightSpc = glm::vec3(1.0f, 1.0f, 1.0f);

bool shiftDown = false;

int main() {

   // One of the few functions that can be called before GLFW is initalized
    glfwSetErrorCallback(error_callback);

    // Need to initialize GLFW first to use any functions tha require it
    if (!glfwInit()) {
        std::cerr << "Could not initialize GLFW" << std::endl;
        return 1;
    }
    else {
        // Using OpenGL 3.3 Core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    // Create combined GLFW window and context object
    // Context creation is dependent on correctly installed drivers
    GLFWwindow* window = glfwCreateWindow(800, 800, "CPSC6050: Shading and Texturing", NULL, NULL);
    if(!window) {
        glfwTerminate();
        std::cerr << "Could not create window" << std::endl;
        return 1;
    }

    // In order to use OpenGL API, you make a context current. In this case, our window
    glfwMakeContextCurrent(window);

    // Set window callbacks and settings
    glfwSetKeyCallback(window, key_callback); // ESCAPE to close window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1); // Set swap interval to 1; by default it is 0 and will waste CPU and GPU time on fast machines

    // Access to all OpenGL core and extension functions supported by both the context we created and the glad loader we generated.
    // Load OpenGL function pointers by retrieving function address and casting to GLADloadproc type (Used by GLAD to load functions)
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Could not load GLAD" << std::endl;
        return 1;
    }

    // GUI Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Create Shader Programs
    Shader simpleShader("shaders/simpleShader/shader.vs", "shaders/simpleShader/shader.fs");
    Shader cubeLightShader("shaders/simpleShader/shader.vs", "shaders/simpleShader/shaderLight.fs");

    // Create model
    std::vector<Model*> sceneObjects;

    Model* cube = new Model("./models/cube.obj");
    Model* alien = new Model("./models/alien.obj");
    Model cubeLight("./models/cube.obj");

    alien->ChangeDiffuse(glm::vec3(1.0f, 0.0f, 0.0f));
    cube->ChangeDiffuse(glm::vec3(0.0f, 0.0f, 1.0f));

    alien->ChangeShine(128.0f);
    cube->ChangeShine(8.0f);

    sceneObjects.push_back(cube);
    sceneObjects.push_back(alien);
    
    // Create buffer and generate ID
    unsigned int vpUBO, lightUBO, materialUBO, dirLightUBO;       
    glGenBuffers(1, &vpUBO);
    glGenBuffers(1, &lightUBO);
    glGenBuffers(1, &materialUBO);
    glGenBuffers(1, &dirLightUBO);

    // Get Uniform block location
    unsigned int simpleMVPBlockIdx = glGetUniformBlockIndex(simpleShader.ID, "Matrices");
    unsigned int simpleLightBlockIdx = glGetUniformBlockIndex(simpleShader.ID, "pLight");
    unsigned int simpleMaterialBlockIdx = glGetUniformBlockIndex(simpleShader.ID, "Material");
    unsigned int simpleDirLightBlockIdx = glGetUniformBlockIndex(simpleShader.ID, "DirLight");

    unsigned int cubeLightMVPBlockIdx = glGetUniformBlockIndex(cubeLightShader.ID, "Matrices");

    // Bind each shaders uniform block to a binding point
    glUniformBlockBinding(simpleShader.ID, simpleMVPBlockIdx, 0);
    glUniformBlockBinding(simpleShader.ID, simpleLightBlockIdx, 1);
    glUniformBlockBinding(simpleShader.ID, simpleMaterialBlockIdx, 2);
    glUniformBlockBinding(simpleShader.ID, simpleDirLightBlockIdx, 3);

    glUniformBlockBinding(cubeLightShader.ID, cubeLightMVPBlockIdx, 0);

    // Bind UBO and reserve space in the uniform buffer object
    glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0); // Unbind target

    glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
    glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4) + 3 * sizeof(float), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, materialUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4) + 1 * sizeof(float), NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, dirLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Bind all of the uniform buffer objects to binding points
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, vpUBO, 0, 2 * sizeof(glm::mat4));
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightUBO, 0, 4 * sizeof(glm::vec4) + 3 * sizeof(float));
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, materialUBO, 0, 2 * sizeof(glm::vec4) + 1 * sizeof(float));
    glBindBufferRange(GL_UNIFORM_BUFFER, 3, dirLightUBO, 0, 4 * sizeof(glm::vec4));
    
    // Colors
    glm::vec3 skyblue(135.0f, 206.0f, 235.0f);
    skyblue =  1/255.0f * skyblue;

    // Window Clear color
    glm::vec3 windowColor;
    windowColor = skyblue;

    // Depth testing
    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents(); // Process received events
        
        glClearColor(windowColor.x, windowColor.y, windowColor.z, 1.0f); // Set color to clear window with
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen with color
        
        // GUI Start Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // GUI Widgets
        ImGui::Begin("My name is window, ImGUI Window");
        ImGui::Text("Hello there adventurer!");
        
        float color[3];
        if(ImGui::ColorEdit3("Object Color", color)) {

            glm::vec3 colorIn = glm::vec3(color[0], color[1], color[2]);

            cube->ChangeDiffuse(colorIn);
        }

        ImGui::End();

        // Camera
        glm::vec3 viewPos = glm::vec3(0, 0, -4);

        glm::mat4 view = glm::mat4(1.0f);
        view *= glm::lookAt(viewPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        // Projection
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), 800.0f/800.0f, 0.1f, 100.0f);

        // Bind uniform buffer object and send matrices to vertex shader
        glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Load light properties
        glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(plightAmbient));
        glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(plightDiffuse));
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(plightSpecular));
        glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), sizeof(float), &constant);
        glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4) + 1 * sizeof(float), sizeof(float), &linear);
        glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4) + 2 * sizeof(float), sizeof(float), &quadratic);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBuffer(GL_UNIFORM_BUFFER, dirLightUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(dirLightDir));
        glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(dirLightAmb));
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(dirLightDif));
        glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(dirLightSpc));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Light
        cubeLightShader.use();
        
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

        glUniform3f(glGetUniformLocation(cubeLightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z); 

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.1f));
        unsigned int lightModelLoc = glGetUniformLocation(cubeLightShader.ID, "model");
        glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, &lightModel[0][0]);

        glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(lightPos));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        cubeLight.Draw();

        // Scene
        simpleShader.use();
        
        for (int i = 0; i < sceneObjects.size(); i++) {
            
            // Load material properties
            glBindBuffer(GL_UNIFORM_BUFFER, materialUBO);
            sceneObjects.at(i)->SetMaterials();
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(cos(i), sin(i), 0.0f));
            unsigned int modelLoc = glGetUniformLocation(simpleShader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

            sceneObjects.at(i)->Draw();
        
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
        glfwSwapBuffers(window); // Swap front and back buffer

    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    
    // Clean up and shut down
    glDeleteBuffers(1, &vpUBO);
    glDeleteBuffers(1, &lightUBO);
    glDeleteBuffers(1, &dirLightUBO);
    glDeleteBuffers(1, &materialUBO);
    glfwTerminate(); // Release all GLFW resources and close windows

    return 0;
}


/*
 * In case a GLFW function fails, an error is reported 
 * to the GLFW error callback. You can receive these 
 * reports with an error callback.
 *
 */
void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

/* 
 * Receive key press when ESCAPE key is pressed
 */
void key_callback(GLFWwindow *MyWindow, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(MyWindow, true);
    }
    else if (key == GLFW_KEY_LEFT) {
        lightPos += glm::vec3(0.1f, 0.0f, 0.0f);
    }
    else if (key == GLFW_KEY_UP) {
        if (shiftDown) {
            lightPos += glm::vec3(0.0f, 0.1f, 0.0f);
        }
        else {
            lightPos += glm::vec3(0.0f, 0.0f, 0.1f);
        }
    }
    else if (key == GLFW_KEY_DOWN) {
        if (shiftDown) {
            lightPos -= glm::vec3(0.0, 0.1f, 0.0f);
        }
        else {
            lightPos -= glm::vec3(0.0f, 0.0f, 0.1f);
        }
    }
    else if (key == GLFW_KEY_RIGHT) {
        lightPos -= glm::vec3(0.1f, 0.0f, 0.0f);
    }
    else if (key == GLFW_KEY_LEFT_SHIFT) {
        if (action == GLFW_PRESS) {
            shiftDown = true;
        }
        else if (action == GLFW_RELEASE) {
            shiftDown = false;
        }
    }
}

/*
 * Called when framebuffer size changes; Is necessary for any window
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, 800, 800);
}
