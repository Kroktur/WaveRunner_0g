#include "Core/TextComponent.h"
#include "VulkanCore.h"
#include "Core/Vertex.h"

void Text::SetText(const std::string& text)
{
	m_message.data = text;
	for (auto d : dirty)
	{
		d = true;
	}
	m_message.isDirty = true;
}

void Text::SetAlign(const Align& align)
{
	m_align = align;
	for (auto d : dirty)
	{
		d = true;
	}
	m_message.isDirty = true;
}

std::string Text::GetText() const
{
	return  m_message.data;
}

Text::Align Text::GetAlign() const
{
	return m_align;
}

void Text::Bind(const vk::raii::CommandBuffer* buffer, int frameId)
{
	buffer->bindVertexBuffers(0, *m_buffers[frameId].m_vertexBuffer.Get(), { 0 });
	buffer->bindIndexBuffer(*m_buffers[frameId].m_indexBuffer.Get(), 0, vk::IndexType::eUint32);
}

float Text::Offset(const Align& align, float totalAdvance, float currentAdvance)
{
	switch (align)
	{
	case  Align::Left:
		return 0.0f;
	case Align::Center:
		return (totalAdvance - currentAdvance) / 2.0f ;
	case Align::Right:
		return (totalAdvance - currentAdvance) ;
	 default:
		throw std::out_of_range("case not handle");
	}
}

void Text::Upload(KGR::_Vulkan::VulkanCore* core , int frameId)
{

	if (m_buffers.size() > frameId && !dirty[frameId])
		return;


	// if the message is dirty clear the buffer and let's calculate
	if (m_buffers.size() <= frameId )
	{
		m_buffers.resize(frameId + 1 );
		dirty.resize(frameId  + 1, true);
		
	}
	else if (m_buffers[frameId].m_vertexBuffer.GetSize() != 0)
	{
		m_buffers[frameId].m_vertexBuffer.Get().clear();
		m_buffers[frameId].m_indexBuffer.Get().clear();
	}
	dirty[frameId] = false;

	// create the temp vector for vertices and index
	std::vector<Vertex2D> vertices;
	std::vector<uint32_t> indices;

	// create the vector of screen ( to support \n )
	std::vector<std::string> splitMessage;
	// and push a first string in it
	splitMessage.emplace_back();
	// for all letter 
	for (auto& m :m_message.data)
	{
		// if it's not a \n just add the char to the last string
		if (m != '\n')
			splitMessage.back().push_back(m);
		// else add a new string to the vector
		else 
			splitMessage.emplace_back();
	}
	// calculate here the advance total of the text base on pixel size
	float totalAdvance = 0.0f;
	std::vector<float> advances;
	for (auto& str: splitMessage)
	{
		// create the local one 
		advances.emplace_back(0.0f);
		for (auto& c : str)
		{
			advances.back() += font->GetGlyph(c).advance;
		}
		// and compare it to the global one to store only the longest 
		totalAdvance = std::max(totalAdvance, advances.back());
	}

	// to match with the ui wee need a square of 1 by 1 that start with -0.5 and end with 0.5

	// this is the scale for the x to go from pixel to NDC
	float scaleX = 1.0f / totalAdvance;
	// this is the scale for the y to go from pixel to NDC
	float scaleY = 1.0f / (static_cast<float>(std::abs(font->GetAscent()) + font->GetDescent()));
	// divide this by the amount of line of the text
	scaleY /= static_cast<float>(splitMessage.size());

	// this is the index to fill the indexBuffer
	uint32_t indexOffset = 0;
	// the number of line 
	float lineNumber = static_cast<float>(splitMessage.size());
	// the scale for the line 
	float lineOffset = 1.0f / lineNumber;


	// now the tricky part the offest.y of a glyph in pixel return a negative value so we need to add
	// this to the bottom of our quad to get the top y 
	// so with that info we now that we need to calculate the bottom of the first line to start the text 
	float startY = 0.5f - (lineNumber - 1) * lineOffset;

	for (size_t row = 0; row < static_cast<size_t>(lineNumber); ++row)
	{
		// the x start to the left of the square 
		float startX = -0.5f;
		for (size_t col = 0; col < splitMessage[row].size(); ++col)
		{
			// get the glyph
			auto glyph = font->GetGlyph(splitMessage[row][col]);

			// calculate the start on x
			float offset = Text::Offset(m_align, totalAdvance, advances[row]);
			float minX = startX + glyph.offset.x * scaleX + offset * scaleX;
			// same for y remind that y is negative and descent is how much letters go below the baseline
		    // we subtract it to keep line consistency while making sure the text stays inside the quad
			float minY = startY + glyph.offset.y * scaleY - font->GetDescent() * scaleY;
			// calculate the end x base on the size 
			float maxX = minX + glyph.size.x * scaleX;
			// same for y
			float maxY = minY + glyph.size.y * scaleY;

			
			// add the vertices 
			vertices.push_back({ {minX, minY},     {glyph.min.x, glyph.min.y} ,{minX + 0.5f, minY + 0.5f}});
			vertices.push_back({ {maxX, minY},     {glyph.max.x, glyph.min.y},{maxX + 0.5f, minY + 0.5f} });
			vertices.push_back({ {maxX, maxY}, {glyph.max.x, glyph.max.y} ,{maxX + 0.5f, maxY + 0.5f} });
			vertices.push_back({ {minX, maxY}, {glyph.min.x, glyph.max.y} ,{minX + 0.5f, maxY + 0.5f} });

			// add the index 
			indices.push_back(indexOffset + 0);
			indices.push_back(indexOffset + 2);
			indices.push_back(indexOffset + 1);

			indices.push_back(indexOffset + 2);
			indices.push_back(indexOffset + 0);
			indices.push_back(indexOffset + 3);
			// increment by 4 to keep coherence between indices
			indexOffset += 4;
			// advance the text to keep it clear with no big space 
			startX += glyph.advance * scaleX;
		}
		// add the offset to go on the next line 
		startY += lineOffset;
	}
	// create the buffer and set the size for draw 
	m_buffers[frameId].m_vertexBuffer = core->CreateVertexBuffer(vertices);
	m_buffers[frameId].m_indexBuffer = core->CreateIndexBuffer(indices);
	m_size = indices.size();
}

size_t Text::GetIndexSize() const 
{
	return m_size;
}
