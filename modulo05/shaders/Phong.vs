#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 tex_coord;
layout (location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 finalColor;
out vec3 scaledNormal;
out vec3 fragmentPos;
out vec4 vertexColor;
out vec2 texCoord;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f);
	finalColor = color;
	scaledNormal = mat3(transpose(inverse(model))) * normal;
	fragmentPos = vec3(model * vec4(position, 1.0f));
	vertexColor = vec4(color,1.0);
    texCoord = vec2(tex_coord.x, 1 - tex_coord.y);
}