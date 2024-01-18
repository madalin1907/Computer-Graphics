// ===============================================
// |            Grafica pe calculator            |
// ===============================================
// |         PROIECT 2 - Peisaj de iarna         |
// ===============================================

//	Biblioteci
#include <iostream>						//  Biblioteca standard pentru operatii de intrare/iesire (e.g. std::cout)
#include <windows.h>					//	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>						//  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <math.h>						//	Biblioteca pentru calcule matematice;
#include <GL/glew.h>					//  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>			    //	Include functii pentru: 
										//		- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
										//		- desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
										//		- crearea de meniuri si submeniuri;
#include "loadShaders.h"				//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"					//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"


//  Identificatorii obiectelor de tip OpenGL
GLuint
	VaoGround, VboGround, EboGround,
	VaoTreeTrunk, VboTreeTrunk, EboTreeTrunk,
	VaoTreeCrown, VboTreeCrown, EboTreeCrown,
	VaoTreeStar, VboTreeStar, EboTreeStar,

	ColorBufferId,
	ProgramId,
	myMatrixLocation,
	matrUmbraLocation,
	viewLocation,
	projLocation,
	matrRotlLocation,
	lightColorLocation,
	lightPosLocation,
	fogColorLocation,
	viewPosLocation,
	codColLocation;

int codCol;

// matrice utilizate
glm::mat4 myMatrix, matrRot;


// valoarea lui PI
float const PI = 3.141592f;


// elemente pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 100.0f;
float alpha = PI / 8, beta = 0.0f, dist = 450.0f;
float Obsx, Obsy, Obsz;	
float Vx = 0.0, Vy = 0.0, Vz = 1.0;
glm::mat4 view;

// elemente pentru matricea de proiectie
float width = 1280, height = 730, xwmin = -800.f, xwmax = 800, ywmin = -600, ywmax = 600, znear = 0.1, zfar = 1, fov = 45;
glm::mat4 projection;

const GLfloat winWidth = 1280.0f, winHeight = 730.0f;

// sursa de lumina
float xL = 200.0f, yL = 0.0f, zL = 400.0f;

// matricea umbrei
float matrUmbra[4][4];


// Elemente pentru reprezentarea cilindrului (trunchiul bradului)
const int NR_MERIDIANE_CILINDRU = 20, RAZA_CILINDRU = 25, INALTIME_CILINDRU = 75;

// Elemente pentru reprezentarea cilindrului (coroana bradului)
const int NR_MERIDIANE_CON = 25, RAZA_CON = 75, INALTIME_CON = 175;


void processNormalKeys(unsigned char key, int x, int y) {
	switch (key)
	{
	case '+':
		dist += 10.0;
		break;
	case '-':
		dist -= 10.0;
		break;
	case 'w':
		Refx -= 10.0;
		break;
	case 'a':
		Refy -= 10.0;
		break;
	case 's':
		Refx += 10.0;
		break;
	case 'd':
		Refy += 10.0;
		break;
	}

	if (key == 27)
		exit(0);
}


void processSpecialKeys(int key, int xx, int yy) {
	switch (key) 
	{
	case GLUT_KEY_LEFT:
		beta -= 0.03;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.03;
		break;
	case GLUT_KEY_UP:
		alpha += 0.03;
		break;
	case GLUT_KEY_DOWN:
		alpha -= 0.03;
		break;
	}
}


