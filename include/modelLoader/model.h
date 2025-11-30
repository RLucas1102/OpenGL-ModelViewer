#ifndef MODEL_H
#define MODEL_H

#include <modelLoader/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

class Model {
    private:

        // Vertex data
        std::vector<Mesh> _meshes;
        
        // Material Properties
        glm::vec3 _diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
        glm::vec3 _specular = glm::vec3(0.5f, 0.5f, 0.5f);
        float _shininess = 32.0f;

        void loadModel(std::string path) {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            }

            processNode(scene->mRootNode, scene);

        }

        void processNode(aiNode* node, const aiScene* scene) {

            for (int i = 0; i < node->mNumMeshes; i++) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                _meshes.push_back(processMesh(mesh, scene));
            }

            for (int i = 0; i < node->mNumChildren; i++) {
                processNode(node->mChildren[i], scene);
            }
            
        }

        Mesh processMesh(aiMesh* mesh, const aiScene* scene) {

            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;

            for (int i = 0; i < mesh->mNumVertices; i++) {

                Vertex vertex;
                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.position = vector;

                // normals
                if (mesh->HasNormals())
                {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.normal = vector;
                }

                vertices.push_back(vertex);

            }

            for (int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for(int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }
            
            return Mesh(vertices, indices);

        }

    public:

        Model(const char* path) { loadModel(path); }

        void Draw() {

            for (int i = 0; i < _meshes.size(); i++) {
                _meshes[i].draw();
            } 
        }

        void SetMaterials() {

            glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_diffuse)); 
            glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(_specular)); 
            glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(float), &_shininess); 

        }

        void ChangeDiffuse(const glm::vec3& inColor) { _diffuse = inColor; }

        void ChangeSpecular(const glm::vec3& inColor) { _specular = inColor; }

        void ChangeShine(const float inShine) { _shininess = inShine; }
        

};


#endif