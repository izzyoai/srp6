#include"srp6.h"
#include"sqlmgr.h"
#include"config.h"
#include"message.h"

using namespace std;

int _handle(SOCKET _temp_socket);

int main()
{
	/* create network service*/
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsadData;
	if (WSAStartup(sockVersion, &wsadData) != 0)
	{
		return 0;
	}
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		return 0;
	}
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8866);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	/* bind port 8866 */
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		cout << "bind error" << endl;
	}
	/* listen */
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		cout << "listen error" << endl;
		return 0;
	}

	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	while (true)
	{
		cout << "waiting for connection" << endl;
		sClient = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			cout << "accept error" << endl;
			continue;
		}
		cout << "connected " << inet_ntoa(remoteAddr.sin_addr) << endl;
		_handle(sClient);
		

	}
	closesocket(slisten);
	WSACleanup();

	return 0;
}

int _handle(SOCKET _tmp_socket)
{

	/* connect mysql */
	Config cf;
	cf.ReadFile("../config/config.conf");
	auto hostname = cf.Read("HostName");
	auto username = cf.Read("UserName");
	auto database = cf.Read("Database");
	auto port = cf.Read("Port");
	auto passwd = cf.Read("Passwd");

	SqlMGR sqlmgr;
	sqlmgr.Init(hostname, username, passwd, database, port);
	/* srp6 */
	SRP6 srp;

	/* handle data */
	DataHeader header;
	int len = recv(_tmp_socket, (char*)&header, sizeof(header), 0);
	if (len <= 0)
	{
		cout << "client exit connection" << endl;
		return 0;
	}
	
	switch (header.cmd) {
	/* rev username */
	case CMD::CMD_LOGIN_USER:
	{
		Login_User user;
		recv(_tmp_socket, (char*)&user + sizeof(DataHeader), sizeof(Login_User) - sizeof(DataHeader), 0);
		cout << "received name from client :" << user.username << endl;

		string username = user.username;
		map<string, string> usermap;

		sqlmgr.Query(SQL_QUERY_ACCOUNT + "'" + username + "'", usermap);
		/* user empty */
		if (usermap.empty())
		{
			Result res(CMD::CMD_LOGIN_USER_RESULT);
			res.flag = -1;
			send(_tmp_socket, (const char*)&res, sizeof(res), 0);
		}
		else
		{
			Result res(CMD::CMD_LOGIN_USER_RESULT);
			res.flag = 1;
			send(_tmp_socket, (const char*)&res, sizeof(res), 0);
			recv(_tmp_socket, (char*)&header, sizeof(header), 0);
			if (header.cmd == CMD::CMD_LOGIN_A)
			{
				Login_A recv_a;
				recv(_tmp_socket, (char*)&recv_a + sizeof(DataHeader), sizeof(Login_A) - sizeof(DataHeader), 0);
				string A = recv_a.A;
				cout << "recived A from client :" << A << endl;
				auto _salt = usermap.find("salt");
				string salt = _salt->second;
				if (salt.length() < 32)
				{
					for (int i = salt.length() - 1; i < 32; i++)
					{
						salt += (char)0;
					}
				}
				auto _verifier = usermap.find("verifier");
				string verifier = _verifier->second;
				if (verifier.length() < 32)
				{
					for (int i = verifier.length() - 1; i < 32; i++)
					{
						verifier += (char)0;
					}
				}
				string B = srp.GetBigNum();
				Login_B send_b;
				strcpy(send_b.B, B.c_str());
				strcpy(send_b.salt, salt.c_str());

				cout << "server send B and salt---> client" << endl;
				cout << "B:" << B << endl;
				cout << "salt:" << salt << endl;
				send(_tmp_socket, (const char*)&send_b, sizeof(send_b), 0);
				string K = srp.CalculateKey(A, B, verifier);
				string M = srp.CalculateM(A, B, K);
				cout << "K and M by A , B calculation" << endl;
				cout << "K:" << K << endl;
				cout << "M" << M << endl;

				recv(_tmp_socket, (char*)&header, sizeof(header), 0);
				
				if (header.cmd == CMD::CMD_LOGIN_M)
				{
					
					Login_M recv_m;
					recv(_tmp_socket, (char*)&recv_m + sizeof(header), sizeof(recv_m) - sizeof(header), 0);
					cout << "recived M from client :" << recv_m.M << endl;
					if (recv_m.M == M)
					{
						cout << username << "login success" << endl;
						Result success(CMD::CMD_LOGIN_USER_RESULT);
						success.flag = 1;
						send(_tmp_socket, (const char*)&success, sizeof(success), 0);
					}
				}
			}
		}

		break;
	}

	/* rev Sigin */
	case CMD::CMD_SIGIN:
	{
		Sigin sigin;
		recv(_tmp_socket, (char*)&sigin + sizeof(DataHeader), sizeof(Sigin) - sizeof(DataHeader), 0);
		cout << "sigin.username:" << sigin.username << " sigin.salt:" << sigin.salt << " sigin.verifier:" << sigin.verifier <<  endl;
		sqlmgr.Insert(SQL_INSERT_ACCOUNT + " VALUE ('" + sigin.username + "','" + sigin.salt + "','" + sigin.verifier + "')");
		break;
	}
	/* ERROR */
	default:
	{
		break;
	}
	}
	closesocket(_tmp_socket);
	return 1;
}