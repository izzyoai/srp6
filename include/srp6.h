#ifndef SRP6_H
#define SRP6_H

#include<iostream>

#include"openssl/bn.h"
#include"openssl/sha.h"

class SRP6 {

public:

	SRP6();

	~SRP6();

	std::string CalculateVerifier(std::string username, std::string password, std::string salt);

	std::string GetRandSalt();

	std::string GetBigNum();

	std::string CalculateKey(std::string A, std::string B, std::string v);

	std::string CalculateM(std::string A, std::string B, std::string K);

private:

	BIGNUM* _N;

	BIGNUM* _g;
};


#endif