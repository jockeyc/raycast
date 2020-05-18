#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include <iostream>
#include "occupancyHistogramTree.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);
unsigned int initFrameBuffer(unsigned int& texture);
unsigned int load3DTexture(char const* path, GLuint w, GLuint h, GLuint d);
unsigned int load1DTexture(const char* filename);
unsigned int loadTexture2(char const* path);
void initGL();
void initBufferObject();	// set up vertex data (and buffer(s)) and configure vertex attributes
void initGeometryBufferObject();
void initCubeBufferObject();
void initUniformBufferObject();
void initTextureImage();
void clearTextureImage();
void loadVolumeData();
void initMatrix();
void initShader();	// build and compile our shader program
void initOccupancyHistogramTree();
void initGeometryArray();
void updateGeometryArray();
void processCullCacheMiss();
void renderLoop();
void renderLoop2();
void RenderRaySegmentList();
void RenderSparseLeap();
void destroy();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int RAY_SEGMENT_LIST_LENGTH = 30;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
bool firstMouse = true;
float lastX = 400, lastY = 300;

float cubeVertices[] = {
	// Back face
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,// Bottom-left
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,// top-right
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,// bottom-right         
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,// top-right
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,// bottom-left
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,// top-left
	// Front face
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,// bottom-left
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,// bottom-right
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,// top-right
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,// top-right
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,// top-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,// bottom-left
	// Left face
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,// top-right
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,// top-left
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,// bottom-left
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,// bottom-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,// bottom-right
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,// top-right
	// Right face
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,// top-left
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,// bottom-right
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,// top-right         
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,// bottom-right
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,// top-left
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,// bottom-left     
	// Bottom face
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,// top-right
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,// top-left
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,// bottom-left
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,// bottom-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,// bottom-right
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,// top-right
	// Top face
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,// top-left
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,// bottom-right
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,// top-right     
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,// bottom-right
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,// top-left
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f// bottom-left        
};
float* geometryOffset;
float* geometryScale;
unsigned int* geometryInfo;
unsigned int* clearData2D;
unsigned int* clearData3DUI;
float* clearData3DF;

int geometryOffsetSize;
int geometryArraySize;

GLFWwindow* window;

glm::mat4 mvp;
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat4 rot;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
int i = 0;

OccupancyHistogramTree* occupancyHistogramTree;

unsigned int RaySegmentListCountTexture2D;
unsigned int RaySegmentListDepthTexture3D;
unsigned int RaySegmentListClassTexture3D;
unsigned int RaySegmentListTypeTexture3D;
unsigned int FrontFacePositionTexture2D;
unsigned int RaySegmentListInstanceID;
unsigned int ReportUnknownInstanceID;
unsigned int InstanceCounter;

unsigned int ReportFrameBuffer;

unsigned char* volumeData;
int volumeDataSize;

unsigned int geometryVBO, geometryVAO;
unsigned int cubeVBO, cubeVAO;
unsigned int UBO;
unsigned int PBO32UI,PBO32F,PPBO;	//Pxiel buffer object for cleaning texture data;
unsigned int geometryOffsetBuffer;
unsigned int geometryScaleBuffer;
unsigned int geometryInfoBuffer;

unsigned int texture;
unsigned int volumeTex;
unsigned int tex1d;
unsigned int FBO;

Shader boxShader;
Shader frontShader;
Shader raySegmentListShader;
Shader sparseLeapShader;

