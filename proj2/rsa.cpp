
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

void randomRange (PosInt& result, int bits, int sub);

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
  for (int i=0; i<blocklen-1; ++i) topByte.mul(byte);

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

    randomRange(p, 116, 119);
    randomRange(q, 124, 127);

    bool p_prime = false;
    bool q_prime = false;

    //test for primality
    while(!p_prime){
      //find an obviously not prime number
      while(p.isEven()){
	randomRange(p, 116, 119);
      }

      //test if obviously not prime is actually prime multiple times
      bool prime_now = true;
      for(int i = 0; i < 30; i++)
      {
	prime_now = p.MillerRabin();

	if(!prime_now)
	  break;
      }

      if(prime_now)
	p_prime = true;
      else
	randomRange(p, 116, 119);
      
    }

    cout << "prime p is found. it is " << p << "\n";

    while(!q_prime)
    {
      //find an obviously not prime number
      while(q.isEven()){
	randomRange(q, 124, 127);
      }

      //test if obviously not prime is actually prime multiple times
      bool prime_now = true;
      for(int i = 0; i < 30; i++)
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
	randomRange(q, 124, 127);
    }

    cout << "prime q is found. it is " << q << "\n";

    PosInt n(p);
    n.mul(q);

    cout << "n is now: " << n << "\n";

    // These are just "dummy" values! Replace with your actual code
    PosInt e (7);
    PosInt d (13);

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
        next.mul(curByte); // next *= curByte
        M.add(next);     // M = M + next
        curByte.div(byte);
      }

      if (curByte.isZero() || (!keepGoing && !M.isZero())) {
        // HERE'S WHERE YOU HAVE TO DO THE ENCRYPTION!
        PosInt E (1234); // THIS IS JUST A "DUMMY" VALUE
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
      // You have to decrypt E and print out the 10 characters it holds.
      // Note: use the "convert" function to turn a PosInt into
      // a regular "int" - and then into a char!.
      // Follow the procedure from encryption, only in reverse.
      cout << "abcdefg\n"; // THIS IS JUST A DUMMY!
    }
    ////////////////// (end of decryption) /////////////////////////////
    if (interactive)
      cerr << "Message successfully decrypted." << endl;
    privin.close();
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
  // YOU HAVE TO FILL THIS IN!
}

/////////////////end modular exponentiation/////////////////

//random number with range of 2^power
void randomRange (PosInt& result, int bits, int sub)
{
  //random number between 2^95 & 2^97

  PosInt power(sub);
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
  // Again, this is just a suggested general outline...
  if (len <= 3) {
    // base case
    // YOU FILL THIS IN
  }
  else {
    // recursive case
    // YOU FILL THIS IN TOO.
    // Hint: you will have to allocate some memory for U, V, P0, P1, and P2
    // Another hint: use the addArray and subArray helper methods from the
    // PosInt class!
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

