/*
MIT License

Copyright (c) 2019 Nicolas Schneider

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef LOG_H
#define LOG_H
#include <fstream>
#include <mutex>

#define print(a, ...) Debug::Log("%s:%d		" a, __FILE__, __LINE__, __VA_ARGS__)
//#define print(a, ...) Debug::Log("%s(%s:%d) " a,  __func__,__FILE__, __LINE__, __VA_ARGS__)
#define log(a, ...) print(a "\n", __VA_ARGS__)

namespace dm
{
class Debug
{
public:
	static void Log(const std::string& string);

	template<typename... Args>
	static void Log(const std::string &format, Args &&... args)
	{
		Log(StringFormat(format, std::forward<Args>(args)...));
	}

	static std::string Print();
private:
	static std::mutex m_Mutex;
	static std::string m_Stream;

	template<typename... Args>
	static std::string StringFormat(const std::string &format, Args &&... args)
	{
		const size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // Excludes the '\0'
	}
};
}

#endif