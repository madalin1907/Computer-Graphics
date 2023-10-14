#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;

//  Identificatorii obiectelor de tip OpenGL;
GLuint
VaoId,
VboId,
ColorBufferId,
ProgramId,
codColStangaLocation,
codColDreaptaLocation;

GLint winWidth = 600, winHeight = 400;
int codColStanga = 0, codColDreapta = 1;
GLuint fata1 = GL_FRONT, fata2 = GL_BACK;

void CreateVBO(void)
{
	//  Coordonatele varfurilor;
	GLfloat Vertices[] = {
		//	Triunghiul din stanga vazut din fata (varfurile sunt in sens trigonometric)
		-0.95f, -0.9f, 0.0f, 1.0f,	 // Jos stanga;
		-0.05f, -0.9f, 0.0f, 1.0f,   // Jos dreapta;
		-0.50f,  0.1f, 0.0f, 1.0f,	 // Sus;
		//	Triunghiul din dreapta vazut din spate (varfurile sunt in sensul ceasornicului)
		 0.05f, -0.2f, 0.0f, 1.0f,   // Jos stanga;
		 0.50f,  0.8f, 0.0f, 1.0f,	 // Sus;
		 0.95f, -0.2f, 0.0f, 1.0f    // Jos dreapta;
	};

	//  Culorile in spectrul RGB ca atribute ale varfurilor;
	GLfloat Colors[] = {
		//	Triunghiul din stanga
		1.0f, 0.0f, 0.0f, 1.0f,   // jos stanga
		1.0f, 0.5f, 0.3f, 1.0f,   // jos dreapta
		1.0f, 1.0f, 0.0f, 1.0f,   // sus
		//	Triughiul din drepata
		0.0f, 1.0f, 0.0f, 1.0f,   // jos stanga
		0.0f, 1.0f, 1.0f, 1.0f,   // sus
		0.6f, 0.2f, 1.0f, 1.0f    // jos dreapta
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

//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("02_06_Shader.vert", "02_06_Shader.frag");
	glUseProgram(ProgramId);
}

//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);   //  Culoarea de fond a ecranului;
	CreateVBO();                            //  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();                        //  Initializarea shaderelor;
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;
	glLineWidth(8.0);						//  Se seteaza dimensiunea liniilor;

	glPolygonMode(fata1, GL_FILL);
	glPolygonMode(fata2, GL_LINE);

	codColStangaLocation = glGetUniformLocation(ProgramId, "codCol");
	glUniform1i(codColStangaLocation, codColStanga);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	codColDreaptaLocation = glGetUniformLocation(ProgramId, "codCol");
	glUniform1i(codColDreaptaLocation, codColDreapta);
	glDrawArrays(GL_TRIANGLES, 3, 3);

	glFlush();
}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}


void UseMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		fata1 = GL_FRONT;
		fata2 = GL_BACK;
		codColStanga = 0;
		codColDreapta = 1;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		fata1 = GL_BACK;
		fata2 = GL_FRONT;
		codColStanga = 2;
		codColDreapta = 0;
	}

	Initialize();
}


//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);				//	Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
	glutInitWindowPosition(100, 100);							//  Pozitia initiala a ferestrei;
	glutInitWindowSize(winWidth, winHeight);							 //  Dimensiunea ferestrei;
	glutCreateWindow("Cerinta 2 - triunghiuri vazute din fata si din spate");   //	Creeaza fereastra de vizualizare, indicand numele acesteia;

	//	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
	//  Trebuie initializat inainte de desenare;

	glewInit();

	Initialize();                       //  Setarea parametrilor necesari pentru afisare;
	glutDisplayFunc(RenderFunction);    //  Desenarea scenei in fereastra;
	glutCloseFunc(Cleanup);             //  Eliberarea resurselor alocate de program;
	glutMouseFunc(UseMouse);

	glutMainLoop();

	return 0;
}
