#include "../../include/http/controllers.hpp"

ControllerRouter *ControllerRouter::instance = nullptr;
AuthController *AuthController::instance = nullptr;
StatusController *StatusController::instance = nullptr;



std::string AuthController::handle(const http::server::request &req){

    if(req.method == "POST" ){
    	if ( req.uri == "/auth/signin" ){
    	    std::clog<< "sono in auth/signin della authcontroller\n";
            return post_sigin(req);
        }
        else if ( req.uri == "/auth/signup" ){
            std::clog<< "sono in auth/signup della authcontroller\n";
    	    return post_signup(req);
    	}
    }
    return std::string("Ricordati di mettermi come rep vuota!!!\n"); //Sarà una rep vuota qui, ricordarsi
    	
}

std::string AuthController::post_sigin(const http::server::request &req){
  
  	std::clog << "sono nella authcontroller::postsignin\n";
  	return std::string("sono nella authcontroller::postsignin\n");
  
  }
  
std::string AuthController::post_signup(const http::server::request &req){

  	std::clog << "sono nella authcontroller::postsignup\n";
  	return std::string("sono nella authcontroller::postsignup\n");

}

std::string StatusController::handle(const http::server::request &req){
    if(req.method == "GET" ){
        if ( req.uri == "/status" ){
      	    std::clog<< "sono in status del statuscontrollerhandle\n";
  	    return get_status(req);
  	}
    }
    return std::string("Ricordati di mettermi come rep vuota!!!\n");
  }
  
  
std::string StatusController::get_status(const http::server::request &req){

  	std::clog << "sono nella StatusController::get_status\n";
  	return std::string("sono nella StatusController::get_status\n");

}
