
/* SI 335 Spring 2016
 * Project 2, RSA program
 * Brandon Pullig
 */

#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "posint.hpp"

using namespace std;

// Prints a message on proper usage and exits with the given code
void usage (const char* progname, int ret);

// Function prototype. You have to fill in the implementation below.
void powmod (PosInt& result, const PosInt& a, const PosInt& b, const PosInt& n);

void randomRange (PosInt& result, int bits);

int main (int argc, char** argv) {
  // Detect if standard in is coming from a terminal
  bool interactive = isatty(fileno(stdin));

  // Seed the random number generator
  srand (time(NULL));

  // Pick the base to use in the PosInt class
  PosInt::setBase (10);

  // These parameters determine the blocking of characters
  PosInt byte(256);
  int blocklen = 10;
  PosInt topByte(1);
  for (int i=0; i<blocklen-1; ++i) topByte.fasterMul(byte);

  if (argc < 2 || argv[1][0] != '-') usage(argv[0],1);
  if (argv[1][1] == 'k') {
    if (argc != 4) usage(argv[0],3);
    ofstream pubout(argv[2]);
    ofstream privout(argv[3]);
    if (! pubout || ! privout) {
      cerr << "Can't open public/private key files for writing" << endl;
      return 4;
    }
    ////////////////////////////////////////////////////////////////////
    //                 KEY GENERATION                                 //
    ////////////////////////////////////////////////////////////////////

    //random number between 2^95 & 2^97
    PosInt p;
    PosInt q;

    randomRange(p, 284);
    randomRange(q, 313);

    bool p_prime = false;
    bool q_prime = false;

    //test for primality
    while(!p_prime){
      //find an obviously not prime number
      while(p.isEven()){
	randomRange(p, 284);
      }

      //test if obviously not prime is actually prime multiple times
      bool prime_now = true;
      for(int i = 0; i < 35; i++)
      {
	prime_now = p.MillerRabin();

	if(!prime_now)
	  break;
      }

      if(prime_now)
	p_prime = true;
      else
	randomRange(p, 284);

    }

    while(!q_prime)
    {
      //find an obviously not prime number
      while(q.isEven()){
	randomRange(q, 312);
      }

      //test if obviously not prime is actually prime multiple times
      bool prime_now = true;
      for(int i = 0; i < 35; i++)
      {
	prime_now = q.MillerRabin();
	if(prime_now)
	  continue;
	else
	  break;
      }

      if(prime_now)
	q_prime = true;
      else
	randomRange(q, 312);
    }


//both p and q are found by now. Calculate n
    PosInt n(p);
    n.fasterMul(q);

//start finding e and d
    PosInt one(1);
    PosInt p_min_1(p);
    p_min_1.sub(one);
    PosInt q_min_1(q);
    q_min_1.sub(one);

//calculate phi(n)
    PosInt phi(p_min_1);
    phi.fasterMul(q_min_1);

//begin finding random number 1<e<phi(n) where gcd(phi(n), e)=1
    PosInt e;
    PosInt gcd_result(0);


    e.rand(phi);

    while(gcd_result.compare(one) != 0){
        e.rand(phi);
        gcd_result.gcd(phi, e);
    };

    PosInt g;
    PosInt s;
    PosInt t;

    g.xgcd(s, t, e, phi);

    PosInt d(s);

    // Print out the keys to their respective files.
    pubout << e << endl << n << endl;
    privout << d << endl << n << endl;

    ///////////////// (end of key generation) //////////////////////////
    pubout.close();
    privout.close();
    if (interactive)
      cerr << "Public/private key pair written to " << argv[2]
           << " and " << argv[3] << endl;
  }
  else if (argv[1][1] == 'e') {
    if (argc != 3) usage(argv[0],3);
    ifstream pubin (argv[2]);
    if (! pubin) {
      cerr << "Can't open public key file for reading" << endl;
      return 4;
    }
    if (interactive)
      cerr << "Type your message, followed by EOF (Ctrl-D)" << endl;
    ////////////////////////////////////////////////////////////////////
    //                  ENCRYPTION WITH PUBLIC KEY                    //
    ////////////////////////////////////////////////////////////////////
    // Read public key from pubin file
    PosInt e, n;
    pubin >> e >> n;

    // Read characters from standard in and encrypt them
    int c;
    PosInt M (0); // Initialize M to zero
    PosInt curByte (topByte);

    bool keepGoing = true;
    while (keepGoing) {
      c = cin.get();

      if (c < 0) keepGoing = false; // c < 0 means EOF or error.
      else {
        PosInt next (c); // next character, converted to a PosInt
        next.fasterMul(curByte); // next *= curByte
        M.add(next);     // M = M + next
        curByte.div(byte);
      }

      if (curByte.isZero() || (!keepGoing && !M.isZero())) {
        // HERE'S WHERE YOU HAVE TO DO THE ENCRYPTION!
        PosInt E(1);
        powmod(E, M, e, n);
        cout << E << endl;

        // Now reset curByte and M and keep going
        curByte.set(topByte);
        M.set(0);
      }
    }
    ////////////////// (end of encryption) /////////////////////////////
    if (interactive)
      cerr << "Message successfully encrypted." << endl;
    pubin.close();
  }
  else if (argv[1][1] == 'd') {
    if (argc != 3) usage(argv[0],3);
    ifstream privin (argv[2]);
    if (! privin) {
      cerr << "Can't open private key file for reading" << endl;
      return 4;
    }
    if (interactive)
      cerr << "Enter encrypted numbers, one at a time, ending with EOF" << endl;
    ////////////////////////////////////////////////////////////////////
    //                 DECRYPTION WITH PRIVATE KEY                    //
    ////////////////////////////////////////////////////////////////////
    // Get private key from file
    PosInt d, n;
    privin >> d >> n;

    // Read numbers from standard in and decrypt them
    PosInt E;

    while (cin >> E) {
        PosInt M(1);
        powmod(M, E, d, n);

        for(int i = 9; i >= 0; i--)
        {
            PosInt base(256);
            PosInt power(i);
            base.pow(power);

            PosInt letter(M);
            letter.div(base);
            int let;
            let = letter.convert();

            if(let != 0)
                cout << char(let);
            else
                break;

            M.mod(base);
        }
    }
    ////////////////// (end of decryption) /////////////////////////////
    if (interactive)
      cerr << "Message successfully decrypted." << endl;
    privin.close();
  }
  else if (argv[1][1] == 'm'){
      if (argc != 4) usage(argv[0],3);
      else{
          PosInt X(argv[2]);
          PosInt Y(argv[3]);

          X.fasterMul(Y);
          cout << X << endl;
      }
  }
  else if (argv[1][1] == 'h') usage(argv[0], 0);
  else usage(argv[0],2);
  return 0;
}

