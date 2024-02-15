#ifndef SQL_MGR_H
#define SQL_MGR_H

#include<string>
#include<iostream>
#include<map>
#include"mysql/mysql.h"
#include"config.h"

/*
		SQL DESCRIBE
  * table name account
  * id bigint auto_increment not null
  * user varchar(32) not null
  * salt binary(32) not null
  * verifier binary(32) not null
  * session_key binary(40)
*/

/*Return account message (sql = SELECT user,salt,verifier,session_key FROM `account` WHERE user = )*/
const static std::string SQL_QUERY_ACCOUNT = "SELECT user,salt,verifier FROM `account` WHERE user = ";
/*insert account user,salt,verifier (sql = INSERT INTO account(user,salt,verifier))*/
const static std::string SQL_INSERT_ACCOUNT = "INSERT INTO account(user,salt,verifier) ";
/*insert session_key (sql = UPDATE account SET session_key =)*/
const static std::string SQL_INSERT_SESSION_KEY_1 = "UPDATE account SET session_key = ";
/*select user (sql = WHERE user =)*/
const static std::string SQL_INSTER_SESSION_KEY_2 = "WHERE user = ";


class SqlMGR {
public:
	SqlMGR();
	~SqlMGR();
	bool Connect(const char* ip, const char* user_name, const char* db_pwd, const char* database_name, const int port);
	void Init(std::string host, std::string user, std::string pwd, std::string database_name, std::string port);
	bool Query(std::string sql,std::map<std::string,std::string>& ret);
	int Insert(std::string sql);
private:
	bool _state;
	MYSQL* _mysql;
	MYSQL_FIELD* _fd;
	char _field[32][32];
	MYSQL_RES* _res;
	MYSQL_ROW _row;
};


#endif