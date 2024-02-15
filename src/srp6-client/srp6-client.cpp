#include"srp6.h"
#include"message.h"
#include<Windows.h>

using namespace std;
int _handle(SOCKET _tmp_socket);
int main()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA Data;
	if (WSAStartup(sockVersion, &Data) != 0)
	{
		return 0;
	}
	while (true)
	{
		SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET)
		{
			cout << "invalid socked" << endl;
			return 0;
		}
		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8866);
		serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		if (connect(sclient, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{
			cout << "connect error" << endl;
			closesocket(sclient);
			return 0;
		}
		_handle(sclient);

	}
	WSACleanup();
	return 0;
}
int _handle(SOCKET _tmp_socket)
{
	SRP6 srp;
	cout << "==========1.login========== " << endl;
	cout << "==========2.sigin========== " << endl;
	cout << "select:";
	int select = 0;
	while (true)
	{
		cin >> select;
		if (select == 1)
		{
			string username;
			string password;
			cout << "username:";
			cin >> username;
			cout << "password:";
			cin >> password;
			Login_User user;
			strcpy(user.username, username.c_str());
			
			/* client send username ---> server */
			cout << "client send username--->server" << endl;
			send(_tmp_socket, (const char*)&user, sizeof(user), 0);
			DataHeader header;
			recv(_tmp_socket, (char*)&header, sizeof(header), 0);
			if (header.cmd == CMD::CMD_LOGIN_USER_RESULT)
			{
				Result res(CMD::CMD_LOGIN_USER_RESULT);
				cout << "RESULT:" << "CMD_LOGIN_USER_RESULT" << endl;
				recv(_tmp_socket, (char*)&res + sizeof(header), sizeof(res) - sizeof(header), 0);
				if (res.flag == 1)
				{
					Login_A send_A;
					string A = srp.GetBigNum();
					strcpy(send_A.A, A.c_str());
					/* client send A ---> server */
					cout << "client send A ---> server" << endl;
					cout << "A:" << A << endl;
					send(_tmp_socket, (const char*)&send_A, sizeof(send_A), 0);
					recv(_tmp_socket, (char*)&header, sizeof(header), 0);
					if (header.cmd == CMD::CMD_LOGIN_B)
					{
						Login_B recv_b;
						/* recv B and salt */
						recv(_tmp_socket, (char*)&recv_b + sizeof(header), sizeof(recv_b) - sizeof(header), 0);
						string B = recv_b.B;
						string salt = recv_b.salt;
						if (salt.length() < 32)
						{
							for (int i = salt.length() - 1; i < 32; i++)
							{
								salt += (char)0;
							}
						}

						cout << "recv B and salt" << endl;
						cout << "B:" << B << endl;

						string verifier = srp.CalculateVerifier(username, password, salt);
						cout << "Verifier by name, password and salt calculation" << endl;
						cout << "verifier:" << verifier << endl;

						string K = srp.CalculateKey(A, B, verifier);
						cout << "K:" << K << endl;

						string M = srp.CalculateM(A, B, K);
						cout << "M:" << M << endl;

						Login_M send_m;
						strcpy(send_m.M, M.c_str());
						/* client send M ---> server */
						cout << "client send M ---> server" << endl;
						send(_tmp_socket, (const char*)&send_m, sizeof(send_m), 0);
						recv(_tmp_socket, (char*)&header, sizeof(header), 0);
						if (header.cmd == CMD::CMD_LOGIN_USER_RESULT)
						{
							recv(_tmp_socket, (char*)&res + sizeof(header), sizeof(res) - sizeof(header), 0);
							if (res.flag == 1)
							{
								cout << "login success" << endl;
							}
						}
					}
				}

			}
		}
		else if (select == 2)
		{
			string username;
			string password;
			cout << "username:";
			cin >> username;
			cout << "password:";
			cin >> password;
			string salt = srp.GetRandSalt();
			string verifier = srp.CalculateVerifier(username, password, salt);
			Sigin sigin;
			strcpy(sigin.username, username.c_str());
			strcpy(sigin.salt, salt.c_str());
			strcpy(sigin.verifier, verifier.c_str());
			send(_tmp_socket, (const char*)&sigin, sizeof(sigin), 0);
		}
		break;
	}

	closesocket(_tmp_socket);
	return 1;
}
