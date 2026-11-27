#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
	vec4 color = texture(ourTexture, TexCoord);
	// Pixels transparentes dos sprites não desenham (nem escrevem profundidade).
	if (color.a < 0.5)
		discard;
	FragColor = color;

	//pra ficar em arco iris
	//FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
}