// GROUND
void CreateVAOGround(void) {
	GLfloat Vertices[] = {
		// GRADINA DIN STANGA
		        // coordonate               // culori		   // normale
	   -800.0f, -800.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.95f,  0.0f, 0.0f, 1.0f,		// stanga sus (mai departe de observator)
		800.0f, -800.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.95f,  0.0f, 0.0f, 1.0f,		// stanga jos (mai aproape de observator)
		800.0f, -200.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.95f,  0.0f, 0.0f, 1.0f,	    // dreapta jos (mai aproape de observator)
	   -800.0f, -200.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.95f,  0.0f, 0.0f, 1.0f, 		// dreapta sus (mai departe de observator)

	   // ALEEA DIN MIJLOC
				// coordonate               // culori		   // normale
	   -800.0f, -200.0f,  0.0f, 1.0f,  0.4f, 0.4f,  0.4f,  0.0f, 0.0f, 1.0f,		// stanga sus (mai departe de observator)
		800.0f, -200.0f,  0.0f, 1.0f,  0.4f, 0.4f,  0.4f,  0.0f, 0.0f, 1.0f,		// stanga jos (mai aproape de observator)
		800.0f,  200.0f,  0.0f, 1.0f,  0.4f, 0.4f,  0.4f,  0.0f, 0.0f, 1.0f,	    // dreapta jos (mai aproape de observator)
	   -800.0f,  200.0f,  0.0f, 1.0f,  0.4f, 0.4f,  0.4f,  0.0f, 0.0f, 1.0f, 		// dreapta sus (mai departe de observator)
		
	   // GRADINA DIN DREAPTA
				// coordonate               // culori		   // normale
	   -800.0f,  200.0f,  0.0f, 1.0f,  1.0f, 1.0f,  0.95f,  0.0f, 0.0f, 1.0f,		// stanga sus (mai departe de observator)
		800.0f,  200.0f,  0.0f, 1.0f,  1.0f, 1.0f,  0.95f,  0.0f, 0.0f, 1.0f,		// stanga jos (mai aproape de observator)
		800.0f,  800.0f,  0.0f, 1.0f,  1.0f, 1.0f,  0.95f,  0.0f, 0.0f, 1.0f,	    // dreapta jos (mai aproape de observator)
	   -800.0f,  800.0f,  0.0f, 1.0f,  1.0f, 1.0f,  0.95f,  0.0f, 0.0f, 1.0f, 		// dreapta sus (mai departe de observator)
	};

	GLubyte Indices[] = {
		 0, 1,  2,  3,  // gradina din stanga
		 4, 5,  6,  7,  // aleea din mijloc
		 8, 9, 10, 11,  // gradina din dreapta
	};

	glGenVertexArrays(1, &VaoGround);
	glBindVertexArray(VaoGround);

	glGenBuffers(1, &VboGround);
	glGenBuffers(1, &EboGround);

	glBindBuffer(GL_ARRAY_BUFFER, VboGround);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboGround);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}


// TRUNCHIUL BRADULUI
void CreateVAOTreeTrunk(void) {
	GLfloat Vertices[10 * (NR_MERIDIANE_CILINDRU * 2)];
	GLubyte Indices[6 * NR_MERIDIANE_CILINDRU];

	for (int merid = 0; merid < NR_MERIDIANE_CILINDRU; ++merid)
	{
		float u = 2 * PI * merid / NR_MERIDIANE_CILINDRU;
		float x_vf = RAZA_CILINDRU * cosf(u);
		float y_vf = RAZA_CILINDRU * sinf(u);


		// baza inferioara

		// coordonatele varfului
		Vertices[10 * merid]     = x_vf;
		Vertices[10 * merid + 1] = y_vf;
		Vertices[10 * merid + 2] = 0.0;
		Vertices[10 * merid + 3] = 1.0;

		// culoarea varfului
		Vertices[10 * merid + 4] = 0.627f;
		Vertices[10 * merid + 5] = 0.321f;
		Vertices[10 * merid + 6] = 0.168f;

		// normalele varfului
		Vertices[10 * merid + 7] = x_vf;
		Vertices[10 * merid + 8] = y_vf;
		Vertices[10 * merid + 9] = 0.0;

		// indicii pentru baza inferioara
		Indices[merid] = merid;


		// baza superioara 

		// coordonatele varfului
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid)]	   = x_vf;
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid) + 1] = y_vf;
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid) + 2] = INALTIME_CILINDRU;
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid) + 3] = 1.0;

		// culoarea varfului
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid) + 4] = 0.627f;
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid) + 5] = 0.321f;
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid) + 6] = 0.168f;

		// normalele varfului
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid) + 7] = x_vf;
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid) + 8] = y_vf;
		Vertices[10 * (NR_MERIDIANE_CILINDRU + merid) + 9] = INALTIME_CILINDRU;

		// indicii pentru baza superioara
		Indices[NR_MERIDIANE_CILINDRU + merid] = NR_MERIDIANE_CILINDRU + merid;

		// indicii pentru fetele laterale
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid] = merid;
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid + 1] = NR_MERIDIANE_CILINDRU + merid;
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid + 2] = NR_MERIDIANE_CILINDRU + (merid + 1) % NR_MERIDIANE_CILINDRU;
		Indices[2 * NR_MERIDIANE_CILINDRU + 4 * merid + 3] = (merid + 1) % NR_MERIDIANE_CILINDRU;
	};


	glGenVertexArrays(1, &VaoTreeTrunk);
	glBindVertexArray(VaoTreeTrunk);

	glGenBuffers(1, &VboTreeTrunk);
	glGenBuffers(1, &EboTreeTrunk);

	glBindBuffer(GL_ARRAY_BUFFER, VboTreeTrunk);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboTreeTrunk);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}


