// ===============================================
// |            Grafica pe calculator            |
// ===============================================
// |           PROIECT 2 - Shader.frag           |
// ===============================================
//
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;

#version 330 core
 
in vec3 FragPos;  
in vec3 Normal; 
in vec3 inLightPos;
in vec3 inViewPos;
in vec3 dir;
in vec3 ex_Color; 
 
out vec4 out_Color;
 
uniform vec3 lightColor;
uniform vec3 fogColor;
uniform int codCol; 
 
void main(void) {
    // fog
    float fogDistance = length(inViewPos - FragPos);
    float fogAmount = smoothstep(500.0f, 3500.0f, fogDistance);

    // pentru codCol == 0 este aplicata iluminarea
    if (codCol == 0) {
  	    // Ambient
        float ambientStrength = 0.2f;
        vec3 ambient = ambientStrength * lightColor;
  	
        // Diffuse 
        vec3 normala = normalize(Normal);
        vec3 lightDir = normalize(inLightPos - FragPos);
        float diff = max(dot(normala, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
    
        // Specular
        float specularStrength = 0.1f;
        vec3 viewDir = normalize(inViewPos - FragPos);  //vector catre observator normalizat (V)
        vec3 reflectDir = reflect(-lightDir, normala);  // reflexia razei de lumina (R)
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
        vec3 specular = specularStrength * spec * lightColor;  
        vec3 emission = vec3(0.0, 0.0, 0.0);
        vec3 result = emission + (ambient + diffuse + specular) * ex_Color;
        
        result = mix(result, fogColor, fogAmount);  // adaugam efectul de ceata

	    out_Color = vec4(result, 1.0f);
    }

    // pentru codCol == 1 este desenata umbra
    if (codCol == 1) {
        vec3 shadowColor = vec3(0.1, 0.1, 0.1);
        
        // without fog
        out_Color = vec4(shadowColor, 1.0f);
        
        // with fog
        vec3 result = mix(shadowColor, fogColor, fogAmount);
        out_Color = vec4(result, 1.0);
    }
}
