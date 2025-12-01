#version 330 core

in vec3 NormalEye;
in vec3 FragPosEye;

out vec4 FragColor;

uniform vec3 materialColor;

void main() {
    vec3 L = normalize(-FragPosEye);
    float intensity = dot(NormalEye, L);
    float toonLevel;
    if (intensity > 0.8) {
        toonLevel = 0.8;
    } else if (intensity > 0.5) {
        toonLevel = 0.5;
    } else {
        toonLevel = 0.2;
    }
    vec3 color = materialColor * toonLevel;
    FragColor = vec4(color, 1.0);
}