int main()
{
	initGL();
	initShader();
	//initBufferObject();
	
	initUniformBufferObject();
	loadVolumeData();
	initMatrix();
	initOccupancyHistogramTree();
	initGeometryArray();
	initGeometryBufferObject();
	initCubeBufferObject();
	initTextureImage();
	renderLoop2();
	//renderLoop();
	destroy();
	
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------


void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.ProcessKeyboard(DOWN, deltaTime);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;
	if (xoffset > 2 || yoffset > 2 ) updateGeometryArray();
	camera.ProcessMouseMovement(xoffset, yoffset);
	
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int loadTexture2(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;


		glBindTexture(GL_TEXTURE_1D, textureID);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int load3DTexture(char const* path, GLuint w, GLuint h, GLuint d) {
	FILE* fp;
	int size = w * h * d;
	volumeData = new unsigned char[size];			  // 8bit
	if (!(fp = fopen(path, "rb")))
	{
		cout << "Error: opening .raw file failed" << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		cout << "OK: open .raw file successed" << endl;
	}
	if (fread(volumeData, sizeof(char), size, fp) != size)
	{
		cout << "Error: read .raw file failed" << endl;
		exit(1);
	}
	else
	{
		cout << "OK: read .raw file successed" << endl;
	}
	fclose(fp);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE, volumeData);

	//delete[]volumeData;
	cout << "volume texture created" << endl;
	return textureID;
}

unsigned int initFrameBuffer(unsigned int& texture) {
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// 生成纹理
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	unsigned int depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return fbo;
}

unsigned int load1DTexture(const char* filename)
{
	// read in the user defined data of transfer function
	ifstream inFile(filename, ifstream::in);
	if (!inFile)
	{
		cerr << "Error openning file: " << filename << endl;
		exit(EXIT_FAILURE);
	}

	const int MAX_CNT = 10000;
	GLubyte* tff = (GLubyte*)calloc(MAX_CNT, sizeof(GLubyte));
	inFile.read(reinterpret_cast<char*>(tff), MAX_CNT);
	if (inFile.eof())
	{
		size_t bytecnt = inFile.gcount();
		*(tff + bytecnt) = '\0';
		cout << "bytecnt " << bytecnt << endl;
	}
	else if (inFile.fail())
	{
		cout << filename << "read failed " << endl;
	}
	else
	{
		cout << filename << "is too large" << endl;
	}
	unsigned int tff1DTex;
	glGenTextures(1, &tff1DTex);
	glBindTexture(GL_TEXTURE_1D, tff1DTex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tff);
	free(tff);
	return tff1DTex;
}

unsigned int gen3DImage(unsigned int w, unsigned int h, unsigned int d, unsigned int& textureID, bool isFloat) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	if (isFloat) {
		//glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, w, h, d, 0, GL_RED, GL_FLOAT, NULL);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, w, h, d);
	}
	else{
		//glTexImage3D(GL_TEXTURE_3D, 0, GL_R32UI, w, h, d, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32UI, w, h, d);
	}
	glBindTexture(GL_TEXTURE_3D, 0);

	return textureID;
}

unsigned int gen2DImage(unsigned int w, unsigned int h, unsigned int& textureID) {

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, w, h);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

unsigned int genPosition2DImagee(unsigned int w, unsigned int h, unsigned int& textureID) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, w, h);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

unsigned int gen1DImage(unsigned int size, unsigned int& textureID) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_1D, textureID);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32UI, size);
	glBindTexture(GL_TEXTURE_1D, 0);

	return textureID;
}

void initGL() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
}

void initBufferObject(){
	glGenVertexArrays(1, &geometryVAO);
	glGenBuffers(1, &geometryVBO);
	glBindVertexArray(geometryVAO);

	glBindBuffer(GL_ARRAY_BUFFER, geometryVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	FBO = initFrameBuffer(texture);
}

void initGeometryBufferObject() {
	glGenVertexArrays(1, &geometryVAO);
	glGenBuffers(1, &geometryVBO);
	glGenBuffers(1, &geometryOffsetBuffer);
	glGenBuffers(1, &geometryScaleBuffer);
	glGenBuffers(1, &geometryInfoBuffer);

	glBindVertexArray(geometryVAO);

	glBindBuffer(GL_ARRAY_BUFFER, geometryVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, geometryOffsetBuffer);
	glBufferData(GL_ARRAY_BUFFER, geometryArraySize * 3 * sizeof(float), geometryOffset, GL_STREAM_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glVertexAttribDivisor(2, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, geometryScaleBuffer);
	glBufferData(GL_ARRAY_BUFFER, geometryArraySize * sizeof(float), geometryScale, GL_STREAM_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glVertexAttribDivisor(3, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, geometryInfoBuffer);
	glBufferData(GL_ARRAY_BUFFER, geometryArraySize * 3 * sizeof(unsigned int), geometryInfo, GL_STREAM_DRAW);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, 3 * sizeof(unsigned int), (void*)0);
	glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, 3 * sizeof(unsigned int), (void*)(1 * sizeof(unsigned int)));
	glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, 3 * sizeof(unsigned int), (void*)(2 * sizeof(unsigned int)));
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	// position attribute
	FBO = initFrameBuffer(texture);
}

