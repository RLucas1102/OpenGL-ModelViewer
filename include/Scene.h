#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include <modelLoader/model.h>
#include <ShaderLoader.h>

struct ObjectMaterial {

    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

};

struct DirLight {

    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

};

struct PointLight {

    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    
    float constant;
    float linear;
    float quadratic;

};

struct Camera { 

    glm::vec3 position;
    glm::mat4 view;
    glm::mat4 projection;

};

class Scene
{
    private:

        Model* _sceneObject;
        Shader* _sceneShader;
        unsigned int _mvpUBO, _dirLightUBO, _pointLightUBO, _materialUBO;
        DirLight* _sDirLight;
        PointLight* _sPLight; 
        Camera* _sCamera;
        ObjectMaterial* _sMaterial;
        glm::mat4 _sModelMat;

    public:
        Scene() {

            _sceneObject    = nullptr;
            _sceneShader    = nullptr;
            _sDirLight      = new DirLight;
            _sPLight        = new PointLight;
            _sCamera        = new Camera;
            _sMaterial      = new ObjectMaterial;
            _sModelMat      = glm::mat4(1.0f);

        };

        void SetShader(const char* vShaderPath, const char* fShaderPath) { _sceneShader = new Shader(vShaderPath, fShaderPath); }

        void SetObject(const char* modelPath) { _sceneObject = new Model(modelPath); }

        void SetupUniforms() {

            // Generate IDs
            glGenBuffers(1, &_mvpUBO);
            glGenBuffers(1, &_pointLightUBO);
            glGenBuffers(1, &_materialUBO);
            glGenBuffers(1, &_dirLightUBO);

            // Get Uniform block location
            unsigned int mvpBlockIdx         = glGetUniformBlockIndex(_sceneShader->ID, "Matrices");
            unsigned int lightBlockIdx      = glGetUniformBlockIndex(_sceneShader->ID, "pLight");
            unsigned int materialBlockIdx   = glGetUniformBlockIndex(_sceneShader->ID, "Material");
            unsigned int dirLightBlockIdx   = glGetUniformBlockIndex(_sceneShader->ID, "DirLight");

            // Bind each shaders uniform block to a binding point
            glUniformBlockBinding(_sceneShader->ID, mvpBlockIdx, 0);
            glUniformBlockBinding(_sceneShader->ID, lightBlockIdx, 1);
            glUniformBlockBinding(_sceneShader->ID, materialBlockIdx, 2);
            glUniformBlockBinding(_sceneShader->ID, dirLightBlockIdx, 3);

            // Bind UBO and reserve space in the uniform buffer object
            glBindBuffer(GL_UNIFORM_BUFFER, _mvpUBO);
            glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0); // Unbind target

            glBindBuffer(GL_UNIFORM_BUFFER, _pointLightUBO);
            glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4) + 3 * sizeof(float), NULL, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBuffer(GL_UNIFORM_BUFFER, _materialUBO);
            glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4) + 1 * sizeof(float), NULL, GL_STATIC_DRAW);

            glBindBuffer(GL_UNIFORM_BUFFER, _dirLightUBO);
            glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            // Bind all of the uniform buffer objects to binding points
            glBindBufferRange(GL_UNIFORM_BUFFER, 0, _mvpUBO,         0, 3 * sizeof(glm::mat4));
            glBindBufferRange(GL_UNIFORM_BUFFER, 1, _pointLightUBO,  0, 4 * sizeof(glm::vec4) + 3 * sizeof(float));
            glBindBufferRange(GL_UNIFORM_BUFFER, 2, _materialUBO,    0, 2 * sizeof(glm::vec4) + 1 * sizeof(float));
            glBindBufferRange(GL_UNIFORM_BUFFER, 3, _dirLightUBO,    0, 4 * sizeof(glm::vec4));

        }

        void SetDirLightDirection(const glm::vec3& direction) { _sDirLight->direction = direction; }

        void SetDirLightAmbient(const glm::vec3& ambient) { _sDirLight->ambient = ambient; }

        void SetDirLightDiffuse(const glm::vec3& diffuse) { _sDirLight->diffuse = diffuse; }

        void SetDirLightSpecular(const glm::vec3& specular) { _sDirLight->specular = specular; }
        
        void SetPLightPosition(const glm::vec3& position) { _sPLight->position = position; }

        void SetPLightAmbient(const glm::vec3& ambient) { _sPLight->ambient = ambient; }

        void SetPLightDiffuse(const glm::vec3& diffuse) { _sPLight->diffuse = diffuse; }

        void SetPLightSpecular(const glm::vec3& specular) { _sPLight->specular = specular; }

        void SetPLightAttenuation(float constant, float linear, float quadratic) {
            _sPLight->constant  = constant;
            _sPLight->linear    = linear;
            _sPLight->quadratic = quadratic;
        }

        void SetModelMat(const glm::mat4& modelMat) {_sModelMat = modelMat; }

        void SetObjectColor(const glm::vec3 diffuse) {_sMaterial->diffuse = diffuse; }

        void SetObjectSpec(const glm::vec3 specular) {_sMaterial->specular = specular; }

        void SetObjectShine(float* shininess) {_sMaterial->shininess = *shininess; }

        void SetCameraPos(const glm::vec3& position) { 
            _sCamera->position = position;
        
            _sCamera->view = glm::lookAt(_sCamera->position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        }

        void SetCameraProj(float fov) {
            _sCamera->projection = glm::perspective(glm::radians(fov), 800.0f/800.0f, 0.1f, 100.0f);
        }

        void LoadMVPUniforms() {
            // Bind uniform buffer object and send matrices to vertex shader
            glBindBuffer(GL_UNIFORM_BUFFER, _mvpUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_sCamera->view));
            glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_sCamera->projection));
            glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_sModelMat));
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void LoadLightUniforms() {
            // Load light properties
            glBindBuffer(GL_UNIFORM_BUFFER, _pointLightUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sPLight->ambient));
            glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sPLight->diffuse));
            glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sPLight->specular));
            glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sPLight->position));
            glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), sizeof(float), &_sPLight->constant);
            glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4) + 1 * sizeof(float), sizeof(float), &_sPLight->linear);
            glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4) + 2 * sizeof(float), sizeof(float), &_sPLight->quadratic);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBuffer(GL_UNIFORM_BUFFER, _dirLightUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sDirLight->direction));
            glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sDirLight->ambient));
            glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sDirLight->diffuse));
            glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sDirLight->specular));
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void LoadMaterialUniforms() {
            // Load material properties
            glBindBuffer(GL_UNIFORM_BUFFER, _materialUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sMaterial->diffuse)); 
            glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_sMaterial->specular)); 
            glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(float), &_sMaterial->shininess); 
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void Render() {

            LoadMVPUniforms();

            LoadLightUniforms();

            LoadMaterialUniforms();

            _sceneShader->use();

            _sceneObject->Draw();
            
        }



};


#endif