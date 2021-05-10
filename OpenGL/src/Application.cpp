#include<GL/glew.h>
#include <GLFW/glfw3.h>

#include<iostream>
#include"stb_image.h"

#include <fstream>
#include<string>	
#include<sstream>

#include"Texture.h"

#include"Renderer.h" 
#include"VertexBuffer.h"
#include"VertexBufferLayout.h"
#include"IndexBuffer.h"
#include"Shader.h"
#include"VertexArray.h"

#include<glm.hpp>
#include<gtx/vector_angle.hpp>
#include<gtc/matrix_transform.hpp>
#include"objLoader.h"
#include <math.h>

#include "extra include/IMGUI/imgui.h"
#include "extra include/IMGUI/imgui_impl_glfw.h"
#include "extra include/IMGUI/imgui_impl_opengl3.h"

#include <irrKlang.h>
#include <Windows.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib")

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600


/*void ShowFPS(GLFWwindow *window, double& lastTime, int &nbFrames) {
	double currentTime = glfwGetTime();
	double delta = currentTime - lastTime;
	nbFrames++;
	if (delta >= 1.0) { // If last cout was more than 1 sec ago
		//std::cout << 1000.0 / double(nbFrames) <<std:: endl;

		double fps = double(nbFrames) / delta;

		std::stringstream ss;
		ss << "name :" << " " << " [" << fps << " FPS]";

		glfwSetWindowTitle(window, ss.str().c_str());

		nbFrames = 0;
		lastTime = currentTime;
	}
} */
struct Vertex
{
public:
	glm::vec3 Position;
	glm::vec2 UV;
	glm::vec3 Normal;
};
glm::vec3 Lerp(glm::vec3 from, glm::vec3 to, float smooth) {
	return (from * smooth) + (to * (1 - smooth));
}
float LerpF(float from, float to, float smooth) {
	return (from * smooth) + (to * (1 - smooth));
}
void LoadOBJ(objl::Loader loader, Vertex* verexes, unsigned int* indices) {

	Vertex vert;
	int s = loader.LoadedIndices.size();


	int i = 0;
	for (objl::Vertex v : loader.LoadedVertices) {

		//std::cout <<"Position ::  "<< v.Position.X << " " << v.Position.Y << " " << v.Position.Z << "  UVS::   " << v.TextureCoordinate.X << " " << v.TextureCoordinate.X << " " << "\n";
		vert.Position.x = v.Position.X;
		vert.Position.y = v.Position.Y;
		vert.Position.z = v.Position.Z;

		vert.UV.x = v.TextureCoordinate.X;
		vert.UV.y = v.TextureCoordinate.Y;

		vert.Normal.x = v.Normal.X;
		vert.Normal.y = v.Normal.Y;
		vert.Normal.z = v.Normal.Z;

		verexes[i++] = vert;
	}
	i = 0;
	for (unsigned int x : loader.LoadedIndices) {
		//std::cout << x << std::endl;
		indices[i++] = x;
	}

}
static glm::mat4 Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
	glm::mat4 model_matrix(1.0f);
	model_matrix = glm::translate(model_matrix, position);
	model_matrix = glm::rotate(model_matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));		//x
	model_matrix = glm::rotate(model_matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));		//y
	model_matrix = glm::rotate(model_matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));		//z
	model_matrix = glm::scale(model_matrix, scale);
	return model_matrix;
}
glm::vec3 MoveForward(glm::vec3& pos, const glm::vec3 rot, const float speed) {

	glm::vec3 forward(0.0f);
	forward.x = sin(glm::radians(rot.y));
	forward.y = -tan(glm::radians(rot.x));
	forward.z = cos(glm::radians(rot.y));

	pos += forward * speed;
	return forward;
}
void UpdateInput(GLFWwindow* window, glm::vec3& pos, float distance) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();
		glfwTerminate();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{

		pos.y += distance;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		pos.y -= distance;
	}

}


