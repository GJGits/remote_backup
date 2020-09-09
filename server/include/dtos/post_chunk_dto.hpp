#pragma once

#include <iostream>

#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "json-serializable.hpp"

class PostChunkDTO {
private:
  std::string username;
  int chunk_id;
  int chunk_size;
  std::string file_path;
  std::string chunk_hash;
  std::string timestamp_locale;
  std::vector<char> chunk_body;
  std::string number_of_chunks;

  // POST
  // /chunk/{username}/{chunk_id}/{chunk_size}/{chunk_hash}/{file_pathBASE64}/{timestamp_locale}
public:
  PostChunkDTO(){};
  std::string getusername() const;
  int getchunk_id() const;
  int getchunk_size() const;
  std::string getchunk_hash() const;
  std::string getfile_path() const;
  std::string gettimestamp_locale() const;
  std::vector<char> getchunk_body() const;
  std::string getnumber_of_chunks() const;

  const void fill(std::string requri,
                  std::vector<char> reqbody) { // todo: passarlo per
                                               // riferimento, non per copia
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    for (unsigned int i = 0; i < uri_elements.size(); i++) {
      switch (i) {
      case 2:
        username = uri_elements[i];
        break;
      case 3:
        chunk_id = std::stoi(uri_elements[i]);
        break;
      case 4:
        chunk_size = std::stoi(uri_elements[i]);

        break;
      case 5:
        chunk_hash = uri_elements[i];
        break;
      case 6:
        number_of_chunks = uri_elements[i];
        break;
      case 7:
        file_path = macaron::Base64::Decode(uri_elements[i]);
        break;
      case 8:

        timestamp_locale = uri_elements[i];
        break;
      default:
        break;
      }
    }
    chunk_body = reqbody;
    for (auto it = chunk_body.begin(); it != chunk_body.end(); it++) {
      std::clog << "char body (server): " << *it << "\n";
    }
    std::clog << "arrivo qui\n";
  }
};