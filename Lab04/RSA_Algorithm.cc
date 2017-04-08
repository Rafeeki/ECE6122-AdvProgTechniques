// RSA Assignment for ECE4122/6122 Spring 2017
// Ryan Gentry

#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "RSA_Algorithm.h"

using namespace std;

// Implement the RSA_Algorithm methods here

// Constructor
RSA_Algorithm::RSA_Algorithm()
  : rng(gmp_randinit_default)
{
  // get a random seed for the random number generator
  int dr = open("/dev/random", O_RDONLY);
  if (dr < 0)
    {
      cout << "Can't open /dev/random, exiting" << endl;
      exit(0);
    }
  unsigned long drValue;
  read(dr, (char*)&drValue, sizeof(drValue));
  //cout << "drValue " << drValue << endl;
  rng.seed(drValue);
// No need to init n, d, or e.
}

// Fill in the remainder of the RSA_Algorithm methods
void RSA_Algorithm::GenerateRandomKeyPair(size_t sz)
{ // gmp_randclass rng already created in constructor
  // Select two random prime numbers p & q of size sz bits
  mpz_class p = rng.get_z_bits(sz);
  mpz_class q = rng.get_z_bits(sz);
  // Determine if p is likely to be prime, if not find new p
  while(mpz_probab_prime_p(p.get_mpz_t(),100) == 0)
  {
	p = rng.get_z_bits(sz);
  }
  // Determine if q is likely to be prime, if not find new q
  while(mpz_probab_prime_p(q.get_mpz_t(),100) == 0)
  {
	q = rng.get_z_bits(sz);
  }
  // Compute n = p*q and phi = (p-1)*(q-1)
  mpz_mul(n.get_mpz_t(),p.get_mpz_t(),q.get_mpz_t());
  mpz_class phi((p-1)*(q-1));
  // Choose a random number d of size sz*2
  d = rng.get_z_bits(sz*2);
  // Calculate GCD(d,phi) and store in g
  mpz_class g;
  mpz_gcd(g.get_mpz_t(),d.get_mpz_t(),phi.get_mpz_t());
  mpz_class one = 1;
  // Check that d<phi & GCD(d,phi)==1, if not find new d
  while(mpz_cmp(d.get_mpz_t(),phi.get_mpz_t())>=0 || mpz_cmp(g.get_mpz_t(),one.get_mpz_t())!=0)
  {
	d = rng.get_z_bits(sz*2);
	mpz_gcd(g.get_mpz_t(),d.get_mpz_t(),phi.get_mpz_t());
  }
  // Compute e as multiplicative inverse of d mod(phi
  if(mpz_invert(e.get_mpz_t(),d.get_mpz_t(),phi.get_mpz_t())==0)
  { cout << "E is undefined!" << endl; }
}

mpz_class RSA_Algorithm::Encrypt(mpz_class M)
{ // Given public key (n,e) & plaintext message M, compute
  // ciphertext message C = (M^d) % n
  mpz_class C;
  mpz_powm(C.get_mpz_t(),M.get_mpz_t(),e.get_mpz_t(),n.get_mpz_t());
  return C;
}

mpz_class RSA_Algorithm::Decrypt(mpz_class C)
{ // Given private key (n,d) & ciphertext message C, compute
  // plaintext message M = (C^e) % n
  mpz_class M;
  mpz_powm(M.get_mpz_t(),C.get_mpz_t(),d.get_mpz_t(),n.get_mpz_t());
  return M;
}

void RSA_Algorithm::PrintND()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << endl;
}

void RSA_Algorithm::PrintNE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " e " << e << endl;
}

void RSA_Algorithm::PrintNDE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << " e " << e << endl;
}

void RSA_Algorithm::PrintM(mpz_class M)
{ // Do not change this, right format for the grading script
  cout << "M " << M << endl;
}

void RSA_Algorithm::PrintC(mpz_class C)
{ // Do not change this, right format for the grading script
  cout << "C " << C << endl;
}
