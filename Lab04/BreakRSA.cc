// Grad student portion of the RSA assignment
// Spring 2017 ECE6122

#include <iostream>
#include <math.h>

#include "RSA_Algorithm.h"

using namespace std;

unsigned long FermatFactor(unsigned long N)
{ // Fermat Factorization is based on representation of an odd integer
  // as the difference of two squares: N = a^2 - b^2
  // Because N is guaranteed to be 64-bit number, can use the std sqrt function with precision
  unsigned long a = sqrt(N); 
  // First guess at b^2
  unsigned long b2 = a*a - N;
  while(true)
  { // Iterate a++ until you find b2 that has a true square root
	unsigned long b2Root = sqrt(b2);
	if((b2Root*b2Root)==b2) break;
	a++;
	b2 = a*a - N;
  }
  // From above, N = (a + b) * (a - b), and the factors are (a + b) & (a - b)
  return a - sqrt(b2);
}
int main(int argc, char** argv)
{ // Arguments are as follows:
  //argv[1] = n;
  //argv[2] = e;  // n and e are the public key
  //argv[3] = first 6 characters of encrypted message
  //argv[4] = next 6 characters .. up to argv[12] are the lsat 6 characters
  // The number of arguments will always be exacty 12, and each argument past the
  // public key contain 6 ascii characters of the encrypted message.
  // Each of the 32bit values in the argv[] array are right justified in the
  // low order 48 bits of each unsigned long.  The upper 16 bits are always
  // zero, which insures the each unsigned long is < n (64 bits) and therefore
  // the RSA encryption will work.

  // Below is an example of the BreakRSA and command line arguments:

// ./BreakRSA  2966772883822367927 2642027824495698257  817537070500556663 1328829247235192134 
// 1451942276855579785 2150743175814047358 72488230455769594 1989174916172335943 962538406513796755 
// 1069665942590443121 72678741742252898 1379869649761557209

//   The corect output from the above is:
//   HelloTest  riley CekwkABRIZFlqmWTanyXLogFgBUENvzwHpEHRCZIKRZ
//
//   The broken (computed) private key for the above is 4105243553



  // Our one and only RSA_Algorithm object
  RSA_Algorithm rsa;
  
  // First "break" the keys by factoring n and computing d
  // Set the keys in the rsa object afer calculating d
  rsa.n = mpz_class(argv[1]);
  rsa.e = mpz_class(argv[2]);
  unsigned long n = rsa.n.get_ui();
  unsigned long q = FermatFactor(n);
  unsigned long p = n/q;
  mpz_class phi((p-1)*(q-1));
  mpz_class e = rsa.e;
  // Set rsa.d to the calculated private key d
  mpz_class d;
  mpz_invert(d.get_mpz_t(),e.get_mpz_t(),phi.get_mpz_t());
  rsa.d = d;
  rsa.n = n;
  // rsa.d = mpz_class(// broken d value here) 
  for (int i = 3; i < 13; ++i)
    { // Decrypt each set of 6 characters
      mpz_class c(argv[i]);
      mpz_class m = rsa.Decrypt(c);
      //  use the get_ui() method in mpz_class to get the lower 48 bits of the m
      unsigned long ul = m.get_ui();
      // Now print the 6 ascii values in variable ul.  
      // As stated above the 6 characters
      // are in the low order 48 bits of ui.
      for (int j = 0; j < 6; ++j)
        {
          unsigned char ch = ul >> (48 - j * 8 - 8);
          cout << ch;
        }
    }
  cout << endl;
}

