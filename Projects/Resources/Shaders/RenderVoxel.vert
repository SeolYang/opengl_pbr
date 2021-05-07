#version 450 core
out vec4 color;

uniform int volumeDim;
uniform sampler3D voxelVolume;

void main() {
	vec3 pos; // Center of voxel
	pos.x = gl_VertexID % volumeDim;
	pos.z = (gl_VertexID / volumeDim) % volumeDim;
	pos.y = gl_VertexID / (volumeDim*volumeDim);

	color = texture(voxelVolume, pos/volumeDim);
	gl_Position = vec4(pos - (volumeDim*0.5f), 1.0f);
}