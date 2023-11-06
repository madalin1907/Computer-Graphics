// ================================================
//              Grafica pe calculator             |
// ================================================
//            PROIECT 1 - Joc de Biliard          |
// ================================================

// Biblioteci
#include <chrono>						 // Biblioteci utilizate pentru adaugarea de deelay-uri in program;
#include <thread>						 
#include <string>						 //	 Biblioteca pentru lucrul cu siruri de caractere;
#include <windows.h>					 //	 Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>						 //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>					 //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>				 //	 Include functii pentru: 
										 //	    - gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
										 //     - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
										 //     - crearea de meniuri si submeniuri;
#include "loadShaders.h"				 //	 Fisierul care face legatura intre program si shadere;
#include "SOIL.h"						 //	 Biblioteca pentru texturare;
#include "glm/glm.hpp"					 //	 Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Variabile folosite pentru desenarea bilelor
const int ballsNumber = 10; 						 //  Numarul de bile
const GLuint circleVertices = 25;					 //	 Numarul de varfuri ce compun poligoanele;
const float PI = 3.14159265359;						 //	 Valoarea lui PI;
const int radius = 20;								 //	 Raza bilelor;
GLfloat ballsPositions[circleVertices][2];			 //	 Vector pentru pozitiile initiale ale bilelor

const GLfloat winWidth = 1280, winHeight = 730;										 //	 Dimensiunile ferestrei de afisare;
const float xMin = -640, xMax = 640.f, yMin = -365.0f, yMax = 365.0f;				 //	 Variabile pentru proiectia ortogonala;

std::chrono::steady_clock::time_point startTime;									 //  Variabila pentru calcularea timpului de rulare a programului;

GLuint VaoId, VboId, EboId, ProgramId, myMatrixLocation, texture, drawCodeLocation;  //  Identificatorii obiectelor de tip OpenGL;
glm::mat4 myMatrix, resizeMatrix;													 //	 Variabile catre matricile de transformare;
int drawCode;																		 //	 Variabila pentru desenarea bilelor


//	Functia de incarcare a texturilor in program;
void LoadTexture(const char* photoPath)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(photoPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}


//  Crearea si compilarea obiectelor de tip shader;
void CreateShaders(void)
{
	ProgramId = LoadShaders("Biliard_Shader.vert", "Biliard_Shader.frag");
	glUseProgram(ProgramId);
}


