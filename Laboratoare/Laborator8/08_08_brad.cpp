// ===============================================
// |		    Grafica pe calculator            |
// ===============================================
// |      Laboratorul VIII - 08_08_brad.cpp      |
// ===============================================

//	Biblioteci
#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <math.h>			//	Biblioteca pentru calcule matematice;
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


//  Identificatorii obiectelor de tip OpenGL;
GLuint
VaoId1, VaoId2,
VboId1, VboId2,
EboId1, EboId2,
ProgramId,
viewLocation,
projLocation,
codColLocation;

float const PI = 3.141592f;


// Elemente pentru reprezentarea cilindrului
const int NR_MERIDIANE_CILINDRU = 25, RAZA_CILINDRU = 25;

// Elemente pentru reprezentarea cilindrului
const int NR_MERIDIANE_CON = 25, RAZA_CON = 75, INALTIME_CON = 175;

// alte variabile
int codCol;

// variabile pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = 0.0f, beta = 0.0f, dist = 300.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0f, Vy = 0.0f, Vz = -1.0f;
float incr_alpha1 = 0.01f, incr_alpha2 = 0.01f;

// variabile pentru matricea de proiectie
float width = 800, height = 600, znear = 1, fov = 30;

// pentru fereastra de vizualizare 
GLint winWidth = 1000, winHeight = 600;

// vectori
glm::vec3 Obs, PctRef, Vert;

// matrice utilizate
glm::mat4 view, projection;


void ProcessNormalKeys(unsigned char key, int x, int y)
{
	switch (key) {			//	Apasarea tastelor `+` si `-` schimba pozitia observatorului (se departeaza / aproprie);
	case '-':
		dist -= 5.0;
		break;
	case '+':
		dist += 5.0;
		break;
	}
	if (key == 27)
		exit(0);
}


void ProcessSpecialKeys(int key, int xx, int yy)
{
	switch (key)						//	Procesarea tastelor 'LEFT', 'RIGHT', 'UP', 'DOWN';
	{									//	duce la deplasarea observatorului pe axele Ox si Oy;
	case GLUT_KEY_LEFT:
		beta -= 0.01f;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.01f;
		break;
	case GLUT_KEY_UP:
		alpha += incr_alpha1;
		if (abs(alpha - PI / 2) < 0.05)
		{
			incr_alpha1 = 0.f;
		}
		else
		{
			incr_alpha1 = 0.01f;
		}
		break;
	case GLUT_KEY_DOWN:
		alpha -= incr_alpha2;
		if (abs(alpha + PI / 2) < 0.05)
		{
			incr_alpha2 = 0.f;
		}
		else
		{
			incr_alpha2 = 0.01f;
		}
		break;
	}
}


//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("08_04_Shader.vert", "08_04_Shader.frag");
	glUseProgram(ProgramId);
}


void CreateVAO1(void)
{
	// CILINDRUL (TRUNCHIUL BRADULUI)
	// Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices[NR_MERIDIANE_CILINDRU * 2];
	glm::vec3 Colors[NR_MERIDIANE_CILINDRU * 2];
	GLushort Indices[NR_MERIDIANE_CILINDRU * 6];

	for (int merid = 0; merid < NR_MERIDIANE_CILINDRU; ++merid)
	{
		// implementarea reprezentarii parametrice
		float u = 2 * PI * merid / NR_MERIDIANE_CILINDRU;

		float x_vf = RAZA_CILINDRU * cosf(u);
		float y_vf = RAZA_CILINDRU * sinf(u);

		// baza inferioara
		Vertices[merid] = glm::vec4(x_vf, y_vf, -65.0, 1.0);
		Colors[merid] = glm::vec3(0.627f, 0.321f, 0.168);
		Indices[merid] = merid;

		// baza superioara 
		Vertices[NR_MERIDIANE_CILINDRU + merid] = glm::vec4(x_vf, y_vf, 0.0, 1.0);
		Colors[NR_MERIDIANE_CILINDRU + merid] = glm::vec3(0.445f, 0.197f, 0.035);
		Indices[NR_MERIDIANE_CILINDRU + merid] = NR_MERIDIANE_CILINDRU + merid;


		// indicii pentru fetele laterale
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid] = merid;
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid + 1] = NR_MERIDIANE_CILINDRU + merid;
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid + 2] = NR_MERIDIANE_CILINDRU + merid + 1;
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid + 3] = merid + 1;
	};

	Indices[6 * NR_MERIDIANE_CILINDRU - 2] = NR_MERIDIANE_CILINDRU;
	Indices[6 * NR_MERIDIANE_CILINDRU - 1] = 0;


	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId1);
	glBindVertexArray(VaoId1);
	glGenBuffers(1, &VboId1); // atribute
	glGenBuffers(1, &EboId1); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices));
}


