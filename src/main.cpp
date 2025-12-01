#define GLFW_INCLUDE_NONE // Ensures no header conflicts; GLFW and GLAD can interfere if swapped
#include <glad/glad.h> // Extension loader library
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Scene.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// Callbacks
void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* MyWindow, int key, int scancode, int action, int mods); // Make local to this file
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

    // Scene Setup
    Scene* myScene = new Scene();

    // Shader setup
    myScene->SetShader("shaders/simpleShader/shader.vs", "shaders/simpleShader/shader.fs");

    myScene->SetupUniforms();

    // Object setup
    float color[] = {0.5f, 0.5f, 0.5f};
    float* shininess = new float(32.0f);

    myScene->SetObject("models/alien.obj");
    myScene->SetObjectColor(glm::vec3(0.5f, 0.5f, 0.5f));
    myScene->SetObjectSpec(glm::vec3(0.5f, 0.5f, 0.5f));
    myScene->SetObjectShine(shininess);

    // Camera Setup
    glm::vec3 viewPos = glm::vec3(0, 0, -4);
    float fov = 45.0f;

    myScene->SetCameraPos(viewPos);
    myScene->SetCameraProj(fov);

    // Light Setup
    // Point light properties
    glm::vec3 lightPos = glm::vec3(0, 0, -2);
    glm::vec3 plightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 plightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 plightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    float constant = 1.0f;
    float linear = 0.7f;
    float quadratic = 1.8f;

    myScene->SetPLightPosition(lightPos);
    myScene->SetPLightAmbient(plightAmbient);
    myScene->SetPLightDiffuse(plightDiffuse);
    myScene->SetPLightSpecular(plightSpecular);
    myScene->SetPLightAttenuation(constant, linear, quadratic);

    // Directional light properties
    glm::vec3 dirLightDir = glm::vec3(0.0f, -1.0f, -1.0f);
    glm::vec3 dirLightAmb = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 dirLightDif = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 dirLightSpc = glm::vec3(1.0f, 1.0f, 1.0f);

    myScene->SetDirLightDirection(dirLightDir);
    myScene->SetDirLightAmbient(dirLightAmb);
    myScene->SetDirLightDiffuse(dirLightDif);
    myScene->SetDirLightSpecular(dirLightSpc);

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
        ImGui::Begin("Scene Properties");
        
        ImGui::ColorEdit3("Object Color", color);
        glm::vec3 colorVec = glm::vec3(color[0], color[1], color[2]);
        myScene->SetObjectColor(colorVec);

        ImGui::SliderFloat("Shininess", shininess, 2.0f, 256.0f);
        myScene->SetObjectShine(shininess);

        ImGui::End();

        myScene->Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
        glfwSwapBuffers(window); // Swap front and back buffer

    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    
    // Clean up and shut down
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

}

/*
 * Called when framebuffer size changes; Is necessary for any window
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, 800, 800);
}