////////////////////////////////////////////////////////////
//              MODULAR EXPONENTIATION                    //
////////////////////////////////////////////////////////////

// Computes a^b mod n, and stores the answer in "result".
void powmod (PosInt& result, const PosInt& a, const PosInt& b, const PosInt& n) {
    PosInt zero(0);
    PosInt two(2);

    PosInt base(a);
    PosInt expo(b);

    while (expo.compare(zero) == 1)
    {
        if (!expo.isEven())
        {
            result.fasterMul(base);
            result.mod(n);
        }

        base.fasterMul(base);
        base.mod(n);
        expo.div(two);
    }
}
/////////////////end modular exponentiation/////////////////

//random number with of length around 2^bits
void randomRange (PosInt& result, int bits)
{

  PosInt power(bits+3);
  PosInt base(2);
  PosInt power2(bits);
  PosInt base2(2);

  //raise 2^sub
  base.pow(power);

  //raise 2^bits
  base2.pow(power2);

  //random number between 0-2^96
  result.rand(base);
  //now random number between 2^95-2^97
  result.add(base2);
}

////////////////////////////////////////////////////////////
//              KARATSUBA MULTIPLICATION                  //
////////////////////////////////////////////////////////////

// Sets "this" PosInt object to "this" times x.
void PosInt::fasterMul (const PosInt& x) {
  // This is a suggestion of how to do this one:

  // First figure out the larger of the two input sizes
  int n = digits.size();
  if (n < x.digits.size()) n = x.digits.size();

  // Now copy the inputs into vectors of that size, with zero-padding
  vector<int> mycopy(digits);
  vector<int> xcopy(x.digits);
  mycopy.resize(n, 0);
  xcopy.resize(n, 0);

  // Set "this" digit vector to a zeroed-vector of size 2n
  digits.assign (2*n, 0);

  // Now call the array version to do the actual multiplication
  fastMulArray (&digits[0], &mycopy[0], &xcopy[0], n);

  // We have to call normalize in case there are leading zeros
  normalize();
}

