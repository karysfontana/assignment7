#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform mat4 modelview;
uniform mat3 normalmatrix;
uniform mat4 projection;

out vec3 NormalEye;
out vec3 FragPosEye;

void main() {
    FragPosEye = vec3(modelview * vec4(vPosition, 1.0));
    NormalEye = normalmatrix * vNormal;       
    gl_Position = projection * vec4(FragPosEye, 1.0);
}