//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	GLfloat Vertices[(ballsNumber * circleVertices + 4) * 9];		// ballsNumber * circleVertices varfuri pentru bile + 4 varfuri pentru masa de biliard
																	// fiecare varf avand 9 valori: 4 pentru pozitie, 3 pentru culoare, 2 pentru texturare
										

	// MASA DE BILIARD

	// Se adauga manual coordonatele varfurilor mesei de biliard
	//    coordonata x    |     coordonata y      |         coordonate de texturare
	Vertices[0]  = -640.0f; Vertices[1]  = -365.0f; Vertices[7]  = 0.0f; Vertices[8]  = 0.0f;
	Vertices[9]  =  640.0f; Vertices[10] = -365.0f; Vertices[16] = 1.0f; Vertices[17] = 0.0f;
	Vertices[18] =  640.0f; Vertices[19] =  365.0f; Vertices[25] = 1.0f; Vertices[26] = 1.0f;
	Vertices[27] = -640.0f; Vertices[28] =  365.0f; Vertices[34] = 0.0f; Vertices[35] = 1.0f;

	
	// BILELE DE BILIARD
	
	// Vectorul cu pozitiile initiale ale bilelor
	//       coordonata x         |        coordonata y
	ballsPositions[0][0] = -282.0f; ballsPositions[0][1] =    0.0f;
	ballsPositions[1][0] =  215.0f; ballsPositions[1][1] =    0.0f;
	ballsPositions[2][0] =  250.0f; ballsPositions[2][1] =  -20.0f;
	ballsPositions[3][0] =  250.0f; ballsPositions[3][1] =   20.0f;
	ballsPositions[4][0] =  285.0f; ballsPositions[4][1] =  -40.0f;
	ballsPositions[5][0] =  285.0f; ballsPositions[5][1] =   40.0f;
	ballsPositions[6][0] =  320.0f; ballsPositions[6][1] =  -20.0f;
	ballsPositions[7][0] =  320.0f; ballsPositions[7][1] =   20.0f;
	ballsPositions[8][0] =  355.0f; ballsPositions[8][1] =    0.0f;
	ballsPositions[9][0] =  285.0f; ballsPositions[9][1] =    0.0f;


	// Se creeaza coordonatele bilelor de biliard
	for (int i = 0; i < ballsNumber; ++i) {
		for (int j = 0; j < circleVertices; ++j) {
			float angle = 2 * PI * j / circleVertices;
			Vertices[(4 + i * circleVertices + j) * 9]     = radius * cos(angle) + ballsPositions[i][0];    // coordonata x
			Vertices[(4 + i * circleVertices + j) * 9 + 1] = radius * sin(angle) + ballsPositions[i][1];    // coordonata y

			Vertices[(4 + i * circleVertices + j) * 9 + 7] = 0.5f * (1.0f + cos(angle));                    // coordonata de texturare x
			Vertices[(4 + i * circleVertices + j) * 9 + 8] = 0.5f * (1.0f + sin(angle));                    // coordonata de texturare y
		}
	}


	// Se modifica aceste coordonate pentru toate varfurile din scena, fiind identice
	for (int i = 0; i < ballsNumber * circleVertices + 4; i++) {
		Vertices[i * 9 + 2] = 0.0f;							 		 // coordonata z
		Vertices[i * 9 + 3] = 1.0f;								     // coordonata w

		Vertices[i * 9 + 4] = 0.0f;									 // valoarea pentru culoarea rosie
		Vertices[i * 9 + 5] = 0.0f;									 // valoarea pentru culoarea verde
		Vertices[i * 9 + 6] = 0.0f;									 // valoarea pentru culoarea albastra
	}


	//	Indicii care determina ordinea de parcurgere a varfurilor;
	static const GLuint Indices[] =
	{
		0, 1, 2, 3, // indices for the table
	};


	//  Transmiterea datelor prin buffere;
	
	//  Se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO;
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer pentru VARFURI - COORDONATE, CULORI si TEXTURARE;
	glGenBuffers(1, &VboId);														//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);											//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	
	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);									//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
	glEnableVertexAttribArray(0);				//  Se asociaza atributul (0 = coordonate) pentru shader;
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);				//  Se asociaza atributul (1 = culoare) pentru shader;
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);				//  Se asociaza atributul (2 = texturare) pentru shader;
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}


//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}


//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI(Coordonate, Culori, Textura), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

	//  Eliberarea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}


//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}


//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	startTime = std::chrono::steady_clock::now();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului;
	CreateShaders();							//  Initilizarea shaderelor;
	
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	drawCodeLocation = glGetUniformLocation(ProgramId, "drawCode");

	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}


//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Calculate the time elapsed in seconds
	auto currentTime = std::chrono::steady_clock::now();
	double elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

	CreateVBO();

	// Load and bind the texture as before
	LoadTexture("./textures/Table.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// Only draw the polygon if 3 seconds have elapsed
	/*if (elapsedTime >= 3.0) {
		myMatrix = resizeMatrix;
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(0));
	}*/

	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	drawCode = 1;
	glUniform1i(drawCodeLocation, drawCode);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(0));

	for (int i = 0; i < ballsNumber; ++i) {
		std::string textureFileName = "./textures/" + std::to_string(i) + "ball.png";

		LoadTexture(textureFileName.c_str());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

		glDrawArrays(GL_POLYGON, 4 + i * circleVertices, circleVertices);
	}

	glutSwapBuffers();
	glFlush();
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);			//	Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
	glutInitWindowSize(winWidth, winHeight);				//  Dimensiunea ferestrei;
	glutInitWindowPosition(0, 0);							//  Pozitia initiala a ferestrei;
	glutCreateWindow("Joc de Biliard");						//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	glewInit();

	Initialize();			
	glutDisplayFunc(RenderFunction);						//  Desenarea scenei in fereastra;
	glutIdleFunc(RenderFunction);							//	Asigura rularea continua a randarii;
	glutCloseFunc(Cleanup);								    //  Eliberarea resurselor alocate de program;
	
	glutMainLoop();
	return 0;
}