void CreateVAO2(void)
{
	// CONUL (CRENGILE BRADULUI)
	//  Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices[NR_MERIDIANE_CON + 1];
	glm::vec3 Colors[NR_MERIDIANE_CON + 1];
	GLushort Indices[4 * NR_MERIDIANE_CON];

	// Se creeaza baza conului (un cerc)
	for (int i = 0; i < NR_MERIDIANE_CON; ++i)
	{
		float u = 2 * PI * i / NR_MERIDIANE_CON;

		float x = RAZA_CON * cos(u);
		float y = RAZA_CON * sin(u);
		float z = 0;

		Vertices[i] = glm::vec4(x, y, z, 1);
		Colors[i] = glm::vec3(0, 0.4, 0);
		Indices[i] = i; // indicii pentru baza conului
	}

	// Se creeaza varful conului
	Vertices[NR_MERIDIANE_CON] = glm::vec4(0, 0, INALTIME_CON, 1);
	Colors[NR_MERIDIANE_CON] = glm::vec3(0, 0.6, 0);

	// Se creeaza indicii pentru fetele conului
	for (int i = 0; i < NR_MERIDIANE_CON; ++i)
	{
		Indices[NR_MERIDIANE_CON + 3 * i] = NR_MERIDIANE_CON;
		Indices[NR_MERIDIANE_CON + 3 * i + 1] = i;
		Indices[NR_MERIDIANE_CON + 3 * i + 2] = (i + 1) % NR_MERIDIANE_CON;
	}


	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId2);
	glBindVertexArray(VaoId2);
	glGenBuffers(1, &VboId2); // atribute
	glGenBuffers(1, &EboId2); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices));
}


//	Schimba inaltimea/latimea scenei in functie de modificarile facute de utilizator ferestrei (redimensionari);
void ReshapeFunction(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	winWidth = newWidth;
	winHeight = newHeight;
	width = winWidth / 10, height = winHeight / 10;
}


// Elimina obiectele de tip shader dupa rulare;
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

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId1);
	glDeleteBuffers(1, &EboId1);
	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &EboId2);
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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	CreateVAO1();
	CreateVAO2();
	CreateShaders();
	viewLocation = glGetUniformLocation(ProgramId, "viewShader");
	projLocation = glGetUniformLocation(ProgramId, "projectionShader");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
}


//	Functie utilizata in setarea matricelor de vizualizare si proiectie;
void SetMVP(void)
{
	//pozitia observatorului
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	// reperul de vizualizare
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);   // se schimba pozitia observatorului	
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// matricea de proiectie 
	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);
}


//	Functia de desenare a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	SetMVP();

	codCol = 0;
	glUniform1i(codColLocation, codCol);

	// CILINDRUL
	glBindVertexArray(VaoId1);

	// desenarea bazei superioare
	glDrawElements(GL_TRIANGLE_FAN, NR_MERIDIANE_CILINDRU, GL_UNSIGNED_SHORT, 0);

	// desenarea bazei inferioare
	glDrawElements(GL_TRIANGLE_FAN, NR_MERIDIANE_CILINDRU, GL_UNSIGNED_SHORT, (GLvoid*)(NR_MERIDIANE_CILINDRU * sizeof(GLushort)));

	// desenarea fetelor laterale
	glDrawElements(GL_QUADS, 4 * NR_MERIDIANE_CILINDRU, GL_UNSIGNED_SHORT, (GLvoid*)(2 * NR_MERIDIANE_CILINDRU * sizeof(GLushort)));


	// CONUL
	glBindVertexArray(VaoId2);

	// desenarea bazei
	glDrawElements(GL_TRIANGLE_FAN, NR_MERIDIANE_CON, GL_UNSIGNED_SHORT, 0);

	// desenarea fetelor laterale
	for (int i = 0; i < NR_MERIDIANE_CON; ++i)
	{
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort) * (NR_MERIDIANE_CON + 3 * i)));
	}

	glutSwapBuffers();
	glFlush();
}


//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);							//	Se folosesc 2 buffere pentru desen (unul pentru afisare si unul pentru randare => animatii cursive) si culori RGB + 1 buffer pentru adancime;
	glutInitWindowSize(winWidth, winHeight);											//  Dimensiunea ferestrei;
	glutInitWindowPosition(100, 100);													//  Pozitia initiala a ferestrei;
	glutCreateWindow("Desenarea si survolarea unui brad");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	//	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
	//  Trebuie initializat inainte de desenare;

	glewInit();

	Initialize();							//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutReshapeFunc(ReshapeFunction);		//	Schimba inaltimea/latimea scenei in functie de modificarile facute de utilizator ferestrei (redimensionari);
	glutDisplayFunc(RenderFunction);		//  Desenarea scenei in fereastra;
	glutIdleFunc(RenderFunction);			//	Asigura rularea continua a randarii;
	glutKeyboardFunc(ProcessNormalKeys);	//	Functii ce proceseaza inputul de la tastatura utilizatorului;
	glutSpecialFunc(ProcessSpecialKeys);
	glutCloseFunc(Cleanup);					//  Eliberarea resurselor alocate de program;

	//  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
	//  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

	glutMainLoop();
	return 0;
}