	
#include "Mesh.h"
#include "GL\glew.h"
#include "Vertex.h"

/******************************************************************************/
/*!
\brief
Default constructor - generate VBO/IBO here

\param meshName - name of mesh
*/
/******************************************************************************/
Mesh::Mesh(const std::string& meshName)
	: name(meshName)
	, mode(DRAW_TRIANGLES)
	, textureID(0)
{
	//generate buffers
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer); //generate index buffer
}

/******************************************************************************/
/*!
\brief
Destructor - delete VBO/IBO here
*/
/******************************************************************************/
Mesh::~Mesh()
{
	// Cleanup VBO here
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
	if (textureID > 0) {
		glDeleteTextures(1, &textureID);
	}
}

/******************************************************************************/
/*!
\brief
OpenGL render code
*/
/******************************************************************************/
void Mesh::Render()
{
	glEnableVertexAttribArray(0); //1st attribute buffer : vertices
	glEnableVertexAttribArray(1); //2nd attribute buffer : colors 
	glEnableVertexAttribArray(2); //3rd attribute buffer : normals
	

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,    //attribute 0. Must match the layout in the shader. Usually 0 is for vertex
		3,    // size
		GL_FLOAT, //type
		GL_FALSE, //normalized?
		sizeof(Vertex),    // stride
		(void*) 0     //array buffer offset 
	);
	glVertexAttribPointer(
		1,    //attribute 0. Must match the layout in the shader. Usually 0 is for vertex
		3,    // size
		GL_FLOAT, //type
		GL_FALSE, //normalized?
		sizeof(Vertex),    // stride
		(void*)sizeof(Position)     //array buffer offset 
	);

	if (textureID > 0) {
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position) + sizeof(Color) + sizeof(Vector3)));
	}

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position) + sizeof(Color)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	if (materials.size() == 0) {
		if (mode == DRAW_TRIANGLE_STRIP) {
			glDrawElements(GL_TRIANGLE_STRIP, indexSize, GL_UNSIGNED_INT, 0);
		}
		else if (mode == DRAW_LINES) {
			glDrawElements(GL_LINES, indexSize, GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
		}
	}
	else {
		for (unsigned i = 0, offset = 0; i < materials.size(); i++) {
			Material& material = materials[i];
			glUniform3fv(locationKa, 1, &material.kAmbient.r);
			glUniform3fv(locationKd, 1, &material.kDiffuse.r);
			glUniform3fv(locationKs, 1, &material.kSpecular.r);
			glUniform1f(locationNs, material.kShininess);
			if (mode == DRAW_TRIANGLE_STRIP) {
				glDrawElements(GL_TRIANGLE_STRIP, material.size, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned)));
			}
			else if (mode == DRAW_LINES) {
				glDrawElements(GL_LINES, material.size, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned)));
			}
			else {
				glDrawElements(GL_TRIANGLES, material.size, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned)));
				offset += material.size;
			}
		}
	}

	if (textureID > 0) {
		glDisableVertexAttribArray(3);
	}

	
}

void Mesh::Render(unsigned offset, unsigned count)
{
	glEnableVertexAttribArray(0); //1st attribute buffer : vertices
	glEnableVertexAttribArray(1); //2nd attribute buffer : colors
	glEnableVertexAttribArray(2); // 3rd attribute : normals
	if (textureID > 0)
	{
		glEnableVertexAttribArray(3);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Position));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position) + sizeof(Color)));
	if (textureID > 0)
	{
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void*)(sizeof(Position) + sizeof(Color) +
				sizeof(Vector3)));
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	if (mode == DRAW_LINES)
		glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
	else if (mode == DRAW_TRIANGLE_STRIP)
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
	else
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));


	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);
	if (textureID > 0)
	{
		glDisableVertexAttribArray(3);
	}

}

unsigned Mesh::locationKa;
unsigned Mesh::locationKd;
unsigned Mesh::locationKs;
unsigned Mesh::locationNs;
void Mesh::SetMaterialLoc(unsigned ambient, unsigned diffuse, unsigned specular, unsigned shininess)
{
	locationKa = ambient;
	locationKd = diffuse;
	locationKs = specular;
	locationNs = shininess;
}
