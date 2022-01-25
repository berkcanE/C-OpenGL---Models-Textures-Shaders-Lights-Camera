#pragma once

#include <GL/glew.h>



class Material
{
private:
	GLfloat shininess;
	GLfloat specularIntensity;


public:
	Material();
	Material(GLfloat sIntensity, GLfloat shine);
	void useSpecular(GLuint specularIntensityLocation, GLuint shininessLocation);
	~Material();


};