void initCubeBufferObject(){
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void initUniformBufferObject() {
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 64*3, NULL, GL_STATIC_DRAW); // 分配内存
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
}

void initTextureImage() {
	gen2DImage(SCR_WIDTH, SCR_HEIGHT, RaySegmentListCountTexture2D);
	gen3DImage(SCR_WIDTH, SCR_HEIGHT, RAY_SEGMENT_LIST_LENGTH, RaySegmentListDepthTexture3D, true);
	gen3DImage(SCR_WIDTH, SCR_HEIGHT, RAY_SEGMENT_LIST_LENGTH, RaySegmentListClassTexture3D, false);
	gen3DImage(SCR_WIDTH, SCR_HEIGHT, RAY_SEGMENT_LIST_LENGTH, RaySegmentListTypeTexture3D, false);
	gen3DImage(SCR_WIDTH, SCR_HEIGHT, RAY_SEGMENT_LIST_LENGTH, RaySegmentListInstanceID, false);
	genPosition2DImagee(SCR_WIDTH, SCR_HEIGHT, FrontFacePositionTexture2D);
	gen1DImage(100, ReportUnknownInstanceID);

	glBindImageTexture(0, RaySegmentListCountTexture2D, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(1, RaySegmentListDepthTexture3D, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(2, RaySegmentListClassTexture3D, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(3, RaySegmentListTypeTexture3D, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(4, FrontFacePositionTexture2D, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(5, RaySegmentListInstanceID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(6, ReportUnknownInstanceID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	glGenBuffers(1, &PBO32UI);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO32UI);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, SCR_WIDTH * SCR_HEIGHT * RAY_SEGMENT_LIST_LENGTH * sizeof(GLuint), NULL, GL_STATIC_COPY);
	unsigned int* ptr1 = (unsigned int*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
	memset(ptr1, 0, SCR_WIDTH * SCR_HEIGHT * RAY_SEGMENT_LIST_LENGTH * sizeof(GLuint));
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glGenBuffers(1, &PBO32F);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO32F);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, SCR_WIDTH * SCR_HEIGHT * RAY_SEGMENT_LIST_LENGTH * sizeof(GLfloat), NULL, GL_STATIC_COPY);
	float* ptr2 = (float*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
	memset(ptr2, 0, SCR_WIDTH * SCR_HEIGHT * RAY_SEGMENT_LIST_LENGTH * sizeof(GLfloat));
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glGenBuffers(1, &PPBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PPBO);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, 10000 * sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
	unsigned int* ptr3 = (unsigned int *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
	memset(ptr3, 1,100 * sizeof(GLuint));
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glGenBuffers(1, &InstanceCounter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, InstanceCounter);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);
	unsigned int zero = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 7, InstanceCounter);
}

void clearTextureImage(){
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO32F);

	//clear DepthTexture3D
	glBindTexture(GL_TEXTURE_3D, RaySegmentListDepthTexture3D);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, SCR_WIDTH, SCR_HEIGHT, RAY_SEGMENT_LIST_LENGTH, GL_RED, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_3D, 0);

	//clear FrontFacePositionTexture2D
	glBindTexture(GL_TEXTURE_2D, FrontFacePositionTexture2D);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO32UI);

	//clear ClassTexture3D
	glBindTexture(GL_TEXTURE_3D, RaySegmentListClassTexture3D);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, SCR_WIDTH, SCR_HEIGHT, RAY_SEGMENT_LIST_LENGTH, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_3D, 0);

	//clear TypeTexture3D
	glBindTexture(GL_TEXTURE_3D, RaySegmentListTypeTexture3D);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, SCR_WIDTH, SCR_HEIGHT, RAY_SEGMENT_LIST_LENGTH, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_3D, 0);

	//clear CountTexture2D
	glBindTexture(GL_TEXTURE_2D, RaySegmentListCountTexture2D);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//clear unknownInstancedIDTexture
	glBindTexture(GL_TEXTURE_1D, ReportUnknownInstanceID);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 100, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_1D, 0);

	//clear InstancedIDTexture
	glBindTexture(GL_TEXTURE_3D, RaySegmentListInstanceID);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, SCR_WIDTH, SCR_HEIGHT, RAY_SEGMENT_LIST_LENGTH, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_3D, 0);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	//clear Atomic Counter
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, InstanceCounter);
	unsigned int zero = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER,  0, sizeof(unsigned int), &zero);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void loadVolumeData() {
	//volumeTex = load3DTexture("2pic2.tif", 128, 128, 128); //
	volumeTex = load3DTexture("test.raw", 512, 512, 512);
	tex1d = loadTexture2("test.png");
}

