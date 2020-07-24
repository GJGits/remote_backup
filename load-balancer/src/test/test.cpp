#include "../../include/test/test.hpp"

void printResult(int tests, int errors) {
  if (errors != 0)
    std::clog << "passed [" << (tests - errors) << "/" << tests << "] KO!\n";
  else
    std::clog << "all tests OK!\n";
}

void testBase64() {

  std::clog << "##### Running Test BASE64 #####\n";
  int tests = 1;
  int errors = 0;

  const std::string real{"Ciao Mario!"};
  std::string encoded = macaron::Base64::Encode(real);
  std::string decoded = macaron::Base64::Decode(encoded);

  if (real.compare(decoded) != 0) {
    errors++;
    std::clog << "error on base64 decoding: real=" << real
              << ", encoded=" << encoded << ", decoded=" << decoded << "\n";
  }

  printResult(tests, errors);
}

void testAll() {
  std::clog << "Running tests...\n";
  testBase64();
}