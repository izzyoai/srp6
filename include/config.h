#ifndef CONFIG_H
#define CONFIG_H

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<map>


class Config {
public:
	Config();
	Config(std::string filename, std::string delimiter = "=", std::string comment = "#");
	std::string Read(const std::string& in_key) const;
	bool FileExist(std::string filename);
	void ReadFile(std::string filename, std::string delimiter = "=", std::string comment = "#");
	bool KeyExist(const std::string& in_key);
	/*check or change delimiter and comment*/
	std::string GetDelimiter();
	std::string GetComment();
	void SetDelimiter(const std::string& delimiter);
	void SetComment(const std::string& comment);
	/*write and read configuration*/
	friend std::ostream& operator<<(std::ostream& os, const Config& cf);
	friend std::istream& operator>>(std::istream& in, Config& cf);
	static void Tirm(std::string& inout_s);
	/*data*/
private:
	std::string m_Delimiter;//separator between key and value
	std::string m_Comment;//spearator between value and comments
	std::map<std::string, std::string> m_Contents;//extracted keys and values

};
#endif
