#include "Sprite.h"

void Sprite::initialize(int texID, int imgWidth, int imgHeight, int nAnimations, int nFrames, float speedX, float speedY)
{
	this->texID = texID;
	this->imgWidth = imgWidth;
	this->imgHeight = imgHeight;
	this->nAnimations = nAnimations;
	this->nFrames = nFrames;
	this->destroyed = false;
	if (speedX && speedY) {
		speed.x = speedX;
		speed.y = speedY;
	}
	else {
		speed.x = 15.0;
		speed.y = 15.0;
	}

	dx = 1.0 / float(nFrames);
	dy = 1.0 / float(nAnimations);

	GLfloat vertices[] = {
		//x   y     z    r    g    b     s     t
		//T1
		-0.5,  0.5, 0.0, 1.0, 0.0, 0.0, 0.0, dy, //v0
		-0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, //v1
		 0.5,  0.5, 0.0, 0.0, 0.0, 1.0, dx, dy, //v2
		 //T2
		-0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, //v1
		 0.5,  0.5, 0.0, 0.0, 0.0, 1.0, dx, dy, //v2
		 0.5, -0.5, 0.0, 0.0, 1.0, 0.0, dx, 0.0  //v3
	};
	
	GLuint VBO;
	

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	
}

void Sprite::setShader(Shader* shader)
{
	this->shader = shader; 
	shader->use();
}


void Sprite::draw()
{
	
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texID);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Sprite::update(bool rotate)
{
	glm::mat4 model = glm::mat4(1); //matriz identidade
	model = glm::translate(model, position);
	model = glm::scale(model, dimensions);
	//rotação spriter inimigos
	if (rotate) {
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	}
	int modelLoc = glGetUniformLocation(shader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	iFrame = (iFrame + 1) % nFrames;

	float offsetx = iFrame * dx;
	float offsety = iAnimation * dy;
	shader->setVec2("offsets", offsetx, offsety);

}

void Sprite::getAABB(glm::vec2& min, glm::vec2& max)
{
	min.x = position.x - dimensions.x / 2.0;
	max.x = position.x + dimensions.x / 2.0;

	min.y = position.y - dimensions.y / 2.0;
	max.y = position.y + dimensions.y / 2.0;

}
