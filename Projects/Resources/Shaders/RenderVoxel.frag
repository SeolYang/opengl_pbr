#version 450 core
in vec4 fragColor;

out vec4 color;

void main() {
	if(fragColor.a < 0.1f)
		discard;

	color = vec4(fragColor.rgb, 1.0f);
	color.xyz = color.xyz/(color.xyz+vec3(1.0));
	color.xyz = pow(color.xyz, vec3(1.0/2.2));
}