/*

Summary:
Used to remove specified chars from string's head and tail.

Remark:
Default chars to remove are blank chars: ' ', \t, \r and \n.

Version:
0.0.1

LastUpdatedDate:
2012-06-20

*/


#pragma once
#include <string>


namespace dof {

//General purpose function.
template<typename CharType>
void Trim(std::basic_string<CharType>& str, const CharType* const blankChars) {
	str.erase(str.find_last_not_of(blankChars) + 1);
	str.erase(0, str.find_first_not_of(blankChars));
}

//Forward declaration.
template<typename CharType>
void Trim(std::basic_string<CharType>& str);

//Implementation of narrow char.
template<>
void Trim<char>(std::string& str) {
	Trim(str, " \t\r\n");
}

//Implementation of wide char.
template<>
void Trim<wchar_t>(std::wstring& str) {
	Trim(str, L" \t\r\n");
}

}