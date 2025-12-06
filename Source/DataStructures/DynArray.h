#pragma once

#include <stdint.h>

template <typename T>
class DynArray
{
public:
	DynArray() = default;

	explicit DynArray(size_t length);

	DynArray(size_t length, const T& data);

	void Fill(const T& data);
private:
	T* m_Data = nullptr;
	size_t m_Length = 0;
};