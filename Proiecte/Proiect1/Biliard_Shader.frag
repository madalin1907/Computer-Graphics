// ================================================
// |            Grafica pe calculator             |
// ================================================
// |          PROIECT 1 - Joc de Biliard          |
// ================================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;

#version 330 core

//	Variabile de intrare (dinspre Shader.vert);
in vec4 ex_Color;
in vec2 tex_Coord;

//	Variabile de iesire	(spre programul principal);
out vec4 out_Color;		//	Culoarea actualizata;

// Variabile uniforme
uniform sampler2D myTexture;
uniform int drawCode;

void main(void)
{
	switch (drawCode)
	{
	case 0:
		out_Color = ex_Color;
		break;
	case 1:
		out_Color = texture(myTexture, tex_Coord);
		break;
	}
}
