#version 450 core
layout(points) in;
layout(line_strip, max_vertices = 24) out;

in vec3 worldPosGeom[];

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 boundingBoxMin;
uniform vec3 boundingBoxMax;

void main()
{
    vec4 v0 = projMatrix * viewMatrix * worldMatrix * vec4(boundingBoxMin, 1.0f);
    vec4 v1 = projMatrix * viewMatrix * worldMatrix * vec4(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z, 1.0f);
    vec4 v2 = projMatrix * viewMatrix * worldMatrix * vec4(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z, 1.0f);
    vec4 v3 = projMatrix * viewMatrix * worldMatrix * vec4(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z, 1.0f);
    vec4 v4 = projMatrix * viewMatrix * worldMatrix * vec4(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z, 1.0f);
    vec4 v5 = projMatrix * viewMatrix * worldMatrix * vec4(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z, 1.0f);
    vec4 v6 = projMatrix * viewMatrix * worldMatrix * vec4(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z, 1.0f);
    vec4 v7 = projMatrix * viewMatrix * worldMatrix * vec4(boundingBoxMax, 1.0f);

    gl_Position = v0;
    EmitVertex();
    gl_Position = v1;
    EmitVertex();
    EndPrimitive();

    gl_Position = v0;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
    EndPrimitive();

    gl_Position = v1;
    EmitVertex();
    gl_Position = v2;
    EmitVertex();
    EndPrimitive();

    gl_Position = v2;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
    EndPrimitive();

    gl_Position = v1;
    EmitVertex();
    gl_Position = v4;
    EmitVertex();
    EndPrimitive();

    gl_Position = v0;
    EmitVertex();
    gl_Position = v5;
    EmitVertex();
    EndPrimitive();

    gl_Position = v4;
    EmitVertex();
    gl_Position = v5;
    EmitVertex();
    EndPrimitive();

    gl_Position = v4;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
    EndPrimitive();

    gl_Position = v5;
    EmitVertex();
    gl_Position = v6;
    EmitVertex();
    EndPrimitive();

    gl_Position = v6;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
    EndPrimitive();

    gl_Position = v2;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
    EndPrimitive();

    gl_Position = v3;
    EmitVertex();
    gl_Position = v6;
    EmitVertex();
    EndPrimitive();
}