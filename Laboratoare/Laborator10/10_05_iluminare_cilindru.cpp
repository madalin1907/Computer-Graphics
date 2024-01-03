﻿//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul X - 10_03_iluminare_sfera.cpp |
// =============================================
// 
//	Program ce deseneaza o sfera care este iluminata de o sursa de lumina, folosindu - se tehnicile MODERN OpenGL;// 
//	Elemente de NOUTATE (modelul de iluminare):
//	 - coordonata Z a fiecarui varf este "perturbata" (se adauga "zgomot");
//	 - fiecare varf are asociata o normala;
//	 - implementarea modelului de iluminare  se va face in:
//		* in shaderul de FRAGMENT (se utilizeaza shaderele 10_03f*)
//			sau
//		* in shaderul de VARFURI (se utilizeaza shaderele 10_03v* - in acesta exista optiunea alegerii culorii simple, fara iluminare)
//	 - folosirea meniului glutMenu (alegerea dintre cele 4 optiuni mentionate mai sus);
//   - din programul principal sunt transferate in shaderul de varfuri toate informatiile geometrice (coordonate, normale, pozitia observatorului, pozitia sursei de lumina)
//   - are loc trasferul dinspre shderul de varfuri inspre shaderul de fragmente (ulterior efectuarii transformarilor) urmatoarele variabile:
//		* FragPos -> pozitia fragmentului;
//		* Normal -> normala;
//	    * inLightPos -> pozitia sursei de lumina;
//		* inViewPos -> pozitia observatorului;
//   - modelul de iluminare este implementat in shaderul de fragment; 
//
// 
//  
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
VaoId,
VboId,
EboId,
ProgramIdf,
ProgramIdv,
viewLocation,
projLocation,
lightColorLoc, lightPosLoc, viewPosLoc,
rendermode;


// pentru fereastra de vizualizare 
GLint winWidth = 1200, winHeight = 900;

float const PI = 3.141592f;

// Elemente pentru reprezentarea cilindrului
const int NR_MERIDIANE_CILINDRU = 25, RAZA_CILINDRU = 35;

// variabile pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = 0.0f, beta = 0.0f, dist = 300.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0f, Vy = 0.0f, Vz = -1.0f;
float incr_alpha1 = 0.01f, incr_alpha2 = 0.01f;

// variabile pentru matricea de proiectie
float width = 800, height = 600, znear = 1, fov = 30;


// vectori
glm::vec3 Obs, PctRef, Vert;

// matrice utilizate
glm::mat4 view, projection;


//	Identificatori optiuni meniu;
enum {
	Il_Frag, Il_Vert
};

//	Meniu pentru selectia optiunilor;
void Menu(int selection)
{
	rendermode = selection;
	glutPostRedisplay();
}

void ProcessNormalKeys(unsigned char key, int x, int y)
{
	switch (key) {				//	Apasarea tastelor `+` si `-` schimba pozitia observatorului (se departeaza / aproprie);
	case '-':
		dist -= 10.0;
		break;
	case '+':
		dist += 10.0;
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
		beta -= 0.05f;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.05f;
		break;
	case GLUT_KEY_UP:
		alpha += incr_alpha1;
		if (abs(alpha - PI / 2) < 0.05)
		{
			incr_alpha1 = 0.f;
		}
		else
		{
			incr_alpha1 = 0.05f;
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
			incr_alpha2 = 0.05f;
		}
		break;
	}
}

//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShadersVertex(void)
{
	ProgramIdv = LoadShaders("10_03v_Shader.vert", "10_03v_Shader.frag");
	glUseProgram(ProgramIdv);
}

void CreateShadersFragment(void)
{
	ProgramIdf = LoadShaders("10_03f_Shader.vert", "10_03f_Shader.frag");
	glUseProgram(ProgramIdf);
}

