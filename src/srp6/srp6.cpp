#include"srp6.h"

using namespace std;

SRP6::SRP6()
{
	_g = BN_new();
	_N = BN_new();
	BN_dec2bn(&_g, "7");
	BN_hex2bn(&_N, "C11F89C553CED9CED2E07F800B64CD775256E9039768FF75828692A3F99238C5");

	BIGNUM* s = BN_new();
	BN_rand(s, 256, 1, 1);
	unsigned char to[32] = { '0' };
	BN_bn2bin(s, to);
}

SRP6::~SRP6()
{
	if (_g != nullptr)
	{
		BN_free(_g);
		_g = nullptr;
	}
	if (_N != nullptr)
	{
		BN_free(_N);
		_N = nullptr;
	}
}

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

string SRP6::GetRandSalt()
{
	BIGNUM* s = BN_new();
	BN_rand(s, 256, 1, 1);
	unsigned char to[32] = { 0 };
	BN_bn2bin(s, to);
	string salt = "";
	for (int i = 0; i < 32; i++)
	{
		salt += to[i];
	}
	return salt;
}

string SRP6::GetBigNum()
{
	BIGNUM* A = BN_new();
	BN_rand(A, 128, 1, 1);
	return BN_bn2hex(A);
}

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