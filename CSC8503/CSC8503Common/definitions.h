#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

//#define DEBUG_MODE

extern bool renderQuadTreeMode;

// Printing out function
template <typename T>
inline void PrintToConsole(T element, int numNewLines = NULL)
{
	std::string newLines;
	for (int i = 0; i < numNewLines; i++)
		newLines += "\n";
	std::cout << element << newLines;
}

// Thing to string function
template <typename T>
inline std::string ToString(const T& value)
{
	std::ostringstream ss;
	ss << value;
	return ss.str();
}