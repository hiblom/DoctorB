#pragma once
#include "CppUnitTest.h"
#include "Square.h"
#include "Piece.h"
#include "Move.h"

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<Square>(const class Square& t) { 
				std::string s = t.ToString();
				return std::wstring(s.begin(), s.end());
			}
			template<> static std::wstring ToString<Piece>(const class Piece& t) {
				std::string s = t.ToString();
				return std::wstring(s.begin(), s.end());
			}
			template<> static std::wstring ToString<Move>(const class Move& t) {
				std::string s = t.ToString();
				return std::wstring(s.begin(), s.end());
			}
		}
	}
}