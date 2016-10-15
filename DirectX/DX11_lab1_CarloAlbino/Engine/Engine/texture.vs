// GLOBALS //
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// TYPEDEFS //
struct VertexInputType
{
    float2 position : POSITION;
    float2 size : SIZE;
	float4 color : COLOR;
	float4 source : SOURCE;
};

struct PixelInputType
{
    float2 position : POSITION;
    float2 size : SIZE;
	float4 color : COLOR;
	float4 source : SOURCE;
};

// Vertex Shader
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;

    // Calculate the position of the vertex against the world, view, and projection matrices.
   	output.position = input.position;
	output.size = input.size;
	output.color = input.color;
	output.source = input.source;
    
    return output;
}