void initMatrix() {
	//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	

	view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	mvp = projection * view * model;
}

void initShader() {
	boxShader=Shader("StandardBackVertex.glsl", "StandardBackFragment.glsl");
	frontShader=Shader("StandardFrontVertex.glsl", "StandardFrontFragment.glsl");
	raySegmentListShader = Shader("RaySegmentListVertex.glsl", "RaySegmentListFragment.glsl");
	sparseLeapShader = Shader("SparseLeapVertex.glsl", "SparseLeapFragment.glsl");
}

void initOccupancyHistogramTree() {
	occupancyHistogramTree = new OccupancyHistogramTree();
	occupancyHistogramTree->setMaxDepth(5);
	occupancyHistogramTree->setPosition(glm::vec3(-0.5f), glm::vec3(0.5f));
	occupancyHistogramTree->setCameraPos(camera.Position);
	occupancyHistogramTree->setVolumeData(volumeData);
	occupancyHistogramTree->setDataSize(512, 512, 512);
	occupancyHistogramTree->setMaxNode(16777216);
	occupancyHistogramTree->subDivisionRoot();
	occupancyHistogramTree->Propagation();
	occupancyHistogramTree->TraversalOccupancyGeometryGeneration();
}

void initGeometryArray(){
	occupancyHistogramTree->TraversalIndexOrderRoot();
	glm::vec3 geometryCenter;
	geometryArraySize = occupancyHistogramTree->OccupancyIndexArray.size();
	geometryOffset = new float[geometryArraySize * 3];
	geometryScale = new float[geometryArraySize];
	geometryInfo = new unsigned int[geometryArraySize * 3];
	int offsetIndex = 0,infoIndex=0;
	vector<float> v;
	for (int i = 0; i < geometryArraySize; i++) {
		OHTreeNode* node = occupancyHistogramTree->OccupancyIndexArray[i]->node;
		NodeInfo* nodeInfo = occupancyHistogramTree->OccupancyIndexArray[i];
		geometryCenter = (node->minPos + node->maxPos)* 0.5f;
		geometryOffset[offsetIndex++] = geometryCenter.x;
		geometryOffset[offsetIndex++] = geometryCenter.y;
		geometryOffset[offsetIndex++] = geometryCenter.z;
		geometryScale[i] = (node->maxPos.x - node->minPos.x) / 1.0f;
		geometryInfo[infoIndex++] = (unsigned int)nodeInfo->faceOrder;
		geometryInfo[infoIndex++] = (unsigned int)node->occupancyClass;
		geometryInfo[infoIndex++] = (unsigned int)node->parent->occupancyClass;
		v.push_back(glm::distance(camera.Position, geometryCenter));
	}

}

