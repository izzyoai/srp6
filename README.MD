# 利用srp6概念实现登录无需密码

## 1.使用openssl大数算法BN_mod_exp

利用BN_mode_exp计算verifier验证器

```c++
string SRP6::CalculateVerifier(string username, string password, string salt)
{
	auto upper = [](string s) ->string
		{
			for (int i = 0; i < s.length(); i++)
			{
				s[i] = toupper(s[i]);
			}
			return s;
		};
	string u = upper(username) + ":" + upper(password);

	//h1
	SHA_CTX ctx_h1;
	SHA1_Init(&ctx_h1);
	for (int i = 0; i < u.length(); i++)
	{
		SHA1_Update(&ctx_h1, (void*)&u[i], 1);
	}
	unsigned char h1[20] = { 0 };
	SHA1_Final(h1, &ctx_h1);

	//buf ->h1 salt
	unsigned char buf[52] = { 0 };
	for (int i = 0; i < 32; i++)
	{
		//unsigned char not char
		buf[i] = salt[i];
	}
	for (int i = 32; i < 52; i++)
	{
		buf[i] = h1[i - 32];
	}

	//h2
	SHA_CTX ctx_h2;
	SHA1_Init(&ctx_h2);
	for (int i = 0; i < 52; i++)
	{
		SHA1_Update(&ctx_h2,(void*)&buf[i],1);
	}
	unsigned char bh2[20] = { 0 };
	SHA1_Final(bh2, &ctx_h2);

	BIGNUM* h2 = BN_new();
	BN_bin2bn(bh2, 20, h2);

	//calculate
	BN_CTX* bn_ctx = BN_CTX_new();

	BIGNUM* verifier = BN_new();
	BN_mod_exp(verifier, _N, _g, h2, bn_ctx);
	unsigned char ver[32] = { 0 };
	BN_bn2bin(verifier, ver);
	string v = "";
	for (int i = 0; i < 32; i++)
	{
		v += ver[i];
	}
	return v;
}
```



## 2.利用openssl的SHA1

利用openssl的SHA1计算K与M

```c++
string SRP6::CalculateKey(string A, string B, string v)
{
	string K = "";
	BIGNUM* s = BN_new();
	BIGNUM* a = BN_new();
	BIGNUM* b = BN_new();
	
	BN_CTX* ctx = BN_CTX_new();
	BN_hex2bn(&a, A.data());
	BN_hex2bn(&b, B.data());
	BN_mod_exp(s, a, b, _g, ctx);
	unsigned char to[32] = { 0 };
	BN_bn2bin(s, to);

	SHA_CTX sha_ctx;
	SHA1_Init(&sha_ctx);

	for (int i = 0; i < 32; i++)
	{
		SHA1_Update(&sha_ctx, (void*)&to[i], 1);
	}
	for (int i = 32; i < 64; i++)
	{
		SHA1_Update(&sha_ctx, (void*)&v[i - 32], 1);
	}
	unsigned char key[20] = { 0 };
	SHA1_Final(key, &sha_ctx);
	for (int i = 0; i < 20; i++)
	{
		K += key[i];
	}
	return K;
}

string SRP6::CalculateM(string A, string B, string K)
{
	SHA_CTX sha_ctx;
	SHA1_Init(&sha_ctx);
	unsigned char buf[84] = { 0 };

	for (int i = 0; i < 20; i++)
	{
		SHA1_Update(&sha_ctx, (void*)&K[i], 1);
	}
	for (int i = 20; i < 52; i++)
	{
		SHA1_Update(&sha_ctx, (void*)&A[i-20], 1);
	}
	for (int i = 52; i < 84; i++)
	{
		SHA1_Update(&sha_ctx, (void*)&B[i - 52], 1);
	}

	unsigned char key[20] = { 0 };
	SHA1_Final(key, &sha_ctx);
	string ret = "";
	for (int i = 0; i < 20; i++)
	{
		ret += key[i];
	}
	return ret;
}
```



## 3.具体流程如下

server

```
waiting for connection
connected 127.0.0.1
Database Connected Succeed
received name from client :user
recived A from client :D9AFA8715F8EE685322CDD52F909581D
server send B and salt---> client
B:D7462DE8B66DB6F0E8F0A95DE2916031
V漋L:?櫾橯5穲␦9;j?0m参?
K and M by A , B calculation
K:?殠y濮觷犗??詝
M#~暪?汧閡K姙掭蕞€8
recived M from client :#~暪?汧閡K姙掭蕞€8
userlogin success
```

client

```
==========1.login==========
==========2.sigin==========
select:1
username:user
password:123456
client send username--->server
RESULT:CMD_LOGIN_USER_RESULT
client send A ---> server
A:D9AFA8715F8EE685322CDD52F909581D
recv B and salt
B:D7462DE8B66DB6F0E8F0A95DE2916031
Verifier by name, password and salt calculation
?yTS婠r:6;
鮁kN飇
K:?殠y濮觷犗??詝
M:#~暪?汧閡K姙掭蕞€8
client send M ---> server
login success
```



## 4.主要技术点

- 基于mysql的工具构建c++程序对表account进行增查
- 基于openssl大数的BN_mod_exp对三个数进行指模运算
- 基于openssl的SHA1对A,B,V进行加密，以及K的加密
- 基于windows网络，进行客户端和服务端的开发

## 5.不足

- 前期没有很好的构思，导致项目过程中不断的修修改改
- 网络上的很多教程借鉴了很多，却没有记住要点
- 这个项目草草收尾，没有更多想法继续去完善，比如session_key，以及修改账户密码的功能等等

## 6.谢谢观看