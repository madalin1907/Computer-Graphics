﻿//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul IV - 04_04_Cerinta1.cpp |
// ======================================
// 
//	Biblioteci

#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
							//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
							//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
							//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

GLuint VaoId, VboId, EboId, ProgramId, myMatrixLocation; //  Identificatorii obiectelor de tip OpenGL;
GLfloat winWidth = 800, winHeight = 600; //	Dimensiunile ferestrei de afisare;
glm::mat4 myMatrix, resizeMatrix; //	Variabile catre matricile de transformare;
float xMin = -400, xMax = 400.f, yMin = -300.f, yMax = 300.f; //	Variabile pentru proiectia ortogonala;


//  Crearea si compilarea obiectelor de tip shader;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("04_04_Shader.vert", "04_04_Shader.frag");
	glUseProgram(ProgramId);
}

//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	//  Coordonatele varfurilor;
	static const GLfloat Vertices[] =
	{
		-250.0f,  -50.0f, 0.0f, 1.0f,
		-150.0f,  -50.0f, 0.0f, 1.0f,
		 -50.0f,  -50.0f, 0.0f, 1.0f,
		  50.0f,  -50.0f, 0.0f, 1.0f,
		 150.0f,  -50.0f, 0.0f, 1.0f,
		 250.0f,  -50.0f, 0.0f, 1.0f,

		 250.0f,   50.0f, 0.0f, 1.0f,
		 150.0f,   50.0f, 0.0f, 1.0f,
		  50.0f,   50.0f, 0.0f, 1.0f,
		 -50.0f,   50.0f, 0.0f, 1.0f,
	    -150.0f,   50.0f, 0.0f, 1.0f,
		-250.0f,   50.0f, 0.0f, 1.0f
	};

	//	Culorile ca atribute ale varfurilor;
	static const GLfloat Colors[] =
	{
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 
		0.0f, 0.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 
		1.0f, 0.0f, 0.0f, 1.0f
	};
	
	//	Indicii care determina ordinea de parcurgere a varfurilor;
	static const GLuint Indices[] =
	{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, // indici pentru varfurile dreptunghiului
		1, 10, 2, 9, 3, 8, 4, 7, // indici pentru liniile verticale din interiorul dreptunghiului
		0, 10, 1, 9, 2, 8, 3, 7, 4, 6, 5, 11 // indici pentru liniile diagonale din interiorul dreptunghiului
	};


	//  Transmiterea datelor prin buffere;
	//  Se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO;
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer comun pentru VARFURI - COORDONATE si CULORI;
	glGenBuffers(1, &VboId);																//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);													//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors) + sizeof(Vertices), NULL, GL_STATIC_DRAW);	//	Definirea bufferului, dimensiunea lui = dimensiunea(COLORS + VERTICES)
	//	Spatiul bufferului este impartit intre zona de COORDONATE si cea de VARFURI:
	//	 - prima sectiune incepe de la 0, are dimensiunea VERTICES si continele datele despre COORDONATE;
	//	 - a doua sectiune incepe de la sizeof(Vertices) - deci la finalul primei sectiuni, are dimensiunea COLORS si contine datele despre CULOARE;
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);				//	COORDONATELE;
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);		//	CULORILE;
	
	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(Vertices));
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
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI(Coordonate + Culori), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

	//  Eliberaea obiectelor de tip VAO;
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului;
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();							//  Initilizarea shaderelor;
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	//	Dreptunghiul "decupat"; 
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;

	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glLineWidth(5.0f);
	
	// Dreptunghiul exterior
	glDrawElements(GL_LINE_LOOP, 12, GL_UNSIGNED_INT, (void*)(0));
	
	// Liniile verticale din interiorul dreptunghiului
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));

	// Liniile orizontale din interiorul dreptunghiului
	glDrawElements(GL_LINES, 10, GL_UNSIGNED_INT, (void*)(20 * sizeof(GLuint)));

	glFlush(); // Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);					//	Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(50, 50);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Cerinta1");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	glewInit();
	Initialize();						//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);	//  Desenarea scenei in fereastra;
	glutCloseFunc(Cleanup);				//  Eliberarea resurselor alocate de program;
	
	glutMainLoop();
	return 0;
}
