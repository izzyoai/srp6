#include"sqlmgr.h"

using namespace std;
SqlMGR::SqlMGR()
{
	_state = false;
	_mysql = new MYSQL;
	_fd = nullptr;
	memset(_field, NULL, sizeof(_field));
	_res = nullptr;
	_row = nullptr;
}
SqlMGR::~SqlMGR()
{
	mysql_close(_mysql);
	_mysql = nullptr;
	_res = nullptr;
	_row = nullptr;
	_fd = nullptr;
}

void SqlMGR::Init(string ip, string user, string pwd, string database_name, string port)
{
	if (_state)
	{
		return;
	}
	int iport = 0;
	for (int i = 0; i < port.size(); i++)
	{
		iport = 10 * iport + (port[i] - '0');
	}
	const char* iip = ip.data();
	const char* iuser = user.data();
	const char* ipwd = pwd.data();
	const char* idatabase = database_name.data();
	Connect(iip, iuser, ipwd, idatabase, iport);
}
bool SqlMGR::Connect(const char* ip, const char* user_name, const char* db_pwd, const char* database_name, const int port)
{
	if (_state == true)
	{
		cout << "Database Connected" << endl;
		return true;
	}
	mysql_init(_mysql);
	if (!(mysql_real_connect(_mysql, ip, user_name, db_pwd, database_name, port, NULL, 0)))
	{
		cout << "ERROR: error connecting to databases:" << mysql_error(_mysql) << endl;
		return false;
	}
	else
	{
		_state = true;
		cout << "Database Connected Succeed" << endl;
		return true;
	}
}

bool SqlMGR::Query(const string sql,map<string,string>& ret)
{
	if (!_state)
	{
		cout << "database not connected" << endl;
		return false;
	}
	char buf[1024];
	for (int i = 0; i < sql.length(); i++)
	{
		buf[i] = sql[i];
	}
	if (mysql_real_query(_mysql, buf, (unsigned long)sql.length()))
	{
		cout << "Query failed:" << mysql_error(_mysql) << endl;
		return false;
	}
	_res = mysql_store_result(_mysql);
	if (_res == nullptr)
	{
		cout << "Query failed" << mysql_error(_mysql) << endl;
		return false;
	}
	_row = mysql_fetch_row(_res);
	/*fields num*/
	int numFields = mysql_num_fields(_res);
	/*row length*/
	int numRow = *mysql_fetch_lengths(_res);
	if (_row == NULL)
	{
		cout << "Query data failed " << mysql_error(_mysql) << endl;
		_res = nullptr;
		return false;
	}


	for (int i = 0; i < numFields; i++)
	{
		auto value = [&]() ->string{
			if (i >= numRow)
			{
				return "";
			}
			return (string)_row[i];
			};
		ret.insert(pair<string, string>((string)mysql_fetch_field(_res)->name,value()));
	}

	mysql_free_result(_res);
	_res = nullptr;

	return true;
}

int SqlMGR::Insert(string sql)
{
	if (!_state)
	{
		cout << "Database not connected" << endl;
		return -1;
	}
	char buf[1024];
	for (int i = 0; i < sql.length(); i++)
	{
		buf[i] = sql[i];
	}
	buf[sql.length()] = '\0';
	return mysql_query(_mysql, buf);
}