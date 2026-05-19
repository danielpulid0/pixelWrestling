#version 110
uniform float time;
void main()
{
    // Color base que tenga el shape/sprite que estamos dibujando
    vec4 base_color = gl_Color;
    
    // Crear un efecto de pulso usando el tiempo
    float pulse = (sin(time * 10.0) + 1.0) * 0.5; // va de 0.0 a 1.0
    
    // Mezclar el color base con blanco/brillo
    vec3 glow = mix(base_color.rgb, vec3(1.0, 1.0, 1.0), pulse * 0.5);
    
    gl_FragColor = vec4(glow, base_color.a);
}
