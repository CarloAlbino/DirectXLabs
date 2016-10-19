#include "bitmapclass.h"

BitmapClass::BitmapClass()
{
	m_guiVertexBuffer = 0;
	m_Texture = 0;
}

BitmapClass::BitmapClass(const BitmapClass& other)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Initialize the vertex buffer.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for GUI
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}

ID3D11ShaderResourceView * BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC guiVertexBufferDesc;
	HRESULT result;

	guiVertexBufferDesc.ByteWidth = sizeof(GuiElementVertexData);
	guiVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	guiVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	guiVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	guiVertexBufferDesc.MiscFlags = 0; 
	guiVertexBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&guiVertexBufferDesc, 0, &m_guiVertexBuffer);
	if (result != S_OK)
	{
		return result;
	}

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	// Release the vertex buffer.
	if (m_guiVertexBuffer)
	{
		m_guiVertexBuffer->Release();
		m_guiVertexBuffer = 0;
	}

	return;
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	GuiElementVertexData* guiVertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	GuiElementVertexData* guiVerticesPtr;
	HRESULT result;

	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// Create the vertex array.
	guiVertices = new GuiElementVertexData();
	if (!guiVertices)
	{
		return false;
	}

	guiVertices->color = D3DXVECTOR4(1.0f, 1.f, 1.f, 1.f);
	guiVertices->size = D3DXVECTOR2((float)m_bitmapWidth / m_screenWidth * 2.f, (float)m_bitmapHeight / m_screenHeight * 2.f);
	guiVertices->position = D3DXVECTOR2(2.f * 50.f / m_screenWidth - 1.f, 1.f - 2.f * 50.f / m_screenHeight);
	guiVertices->source = D3DXVECTOR4(0.f, 1.f, 1.f, 0.f);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_guiVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	guiVerticesPtr = (GuiElementVertexData*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(guiVerticesPtr, (void*)guiVertices, (sizeof(GuiElementVertexData)));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_guiVertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete guiVertices;
	guiVertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(GuiElementVertexData);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_guiVertexBuffer, &stride, &offset);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
