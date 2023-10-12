#ifdef GL_ES
precision medium float;
#endif

in vec2 position;
in vec3 color;
out vec3 Color;

void main()
{
	Color = color;
	gl_Position = vec4(position, 0.0f, 1.0f);
}