#pragma once


namespace dof {


template<typename Element>
class Buffer {

public:
	Buffer(unsigned int length) {
		m_buffer = new Element[length];
		m_length = length;
	}

	~Buffer() { delete [] m_buffer; }

	void Resize(unsigned int length) {
		delete [] m_buffer;
		m_buffer = new Element[length];
		m_length = length;
	}

	Element* Get() { return m_buffer; }
	const Element* Get() const { return m_buffer; }

	operator Element*() { return m_buffer; }
	operator const Element*() const { return m_buffer; }

	Element& operator[](unsigned int index) { return m_buffer[index]; }
	const Element& operator[](unsigned int index) const { return m_buffer[index]; }

	unsigned int GetLength() const { return m_length; }

private:
	Buffer(const Buffer&);
	const Buffer& operator=(const Buffer&);

	Element* m_buffer;
	unsigned int m_length;
};

}