int main(void)
{

	int frameBufferWidth;
	int frameBufferHeight;

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);


	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glewInit();

	{

	//obj file loader
	objl::Loader loader;
	int size;
	loader.LoadFile("res\\OBJ\\player.obj");
	size = loader.LoadedIndices.size();
	Vertex* playerbuffer = new Vertex[size]();
	unsigned int* playerIndices = new unsigned int[size]();
	LoadOBJ(loader, playerbuffer, playerIndices);
	VertexBuffer vb_player(playerbuffer, size * sizeof(Vertex));
	IndexBuffer ib_player(playerIndices, size);


	loader.LoadFile("res\\OBJ\\cop.obj");
	size = loader.LoadedIndices.size();
	Vertex* copbuffer = new Vertex[size]();
	unsigned int* copIndices = new unsigned int[size]();
	LoadOBJ(loader, copbuffer, copIndices);
	VertexBuffer vb_cop(copbuffer, size * sizeof(Vertex));
	IndexBuffer ib_cop(copIndices, size);


	loader.LoadFile("res\\OBJ\\PlayGround.obj");
	size = loader.LoadedIndices.size();
	Vertex* GroundVertexes = new Vertex[size]();
	unsigned int* indices_Ground = new unsigned int[size]();
	LoadOBJ(loader, GroundVertexes, indices_Ground);
	VertexBuffer vb_Ground(GroundVertexes, size * sizeof(Vertex));
	IndexBuffer ib_Ground(indices_Ground, size);

	int treeCount = 23;
	glm::vec3 treePos[] = {
glm::vec3(7.847248	,0.000000		,	16.181660),
glm::vec3(8.042964		,0.000000		, -82.111649),
glm::vec3(-8.620526		,0.000000		, -82.286713),
glm::vec3(7.847249		,0.000000		, -50.970257),
glm::vec3(-8.753074		,0.000000		, -50.912609),
glm::vec3(-8.753076		,0.000000		, 16.239307),
glm::vec3(7.847247		,0.000000		, 83.479858),
glm::vec3(-8.753070		,0.000000		, 83.537506),
glm::vec3(-51.396539		,0.000000		, 58.013248),
glm::vec3(-51.579639		,0.000000		, 74.687332),
glm::vec3(-82.187241		,0.000000		, 58.221321),
glm::vec3(-82.130234		,0.000000		, 74.821503),
glm::vec3(-82.114471		,0.000000		, -75.228531),
glm::vec3(-82.278992		,0.000000		, -58.574505),
glm::vec3(50.367365		, 0.000000		,-74.991501),
glm::vec3(50.553658		, 0.000000		,-58.316662),
glm::vec3(85.227394		, 0.000000		,-8.574152),
glm::vec3(85.413719		, 0.000000		,8.100695),
glm::vec3(85.104813		, 0.000000		,58.557926),
glm::vec3(85.355286		, 0.000000		,75.361099),
glm::vec3(50.325455		, 0.000000		,58.557926),
glm::vec3(50.575924		, 0.000000		,75.361099)
	};

	loader.LoadFile("res\\OBJ\\Tree.obj");
	size = loader.LoadedIndices.size();
	Vertex* tree = new Vertex[size]();
	unsigned int* tree_ib = new unsigned int[size]();
	LoadOBJ(loader, tree, tree_ib);
	VertexBuffer vb_tree(tree, size * sizeof(Vertex));
	IndexBuffer ib_tree(tree_ib, size);

	VertexArray va;

	//...........................Vertex Array Layout........................//
	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(2);
	layout.Push<float>(3);
	//......................................................................//

	//.............................Creating Shaders........................//
	Shader shader("res\\Shaders\\Base.shader");
	shader.Bind();

	//Textures
	std::string path;
	path = "res\\Textures\\player.png";
	Texture t1(path, 0);
	shader.SetUniform1i("u_Texture", 0);

	path = "res\\Textures\\cop.png";
	Texture t2(path, 1);
	shader.SetUniform1i("u_Texture", 1);

	path = "res\\Textures\\street.jpg";
	Texture t3(path, 2);
	shader.SetUniform1i("u_Texture", 2);

	path = "res\\Textures\\tree.jpg";
	Texture t4(path, 3);
	shader.SetUniform1i("u_Texture", 3);

	path = "res\\Textures\\ToonRamp.png";
	Texture t5(path, 4);
	shader.SetUniform1i("u_ToonRamp", 4);

	//.....................................................................//

#pragma region ModelMatrix
	glm::vec3 playerPosition(0.0f);
	glm::vec3 playerRotation(0.0f);
	glm::vec3 carScale(1.f);
	glm::mat4 ModelMatrix(1.0f);
#pragma endregion
#pragma region ViewMatrix

	glm::vec3 cameraPosition(2.0f, 10.0f, -3.0f);
	cameraPosition *= 2;
	glm::vec3 worldUp(0.0f, 0.1f, 0.0f);
	glm::vec3 cameraFront(0);
	glm::mat4 ViewMatrix(1.0f);
#pragma endregion
#pragma region ProjectionMatrix
	float fov = 100.0f;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	//float FW = frameBufferWidth, FH = frameBufferHeight;
	glm::mat4 ProjectionMatrix(1.0f);
	ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(frameBufferWidth / frameBufferHeight), nearPlane, farPlane);
	//ProjectionMatrix = glm::ortho(-FW/24, FW/ 24, -FH/ 24, FH/ 24, 0.1f, 100.f);