void updateGeometryArray(){
	occupancyHistogramTree->setCameraPos(camera.Position);
	occupancyHistogramTree->TraversalIndexOrderRoot();
	glm::vec3 geometryCenter;
	int offsetIndex = 0, infoIndex = 0;
	for (int i = 0; i < geometryArraySize; i++) {
		OHTreeNode* node = occupancyHistogramTree->OccupancyIndexArray[i]->node;
		NodeInfo* nodeInfo = occupancyHistogramTree->OccupancyIndexArray[i];
		geometryCenter = (node->minPos + node->maxPos) * 0.5f;
		geometryOffset[offsetIndex++] = geometryCenter.x;
		geometryOffset[offsetIndex++] = geometryCenter.y;
		geometryOffset[offsetIndex++] = geometryCenter.z;
		geometryScale[i] = (node->maxPos.x - node->minPos.x) / 1.0f;
		geometryInfo[infoIndex++] = (unsigned int)nodeInfo->faceOrder;
		geometryInfo[infoIndex++] = (unsigned int)node->occupancyClass;
		geometryInfo[infoIndex++] = (unsigned int)node->parent->occupancyClass;
	}

	glBindVertexArray(geometryVAO);
	glBindBuffer(GL_ARRAY_BUFFER, geometryOffsetBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, geometryArraySize * 3 * sizeof(float), geometryOffset);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, geometryScaleBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, geometryArraySize * sizeof(float), geometryScale);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, geometryInfoBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, geometryArraySize * 3 * sizeof(float), geometryInfo);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void processCullCacheMiss(){
	unsigned int* count = new unsigned int;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, InstanceCounter);
	glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(unsigned int), count);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	
	unsigned int* data = new unsigned int[geometryArraySize];
	memset(data, 0, sizeof(unsigned int) * geometryArraySize);

	//glBindBuffer(GL_PIXEL_PACK_BUFFER, PPBO);
	glBindTexture(GL_TEXTURE_1D, ReportUnknownInstanceID);
	glGetTexImage(GL_TEXTURE_1D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, data);

	bool flag = false;
	for (int i = 0; i < geometryArraySize; i++) {
		if (data[i] == 1) {
			int index = data[i] * 3;
			glm::vec3 offset = glm::vec3(geometryOffset[index], geometryOffset[index + 1], geometryOffset[index + 2]);
			float scale = geometryScale[data[i]];
			occupancyHistogramTree->culling(offset, scale);
			flag = true;
		}
	}
	if (flag) {

	}

 }

void renderLoop(){
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
		//rot = glm::rotate(rot, glm::radians(1.0f), glm::vec3(0, 1, 0));
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &(rot * model));
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &view);
		glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, &projection);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// input
		// -----
		processInput(window);

		frontShader.setInt("exitPoints", 0);
		frontShader.setInt("volume", 1);
		frontShader.setInt("transferFunc", 2);

		// bind Texture

		// render container
		// render
		// ------
		// 第一处理阶段

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 我们现在不使用模板缓冲
		glEnable(GL_DEPTH_TEST);
		boxShader.use();
		glBindVertexArray(cubeVAO);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		// 第二处理阶段
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // 返回默认
		frontShader.use();
		frontShader.setVec2("ScreenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
		glBindVertexArray(cubeVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, volumeTex);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_1D, tex1d);


		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDisable(GL_CULL_FACE);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		//glFlush();
		glfwPollEvents();
	}
}

void renderLoop2() {
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
		//rot = glm::rotate(rot, glm::radians(1.0f), glm::vec3(0, 1, 0));
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &(rot * model));
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &view);
		glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, &projection);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		processCullCacheMiss();

		// input
		// -----
		processInput(window);

		// bind Texture
		clearTextureImage();
		
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderRaySegmentList();

		RenderSparseLeap();
		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		//glFlush();
		
		glfwPollEvents();
	}
}

void RenderRaySegmentList() {
	glDisable(GL_DEPTH_TEST);
	//glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	raySegmentListShader.use();
	raySegmentListShader.setVec3("cameraPos", camera.Position);
	glBindVertexArray(geometryVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, geometryArraySize);
	//glDrawArraysInstanced(GL_TRIANGLES, 0, 36, i);
	glBindVertexArray(0);
	i+=5;
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSparseLeap(){
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	boxShader.use();
	glBindVertexArray(cubeVAO);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	// 第二处理阶段
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // 返回默认
	sparseLeapShader.use();

	sparseLeapShader.setInt("exitPoints", 0);
	sparseLeapShader.setInt("volume", 1);
	sparseLeapShader.setInt("transferFunc", 2);
	sparseLeapShader.setVec2("ScreenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, volumeTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, tex1d);

	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);
}

void destroy() {
	glDeleteVertexArrays(1, &geometryVAO);
	glDeleteBuffers(1, &geometryVBO);

	glfwTerminate();
}