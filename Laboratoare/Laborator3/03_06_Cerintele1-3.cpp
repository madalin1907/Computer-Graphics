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

GLfloat winWidth = 600, winHeight = 700; //	Dimensiunile ferestrei de afisare;

glm::mat4 myMatrix, resizeMatrix; //	Variabile catre matricile de transformare;
glm::mat4 p1RotationMatrix, p1TranslationMatrix1, p1TranslationMatrix2; // matricile de rotatie si translatie pentru poligonul P1
glm::mat4 p2ScaleMatrix, p2TranslationMatrix1, p2TranslationMatrix2; // matricile de scalare si translatie pentru poligonul P2

int codCol; //	Variabila ce determina schimbarea culorii pixelilor in shader;

float xMin = 0, xMax = 600, yMin = 0, yMax = 700;//	Variabile pentru proiectia ortogonala;
float deltax = xMax - xMin, deltay = yMax - yMin; // lungimile laturilor dreptunghiului decupat

const float PI = 3.141592;				//	Valoarea lui PI;

//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("03_06_Shader.vert", "03_06_Shader.frag");
	glUseProgram(ProgramId);
}

//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	//  Coordonatele varfurilor;
	GLfloat Vertices[] = {
		// coordonatele dreptunghiului folosit drept fundal (colturile ferestrei)
		  0.0f,   0.0f, 0.0f, 1.0f, // stanga jos
		  0.0f, 700.0f, 0.0f, 1.0f, // stanga sus
		600.0f, 700.0f, 0.0f, 1.0f, // dreapta sus
		600.0f,   0.0f, 0.0f, 1.0f, // dreapta jos

		// coordonatele varfurilor poligonului P1 (convex)
		200.0f, 150.0f, 0.0f, 1.0f, // stanga jos
		200.0f, 275.0f, 0.0f, 1.0f, // stanga sus
		300.0f, 350.0f, 0.0f, 1.0f, // sus
		400.0f, 275.0f, 0.0f, 1.0f, // dreapta sus
		400.0f, 150.0f, 0.0f, 1.0f, // dreapta jos
		300.0f,  75.0f, 0.0f, 1.0f, // jos


		// coordonatele varfurilor poligonului P2 (concav)
		300.0f, 425.0f, 0.0f, 1.0f, // mijloc
		200.0f, 350.0f, 0.0f, 1.0f, // stanga jos
		200.0f, 525.0f, 0.0f, 1.0f, // stanga sus
		300.0f, 625.0f, 0.0f, 1.0f, // sus
		400.0f, 525.0f, 0.0f, 1.0f, // dreapta sus
		400.0f, 350.0f, 0.0f, 1.0f, // dreapta jos

		// coordonatele centrului poligonului P1
		300.0f, 212.5f, 0.0f, 1.0f,

		// coordonatele centrului poligonului P2
		300.0f, 487.5f, 0.0f, 1.0f,

		// coordonatele varfului situat intre poligoanele P1 si P2
		300.0f, 387.5f, 0.0f, 1.0f,
	};

	//	Culorile varfurilor din colturi;
	GLfloat Colors[] = {
		// culorile varfurilor dreptunghiului folosit drept fundal
		0.0f, 0.0f, 0.0f, 1.0f, // stanga jos
		0.5f, 0.5f, 0.5f, 1.0f, // stanga sus
		1.0f, 1.0f, 1.0f, 1.0f, // dreapta sus
		0.8f, 0.8f, 0.8f, 1.0f, // dreapta jos

		// culorile varfurilor poligonului P1 (convex)
		1.0f, 0.0f, 0.0f, 1.0f, // stanga jos
		0.0f, 1.0f, 0.0f, 1.0f, // stanga sus
		0.0f, 0.0f, 1.0f, 1.0f, // sus
		1.0f, 1.0f, 0.0f, 1.0f, // dreapta sus
		1.0f, 0.0f, 1.0f, 1.0f, // dreapta jos
		0.0f, 1.0f, 1.0f, 1.0f, // jos


		// culorile varfurilor poligonului P2 (concav)
		0.0f, 0.0f, 1.0f, 1.0f, // mijloc
		0.0f, 1.0f, 0.0f, 1.0f, // stanga jos
		1.0f, 1.0f, 0.0f, 1.0f, // stanga sus
		1.0f, 0.0f, 0.3f, 1.0f, // sus
		1.0f, 1.0f, 0.0f, 1.0f, // dreapta sus
		1.0f, 1.0f, 0.0f, 1.0f  // dreapta jos
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului;
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();							//  Initilizarea shaderelor;

	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);

	// coordonatele centrului poligonului P1: (300.0, 212.5)
	// coordonatele originii: (0.0, 0.0)
	// coordonatele varfului situat intre poligoanele P1 si P2: (300.0, 387.5)
	//
	// matricele de transformare pentru poligonul P1
	p1TranslationMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(-300.0f, -212.5f, 0.0f)); // translatie cu centrul in originea sistemului de coordonate
	p1RotationMatrix = glm::rotate(glm::mat4(1.0f), PI / 3, glm::vec3(0.0f, 0.0f, 1.0f)); // rotatie in jurul centrului poligonului P1
	p1TranslationMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 387.5f, 0.0f)); // translatie cu centrul in varful situat intre poligoanele P1 si P2

	// coordonatele centrului poligonului P2: (300.0, 487.5)
	// coordonatele originii: (0.0, 0.0)
	//
	// matricele de transformare pentru poligonul P2
	p2TranslationMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(-300.0f, -487.5f, 0.0f)); // translatie cu centrul in originea sistemului de coordonate
	p2ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 1.0f)); // scalare cu factorul 0.5
	p2TranslationMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 487.5f, 0.0f)); // translatie inapoi in centrul poligonului P2
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;

	//	Matrice pentru elemente "fixe" - puncte, axe;
	//	Transmiterea variabilei uniforme pentru MATRICEA DE TRANSFORMARE spre shadere;
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	codCol = 0;									//  Culoarea;
	glUniform1i(codColLocation, codCol);		//	Transmiterea variabilei uniforme pentru COLORARE spre shadere;

	glDrawArrays(GL_POLYGON, 0, 4); // desenarea dreptunghiului folosit drept background
	glDrawArrays(GL_POLYGON, 4, 6); // desenarea poligonului P1 (convex)
	glDrawArrays(GL_POLYGON, 10, 6); // desenarea poligonului P2 (concav)


	codCol = 1;
	glUniform1i(codColLocation, codCol);

	// desenarea centrelor poligoanelor P1 si P2, a varfului situat intre poligoanele P1 si P2 si a originii sistemului de coordonate
	glPointSize(3.0f);
	glDrawArrays(GL_POINTS, 16, 3);

	// desenarea poligoanelor dupa transformari
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0f);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);

	// desenarea poligonului P1 dupa transformari
	myMatrix = resizeMatrix * p1TranslationMatrix2 * p1RotationMatrix * p1TranslationMatrix1;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawArrays(GL_POLYGON, 4, 6);

	// desenarea poligonului P2 dupa transformari
	myMatrix = resizeMatrix * p2TranslationMatrix2 * p2ScaleMatrix * p2TranslationMatrix1;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawArrays(GL_POLYGON, 10, 6);

	glFlush();	//  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);					//	Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(50, 50);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Cerintele 1-3");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	glewInit();

	Initialize();						//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);	//  Desenarea scenei in fereastra;
	glutCloseFunc(Cleanup);				//  Eliberarea resurselor alocate de program;

	glutMainLoop();

	return 0;
}