// COROANA BRADULUI
void CreateVAOTreeCrown(void) {
	GLfloat Vertices[10 * (NR_MERIDIANE_CON + 1)];
	GLubyte Indices[4 * NR_MERIDIANE_CON];

	// Se creeaza baza conului (un cerc)
	for (int i = 0; i < NR_MERIDIANE_CON; ++i)
	{
		float u = 2 * PI * i / NR_MERIDIANE_CON;

		float x = RAZA_CON * cos(u);
		float y = RAZA_CON * sin(u);
		float z = INALTIME_CILINDRU;

		// coordonatele varfului
		Vertices[10 * i] = x;
		Vertices[10 * i + 1] = y;
		Vertices[10 * i + 2] = z;
		Vertices[10 * i + 3] = 1.0;

		// culoarea varfului
		Vertices[10 * i + 4] = 0.0;
		Vertices[10 * i + 5] = 0.4;
		Vertices[10 * i + 6] = 0.0;

		// normalele varfului
		Vertices[10 * i + 7] = x;
		Vertices[10 * i + 8] = y;
		Vertices[10 * i + 9] = z - 50.0;

		// indicii pentru baza conului
		Indices[i] = i;
	}

	// Se creeaza varful conului

	// coordonatele varfului
	Vertices[10 * NR_MERIDIANE_CON]		= 0.0;
	Vertices[10 * NR_MERIDIANE_CON + 1] = 0.0;
	Vertices[10 * NR_MERIDIANE_CON + 2] = INALTIME_CILINDRU + INALTIME_CON;
	Vertices[10 * NR_MERIDIANE_CON + 3] = 1.0;

	// culoarea varfului
	Vertices[10 * NR_MERIDIANE_CON + 4] = 0.0;
	Vertices[10 * NR_MERIDIANE_CON + 5] = 0.6;
	Vertices[10 * NR_MERIDIANE_CON + 6] = 0.0;

	// normalele varfului
	Vertices[10 * NR_MERIDIANE_CON + 7] = 0.0;
	Vertices[10 * NR_MERIDIANE_CON + 8] = 0.0;
	Vertices[10 * NR_MERIDIANE_CON + 9] = 1.0;

	// Se creeaza indicii pentru fetele conului
	for (int i = 0; i < NR_MERIDIANE_CON; ++i)
	{
		Indices[NR_MERIDIANE_CON + 3 * i]	  = NR_MERIDIANE_CON;
		Indices[NR_MERIDIANE_CON + 3 * i + 1] = i;
		Indices[NR_MERIDIANE_CON + 3 * i + 2] = (i + 1) % NR_MERIDIANE_CON;
	}


	glGenVertexArrays(1, &VaoTreeCrown);
	glBindVertexArray(VaoTreeCrown);

	glGenBuffers(1, &VboTreeCrown);
	glGenBuffers(1, &EboTreeCrown);

	glBindBuffer(GL_ARRAY_BUFFER, VboTreeCrown);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboTreeCrown);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}



