// ECE4122/6122 RSA Encryption/Decryption assignment
// Spring Semester 2017
// Ryan Gentry

#include <iostream>
#include "RSA_Algorithm.h"

using namespace std;

int main()
{
  // Instantiate the one and only RSA_Algorithm object
  RSA_Algorithm RSA;
  
  // Loop from sz = 32 to 1024 inclusive
  // for each size choose 10 different key pairs
  // For each key pair choose 10 differnt plaintext 
  // messages making sure it is smaller than n.
  // If not smaller then n then choose another
  // For eacm message encrypt it using the public key (n,e).
  // After encryption, decrypt the ciphertext using the private
  // key (n,d) and verify it matches the original message.

  for (size_t sz = 32; sz <=1024; sz = sz*2)
  {
    for (int i = 0; i < 100; ++i)
    {
	RSA.GenerateRandomKeyPair(sz);
	RSA.PrintNDE();
	for (int j = 0; j < 100; ++j)
	{
		mpz_class M = RSA.rng.get_z_bits(sz*2);
		while(mpz_cmp(M.get_mpz_t(),RSA.n.get_mpz_t())>=0)
		{
			M = RSA.rng.get_z_bits(sz*2);
		}
		RSA.PrintM(M);
		mpz_class C = RSA.Encrypt(M);
		RSA.PrintC(C);
		mpz_class M1 = RSA.Decrypt(C);
		if (mpz_cmp(M1.get_mpz_t(),M.get_mpz_t())!=0)
		{
			cout << "M1 " << M1 << " != M " << M << endl;
			break;
		}
	}
    }
  }
}
  
