#version 450 core
in vec4 fragColor;

out vec4 color;

void main() {
	if(fragColor.a < 0.5f)
		discard;

	color = vec4(fragColor);
}