void CreateVAOTreeStar(void) {
	GLfloat Vertices[] = {
	// CERCUL MIC
		    // coordonate               // culori		      // normale
	  0.0f,   0.0f, 250.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f,   0.0f, 250.0f,		// jos	
	  0.0f,  10.0f, 258.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f,  10.0f, 258.0f,		// dreapta jos
	  0.0f,   7.0f, 269.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f,   7.0f, 269.0f,		// dreapta sus
	  0.0f,  -7.0f, 269.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f,  -7.0f, 269.0f,		// stanga sus
	  0.0f, -10.0f, 258.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f, -10.0f, 258.0f,		// stanga jos

	// CERCUL MARE
		    // coordonate               // culori		      // normale
	  0.0f,  17.0f, 240.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f,  17.0f, 240.0f,		// dreapta jos
	  0.0f,  25.0f, 269.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f,  25.0f, 269.0f,		// dreapta sus
	  0.0f,   0.0f, 285.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f,   0.0f, 285.0f, 		// sus
	  0.0f, -25.0f, 269.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f, -25.0f, 269.0f,		// stanga sus
	  0.0f, -17.0f, 240.0f, 1.0f,   1.0f, 1.0f, 0.0f,    0.0f, -17.0f, 240.0f,		// stanga jos

	// CENTRUL STELEI
		    // coordonate               // culori		      // normale
	 -7.0f,   0.0f, 260.0f, 1.0f,   1.0f, 0.3f, 0.0f,   -7.0f,   0.0f, 260.0f,		// mijloc spate
	  7.0f,   0.0f, 260.0f, 1.0f,   1.0f, 0.3f, 0.0f,    7.0f,   0.0f, 260.0f,		// mijloc fata
	};

	GLubyte Indices[] = {
		 10, 0, 5, 10, 5, 1,
		 10, 1, 6, 10, 6, 2, 
		 10, 2, 7, 10, 7, 3, 
		 10, 3, 8, 10, 8, 4,
		 10, 4, 9, 10, 9, 0,

		 11, 0, 5, 11, 5, 1,
		 11, 1, 6, 11, 6, 2,
		 11, 2, 7, 11, 7, 3,
		 11, 3, 8, 11, 8, 4,
		 11, 4, 9, 11, 9, 0,
	};

	glGenVertexArrays(1, &VaoTreeStar);
	glBindVertexArray(VaoTreeStar);

	glGenBuffers(1, &VboTreeStar);
	glGenBuffers(1, &EboTreeStar);

	glBindBuffer(GL_ARRAY_BUFFER, VboTreeStar);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboTreeStar);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}

void DestroyVBO(void) {
	//  Eliberarea atributelor din shadere (pozitie, culoare, normale etc.)
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &VboGround);
	glDeleteBuffers(1, &EboGround);

	glDeleteBuffers(1, &VboTreeTrunk);
	glDeleteBuffers(1, &EboTreeTrunk);

	glDeleteBuffers(1, &VboTreeCrown);
	glDeleteBuffers(1, &EboTreeCrown);

	glDeleteBuffers(1, &VboTreeStar);
	glDeleteBuffers(1, &EboTreeStar);


	//  Dezactivarea VAO (Vertex Array Object)
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoGround);
	glDeleteVertexArrays(1, &VaoTreeTrunk);
	glDeleteVertexArrays(1, &VaoTreeCrown);
	glDeleteVertexArrays(1, &VaoTreeStar);
}


void CreateShaders(void) {
	ProgramId = LoadShaders("Shader.vert", "Shader.frag");
	glUseProgram(ProgramId);
}


void DestroyShaders(void) {
	glDeleteProgram(ProgramId);
}


void Cleanup(void) {
	DestroyShaders();
	DestroyVBO();
}


void Initialize(void) {
	myMatrix = glm::mat4(1.0f);
	matrRot = glm::rotate(glm::mat4(1.0f), PI / 8, glm::vec3(0.0, 0.0, 1.0));
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 

	CreateVAOGround();
	CreateVAOTreeTrunk();
	CreateVAOTreeCrown();
	CreateVAOTreeStar();

	CreateShaders();

	// locatii pentru shader-e
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	matrUmbraLocation = glGetUniformLocation(ProgramId, "matrUmbra");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	lightColorLocation = glGetUniformLocation(ProgramId, "lightColor");
	lightPosLocation = glGetUniformLocation(ProgramId, "lightPos");
	viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	fogColorLocation = glGetUniformLocation(ProgramId, "fogColor");
}


