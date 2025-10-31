#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Using a struct to represent vertices is convenient for later as it translates 1-to-1 with OpenGL
struct Vertex
{
    glm::vec3 position;
};


class Mesh {

    private:
        // Create vectors to hold vertex data and index data, respectively
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;

        // Creating buffers and vertex array
        unsigned int _VBO, _EBO, _VAO, _instanceVBO, _colorsVBO;

        void setupMesh() {

            // Generate IDs for buffers and vertex arrays
            glGenBuffers(1, &_VBO);
            glGenBuffers(1, &_EBO);
            glGenVertexArrays(1, &_VAO);

            // Bind a vertex array; all subsequent VBOs and attributes will be bound to this object
            glBindVertexArray(_VAO);

            // Bind array buffer type and then vertex data
            glBindBuffer(GL_ARRAY_BUFFER, _VBO);
            glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

            // Bind element buffer type and then index data
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

            // Position attribute
            // Index = 0
            // Number of components = 3
            // Type = GL_FLOAT
            // Normalized = False
            // Stride: 3 * 4 = 12 (Each vertex is 12 bytes apart)
            // Offset: offsetof() should return 0
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(0); // Enable position attribute at position 0

            // Unbind current VAO
            glBindVertexArray(0);

        }

    public:

        // Default constructor
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) : _vertices(vertices), _indices(indices) {

            // Load data onto GPU
            setupMesh();

        };

        void draw() {

            // Bind VAO and draw mesh
            glBindVertexArray(_VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(_indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

        }

        void drawInstanced(int numObjects) {

            // Bind VAO and draw mesh
            glBindVertexArray(_VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(_indices.size()), GL_UNSIGNED_INT, 0, numObjects);
            glBindVertexArray(0);

        }

        void setInstancedDraw(int numObjects, const glm::mat4* modelMatrices) {

            // Bind VAO
            glBindVertexArray(_VAO);

            // Generate ID for VBO and load instance data
            glGenBuffers(1, &_instanceVBO);
            glBindBuffer(GL_ARRAY_BUFFER, _instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numObjects, &modelMatrices[0], GL_STATIC_DRAW);

            // Instance Attribute
            std::size_t v4s = sizeof(glm::vec4);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)0); // Start at 0 offset because new VBO
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(1 * v4s)); // Start at 0 offset because new VBO
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(2 * v4s)); // Start at 0 offset because new VBO
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(3 * v4s)); // Start at 0 offset because new VBO
            glEnableVertexAttribArray(4);

            glVertexAttribDivisor(1, 1); // OpenGL now knows this an instanced vertex attribute. 1 means Change value every instance
            glVertexAttribDivisor(2, 1); // OpenGL now knows this an instanced vertex attribute. 1 means Change value every instance
            glVertexAttribDivisor(3, 1); // OpenGL now knows this an instanced vertex attribute. 1 means Change value every instance
            glVertexAttribDivisor(4, 1); // OpenGL now knows this an instanced vertex attribute. 1 means Change value every instance

            glBindVertexArray(0);

        }

        void setInstanceColors(int numObjects, const glm::vec3* colors) {

            // Bind VAO
            glBindVertexArray(_VAO);

            // Generate ID for VBO and load instance data
            glGenBuffers(1, &_colorsVBO);
            glBindBuffer(GL_ARRAY_BUFFER, _colorsVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numObjects, &colors[0], GL_STATIC_DRAW);

            // Instance Attribute
            glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0); // Start at 0 offset because new VBO
            glEnableVertexAttribArray(5);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glVertexAttribDivisor(5, 1); // OpenGL now knows this an instanced vertex attribute. 1 means Change value every instance

        }

};

#endif