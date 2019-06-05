#pragma once

class Viewport
{
public:
	Viewport(
		unsigned int width = 800, unsigned int height = 600,
		unsigned int x = 0, unsigned y = 0) :
		m_x(x), m_y(y),
		m_width(width), m_height(height)
	{
	}

	void SetX(unsigned int x) { m_x = x; }
	unsigned int GetX() const { return m_x; }

	void SetY(unsigned int y) { m_y = y; }
	unsigned int GetY() const { return m_y; }

	void SetWidth(unsigned int width) { m_width = width; }
	unsigned int GetWidth() const { return m_width; }

	void SetHeight(unsigned int height) { m_height = height; }
	unsigned int GetHeight() const { return m_height; }

	float GetAspectRatio() const { return (float)m_width / (float)m_height; }

	void Bind();

private:
	unsigned int m_x;
	unsigned int m_y;
	unsigned int m_width;
	unsigned int m_height;

};
