/****************************************************************************************************
 * Name:         Lucas Robenolt
 * 
 * Email:        robenoltl@gmail.com
 * 
 * Last Updated: 3/22/2025
 * 
 * Purpose:      This class is used to easily create shader programs by creating
 *               a shader object that holds a shader program ID. 
 * 
 * Constructors: Shader(const char* vShaderPath, const char* fShaderPath)
 *               Used to create a shader program with only a vertex shader
 *               and fragment shader attached
 * 
 *               Shader(const char* vShaderPath, const char* gShaderPath, const char* fShaderPath)
 *               Used to create a shader program with a vertex, geometry, and fragment shader
 *               attached.
 * 
 ****************************************************************************************************/


#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

class Shader {
    public:
        unsigned int ID; // Holds ID of the create shader program

        // Constructor for a shader program with only vertex and fragment shader
        Shader(const char* vShaderPath, const char* fShaderPath) {

            std::ifstream vShaderFile;
            std::ifstream fShaderFile;
            std::stringstream vShaderCodeStrm;
            std::stringstream fShaderCodeStrm;
            std::string vShaderCodeStr;
            std::string fShaderCodeStr;
            const char* vShaderSource;
            const char* fShaderSource;

            // Specify which error flags should throw exceptions when certain conditions arise while reading from a file.
            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            // Open vertex and fragment shader code
            // ------------------------------------
            try {
                // Open files
                vShaderFile.open(vShaderPath);
                fShaderFile.open(fShaderPath);

                // Copy contents from file to string stream; uses pointer to ifstream buffer
                vShaderCodeStrm << vShaderFile.rdbuf();
                fShaderCodeStrm << fShaderFile.rdbuf();

                // Copy internal buffer of stringstream to string
                vShaderCodeStr = vShaderCodeStrm.str();
                fShaderCodeStr = fShaderCodeStrm.str();

                // Convert to c string
                vShaderSource = vShaderCodeStr.c_str();
                fShaderSource = fShaderCodeStr.c_str();

                // Close files
                vShaderFile.close();
                fShaderFile.close();

            }
            catch (std::ifstream::failure e) {
                std::cerr << "ERROR: Failure reading shader files: " << e.what() << std::endl;
            }

            // Variables for error checking
            int success;
            char infolog[512];

            // Create shader program
            // ---------------------
            unsigned int vertexShader;
            unsigned int fragmentShader;

            // Create vertex shader, add source code, and compile
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vShaderSource, NULL);
            glCompileShader(vertexShader);

            // Check vertex shader compile status
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
                std::cout << "ERROR: Failure compiling vertex shader: " << infolog << std::endl;
            }

            // Create fragment shader, add source code, and compile
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
            glCompileShader(fragmentShader);

            // Check fragment shader compile status
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
                std::cout << "ERROR: Failure compiling fragment shader: " << infolog << std::endl; 
            }

            // Link vertex and fragment shader, then create shader program
            ID = glCreateProgram();
            glAttachShader(ID, vertexShader);
            glAttachShader(ID, fragmentShader);
            glLinkProgram(ID);

            // Check shader program link status
            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if(!success) {
                glGetProgramInfoLog(ID, 512, NULL, infolog);
                std::cout << "ERROR: Failure linking shader program: " << infolog << std::endl;
            }

            // Delete vertex and fragment shader now that they are linked
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

        }

        // Constructor for a shader program with only vertex and fragment shader
        Shader(const char* vShaderPath, const char* gShaderPath, const char* fShaderPath) {

            std::ifstream vShaderFile;
            std::ifstream gShaderFile;
            std::ifstream fShaderFile;
            std::stringstream vShaderCodeStrm;
            std::stringstream gShaderCodeStrm;
            std::stringstream fShaderCodeStrm;
            std::string vShaderCodeStr;
            std::string gShaderCodeStr;
            std::string fShaderCodeStr;
            const char* vShaderSource;
            const char* gShaderSource;
            const char* fShaderSource;

            // Specify which error flags should throw exceptions when certain conditions arise while reading from a file.
            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            // Open vertex and fragment shader code
            // ------------------------------------
            try {
                // Open files
                vShaderFile.open(vShaderPath);
                gShaderFile.open(gShaderPath);
                fShaderFile.open(fShaderPath);

                // Copy contents from file to string stream; uses pointer to ifstream buffer
                vShaderCodeStrm << vShaderFile.rdbuf();
                gShaderCodeStrm << gShaderFile.rdbuf();
                fShaderCodeStrm << fShaderFile.rdbuf();

                // Copy internal buffer of stringstream to string
                vShaderCodeStr = vShaderCodeStrm.str();
                gShaderCodeStr = gShaderCodeStrm.str();
                fShaderCodeStr = fShaderCodeStrm.str();

                // Convert to c string
                vShaderSource = vShaderCodeStr.c_str();
                gShaderSource = gShaderCodeStr.c_str();
                fShaderSource = fShaderCodeStr.c_str();

                // Close files
                vShaderFile.close();
                gShaderFile.close();
                fShaderFile.close();

            }
            catch (std::ifstream::failure e) {
                std::cerr << "ERROR: Failure reading shader files: " << e.what() << std::endl;
            }

            // Variables for error checking
            int success;
            char infolog[512];

            // Create shader program
            // ---------------------
            unsigned int vertexShader;
            unsigned int geometryShader;
            unsigned int fragmentShader;

            // Create vertex shader, add source code, and compile
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vShaderSource, NULL);
            glCompileShader(vertexShader);

            // Check vertex shader compile status
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
                std::cout << "ERROR: Failure compiling vertex shader: " << infolog << std::endl;
            }

            // Create a geometry shader, add source code, and compile
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometryShader, 1, &gShaderSource, NULL);
            glCompileShader(geometryShader);

            glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(geometryShader, 512, NULL, infolog);
                std::cout << "ERROR: Failure compiling geometry shader: " << infolog << std::endl;
            }

            // Create fragment shader, add source code, and compile
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
            glCompileShader(fragmentShader);

            // Check fragment shader compile status
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
                std::cout << "ERROR: Failure compiling fragment shader: " << infolog << std::endl; 
            }

            // Link vertex, geometry, and fragment shader, then create shader program
            ID = glCreateProgram();
            glAttachShader(ID, vertexShader);
            glAttachShader(ID, geometryShader);
            glAttachShader(ID, fragmentShader);
            glLinkProgram(ID);

            // Check shader program link status
            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if(!success) {
                glGetProgramInfoLog(ID, 512, NULL, infolog);
                std::cout << "ERROR: Failure linking shader program: " << infolog << std::endl;
            }

            // Delete vertex and fragment shader now that they are linked
            glDeleteShader(vertexShader);
            glDeleteShader(geometryShader);
            glDeleteShader(fragmentShader);

        }



        void use() { glUseProgram(ID); }

};

#endif