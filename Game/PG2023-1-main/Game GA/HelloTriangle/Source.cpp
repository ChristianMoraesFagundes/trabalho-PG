/*
TRABALHO GA PROCESSAMENTO GRÁFICO - CHRISTIAN MORAES FAGUNDES
*/

#include <iostream>
#include <string>
#include <assert.h>
#include <cstdlib>
using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STB IMAGE
#include "stb_image.h"

//Classe shader
#include "Shader.h"

//Classe sprite
#include "Sprite.h"
#include <list>
#include <ctime>

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

GLuint generateTexture(string filePath, int &width, int &height);
bool testCollision(Sprite spr1, Sprite* character, GLFWwindow* window);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

//Seta o player
Sprite character;

//variavel quantidade de inimigo renderizados na tela
int qtdInimigo = 0;
Sprite setEnemy(Shader* shader, GLuint texID);

//lista com inimigos 
list<Sprite*>enemyList;
int timeToSpawnEnemy = 5;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Trabalho GA!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Compilando e buildando o programa de shader
	Shader shader("HelloTriangle.vs", "HelloTriangle.fs");


	//Inicializa player e background
	int bgwidth, bgheight;
	int charwidth, charheight;

	GLuint texID = generateTexture("../../textures/Background/background.png", bgwidth, bgheight);
	GLuint texID2 = generateTexture("../../textures/Spritesheets/playerShip/ShipSpriteSheet.png", charwidth, charheight);
	
	character.initialize(texID2, charwidth, charheight, 1, 4);
	character.setShader(&shader);
	character.setPosition(glm::vec3(100, 110, 0));
	character.setScale(glm::vec3(charwidth, charheight, 1));

	Sprite background;
	background.initialize(texID, bgwidth, bgheight);
	background.setShader(&shader);
	background.setPosition(glm::vec3(400, 300, 0));
	background.setScale(glm::vec3(bgwidth*0.5, bgheight*0.5, 1));
	
	//Inicializa o primeiro inimigos
	Sprite enemy = setEnemy(&shader,qtdInimigo);
	enemyList.push_back(&enemy);
		
	glUseProgram(shader.ID);

	glm::mat4 projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);

	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	//Ativar o buffer de textura
	glActiveTexture(GL_TEXTURE0);

	//Habilitar teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	//Habilitar a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//tempo incial
	time_t tInicial;
	time(&tInicial);
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		//renderizar inimigos com o tempo
		time_t tfinal;
		time(&tfinal);
		
		if (tfinal-tInicial >= timeToSpawnEnemy) {
			enemy = setEnemy(&shader,qtdInimigo);
			enemyList.push_back(&enemy);
			timeToSpawnEnemy += 5;
			
		}

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();
		testCollision(character, &character , window);
				
		// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(1);
		glPointSize(5);

		background.update();
		background.draw();

		character.update();
		
		if (!character.isDestroyed()) {
			character.draw();
		}
		
		for (Sprite* enemy:enemyList) {
			//true adiciona rotação de 180
			enemy->update(true);
			if (!enemy->isDestroyed()) {
				enemy->draw();
			}
			
			enemy->moveLeft();
		}

		glBindVertexArray(0); //Desconectando o buffer de geometria

		//fim da fase
		if (enemyList.size() == 10) {
			printf("Voce passou de fase!!!!!");
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
	{
		glm::vec2 min, max;
		character.getAABB(min, max);
		if (max.x + 1 < 810) {
			character.moveRight();
		}
	}
	if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
	{
		glm::vec2 min, max;
		character.getAABB(min, max);
		if (min.x - 1 >= 0) {
			character.moveLeft();
		}
				
	}
	if (key == GLFW_KEY_W || key == GLFW_KEY_UP)
	{
		glm::vec2 min, max;
		character.getAABB(min, max);
		if (max.y + 1 < 610) {
			character.moveUp();
		}
	}
	if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)
	{
		glm::vec2 min, max;
		character.getAABB(min, max);
		if (min.y - 1 > 0 ) {
			character.moveDown();
		}
	}
}

GLuint generateTexture(string filePath, int &width, int &height)
{
	GLuint texID;
	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Definindo o método de wrapping e de filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregando a imagen da textura
	int nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	//Manda para OpenGL armazenar a textura e gerar o mipmap
	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

bool testCollision(Sprite spr1, Sprite* pointspr1, GLFWwindow* window)
{
	glm::vec2 min1, min2, max1, max2;
	spr1.getAABB(min1, max1);
	

	//verifica cada inimigo 
	for (Sprite* enemy : enemyList) {
		enemy->getAABB(min2, max2);

		//deixar a colisão mais precisa
		min1.x += 2.0;
		min2.x += 2.0;
		min1.y += 2.0;
		min2.y += 2.0;

		if ((max2.y >= min1.y && max2.y <= max1.y && min2.x >= min1.x && min2.x <= max1.x) || (min2.y >= min1.y && min2.y <= max1.y && min2.x >= min1.x && min2.x <= max1.x) ||
			(max1.y >= min2.y && max1.y <= max2.y && min1.x >= min2.x && min1.x <= max2.x) || (min1.y >= min2.y && min1.y <= max2.y && min1.x >= min2.x && min1.x <= max2.x) && !enemy->isDestroyed()) {
			cout << "Collision!";
			pointspr1->destroy();
			enemy->destroy();
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (max2.x < -5 ) {
			enemy->destroy();
		}

		
	}
		
	return true;
}

Sprite setEnemy(Shader* shader,GLuint texID){

	Sprite enemy;
				
	int charwidth, charheight;

	texID = generateTexture("../../textures/Spritesheets/enemyShip/Ship4SpriteSheet.png", charwidth, charheight);
	
	srand((unsigned int)time(NULL));
	int yAleatorio = rand() % (500 + 1 - 100) + 100;

	enemy.initialize(texID, charwidth, charheight, 1, 4, 5, 5);
	enemy.setShader(shader);
	enemy.setPosition(glm::vec3(900, yAleatorio, 0));
	enemy.setScale(glm::vec3(charwidth, charheight, 1));
	
	qtdInimigo++;
	
	return enemy;
}

