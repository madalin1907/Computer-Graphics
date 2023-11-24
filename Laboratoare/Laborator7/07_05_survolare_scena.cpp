﻿//
// =================================================
// |             Grafica pe calculator             |
// =================================================
// |      Curs VII - 07_05_survolare_scena.cpp     |
// =================================================
// 
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

//	Numarul de multiplicari ale cubului;
#define INSTANCE_COUNT 75

//  Identificatorii obiectelor de tip OpenGL; 
GLuint VaoId, VbPos, VbCol, VbModelMat, EboId, ColorBufferId, ProgramId, viewLocation, projLocation, codColLocation, codCol;

const GLfloat PI = 3.141592;	 //	Valoarea lui pi;

GLfloat winWidth = 1200, winHeight = 750;	//	Dimensiunile ferestrei de afisare;

glm::mat4 view, projection;	   //  Matricile de vizualizare si proiectie;

float refX = 0.0f, refY = 0.0f, refZ = 0.0f,										//	Elemente pentru matricea de vizualizare;
	  obsX, obsY, obsZ,
	  vX = 0.0f, vY = 0.0f, vZ = 1.0f;

float alpha = 0.0f, beta = 0.0f, dist = 2500.0f,								    //	Elemente pentru deplasarea pe sfera;
	  incrAlpha1 = 0.01, incrAlpha2 = 0.01;

float width = 1200, height = 750,
	  zNear = 1.f, fov = 90.f * PI / 180;


void ProcessNormalKeys(unsigned char key, int x, int y) {
	switch (key) {						                  //	Apasarea tastelor `+` si `-` schimba pozitia observatorului (se departeaza / aproprie);
	case '-':
		dist -= 15.0;
		break;
	case '+':
		dist += 15.0;
		break;
	}

	if (key == 27)
		exit(0);
}


void ProcessSpecialKeys(int key, int xx, int yy) {
	switch (key) {							//	duce la deplasarea observatorului pe suprafata sferica in jurul cubului;
	case GLUT_KEY_LEFT:
		beta -= 0.01;
		break;

	case GLUT_KEY_RIGHT:
		beta += 0.01;
		break;

	case GLUT_KEY_UP:
		alpha += incrAlpha1;
		if (abs(alpha - PI / 2) < 0.05)
			incrAlpha1 = 0.f;
		else
			incrAlpha1 = 0.01f;
		break;

	case GLUT_KEY_DOWN:
		alpha -= incrAlpha2;
		if (abs(alpha + PI / 2) < 0.05)
			incrAlpha2 = 0.f;
		else
			incrAlpha2 = 0.01f;
		break;
	}
}


//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("07_04_Shader.vert", "07_04_Shader.frag");
	glUseProgram(ProgramId);
}

//  Se initializeaza un vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	//	COORDONATELE varfurilor; 
	GLfloat Vertices[] = {
		//  Coordonatele bazei
		-75.0f, -75.0f, -75.0f, 1.0f,
		 75.0f, -75.0f, -75.0f, 1.0f,
		 75.0f,  75.0f, -75.0f, 1.0f,
		-75.0f,  75.0f, -75.0f, 1.0f,

		//  Coordonatele varfului
		  0.0f,   0.0f,  45.0f, 1.0f
	};

	//	CULORILE instantelor;
	//	Culorile sunt generate in functie de indexul de instatiere - fiecare cub va avea o singura culoare;
	glm::vec4 Colors[INSTANCE_COUNT];
	for (int i = 0; i < INSTANCE_COUNT; i++)
	{
		float a = float(i) / 4.0f;
		float b = float(i) / 5.0f;
		float c = float(i) / 6.0f;
		Colors[i][0] = 0.65f + 0.30f * (sinf(a + 2.0f) + 1.0f);
		Colors[i][1] = 0.25f + 0.25f * (sinf(b + 3.0f) + 1.0f);
		Colors[i][2] = 0.55f + 0.35f * (sinf(c + 4.0f) + 1.0f);
		Colors[i][3] = 1.0f;
	}

	//  MATRICELE instantelor - se defineste tiparul de desenare al cuburilor prin ROTIREA si TRANSLATIA cubului initial (INSTANCED RENDERING);
	glm::mat4 MatModel[INSTANCE_COUNT];
	for (int i = 0; i < INSTANCE_COUNT; ++i)
	{
		float angle = float(i) / INSTANCE_COUNT * 4 * PI; // Unghiul în functie de index si numarul total de instante
		float radius = 150.0f + 30.0f * i;				  // Marim raza pentru distanta mai mare intre piramide

		float x = radius * cos(angle); 				  // Deplasarea pe OX;
		float y = radius * sin(angle); 				  // Deplasarea pe OY;
		float z = i * 25.0f;						  // Deplasarea pe OZ;

		MatModel[i] = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))
			        * glm::rotate(glm::mat4(1.0f), i * PI / 4, glm::vec3(i, 2 * i * i, i / 3));
	}

	//  Indicii pentru varfuri;
	GLubyte Indices[] =
	{
		0, 1, 2, 3,		//  Baza
		0, 3, 4,	    //  Fata din stanga
		3, 2, 4,        //  Fata din spate
		1, 2, 4,        //  Fata din dreapta
		0, 1, 4,	    //  Fata din fata
		
		0, 1, 2, 3,     //  Conturul bazei
		0, 4, 1, 4, 2, 4, 3, 4
	};

	//  Transmiterea datelor prin buffere;

	//  Se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO;
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer pentru COORDONATE;
	glGenBuffers(1, &VbPos);														//  Generarea bufferului si indexarea acestuia catre variabila VbPos;
	glBindBuffer(GL_ARRAY_BUFFER, VbPos);											//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	//	Se activeaza lucrul cu atribute;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	//  Se creeaza un buffer pentru CULORI;
	glGenBuffers(1, &VbCol);
	glBindBuffer(GL_ARRAY_BUFFER, VbCol);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	//	Se activeaza lucrul cu atribute;
	//  Se asociaza atributul (1 = culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
	//	Rata cu care are loc distribuirea culorilor per instanta;
	glVertexAttribDivisor(1, 1); 

	//  Se creeaza un buffer pentru MATRICEA DE POZITIE (instantierea cuburilor);
	glGenBuffers(1, &VbModelMat);
	glBindBuffer(GL_ARRAY_BUFFER, VbModelMat);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MatModel), MatModel, GL_STATIC_DRAW);
	//	Se activeaza lucrul cu atribute pentru fiecare coloana din fiecare matrice de instantiere;
	//	Se incepe de la primul indice de atribut disponibil (2) si se creeaza pe rand cate unul pentru fiecare coloana (2+i, unde i = 0..3);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(2 + i);
		glVertexAttribPointer(2 + i,              //  Location;
			4, GL_FLOAT, GL_FALSE,                //  vec4;
			sizeof(glm::mat4),                    //  Stride;
			(void*)(sizeof(glm::vec4) * i));      //  Start offset;
		glVertexAttribDivisor(2 + i, 1);
	}

	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

// Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}
void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori, Instanitere), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VbPos);
	glDeleteBuffers(1, &VbCol);
	glDeleteBuffers(1, &VbModelMat);
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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);		//  Culoarea de fond a ecranului;
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();							//  Initilizarea shaderelor;
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	viewLocation = glGetUniformLocation(ProgramId, "viewMatrix");
	projLocation = glGetUniformLocation(ProgramId, "projectionMatrix");
	codColLocation = glGetUniformLocation(ProgramId, "codColShader");
}

//	Functia de desenare a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//  Se curata ecranul OpenGL pentru a fi desenat noul continut (bufferul de culoare & adancime);
	glEnable(GL_DEPTH_TEST);

	//	CreateVBO(); //	 Decomentati acest rand daca este cazul;
	glBindVertexArray(VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, VbPos);
	glBindBuffer(GL_ARRAY_BUFFER, VbCol);
	glBindBuffer(GL_ARRAY_BUFFER, VbModelMat);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

	//	Pozitia observatorului - se deplaseaza pe sfera;
	obsX = refX + dist * cos(alpha) * cos(beta);
	obsY = refY + dist * cos(alpha) * sin(beta);
	obsZ = refZ + dist * sin(alpha);

	//	Matricea de vizualizare;
	glm::vec3 obs = glm::vec3(obsX, obsY, obsZ);		//	Pozitia observatorului;	
	glm::vec3 pctRef = glm::vec3(refX, refY, refZ); 	//	Pozitia punctului de referinta;
	glm::vec3 vert = glm::vec3(vX, vY, vZ);				//	Verticala din planul de vizualizare; 
	view = glm::lookAt(obs, pctRef, vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	//	Realizarea proiectiei;
	projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), zNear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);


	codCol = 0;
	glUniform1i(codColLocation, codCol);

	//	Desenarea bazei
	glDrawElementsInstanced(GL_POLYGON, 4, GL_UNSIGNED_BYTE, 0, INSTANCE_COUNT);

	//	Desenarea fetelor
	glDrawElementsInstanced(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, (void*)(4), INSTANCE_COUNT);

	//  Desenarea muchiilor;
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glLineWidth(1.5);
	
	glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(16), INSTANCE_COUNT);
	glDrawElementsInstanced(GL_LINES, 8, GL_UNSIGNED_BYTE, (void*)(20), INSTANCE_COUNT);

	glutSwapBuffers();	//	Inlocuieste imaginea deseneata in fereastra cu cea randata; 
	glFlush();			//  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}


int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);		//	Se folosesc 2 buffere pentru desen (unul pentru afisare si unul pentru randare => animatii cursive) si culori RGB + 1 buffer pentru adancime;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(30, 30);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Cerinta3");			//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	glewInit();

	Initialize();							//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);		//  Desenarea scenei in fereastra;
	glutIdleFunc(RenderFunction);			//	Asigura rularea continua a randarii;
	glutKeyboardFunc(ProcessNormalKeys);	//	Functii ce proceseaza inputul de la tastatura utilizatorului;
	glutSpecialFunc(ProcessSpecialKeys);
	glutCloseFunc(Cleanup);					//  Eliberarea resurselor alocate de program;

	glutMainLoop();
	return 0;
}
