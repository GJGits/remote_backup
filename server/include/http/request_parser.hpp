//
// request_parser.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <cstddef>
#include <iostream>
#include <tuple>
#include <vector>

#include "../../include/http/request.hpp"
namespace http {
namespace server {

struct request;

/// Parser for incoming requests.
class request_parser {
public:
  /// Construct ready to parse the request method.
  request_parser();

  /// Reset to initial parser state.
  void reset();

  /// Result of parse.
  enum result_type { good, bad, indeterminate };

  /// Parse some data. The enum return value is good when a complete request has
  /// been parsed, bad if the data is invalid, indeterminate when more data is
  /// required. The InputIterator return value indicates how much of the input
  /// has been consumed.
  template <typename InputIterator>
  std::tuple<result_type, InputIterator>
  parse(request &req, InputIterator begin, InputIterator end) {

    std::string search{"Content-Length"};
    //std::vector<std::byte> body;

    // Ciclo while che consuma la richiesta carattere per carattere
    // l'iteratore itera fino alla fine (comprende body se presente)
    while (begin != end) {
      // Recupero headers
      if (req.content_length == 0) {
        // Consumo header un carattere ed avanzo iteratore
        result_type result = consume(req, *begin++);
        // se header non buono restituisco bad
        if (result == bad) {
          return std::make_tuple(result, begin);
        }
        // se header ok ho finito di leggere header ma devo controllare se
        // esiste ancora un body da leggere
        if (result == good) {
          // ciclo sugli header per verificare se esiste un campo
          // Content-Length e se questo e' maggiore di zero.
          for (size_t i = 0; i < req.headers.size(); i++) {
            std::string name = req.headers[i].name;
            std::string value = req.headers[i].value;
            // se entro esiste Content-Lenght e salvo la dimensione in un
            // campo.
            if (name.compare(search) == 0 && std::stoi(value) > 0) {
              req.content_length = std::stoi(value);
            }
          }
          // se dopo aver letto gli header content_length ha
          // valore zero allora non ho body
          if (req.content_length == 0)
            return std::make_tuple(good, begin);
        }
      }
      // Recupero body
      if (req.content_length != 0) {
        req.content_length--;
        req.body->push_back(*begin++);
        result_type result = req.content_length == 0 ? good : indeterminate;
        if (result == good) {
          return std::make_tuple(good, begin);
        }
          
      }
    }
    return std::make_tuple(indeterminate, begin);
  }

private:
  /// Handle the next character of input.
  result_type consume(request &req, char input);

  /// Handle next byte of body
  result_type consume_body(request &req, char input, int &left);

  /// Check if a byte is an HTTP character.
  static bool is_char(int c);

  /// Check if a byte is an HTTP control character.
  static bool is_ctl(int c);

  /// Check if a byte is defined as an HTTP tspecial character.
  static bool is_tspecial(int c);

  /// Check if a byte is a digit.
  static bool is_digit(int c);

  /// The current state of the parser.
  enum state {
    method_start,
    method,
    uri,
    http_version_h,
    http_version_t_1,
    http_version_t_2,
    http_version_p,
    http_version_slash,
    http_version_major_start,
    http_version_major,
    http_version_minor_start,
    http_version_minor,
    expecting_newline_1,
    header_line_start,
    header_lws,
    header_name,
    space_before_header_value,
    header_value,
    expecting_newline_2,
    expecting_newline_3
  } state_;
};

} // namespace server
} // namespace http
