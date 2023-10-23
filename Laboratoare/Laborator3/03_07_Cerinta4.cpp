#include <iostream>			//	Biblioteca necesara pentru afisarea in consola;
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

GLuint VaoId, VboId, ColorBufferId, ProgramId, myMatrixLocation, codColLocation; //  Identificatorii obiectelor de tip OpenGL;
GLfloat winWidth = 700, winHeight = 500; //	Dimensiunile ferestrei de afisare;

glm::mat4 myMatrix, resizeMatrix, translationMatrix, rotationMatrix; //	Variabile catre matricile de transformare;

int codCol;	//	Variabila ce determina schimbarea culorii pixelilor in shader;
float xMin = -350.f, xMax = 350.f, yMin = -250.f, yMax = 250.f; //	Variabile pentru proiectia ortogonala;
float translationStep = 0.1, rotationStep = 0.0015, i = 0.0, alpha = 0.0, angle = 0; //  Variabile pentru deplasarea pe axa Ox si pentru rotatie;


void MoveLeft(void) {
	i = i + alpha;
	if (i <= -335.0)
		alpha = translationStep;
	else if (i >= 335.0)
		alpha = -translationStep;
	angle = angle + rotationStep;
	glutPostRedisplay();
}

void MoveRight(void) {
	i = i + alpha;
	if (i >= 335.0)
		alpha = -translationStep;
	else if (i <= -335.0)
		alpha = translationStep;
	angle = angle - rotationStep;
	glutPostRedisplay();
}

void UseMouse(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			alpha = -translationStep;
		glutIdleFunc(MoveLeft);
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			alpha = translationStep;
		glutIdleFunc(MoveRight);
		break;
	default:
		break;
	}
}


//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("03_07_Shader.vert", "03_07_Shader.frag");
	glUseProgram(ProgramId);
}

//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	//  Coordonatele varfurilor;
	GLfloat Vertices[] = {
	//	Varfuri pentru axa orizontală (ox)
	   -350.0f, 0.0f, 0.0f, 1.0f,
		350.0f, 0.0f, 0.0f, 1.0f,

	//	Varfurile patratului
		-10.0f,  -10.0f, 0.0f, 1.0f,
		 10.0f,  -10.0f, 0.0f, 1.0f,
		 10.0f,   10.0f, 0.0f, 1.0f,
		-10.0f,   10.0f, 0.0f, 1.0f,
	};

	//  Culorile axelor;
	GLfloat Colors[] = {
		//	Culori pentru axa orizontală (ox)
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	//  Transmiterea datelor prin buffere;

	//  Se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO;
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer pentru VARFURI;
	glGenBuffers(1, &VboId);                                                        //  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);                                           //  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);      //  Punctele sunt "copiate" in bufferul curent;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//  Se creeaza un buffer pentru CULOARE;
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
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

	//  Stergerea bufferelor pentru varfuri, culori;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

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
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;
	
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.0, 0.0));
	rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));
	

	//	Desenarea axei
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	codCol = 1;
	glUniform1i(codColLocation, codCol);

	glDrawArrays(GL_LINES, 0, 2);


	//	Desenarea patratului;
	myMatrix = resizeMatrix * translationMatrix * rotationMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	codCol = 2;
	glUniform1i(codColLocation, codCol);
	
	glDrawArrays(GL_POLYGON, 2, 4);

	glutSwapBuffers();	//	Inlocuieste imaginea deseneata in fereastra cu cea randata; 
	glFlush();	//  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);					//	Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(1000, 300);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Cerintele 1-3");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	glewInit();

	Initialize();						//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);	//  Desenarea scenei in fereastra;
	glutMouseFunc(UseMouse);			//	Activarea utilizarii mouseului;
	glutCloseFunc(Cleanup);				//  Eliberarea resurselor alocate de program;

	glutMainLoop();

	return 0;
}