#pragma endregion

	shader.SetUniformMat4("u_Projection", ProjectionMatrix);
	shader.SetUniformMat4("u_View", ViewMatrix);


	//render option
	glEnable(GL_DEPTH_TEST);	//z buffer
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);


	//Enabling Blend 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Renderer renderer;
	va.Bind();



	//............................................//
	glm::vec3 cameraCurrentPosition(0);
	glm::vec3 groundCurrent(0);
	float groundSize = 200;
	int Score = 0;
	int MaxScore = 0;
	bool pause = false;
	int copsCount = 2;
	float coliderSize = 1.2f;

	glm::vec3* copPositions = new glm::vec3[copsCount];
	glm::vec3* copRotations = new glm::vec3[copsCount];

	float gr[8] = {
		0,0,
		groundSize,0,
		0,groundSize,
		groundSize,groundSize
	};

	//...........................................//

	ISoundEngine* SoundEngine = createIrrKlangDevice();
	if (!SoundEngine)
		return 0;
	{//start
		SoundEngine->play2D("res/audio/breakout.mp3", true);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

		//passing light direction nad color to shader:
		shader.SetUniform3f("lightDir", 0.5f, 0.5f, 0.5f);
		shader.SetUniform3f("u_lightColor", 2, 2, 2);

		for (int i = 0; i < copsCount; i++)
		{
			copPositions[i] = glm::vec3(50 + i * 30, 0, 50 + i * 30);
			copRotations[i] = glm::vec3(0, 0, 0);
		}
	}
	float copSpeed[] = { .5f , .6f };
	float playerSpeed = 0.45f;

	float zoom = 2.5f;
	ImVec4 lightColor = ImVec4(1.f, 1.f, 1.f, 1.00f);
	glm::vec3 lightDir(0.5f, 0.5f, 0.5f);
	glm::vec3 playerForward(1);
	bool camView = true;


	ShowWindow(GetConsoleWindow(), SW_HIDE);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))							/////////    UPDATE LOOP   HERE  /////////
	{
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			pause = true;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			pause = false;
		}
		if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		{
			copSpeed[0] = 0;
			copSpeed[1] = 0;

		}
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		{
			camView = false;
			zoom = 0.8f;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		{
			camView = true;
			zoom = 2.5f;
		}

		glClearColor(0.5f, 0.5f, 1, 0);
		glfwSwapInterval(1);

		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		if (camView) {	//camera move
			cameraCurrentPosition = Lerp(cameraCurrentPosition, (zoom * glm::vec3(0, 5, 0) + (playerForward * -5.f)) + playerPosition, 0.9f);
			cameraFront = (playerPosition + playerForward * 2.f) - cameraCurrentPosition;
			ViewMatrix = glm::lookAt(cameraCurrentPosition, cameraCurrentPosition + cameraFront, worldUp);
			shader.SetUniformMat4("u_View", ViewMatrix);
		}

		else {
			cameraCurrentPosition = Lerp(cameraCurrentPosition, (zoom * cameraPosition) + playerPosition, 0.7f);
			cameraFront = playerPosition - cameraCurrentPosition;
			ViewMatrix = glm::lookAt(cameraCurrentPosition, cameraCurrentPosition + cameraFront, worldUp);
			shader.SetUniformMat4("u_View", ViewMatrix);
		}
		renderer.Clear();		 //Clears g & z buffers

		//passing view direction to shader:
		shader.SetUniform3f("lightDir", lightDir);
		shader.SetUniform3f("u_lightColor", lightColor.x, lightColor.y, lightColor.z);

		{ //GUI

			if (MaxScore < Score)
			{
				MaxScore = Score;
			}

			ImGui::Begin("press P to pause Space to run"); // Create a window called "Hello, world!" and append into it.

			ImGui::Text("Max Score = %d", MaxScore);
			ImGui::Text("Curent Score = %d", Score);

			ImGui::SliderFloat("Zoom", &zoom, 0.2f, 4.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("Light Color", (float*)&lightColor); // Edit 3 floats representing a color

			ImGui::SliderFloat("Light dir X", &lightDir.x, -1.f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Light dir Y", &lightDir.y, -1.f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Light dir Z", &lightDir.z, -1.f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f


			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		{	//Draw Ground
			if (playerPosition.x > groundCurrent.x + (groundSize / 2))
			{
				groundCurrent.x += groundSize;
			}
			else if (playerPosition.x < groundCurrent.x - (groundSize / 2))
			{
				groundCurrent.x -= groundSize;
			}
			if (playerPosition.z > groundCurrent.z + (groundSize / 2))
			{
				groundCurrent.z += groundSize;
			}
			else if (playerPosition.z < groundCurrent.z - (groundSize / 2))
			{
				groundCurrent.z -= groundSize;
			}

			int x = glm::sign(playerPosition.x - groundCurrent.x);
			int z = glm::sign(playerPosition.z - groundCurrent.z);

			for (int i = 0; i <= 6; i += 2)
			{

				glm::vec3 grPos(groundCurrent.x + gr[i] * x, 0, groundCurrent.z + gr[i + 1] * z);

				ModelMatrix = Transform(grPos, glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.f, 1.0f));

				shader.SetUniformMat4("u_Model", ModelMatrix);
				shader.SetUniform1i("u_Texture", 2);


				va.AddBuffer(vb_Ground, layout);
				renderer.Draw(va, ib_Ground, shader);

				{	//Draw Trees

					for (int j = 0; j < treeCount; j++)
					{

						glm::vec3 tPos(grPos.x + treePos[j].x, 0, grPos.z + treePos[j].z);

						ModelMatrix = Transform(tPos, glm::vec3(0, tPos.z + tPos.x, 0), glm::vec3(1.0f, 1.f, 1.0f));

						shader.SetUniformMat4("u_Model", ModelMatrix);
						shader.SetUniform1i("u_Texture", 3);

						va.AddBuffer(vb_tree, layout);
						renderer.Draw(va, ib_tree, shader);
					}
				}

			}
		}

		{//Draw Player
			if (!pause)
			{
				UpdateInput(window, playerRotation, 4.5f);
				playerForward = MoveForward(playerPosition, playerRotation, playerSpeed);
			}

			ModelMatrix = Transform(playerPosition, playerRotation, carScale);

			shader.SetUniformMat4("u_Model", ModelMatrix);
			shader.SetUniform1i("u_Texture", 0);



			va.AddBuffer(vb_player, layout);
			renderer.Draw(va, ib_player, shader);		//DrawPlayer
		}

		{//Draw Cops

			for (int n = 0; n < copsCount; n++)
			{
				if (!pause)
				{

					glm::vec3 forward = MoveForward(copPositions[n], copRotations[n], copSpeed[n]);	//TODO: hardcoded speed
					glm::vec3 diffrence = (playerPosition - copPositions[n]);

					float deg = glm::orientedAngle(glm::normalize(glm::vec2(forward.x, forward.z)), glm::normalize(glm::vec2(diffrence.x, diffrence.z)));

					copRotations[n].y += 2.2f * -glm::sign(deg);	//100 is cops rotation speed TODO: hardcoded speed should change
				}

				ModelMatrix = Transform(copPositions[n], copRotations[n], carScale);
				shader.SetUniformMat4("u_Model", ModelMatrix);
				shader.SetUniform1i("u_Texture", 1);

				va.AddBuffer(vb_cop, layout);
				renderer.Draw(va, ib_cop, shader);		//DrawCall
			}
		}

		{//check for Cops hits


			float dist = glm::distance(copPositions[0], copPositions[1]);

			if (dist < coliderSize * 2)
			{
				Score += 2;
				std::cout << "score :" << Score << "\n";
				copPositions[0] = glm::vec3(playerPosition.x + (65), 0, playerPosition.z + 65);
				copPositions[1] = glm::vec3(playerPosition.x + (-65), 0, playerPosition.z - 65);
				SoundEngine->play2D("res/audio/score.mp3", false);

				playerSpeed += 0.02f;
				copSpeed[0] += 0.02f;
				copSpeed[1] += 0.02f;
				zoom += .02f;

			}

			for (int i = 0; i < copsCount; i++)
			{
				for (int n = 0; n < treeCount; n++)
				{
					float dist = glm::distance(copPositions[i], treePos[n] + groundCurrent);

					if (dist < coliderSize)
					{
						SoundEngine->play2D("res/audio/score.mp3", false);
						Score++;
						std::cout << "score :" << Score << "\n";
						if (i)
						{
							copPositions[i] = glm::vec3(playerPosition.x + 65, 0, playerPosition.z + 65);
						}
						else
						{
							copPositions[i] = glm::vec3(playerPosition.x - 65, 0, playerPosition.z - 65);

						}

						playerSpeed += 0.02f;
						copSpeed[0] += 0.02f;
						copSpeed[1] += 0.02f;
						zoom += .02f;

						break;
					}
				}
			}

		}

		{//check for Player hits
			for (int n = 0; n < copsCount; n++)
			{
				float dist = glm::distance(copPositions[n], playerPosition);

				if (dist < coliderSize * 2)
				{

					std::cout << "Your final score :" << Score << "\n";
					Score = 0;
					glfwSwapInterval(50);
					playerPosition = glm::vec3(0);
					copPositions[0] = glm::vec3(playerPosition.x + 65, 0, playerPosition.z + 65);
					copPositions[1] = glm::vec3(playerPosition.x - 65, 0, playerPosition.z - 65);
					SoundEngine->play2D("res/audio/solid.wav", false);


					playerSpeed = .45f;
					copSpeed[0] = .5f;
					copSpeed[1] = .6f;
					zoom = camView ? 2.5 : 0.8f;


				}
			}

			for (int n = 0; n < treeCount; n++)
			{
				float dist = glm::distance(playerPosition, treePos[n] + groundCurrent);

				if (dist < coliderSize)
				{
					std::cout << "Your final score :" << Score << "\n";
					Score = 0;
					glfwSwapInterval(50);
					playerPosition = glm::vec3(0);
					copPositions[0] = glm::vec3(playerPosition.x + 65, 0, playerPosition.z + 65);
					copPositions[1] = glm::vec3(playerPosition.x - 65, 0, playerPosition.z - 65);
					SoundEngine->play2D("res/audio/solid.wav", false);
					playerSpeed = .45f;
					copSpeed[0] = .5f;
					copSpeed[1] = .6f;
					zoom = camView ? 2.5 : 0.8f;
					break;
				}
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}
