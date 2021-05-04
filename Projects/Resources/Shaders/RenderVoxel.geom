#version 450 core
layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

in vec4 color[];

out vec4 fragColor;

void main() {
	fragColor = color[0];

    float scale = 0.5f;
	vec4 v1 = projectionMatrix * modelViewMatrix * (gl_in[0].gl_Position + vec4(-scale, scale, scale, 0));
	vec4 v2 = projectionMatrix * modelViewMatrix * (gl_in[0].gl_Position + vec4(scale, scale, scale, 0));
	vec4 v3 = projectionMatrix * modelViewMatrix * (gl_in[0].gl_Position + vec4(-scale, -scale, scale, 0));
	vec4 v4 = projectionMatrix * modelViewMatrix * (gl_in[0].gl_Position + vec4(scale, -scale, scale, 0));
	vec4 v5 = projectionMatrix * modelViewMatrix * (gl_in[0].gl_Position + vec4(-scale, scale, -scale, 0));
	vec4 v6 = projectionMatrix * modelViewMatrix * (gl_in[0].gl_Position + vec4(scale, scale, -scale, 0));
	vec4 v7 = projectionMatrix * modelViewMatrix * (gl_in[0].gl_Position + vec4(-scale, -scale, -scale, 0));
	vec4 v8 = projectionMatrix * modelViewMatrix * (gl_in[0].gl_Position + vec4(scale, -scale, -scale, 0));

	// +Z
    gl_Position = v1;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
	gl_Position = v4;
    EmitVertex();
    EndPrimitive();
    gl_Position = v1;
    EmitVertex();
    gl_Position = v4;
    EmitVertex();
	gl_Position = v2;
    EmitVertex();
    EndPrimitive();

    // -Z
    gl_Position = v6;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v7;
    EmitVertex();
    EndPrimitive();
    gl_Position = v6;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v5;
    EmitVertex();
    EndPrimitive();

    // +X
    gl_Position = v2;
    EmitVertex();
    gl_Position = v4;
    EmitVertex();
	gl_Position = v8;
    EmitVertex();
    EndPrimitive();
    gl_Position = v2;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v6;
    EmitVertex();
    EndPrimitive();

    // -X
    gl_Position = v5;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v3;
    EmitVertex();
    EndPrimitive();
    gl_Position = v5;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
	gl_Position = v1;
    EmitVertex();
    EndPrimitive();

    // +Y
    gl_Position = v5;
    EmitVertex();
    gl_Position = v1;
    EmitVertex();
	gl_Position = v2;
    EmitVertex();
    EndPrimitive();
    gl_Position = v5;
    EmitVertex();
    gl_Position = v2;
    EmitVertex();
	gl_Position = v6;
    EmitVertex();
    EndPrimitive();

    // -Y
    gl_Position = v3;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v8;
    EmitVertex();
    EndPrimitive();
    gl_Position = v3;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v4;
    EmitVertex();
    EndPrimitive();
}