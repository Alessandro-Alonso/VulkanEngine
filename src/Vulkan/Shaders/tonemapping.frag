#version 450

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D inputImage;

vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() 
{
    // Obtenemos cordenadas exactas en pixeles
    ivec2 texCoord = ivec2(gl_FragCoord.xy);
    
    // Cargamos el color hdr. texelFetch es mas rapido i nitido para el mapeo 1:1 que texture()
    vec3 color = texelFetch(inputImage, texCoord, 0).rgb;

    // Aplicar mapeo de tonos (HDR -> SDR)
    color = aces(color);

    // Correccion gamma (lineal -> sRGB) La mayoria de los monitores esperan sRGB (potencia aproximada 1/2,2)
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, 1.0);
}