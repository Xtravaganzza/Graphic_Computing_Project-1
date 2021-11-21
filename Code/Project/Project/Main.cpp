#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(-100.0f, 2.0f, -45.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;


// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-95.0f, 1.0f, -45.0f);
bool active;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX = PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq, rotRodDer, rotBrazoDer, rotBrazoIzq; //Variables KeyFrames

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotRodDer;
	float rotInc;
	float rotInc2; //2 variables KeyFrames.

	float rotBrazoDer;
	float rotInc3;
	float rotBrazoIzq;
	float rotInc4;
	float rotSled;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;


bool sledTrayectory=false;
bool trayectory1=true;
bool trayectory2;
bool trayectory3;
bool trayectory4;
float rotSled;
float movx=0;
float movy=0;
float movz=-130;

// Positions of the point lights
glm::vec3 houseLights[] = {
	glm::vec3(40, 7.85, -23.2),
	glm::vec3(20, 7.85, -23.2),
	glm::vec3(0, 7.85, -33),
	
	
	glm::vec3(-21, 7.85, -20),
	glm::vec3(-21, 7.85, 0),
	glm::vec3(-21, 7.85, 20),

	glm::vec3(40, 7.85, 46),
	glm::vec3(20, 7.85, 46),
	glm::vec3(0, 7.85, 35),

	glm::vec3(59, 7.85, -10),
	glm::vec3(59, 7.85, 10),
	glm::vec3(59, 7.85, 30),
};

glm::vec3 treeLights[] = {
	glm::vec3(100, 17, -90),
	glm::vec3(85, 30, -80),
	glm::vec3(90, 15, -70),
	glm::vec3(100, 0, -90),
	glm::vec3(85, 5, -80),
	glm::vec3(90, 10, -70),

	glm::vec3(-70, 30, 85),
	glm::vec3(-70, 25, 80),
	glm::vec3(-85, 10, 70),
	glm::vec3(-85, 15, 85),
	glm::vec3(-80, 18, 80),
	glm::vec3(-80, 5, 70),
};

glm::vec3 LightP1;
glm::vec3 LightP2;
glm::vec3 LightP3;




void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].rotRodDer = rotRodDer; //Variable KeyFrame
	KeyFrame[FrameIndex].rotBrazoDer = rotBrazoDer;
	KeyFrame[FrameIndex].rotBrazoIzq = rotBrazoIzq;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	rotRodDer = KeyFrame[0].rotRodDer; //Variable KeyFrames.
	rotBrazoDer = KeyFrame[0].rotBrazoDer;
	rotBrazoIzq = KeyFrame[0].rotBrazoIzq;
}

void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	KeyFrame[playIndex].rotInc2 = (KeyFrame[playIndex + 1].rotRodDer - KeyFrame[playIndex].rotRodDer) / i_max_steps; //Variable KeyFrames
	KeyFrame[playIndex].rotInc3 = (KeyFrame[playIndex + 1].rotBrazoDer - KeyFrame[playIndex].rotBrazoDer) / i_max_steps;
	KeyFrame[playIndex].rotInc4 = (KeyFrame[playIndex + 1].rotBrazoIzq - KeyFrame[playIndex].rotBrazoIzq) / i_max_steps;
}




