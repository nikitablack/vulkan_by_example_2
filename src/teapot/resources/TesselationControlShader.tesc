#version 450

layout(push_constant) uniform TessLevel
{
	float value;
} tessLevel;

layout(vertices = 16) out;

void main()
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = tessLevel.value;
        gl_TessLevelInner[1] = tessLevel.value;

        gl_TessLevelOuter[0] = tessLevel.value;
        gl_TessLevelOuter[1] = tessLevel.value;
        gl_TessLevelOuter[2] = tessLevel.value;
        gl_TessLevelOuter[3] = tessLevel.value;
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}