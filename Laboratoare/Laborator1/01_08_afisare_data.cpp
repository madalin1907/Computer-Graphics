//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul I - 01_02_varfuri_triunghi.cpp |
// ==============================================
//
//  Biblioteci

#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
                            //	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
                            //  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
                            //  - crearea de meniuri si submeniuri;

//  Identificatorii obiectelor de tip OpenGL;
GLuint
VaoId,
VboId,
ColorBufferId,
VertexShaderId,
FragmentShaderId,
ProgramId;

//  Accesarea shaderelor - definite ca siruri de caractere;
//  Pentru moment shaderele nu efectueaza schimbari, pastreaza coordonatele si culorile definite in program;

//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
const GLchar* VertexShader =
{
      "#version 400\n"\

      "layout(location=0) in vec4 in_Position;\n"\
      "layout(location=1) in vec4 in_Color;\n"\
      "out vec4 ex_Color;\n"\

      "void main(void)\n"\
      "{\n"\
      "  gl_Position = in_Position;\n"\
      "  ex_Color = in_Color;\n"\
      "}\n"
};

//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
const GLchar* FragmentShader =
{
      "#version 400\n"\

      "in vec4 ex_Color;\n"\
      "out vec4 out_Color;\n"\

      "void main(void)\n"\
      "{\n"\
      "  out_Color = ex_Color;\n"\
      "}\n"
};

//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
    //  Coordonatele varfurilor;
    GLfloat Vertices[] = {
        // cifra 0
        -0.95f, -0.3f, 0.0f, 1.0f,
        -0.95f,  0.3f, 0.0f, 1.0f,
		-0.80f,  0.3f, 0.0f, 1.0f,
        -0.80f, -0.3f, 0.0f, 1.0f,

        // cifra 8
        -0.75f,  0.0f, 0.0f, 1.0f,
        -0.75f,  0.3f, 0.0f, 1.0f,
        -0.60f,  0.3f, 0.0f, 1.0f,
        -0.60f,  0.0f, 0.0f, 1.0f,

        -0.60f, -0.3f, 0.0f, 1.0f,
        -0.75f, -0.3f, 0.0f, 1.0f,
        -0.75f,  0.0f, 0.0f, 1.0f,

        // simbolul /
        -0.55f, -0.5f, 0.0f, 1.0f,
        -0.50f,  0.5f, 0.0f, 1.0f,

        // cifra 1
        -0.45f, -0.3f, 0.0f, 1.0f,
        -0.45f,  0.3f, 0.0f, 1.0f,
           
        // cifra 0
        -0.40f, -0.3f, 0.0f, 1.0f,
        -0.40f,  0.3f, 0.0f, 1.0f,
        -0.25f,  0.3f, 0.0f, 1.0f,
        -0.25f, -0.3f, 0.0f, 1.0f,

        // simbolul /
        -0.20f, -0.5f, 0.0f, 1.0f,
        -0.15f,  0.5f, 0.0f, 1.0f,

        // cifra 2
        -0.10f,  0.3f, 0.0f, 1.0f,
         0.05f,  0.3f, 0.0f, 1.0f,
         0.05f,  0.0f, 0.0f, 1.0f,
        -0.10f,  0.0f, 0.0f, 1.0f,
        -0.10f, -0.3f, 0.0f, 1.0f,
         0.05f, -0.3f, 0.0f, 1.0f,

        // cifra 0
         0.10f, -0.3f, 0.0f, 1.0f,
		 0.10f,  0.3f, 0.0f, 1.0f,
		 0.25f,  0.3f, 0.0f, 1.0f,
		 0.25f, -0.3f, 0.0f, 1.0f,

        // cifra 2
         0.30f,  0.3f, 0.0f, 1.0f,
		 0.45f,  0.3f, 0.0f, 1.0f,
		 0.45f,  0.0f, 0.0f, 1.0f,
		 0.30f,  0.0f, 0.0f, 1.0f,
		 0.30f, -0.3f, 0.0f, 1.0f,
		 0.45f, -0.3f, 0.0f, 1.0f,

        // cifra 3
         0.50f,  0.3f, 0.0f, 1.0f,
         0.65f,  0.3f, 0.0f, 1.0f,
         0.65f,  0.0f, 0.0f, 1.0f,
         0.50f,  0.0f, 0.0f, 1.0f,

         0.65f,  0.0f, 0.0f, 1.0f,
         0.65f, -0.3f, 0.0f, 1.0f,
         0.50f, -0.3f, 0.0f, 1.0f

    };

    //  Culorile in spectrul RGB ca atribute ale varfurilor;
    GLfloat Colors[] = {
        // red and black
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,        
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
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
void CreateShaders(void)
{
    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
    glCompileShader(VertexShaderId);

    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
    glCompileShader(FragmentShaderId);

    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);
    glUseProgram(ProgramId);
}

//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
    glUseProgram(0);

    glDetachShader(ProgramId, VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);

    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);

    glDeleteProgram(ProgramId);
}

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);   //  Culoarea de fond a ecranului;
    CreateVBO();                            //  Trecerea datelor de randare spre bufferul folosit de shadere;
    CreateShaders();                        //  Initilizarea shaderelor;
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
    glClear(GL_COLOR_BUFFER_BIT);       //  Se curata ecranul OpenGL pentru a fi desenat noul continut;

    glLineWidth(5.0f);

    glDrawArrays(GL_LINE_LOOP, 0, 4); // cifra 0
    glDrawArrays(GL_LINE_LOOP, 4, 4); // cifra 8 (partea de sus)
    glDrawArrays(GL_LINE_STRIP, 7, 4); // cifra 8 (partea de jos)

    glDrawArrays(GL_LINES, 11, 2); // simbolul /

    glDrawArrays(GL_LINES, 13, 2); // cifra 1
    glDrawArrays(GL_LINE_LOOP, 15, 4); // cifra 0

    glDrawArrays(GL_LINES, 19, 2); // simbolul /

    glDrawArrays(GL_LINE_STRIP, 21, 6); // cifra 2
    glDrawArrays(GL_LINE_LOOP, 27, 4); // cifra 0
    glDrawArrays(GL_LINE_STRIP, 31, 6); // cifra 2
    glDrawArrays(GL_LINE_STRIP, 37, 4); // cifra 3 (partea de sus)
    glDrawArrays(GL_LINE_STRIP, 41, 3); // cifra 3 (partea de jos)


    glFlush();                        //  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
    DestroyShaders();
    DestroyVBO();
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
    //  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);             //	 Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
    glutInitWindowPosition(25, 25);						//  Pozitia initiala a ferestrei;
    glutInitWindowSize(1400, 700);							//  Dimensiunea ferestrei;
    glutCreateWindow("Afisare data");   //	 Creeaza fereastra de vizualizare, indicand numele acesteia;

    //	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
    //  Trebuie initializat inainte de desenare;

    glewInit();

    Initialize();                      //  Setarea parametrilor necesari pentru afisare;
    glutDisplayFunc(RenderFunction);    //  Desenarea scenei in fereastra;
    glutCloseFunc(Cleanup);             //  Eliberarea resurselor alocate de program;

    //  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
    //  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

    glutMainLoop();

    return 0;
}