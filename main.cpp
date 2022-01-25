#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
#include "Material.h"

#include "Model.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture groundGrass;


Material shinyMaterial;
Material dullMaterial;

Model helicopter;
Model Soldier;
Model ground;
Model Robot;
Model airbus;
Model soldierParachute;
Model trainBack;


Light mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,	0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};



	unsigned int surfaceIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat surface[] = {
	-10.0f ,0.0f, -10.0f,     0.0f, 0.0f,			0.0f, -1.0f, 0.0f,
	10.0f ,0.0f, -10.0f,     10.0f, 0.0f,			0.0f, -1.0f, 0.0f,
	-10.0f ,0.0f, 10.0f,      0.0f, 10.0f,			0.0f, -1.0f, 0.0f,
	10.0f ,0.0f, 10.0f,      10.0f, 10.0f,			0.0f, -1.0f, 0.0f
	};







	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(surface, surfaceIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj4);
	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj5);
	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj6);
	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj7);
	Mesh* obj8 = new Mesh();
	obj8->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj8);
}




void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}





GLfloat moveHelicopter = 0.0f;
GLfloat moveBackwardhelicopter = 360.0f;
GLfloat jumpingFromPlane = -2.8f;

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();


	//groundGrass = Texture("Textures/GroundGrass.jpg");
	//groundGrass.LoadTextureA();


	shinyMaterial = Material(1.0f, 32);
	dullMaterial = Material(0.3f, 4);


	helicopter = Model();
	helicopter.LoadModel("Models/Seahawk.obj");

	airbus = Model();
	airbus.LoadModel("Models/airbus.obj");


	ground = Model();
	ground.LoadModel("Models/SnowTerrain.obj");


	Soldier = Model();
	Soldier.LoadModel("Models/Soldier.obj");
	Robot = Model();
	Robot.LoadModel("Models/Robot.obj");


	trainBack = Model();
	trainBack.LoadModel("Models/trainBack.obj");

	soldierParachute = Model();
	soldierParachute.LoadModel("Models/SoldierParachute.obj");


	mainLight = Light(1.0f, 1.0f, 1.0f, 0.1f,
		0.0f, 0.0f, -1.0f, 0.3f);

	//Assimp::Importer importer = Assimp::Importer();

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformAmbientIntensity = 0, uniformAmbientColour = 0, uniformDirection = 0, uniformDiffuseIntensity = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);



	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformAmbientColour = shaderList[0].GetuniformAmbientColour();
		uniformAmbientIntensity = shaderList[0].GetuniformAmbientIntensity();
		uniformDirection = shaderList[0].GetuniformDirection();
		uniformDiffuseIntensity = shaderList[0].GetuniformDiffuseIntensity();
		uniformEyePosition = shaderList[0].GetuniformCamPosition();
		uniformSpecularIntensity = shaderList[0].GetuniformSpecularIntensity();
		uniformShininess = shaderList[0].GetuniformShininess();

		mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColour,
			uniformDiffuseIntensity, uniformDirection);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		glm::mat4 model(1.0f);

		model = glm::translate(model, glm::vec3(-2.0f, -7.323f, -2.5f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brickTexture.UseTexture();
		shinyMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();



		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -7.323f, -2.5f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		dullMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -7.323f, -2.5f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		dullMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		// surface
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -7.3f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dullMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		dirtTexture.UseTexture();
		meshList[2]->RenderMesh();


		moveHelicopter += 0.13;
		if (moveHelicopter > 360.0f)
		{
			moveHelicopter = 0.13f;
		}

		moveBackwardhelicopter -= 0.03;

		if (moveBackwardhelicopter < 0)
		{
			moveBackwardhelicopter = 360.0f;
		}

		jumpingFromPlane -= 0.001;
		if (jumpingFromPlane == -7.0f)
		{
			jumpingFromPlane = -2.8f;
		}


		model = glm::mat4(1.0f);
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-3.0f, 2.0f, 0.0f));
		model = glm::rotate(model, 1.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		airbus.RenderModel();

		// you should add plot light over here
		
		model = glm::mat4(1.0f);
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(+12.0f, -20.0f, 0.0f));
		//model = glm::rotate(model, 1.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0051f, 0.0051f, 0.0051f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		trainBack.RenderModel();


		//model = glm::mat4(1.0f);
		//model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(-2.7f, jumpingFromPlane, 0.0f));
		//model = glm::rotate(model, 1.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		////model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//shinyMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		//soldierParachute.RenderModel();

		//airbus1.UseTexture();


		model = glm::mat4(1.0f);
		model = glm::rotate(model, moveHelicopter * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-3.0f, 2.0f, 0.0f));
		model = glm::rotate(model, 1.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		helicopter.RenderModel();


		model = glm::mat4(1.0f);
		model = glm::rotate(model, moveBackwardhelicopter * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-3.0f, -1.5f, 0.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		helicopter.RenderModel();



		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -7.0f, -2.5f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
		model = glm::translate(model, glm::vec3(4.0f, -7.0f, -2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		shinyMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		Soldier.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::rotate(model, 1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-2.0f, -7.323f, -0.6f));
		model = glm::rotate(model, 1.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.14f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.useSpecular(uniformSpecularIntensity, uniformShininess);
		Robot.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}