//  Se initializeaza un vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	//  Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices[NR_MERIDIANE_CILINDRU * 2];
	glm::vec3 Colors[NR_MERIDIANE_CILINDRU * 2];
	glm::vec3 Normals[NR_MERIDIANE_CILINDRU * 2];
		
	GLushort Indices[NR_MERIDIANE_CILINDRU * 6];

	for (int merid = 0; merid < NR_MERIDIANE_CILINDRU; ++merid)
	{
		// implementarea reprezentarii parametrice
		float u = 2 * PI * merid / NR_MERIDIANE_CILINDRU;
		float x_vf = RAZA_CILINDRU * cosf(u);
		float y_vf = RAZA_CILINDRU * sinf(u);

		Vertices[merid] = glm::vec4(x_vf, y_vf, -50.0, 1.0);
		Colors[merid] = glm::vec3(1.0f, 1.0f, 0.0);
		Normals[merid] = glm::vec3(x_vf, y_vf, -50.0);

		Vertices[NR_MERIDIANE_CILINDRU + merid] = glm::vec4(x_vf, y_vf, 50.0, 1.0);
		Colors[NR_MERIDIANE_CILINDRU + merid] = glm::vec3(1.0f, 0.0f, 0.0);
		Normals[NR_MERIDIANE_CILINDRU +merid] = glm::vec3(x_vf, y_vf, 50.0);

		Indices[merid] = merid; // pentru cercul de jos
		Indices[NR_MERIDIANE_CILINDRU + merid] = NR_MERIDIANE_CILINDRU + merid; // pentru cercul de sus

		// indicii pentru fetele laterale
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid] = merid;
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid + 1] = NR_MERIDIANE_CILINDRU + merid;
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid + 2] = NR_MERIDIANE_CILINDRU + merid + 1;
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid + 3] = merid + 1;
	};

	Indices[6 * NR_MERIDIANE_CILINDRU - 2] = NR_MERIDIANE_CILINDRU;
	Indices[6 * NR_MERIDIANE_CILINDRU - 1] = 0;


	// generare VAO/buffere
	glGenBuffers(1, &VboId); // atribute
	glGenBuffers(1, &EboId); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors) + sizeof(Normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), sizeof(Normals), Normals);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices));
	glEnableVertexAttribArray(2); // atributul 2 = normala
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices) + sizeof(Colors)));
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
	glDeleteProgram(ProgramIdv);
	glDeleteProgram(ProgramIdf);
}

//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);
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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // culoarea de fond a ecranului

	// Creare VBO+shader
	CreateVBO();

	CreateShadersFragment();
	// Locatii ptr shader
	lightColorLoc = glGetUniformLocation(ProgramIdv, "lightColor");
	lightPosLoc = glGetUniformLocation(ProgramIdv, "lightPos");
	viewPosLoc = glGetUniformLocation(ProgramIdv, "viewPos");
	viewLocation = glGetUniformLocation(ProgramIdv, "view");
	projLocation = glGetUniformLocation(ProgramIdv, "projection");

	CreateShadersVertex();
	// Locatii ptr shader
	lightColorLoc = glGetUniformLocation(ProgramIdv, "lightColor");
	lightPosLoc = glGetUniformLocation(ProgramIdv, "lightPos");
	viewPosLoc = glGetUniformLocation(ProgramIdv, "viewPos");
	viewLocation = glGetUniformLocation(ProgramIdv, "view");
	projLocation = glGetUniformLocation(ProgramIdv, "projection");

}

//	Functia de desenare a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// CreateVBO(); // decomentati acest rand daca este cazul 
	glBindVertexArray(VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

	//pozitia observatorului
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	// reperul de vizualizare + matricea de proiectie
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);   // se schimba pozitia observatorului	
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	// variable relevante pentru iluminare, de transferat in shader
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLoc, 150.0f, 0.0f, 0.0f);
	glUniform3f(viewPosLoc, Obsx, Obsy, Obsz);

	switch (rendermode)
	{
	case Il_Frag:
		glUseProgram(ProgramIdf);
		break;
	case Il_Vert:
		glUseProgram(ProgramIdv);
		break;
	};

	// desenarea bazei inferioare
	glDrawElements(GL_TRIANGLE_FAN, NR_MERIDIANE_CILINDRU, GL_UNSIGNED_SHORT, 0);

	// desenarea bazei superioare
	glDrawElements(GL_TRIANGLE_FAN, NR_MERIDIANE_CILINDRU, GL_UNSIGNED_SHORT, (GLvoid*)(NR_MERIDIANE_CILINDRU * sizeof(GLushort)));

	// desenarea fetelor laterale
	glDrawElements(GL_QUADS, 4 * NR_MERIDIANE_CILINDRU, GL_UNSIGNED_SHORT, (GLvoid*)(2 * NR_MERIDIANE_CILINDRU * sizeof(GLushort)));

	glutSwapBuffers();
	glFlush();
}


//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);		//	Se folosesc 2 buffere pentru desen (unul pentru afisare si unul pentru randare => animatii cursive) si culori RGB + 1 buffer pentru adancime;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(100, 100);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Iluminarea unui cilindru");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

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

	//	Initializarea meniului - activat prin click dreapta in aplicatie;
	glutCreateMenu(Menu);
	glutAddMenuEntry("Fragment", Il_Frag);
	glutAddMenuEntry("Varfuri", Il_Vert);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	//  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
	//  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

	glutMainLoop();

	return 0;
}