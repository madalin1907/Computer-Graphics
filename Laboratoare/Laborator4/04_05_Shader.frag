//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul IV - 04_05_Shader.frag |
// ======================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
//

#version 330 core

//	Variabile de intrare (dinspre Shader.vert);
in vec4 ex_Color;
in vec2 tex_Coord;

//	Variabile de iesire	(spre programul principal);
out vec4 out_Color;		//	Culoarea actualizata;

// Variabile uniforme
uniform int drawCode;
uniform sampler2D myTexture;

// Variabile pentru culori
vec4 green = vec4(0.0, 1.0, 0.0, 1.0);
vec4 white = vec4(1.0, 1.0, 1.0, 1.0);

void main(void)
{
	switch (drawCode)
	{
	case 0:
		out_Color = ex_Color;                        // pentru desenarea patratului initial
		break;
	case 1:
		out_Color = mix(green, white, 0.4);          // pentru desenarea patratului dupa scalare, apoi translatie
		break;
	case 2:
		out_Color = texture(myTexture, tex_Coord);   // pentru desenarea patratului dupa translatie, apoi scalare
		break;
	}
}
