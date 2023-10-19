#pragma once
#include <string>
#include <regex>
class regexUtils
{
public:
	static std::string get_name_from_string(std::string s)
	{
		const int offset = 5;
		return s.substr(s.find("name:") + offset, s.find(" ") - s.find("name:") - offset);
	}

	static std::string get_password_from_string(std::string s)
	{
		const int offset = 9;
		return s.substr(s.find("password:") + offset, s.find(";") - s.find("password:") - offset);
	}
};