void RenderFunction(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	//  pozitia observatorului
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	//  matrice de vizualizare + proiectie
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);		// se schimba pozitia observatorului	
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz);		// pozitia punctului de referinta
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz);				// verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	//  matricea pentru umbra
	float D = -3.0;
	matrUmbra[0][0] = zL + D;   matrUmbra[0][1] = 0;        matrUmbra[0][2] = 0;        matrUmbra[0][3] = 0;
	matrUmbra[1][0] = 0;        matrUmbra[1][1] = zL + D;   matrUmbra[1][2] = 0;        matrUmbra[1][3] = 0;
	matrUmbra[2][0] = -xL;      matrUmbra[2][1] = -yL;      matrUmbra[2][2] = D;        matrUmbra[2][3] = -1;
	matrUmbra[3][0] = -D * xL;  matrUmbra[3][1] = -D * yL;  matrUmbra[3][2] = -D * zL;  matrUmbra[3][3] = zL;
	glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);

	//  Variabile uniforme pentru iluminare
	glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLocation, xL, yL, zL);
	glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);

	//  culoarea pentru ceata
	glUniform3f(fogColorLocation, 0.5f, 0.5f, 0.5f);



	// --- DESENARE GROUND ---
	glBindVertexArray(VaoGround);

	codCol = 0;
	glUniform1i(codColLocation, codCol);

	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_QUADS, 12, GL_UNSIGNED_BYTE, 0);

	// --- DESENARE BRAD ---
	
	// DESENARE TRUNCHI
	glBindVertexArray(VaoTreeTrunk);

	// desenarea bazei superioare
	glDrawElements(GL_TRIANGLE_FAN, NR_MERIDIANE_CILINDRU, GL_UNSIGNED_BYTE, 0);

	// desenarea bazei inferioare
	glDrawElements(GL_TRIANGLE_FAN, NR_MERIDIANE_CILINDRU, GL_UNSIGNED_BYTE, (GLvoid*)(NR_MERIDIANE_CILINDRU * sizeof(GLubyte)));

	// desenarea fetelor laterale
	glDrawElements(GL_QUADS, 4 * NR_MERIDIANE_CILINDRU, GL_UNSIGNED_BYTE, (GLvoid*)(2 * NR_MERIDIANE_CILINDRU * sizeof(GLubyte)));

	
	// desenare umbra
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_QUADS, 4 * NR_MERIDIANE_CILINDRU, GL_UNSIGNED_BYTE, (GLvoid*)(2 * NR_MERIDIANE_CILINDRU * sizeof(GLubyte)));

	// DESENARE COROANA
	
	glBindVertexArray(VaoTreeCrown);

	codCol = 0;
	glUniform1i(codColLocation, codCol);

	// desenarea bazei
	glDrawElements(GL_TRIANGLE_FAN, NR_MERIDIANE_CON, GL_UNSIGNED_BYTE, 0);

	// desenarea fetelor laterale
	for (int i = 0; i < NR_MERIDIANE_CON; ++i)
	{
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (GLvoid*)((NR_MERIDIANE_CON + 3 * i) * sizeof(GLubyte)));
	}

	// desenare umbra
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, 3 * NR_MERIDIANE_CON, GL_UNSIGNED_BYTE, (GLvoid*)(NR_MERIDIANE_CON * sizeof(GLubyte)));


	// DESENARE STEA

	glBindVertexArray(VaoTreeStar);

	codCol = 0;
	glUniform1i(codColLocation, codCol);


	glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_BYTE, 0);
	
	// desenare umbra
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_BYTE, 0);


	glutSwapBuffers();
	glFlush();
}


int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(winWidth, winHeight);					//  Dimensiunile ferestrei de vizualizare
	glutInitWindowPosition(25, 25);								//  Pozitia initiala a ferestrei de vizualizare
	glutCreateWindow("Peisaj de iarna");						//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	glewInit();

	Initialize();
	glutDisplayFunc(RenderFunction);							//  Desenarea scenei in fereastra;
	glutIdleFunc(RenderFunction);   							//  Functie de apelare constanta 
	
	glutKeyboardFunc(processNormalKeys);                        
	glutSpecialFunc(processSpecialKeys);
	
	glutCloseFunc(Cleanup);										//  Eliberarea resurselor alocate de program;

	glutMainLoop();
	return 0;
}