int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");

	Model Trees((char*)"Models/Trees/trees.obj");
	Model House((char*)"Models/House/house.obj");
	Model LivingRoom((char*)"Models/LivingRoom/livingRoom.obj");
	Model LivingRoom2((char*)"Models/LivingRoom/livingRoom2.obj");
	Model Kitchen((char*)"Models/Kitchen/kitchen.obj");
	Model Bathroom((char*)"Models/Bathroom/bathroom.obj");
	Model Room((char*)"Models/Room/room.obj");
	Model Presents((char*)"Models/Presents/presents.obj");
	Model Windows((char*)"Models/Windows/windows.obj");
	Model MailBox((char*)"Models/Mailbox/mailbox.obj");
	Model Sled((char*)"Models/Sled/sled.obj");
	Model Sign((char*)"Models/Sign/sign.obj");
	//Model Santa((char*)"Models/Santa/Santa_OBJ.obj");
	
	//Model TrenDecorativo((char*)"Models/TrenDecorativo/TrenDecorativo.obj");
	////Piezas o partes del tren animado
	//Model BarraDerCorta((char*)"Models/TrenAnimadoPartes/BarraDerCorta.obj");
	//Model BarraDerLarga((char*)"Models/TrenAnimadoPartes/BarraDerLarga.obj");
	//Model BarraIzqCorta((char*)"Models/TrenAnimadoPartes/BarraIzqCorta.obj");
	//Model BarraIzqLarga((char*)"Models/TrenAnimadoPartes/BarraIzqLarga.obj");
	//Model Campana((char*)"Models/TrenAnimadoPartes/Campana.obj");
	//Model EstructuraTren((char*)"Models/TrenAnimadoPartes/EstructuraTren.obj");
	//Model LlantaDer1((char*)"Models/TrenAnimadoPartes/LlantaDer1.obj");
	//Model LlantaDer2((char*)"Models/TrenAnimadoPartes/LlantaDer2.obj");
	//Model LlantaDer3((char*)"Models/TrenAnimadoPartes/LlantaDer3.obj");
	//Model LlantaDer4((char*)"Models/TrenAnimadoPartes/LlantaDer4.obj");
	//Model LlantaDer5((char*)"Models/TrenAnimadoPartes/LlantaDer5.obj");
	//Model LlantaDer6((char*)"Models/TrenAnimadoPartes/LlantaDer6.obj");
	//Model LlantaDer7((char*)"Models/TrenAnimadoPartes/LlantaDer7.obj");
	//Model LlantaDer8((char*)"Models/TrenAnimadoPartes/LlantaDer8.obj");
	//Model LlantaIzq1((char*)"Models/TrenAnimadoPartes/LlantaIzq1.obj");
	//Model LlantaIzq2((char*)"Models/TrenAnimadoPartes/LlantaIzq2.obj");
	//Model LlantaIzq3((char*)"Models/TrenAnimadoPartes/LlantaIzq3.obj");
	//Model LlantaIzq4((char*)"Models/TrenAnimadoPartes/LlantaIzq4.obj");
	//Model LlantaIzq5((char*)"Models/TrenAnimadoPartes/LlantaIzq5.obj");
	//Model LlantaIzq6((char*)"Models/TrenAnimadoPartes/LlantaIzq6.obj");
	//Model LlantaIzq7((char*)"Models/TrenAnimadoPartes/LlantaIzq7.obj");
	//Model LlantaIzq8((char*)"Models/TrenAnimadoPartes/LlantaIzq7.obj");
	// Build and compile our shader program


	//Inicializaci�n de KeyFrames

	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotRodDer = 0;
		KeyFrame[i].rotInc = 0;
		KeyFrame[i].rotInc2 = 0; //2 Variables KeyFrames

		KeyFrame[i].rotBrazoDer = 0;
		KeyFrame[i].rotInc3 = 0;
		KeyFrame[i].rotBrazoDer = 0;
		KeyFrame[i].rotInc4 = 0;
	}



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};

	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};



	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/Winter/winter-skyboxes/IceLake/posx.jpg");
	faces.push_back("SkyBox/Winter/winter-skyboxes/IceLake/negx.jpg");
	faces.push_back("SkyBox/Winter/winter-skyboxes/IceLake/posy.jpg");
	faces.push_back("SkyBox/Winter/winter-skyboxes/IceLake/negy.jpg");
	faces.push_back("SkyBox/Winter/winter-skyboxes/IceLake/posz.jpg");
	faces.push_back("SkyBox/Winter/winter-skyboxes/IceLake/negz.jpg");


	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 10.0f);
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		/**************************************************************************************************************************
		*													Iluminación	casa													  *
		**************************************************************************************************************************/
		glm::vec3 lightColor1;
		lightColor1.x = abs(sin(glfwGetTime() * LightP1.x));
		lightColor1.y = abs(sin(glfwGetTime() * LightP1.y));
		lightColor1.z = sin(glfwGetTime() * LightP1.z);
		glm::vec3 lightColor2;
		lightColor2.x = abs(sin(glfwGetTime() * LightP2.x));
		lightColor2.y = abs(sin(glfwGetTime() * LightP2.y));
		lightColor2.z = sin(glfwGetTime() * LightP2.z);
		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), houseLights[0].x, houseLights[0].y, houseLights[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);
		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), houseLights[1].x, houseLights[1].y, houseLights[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);
		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), houseLights[2].x, houseLights[2].y, houseLights[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);
		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), houseLights[3].x, houseLights[3].y, houseLights[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);
		// Point light 5
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].position"), houseLights[4].x, houseLights[4].y, houseLights[4].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].diffuse"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[4].specular"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[4].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[4].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[4].quadratic"), 0.032f);
		// Point light 6
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[5].position"), houseLights[5].x, houseLights[5].y, houseLights[5].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[5].diffuse"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[5].specular"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[5].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[5].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[5].quadratic"), 0.032f);
		// Point light 7
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[6].position"), houseLights[6].x, houseLights[6].y, houseLights[6].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[6].diffuse"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[6].specular"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[6].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[6].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[6].quadratic"), 0.032f);
		// Point light 8
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[7].position"), houseLights[7].x, houseLights[7].y, houseLights[7].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[7].diffuse"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[7].specular"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[7].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[7].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[7].quadratic"), 0.032f);
		// Point light 9
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[8].position"), houseLights[8].x, houseLights[8].y, houseLights[8].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[8].diffuse"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[8].specular"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[8].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[8].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[8].quadratic"), 0.032f);
		// Point light 10
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[9].position"), houseLights[9].x, houseLights[9].y, houseLights[9].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[9].diffuse"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[9].specular"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[9].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[9].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[9].quadratic"), 0.032f);
		// Point light 11
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[10].position"), houseLights[10].x, houseLights[10].y, houseLights[10].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[10].diffuse"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[10].specular"), lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[10].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[10].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[10].quadratic"), 0.032f);
		// Point light 12
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[11].position"), houseLights[11].x, houseLights[11].y, houseLights[11].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[11].diffuse"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[11].specular"), lightColor2.x, lightColor2.y, lightColor2.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[11].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[11].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[11].quadratic"), 0.032f);
		/**************************************************************************************************************************
		*													Iluminación	árboles													  *
		**************************************************************************************************************************/
		glm::vec3 lightColor3;
		lightColor3.x = abs(sin(glfwGetTime() * LightP3.x));
		lightColor3.y = abs(sin(glfwGetTime() * LightP3.y));
		lightColor3.z = sin(glfwGetTime() * LightP3.z);
		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[12].position"), treeLights[0].x, treeLights[0].y, treeLights[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[12].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[12].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[12].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[12].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[12].quadratic"), 0.032f);
		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[13].position"), treeLights[1].x, treeLights[1].y, treeLights[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[13].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[13].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[13].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[13].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[13].quadratic"), 0.032f);
		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[14].position"), treeLights[2].x, treeLights[2].y, treeLights[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[14].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[14].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[14].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[14].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[14].quadratic"), 0.032f);
		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].position"), treeLights[3].x, treeLights[3].y, treeLights[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[15].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[15].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[15].quadratic"), 0.032f);
		// Point light 5
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].position"), treeLights[4].x, treeLights[4].y, treeLights[4].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[15].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[15].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[15].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[15].quadratic"), 0.032f);
		// Point light 6
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[16].position"), treeLights[5].x, treeLights[5].y, treeLights[5].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[16].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[16].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[16].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[16].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[16].quadratic"), 0.032f);
		// Point light 7
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[17].position"), treeLights[6].x, treeLights[6].y, treeLights[6].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[17].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[17].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[17].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[17].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[17].quadratic"), 0.032f);
		// Point light 8
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[18].position"), treeLights[7].x, treeLights[7].y, treeLights[7].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[18].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[18].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[18].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[18].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[18].quadratic"), 0.032f);
		// Point light 9
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[19].position"), treeLights[8].x, treeLights[8].y, treeLights[8].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[19].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[19].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[19].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[19].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[19].quadratic"), 0.032f);
		// Point light 10
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[20].position"), treeLights[9].x, treeLights[9].y, treeLights[9].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[20].diffuse"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[20].specular"), lightColor3.x, lightColor3.y, lightColor3.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[20].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[20].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[20].quadratic"), 0.032f);

		// SpotLight
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);




		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO);


		glm::mat4 model(1);
		//Carga de modelo 
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		House.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Kitchen.Draw(lightingShader);


		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Room.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Bathroom.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LivingRoom.Draw(lightingShader);


		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LivingRoom2.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Presents.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Trees.Draw(lightingShader);


		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sled.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sign.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		MailBox.Draw(lightingShader);


		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Sled.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//TrenDecorativo.Draw(lightingShader);

		//////MODELO TREN DECORATIVO
		////model = glm::mat4(1);
		////glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////BarraDerCorta.Draw(lightingShader);

		//////MODELO TREN DECORATIVO
		////model = glm::mat4(1);
		////glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////BarraDerLarga.Draw(lightingShader);

		//////MODELO TREN DECORATIVO
		////model = glm::mat4(1);
		////glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////BarraIzqCorta.Draw(lightingShader);

		////model = glm::mat4(1);
		////glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		////BarraIzqLarga.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Campana.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//EstructuraTren.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaDer1.Draw(lightingShader);

		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaDer2.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaDer3.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaDer4.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaDer5.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaDer6.Draw(lightingShader);

		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaDer7.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaIzq1.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaIzq2.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaIzq3.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaIzq4.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaIzq5.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaIzq6.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaIzq7.Draw(lightingShader);

		////MODELO TREN DECORATIVO
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//LlantaIzq8.Draw(lightingShader);

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Windows.Draw(lightingShader);
		//glDisable(GL_BLEND);





		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 12; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, houseLights[i]);
			model = glm::scale(model, glm::vec3(0.3f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);

			model = glm::mat4(1);
			model = glm::translate(model, treeLights[i]);
			model = glm::scale(model, glm::vec3(0.3f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default
		glDisable(GL_BLEND);



		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}


void animacion()
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += KeyFrame[playIndex].incX;
			posY += KeyFrame[playIndex].incY;
			posZ += KeyFrame[playIndex].incZ;

			rotRodIzq += KeyFrame[playIndex].rotInc;
			rotRodDer += KeyFrame[playIndex].rotInc2; //Variable KeyFrames.
			rotBrazoDer += KeyFrame[playIndex].rotInc3;
			rotBrazoIzq += KeyFrame[playIndex].rotInc4;

			i_curr_steps++;
		}	
	}
	if (sledTrayectory)
	{
		if (trayectory1)
		{
			if (movx == 160.0f)
			{
				trayectory1 = false;
				trayectory2 = true;
				rotSled = -90.0f;
			}
			else
			{
				movx += 0.5f;
			}
		}

		if (trayectory2)
		{
			if (movz == 130)
			{
				trayectory2 = false;
				trayectory3 = true;
				rotSled = 180.0f;
			}
			else
			{
				movz += 0.5f;
			}
		}

		if (trayectory3)
		{
			if (movx ==-140.0f)
			{
				trayectory3 = false;
				trayectory4 = true;
				rotSled = 90.0;

			}
			else
			{
				movx -= 0.5f;
			}
		}

		if (trayectory4)
		{
			if (movz < -130.0f)
			{
				trayectory4 = false;
				trayectory1 = true;
				rotSled = 0.0f;
			}
			else
			{
				movz -= 0.5f;
			}
		}
	}
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		//if (play == false && (FrameIndex > 1))
		//{

		//	resetElements();
		//	//First Interpolation				
		//	interpolation();

		//	play = true;
		//	playIndex = 0;
		//	i_curr_steps = 0;
		//}
		//else
		//{
		//	play = false;
		//}
		sledTrayectory = !sledTrayectory;
	}

	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}

	}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			LightP1 = glm::vec3(1.0f, 0.0f, 0.0f);
			LightP2 = glm::vec3(0.0f, 1.0f, 0.0f);
			LightP3 = glm::vec3(1.0f, 1.0f, 0.0f);
		}
		else
		{
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
			LightP2 = glm::vec3(0.0f, 0.0f, 0.0f);
			LightP3 = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	if (keys[GLFW_KEY_1])
	{

		rot += 1;

	}

	if (keys[GLFW_KEY_2])
	{
		if (rotRodIzq < 80.0f)
			rotRodIzq += 1.0f;
	}

	if (keys[GLFW_KEY_3])
	{
		if (rotRodIzq > -45)
			rotRodIzq -= 1.0f;
	}

	/*Variables KeyFrames*/
	if (keys[GLFW_KEY_4])
	{
		if (rotRodDer < 80.0f)
			rotRodDer += 1.0f;
	}

	if (keys[GLFW_KEY_5])
	{
		if (rotRodDer > -45)
			rotRodDer -= 1.0f;
	}

	if (keys[GLFW_KEY_6])
	{
		if (rotBrazoDer < 80.0f)
			rotBrazoDer += 1.0f;
	}

	if (keys[GLFW_KEY_7])
	{
		if (rotBrazoDer > -45)
			rotBrazoDer -= 1.0f;
	}

	if (keys[GLFW_KEY_8])
	{
		if (rotBrazoIzq < 80.0f)
			rotBrazoIzq += 1.0f;
	}

	if (keys[GLFW_KEY_9])
	{
		if (rotBrazoIzq > -45)
			rotBrazoIzq -= 1.0f;
	}

	//Mov Personaje
	if (keys[GLFW_KEY_H])
	{
		posZ += 1;
	}

	if (keys[GLFW_KEY_Y])
	{
		posZ -= 1;
	}

	if (keys[GLFW_KEY_G])
	{
		posX -= 1;
	}

	if (keys[GLFW_KEY_J])
	{
		posX += 1;
	}




	// Camera controls
	if (keys[GLFW_KEY_W])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime + 1);
	}
	if (keys[GLFW_KEY_S])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime + 1);
	}
	if (keys[GLFW_KEY_A])
	{
		camera.ProcessKeyboard(LEFT, deltaTime + 1);
	}
	if (keys[GLFW_KEY_D])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime + 1);
	}


	if (keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}