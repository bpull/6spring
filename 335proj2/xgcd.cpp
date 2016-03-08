/* SI 335 Spring 2016
 * Code to compute the inverse of 19 mod 101, using
 * the extended Euclidean algorithm
 */

#include <iostream>
#include "posint.hpp"

using namespace std;

int main() {
  // First set it to base 10
  PosInt::setBase(10);

  // Now initialize my x and y
  PosInt x(19);
  PosInt y(101);

  // These will hold (respectively) the gcd and the two multipliers.
  // I already know that g is going to be 1, and I don't care what t is,
  // but I still have to declare PosInt objects for them.
  PosInt g, s, t;

  // Now do the actual XGCD. The posint.hpp header file describes exactly
  // how this will work. At the end, g will be the gcd of x and y, and
  // we will have g = s*x - t*y.
  g.xgcd(s, t, x, y);

  // Now print out the results.
  cout << "The GCD is " << g << endl;
  cout << "The inverse of x mod y is " << s << endl;

  return 0;
}