// This does the real work of Karatsuba's algorithm
// (or whatever multiplication algorithm you might write).
// The input is two arrays of digits, x and y, which both have length len.
// The output is stored in the array dest, which is already allocated
// to the proper length.
void PosInt::fastMulArray (int* dest, const int* x, const int* y, int len) {

  if (len <= 9) {
    // base case
    mulArray(dest, x, len, y, len);
  }
  else {
    // recursive case
    // Hint: you will have to allocate some memory for U, V, P0, P1, and P2
    // Another hint: use the addArray and subArray helper methods from the
    // PosInt class!

    vector<int> X0;
    vector<int> X1;
    vector<int> Y0;
    vector<int> Y1;

    int len0 = len/2;
    int len1 = len/2+len%2;

    X0.reserve(len0);
    X1.reserve(len1);
    Y0.reserve(len0);
    Y1.reserve(len1);

    for(int i = 0; i < len; i++)
    {
        if(i<len0){
            X0[i] = x[i];
            Y0[i] = y[i];
        }
        else{
            X1[i-len0] = x[i];
            Y1[i-len0] = y[i];
        }
    }

    vector<int> U;
    vector<int> V;
    vector<int> P0;
    vector<int> P1;
    vector<int> P2;


    U.reserve(len);
    V.reserve(len);
    P0.reserve(len);
    P1.reserve(len);
    P2.reserve(len);

    U.assign(len, 0);
    V.assign(len, 0);
    P0.assign(len*2, 0);
    P1.assign(len*2, 0);
    P2.assign(len*2, 0);

    //set U
    addArray(&U[0], &X1[0], len1);
    addArray(&U[0], &X0[0], len0);

    //set V
    addArray(&V[0], &Y1[0], len1);
    addArray(&V[0], &Y0[0], len0);

    fastMulArray(&P0[0], &X0[0], &Y0[0], len0);
    fastMulArray(&P1[0], &X1[0], &Y1[0], len1);
    fastMulArray(&P2[0], &U[0], &V[0], len1+1);

    for(int i = 0; i < 2*len0; i++)
        dest[i] = P0[i];
    for(int i = 2*len0; i < 2*len; i++)
        dest[i] = P1[i-2*len0];

    addArray(&dest[len0], &P2[0], (2*len)-len0);
    subArray(&dest[len0], &P0[0], (2*len+1)-len0);
    subArray(&dest[len0], &P1[0], (2*len+1)-len0);
  }
}


// Prints a message on proper usage and exits with the given code
void usage (const char* progname, int ret) {
  cout
    << "Generate a public-private key pair:" << endl
    << "\t" << progname << " -k PUBLIC_KEY_FILE PRIVATE_KEY_FILE" << endl
    << "Encrypt a message with public key:" << endl
    << "\t" << progname << " -e PUBLIC_KEY_FILE" << endl
    << "Decrypt a message with private key:" << endl
    << "\t" << progname << " -d PRIVATE_KEY_FILE" << endl
    << "Multiply 2 numbers using Karatsuba's algorithm:" << endl
    << "\t" << progname << " -m <a> <b>" << endl
    << "Note: PUBLIC_KEY_FILE and PRIVATE_KEY_FILE are any filenames you choose."
    << endl
    << "      Encryption and decryption read and write to/from standard in/out."
    << endl
    << "      You have to end the input with EOF (Ctrl-D if on command line)."
    << endl
    << "      You can use normal bash redirection with < and > to read or" << endl
    << "      write the encryption results to a file instead of standard in/out."
    << endl;
  exit(ret);
}
