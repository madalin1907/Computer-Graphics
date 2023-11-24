//
// ================================================
// |            Grafica pe calculator             |
// ================================================
// | Cursul VII - 07_03_piramida_patrulatera.cpp  |
// ================================================
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


GLuint VaoId, VboId, EboId, ProgramId, viewLocation, projLocation, codColLocation;	//  Identificatorii obiectelor de tip OpenGL;
GLfloat winWidth = 650, winHeight = 650;											//	Dimensiunile ferestrei de afisare;

glm::vec3 obs, pctRef, vert;														//	Vectori pentru matricea de vizualizare;
glm::mat4 view, projection;															//	Variabile catre matricile de transformare;

int codCol;	  //	Variabila ce determina schimbarea culorii pixelilor in shader;

const float PI = 3.141592;	//	Valaorea lui pi;

//	Elemente pentru matricea de vizualizare;
float obsX = 0.0,  obsY = 0.0,  obsZ =  300.f,
	  refX = 0.0f, refY = 0.0f, refZ = -100.f,
	    vX = 0.0;

//	Elemente pentru matricea de proiectie;
float xMin = -325.f,   xMax = 325.f, yMin = -325.f, yMax = 325.f,
	  zNear = 100.f,   zFar = 500.f,
	  width = 650.f, height = 650.f,  fov = 90.f * PI / 180;


void ProcessNormalKeys(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'l':			//	Apasarea tastelor `l` si `r` modifica pozitia verticalei in planul de vizualizare;
		vX += 0.1;
		break;
	case 'r':
		vX -= 0.1;
		break;
	case '+':			//	Apasarea tastelor `+` si `-` schimba pozitia observatorului (se departeaza / aproprie);
		obsZ += 10;
		break;
	case '-':
		obsZ -= 10;
		break;
	}
	if (key == 27)
		exit(0);
}

void ProcessSpecialKeys(int key, int xx, int yy) {
	//	Procesarea tastelor 'LEFT', 'RIGHT', 'UP', 'DOWN';
	//	duce la deplasarea observatorului pe axele Ox si Oy;

	switch (key) {				
	case GLUT_KEY_LEFT:
		obsX -= 5;
		break;
	case GLUT_KEY_RIGHT:
		obsX += 5;
		break;
	case GLUT_KEY_UP:
		obsY += 5;
		break;
	case GLUT_KEY_DOWN:
		obsY -= 5;
		break;
	}
}


//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void) {
	ProgramId = LoadShaders("07_03_Shader.vert", "07_03_Shader.frag");
	glUseProgram(ProgramId);
}


//  Se initializeaza un vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void) {
	GLfloat Vertices[] = {
		//  Coordonatele bazei
		//    Coordonate              |      Culori		
		-75.0f, -75.0f, -75.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 75.0f, -75.0f, -75.0f, 1.0f,   0.5f, 0.1f, 0.9f,
		 75.0f,  75.0f, -75.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		-75.0f,  75.0f, -75.0f, 1.0f,   0.9f, 0.1f, 0.7f,

		//  Coordonatele varfului
		//        Coordonate          |      Culori
		  0.0f,   0.0f,  45.0f, 1.0f,   1.0f, 1.0f, 1.0f,
	};


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

	//  Se creeaza un buffer pentru VARFURI - COORDONATE si CULORI;
	glGenBuffers(1, &VboId);														//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);											//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);									//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
}


//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void) {
	glDeleteProgram(ProgramId);
}


//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void) {
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

	//  Eliberaea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}


//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void) {
	DestroyShaders();
	DestroyVBO();
}


//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului;
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();							//  Initilizarea shaderelor;

	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
}


//	Functia de desenare a graficii pe ecran;
void RenderFunction(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//  Se curata ecranul OpenGL pentru a fi desenat noul continut (bufferul de culoare & adancime);
	glEnable(GL_DEPTH_TEST);

	//	CreateVBO(); //	 Decomentati acest rand daca este cazul;
	glBindVertexArray(VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

	//	Matricea de vizualizare;
	//	Pozitia observatorului;
	obs = glm::vec3(obsX, obsY, obsZ); 	
	//	Pozitia punctului de referinta;
	refX = obsX; refY = obsY;
	pctRef = glm::vec3(refX, refY, refZ);
	//	Verticala din planul de vizualizare; 
	vert = glm::vec3(vX, 1.0f, 0.0f);
	view = glm::lookAt(obs, pctRef, vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);


	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), zNear); 
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);


	codCol = 0;
	glUniform1i(codColLocation, codCol);

	//  Desenarea bazei
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, (void*)(0));

	//	Desenarea fetelor;
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, (void*)(4));

	//  Desenarea muchiilor;
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glLineWidth(3.5);

	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(16));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_BYTE, (void*)(20));

	glutSwapBuffers();	//	Inlocuieste imaginea deseneata in fereastra cu cea randata; 
	glFlush();			//  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}


int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);	//	Se folosesc 2 buffere pentru desen (unul pentru afisare si unul pentru randare => animatii cursive) si culori RGB + 1 buffer pentru adancime;
	glutInitWindowSize(winWidth, winHeight);					//  Dimensiunea ferestrei;
	glutInitWindowPosition(30, 30);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Cerinta 2");								//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	glewInit();

	Initialize();												//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);							//  Desenarea scenei in fereastra;
	glutIdleFunc(RenderFunction);								//	Asigura rularea continua a randarii;
	glutKeyboardFunc(ProcessNormalKeys);						//	Functii ce proceseaza inputul de la tastatura utilizatorului;
	glutSpecialFunc(ProcessSpecialKeys);
	glutCloseFunc(Cleanup);										//  Eliberarea resurselor alocate de program;

	glutMainLoop();
	return 0;
}
