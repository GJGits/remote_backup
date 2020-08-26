#include "../../include/test/test.hpp"

void printResult(int tests, int errors)
{
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

void testSha256() {
    std::string message{"Ciao Mario!"};
    std::clog << "hash: " << Sha256::getSha256(message) << "\n";
}

void testHamacSha256() {
    const unsigned char *text = (const unsigned char *) "Ciao Mario!";
    const unsigned char *secret = (const unsigned char *) "MySuperSecretHere!";
    char kdigest[65];
    HmacSha256::hmac_sha256(text, 11, secret, 17, kdigest);
    std::clog << "kdigest = " << kdigest << "\n";
    std::clog << "kdigest_base64 = " << macaron::Base64::Encode(std::string(kdigest)) << "\n";
}

void testSQLCRUD()
{

    std::clog << "##### TEST SQL CRUD #####\n";
    int tests = 2;
    int errors = 0;
    
    // test insert
    std::string username("Pippo");
    std::string hashedPassword("hashed_password");
    unsigned int salt = 123456;
    UserEntity entity{username, hashedPassword, salt};
    UserRepository userRepo{};
    bool insRes = userRepo.insertUser(entity);
    
    if(!insRes) {
        errors++;
        std::clog << "expected true got false\n";
    }
    // test select
    std::optional<UserEntity> retrieved = userRepo.getUserByUsername(std::string{"Pippo"});

    if(!retrieved.has_value()) {
        errors++;
        std::clog << "select: expected true got false\n";
    }

    // test delete
    userRepo.deleteUserByUsername(username);

    printResult(tests, errors);
}



void testAll()
{
    std::clog << "Running tests...\n";
    testSQLCRUD();
    testSha256();
    testHamacSha256();
}
