#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TextureCode;

uniform sampler2D texturel;
uniform sampler2D texture2;

void main()
{
	//����
	FragColor = mix( texture(texturel,TextureCode), texture(texture2,TextureCode),0.2); 
}