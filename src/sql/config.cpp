#include"config.h"

using namespace std;

Config::Config()
{

}

Config::Config(string filename, string delimiter, string comment)
{
	m_Delimiter = delimiter;
	m_Comment = comment;
	FileExist(filename);
}

bool Config::FileExist(string filename)
{
	ifstream in(filename);
	if (!in)
	{
		cout << "fail to open file:"<<filename << endl;
		return false;
	}
	in >> (*this);
	return true;
}

istream& operator>>(istream& in, Config& cf)
{
	const string delim = cf.m_Delimiter;
	const string comm = cf.m_Comment;

	string str_line = "";
	while (getline(in, str_line))
	{
		//Remove contents
		auto pos = str_line.find_first_of(comm);
		string line = str_line.substr(0, pos);
		pos = line.find_last_of(delim);
		if (pos >= line.length())
		{
			continue;
		}
		auto key = line.substr(0, pos);
		auto value = line.substr(pos+1, line.length());
		Config::Tirm(key);
		Config::Tirm(value);
		cf.m_Contents.insert(pair<string, string>(key, value));
		
	}


	return in;
}

ostream& operator<<(ostream& os, const Config& cf)
{
	for (auto p = cf.m_Contents.begin(); p != cf.m_Contents.end(); p++)
	{
		os << p->first << " " << cf.m_Delimiter << " ";
		os << p->second << endl;
	}
	return os;
}

void Config::Tirm(string& inout_s)
{
	static const char whitespace[] = " \n\r\f\v\t";
	inout_s.erase(0, inout_s.find_first_not_of(whitespace));
	inout_s.erase(inout_s.find_last_not_of(whitespace) + 1U);
}

string Config::Read(const string& key) const
{
	auto p = m_Contents.find(key);
	if (p == m_Contents.end())
	{
		cout << "未匹配到key:"<< key << endl;
		return "";
	}
	return p->second;
}

void Config::ReadFile(string filename, string delimiter, string comment)
{
	m_Delimiter = delimiter;
	m_Comment = comment;
	ifstream in(filename);
	if (!in)
	{
		cout << "未能打开文件:" << filename << endl;
		return;
	}
	in >> (*this);
}

bool Config::KeyExist(const string& key)
{
	auto p = m_Contents.find(key);
	return (p != m_Contents.end());
}

string Config::GetComment()
{
	return m_Comment;
}

string Config::GetDelimiter()
{
	return m_Delimiter;
}

void Config::SetComment(const string& comment)
{
	m_Comment = comment;
}
void Config::SetDelimiter(const string& delimiter)
{
	m_Delimiter = delimiter;
}