#version 110

uniform sampler2D textura;
uniform float flash_intensity;

void main()
{
    vec4 pixeles = texture2D(textura, gl_TexCoord[0].xy);

    // solo afectar pixeles visibles (con alpha > 0)
    if(pixeles.a > 0.0)
    {
        // mezclar hacia blanco segun la intensidad del flash
        pixeles.rgb = mix(pixeles.rgb, vec3(1.0, 1.0, 1.0), flash_intensity);
    }

    gl_FragColor = pixeles;
}
