
#ifndef _MESSAGE_H
#define _MESSAGE_H
enum CMD
{
	CMD_LOGIN_USER,
	CMD_LOGIN_USER_RESULT,
	CMD_LOGIN_A,
	CMD_LOGIN_B,
	CMD_LOGIN_M,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_SIGIN,
	CMD_SIGIN_RESULT,
	CMD_ERROR
};

struct DataHeader 
{
	short cmd;
	short data_length;
};

struct Login_User :public DataHeader 
{
	char username[32];
	Login_User()
	{
		this->cmd = CMD::CMD_LOGIN_USER;
		this->data_length = sizeof(Login_User);
	}
};

struct Login_A :public DataHeader
{
	char A[40];
	Login_A()
	{
		this->cmd = CMD::CMD_LOGIN_A;
		this->data_length = sizeof(Login_A);
	}
};

struct Login_B :public DataHeader 
{
	char B[40];
	char salt[40];
	Login_B()
	{
		this->cmd = CMD::CMD_LOGIN_B;
		this->data_length = sizeof(Login_B);
	}
};

struct Login_M :public DataHeader 
{
	char M[32];
	Login_M()
	{
		this->cmd = CMD::CMD_LOGIN_M;
		this->data_length = sizeof(Login_M);
	}
};
/*ERROR and RESULT*/
struct Result :public DataHeader
{
	int flag;
	Result(short RESULT_TYPE)
	{
		this->cmd = RESULT_TYPE;
		this->data_length = sizeof(Result);
	}
};

struct Logout :public DataHeader
{
	char username[32];
	Logout()
	{
		this->cmd = CMD::CMD_LOGOUT;
		this->data_length = sizeof(Logout);
	}
};

struct Sigin :public DataHeader
{
	char username[32];
	char verifier[40];
	char salt[40];
	Sigin()
	{
		this->cmd = CMD::CMD_SIGIN;
		this->data_length = sizeof(Sigin);
	}
};

#endif




