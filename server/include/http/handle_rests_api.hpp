#include <iostream>
#include <queue>
#include <condition_variable>

class rests_api_get_status{
    static rests_api_get_status *instance;
    std::queue<std::string> requests;
    std::queue<std::string> replies;
    std::condition_variable cv;
    std::mutex m;
    rests_api_get_status(){};
public:
    static rests_api_get_status *getInstance(){
         if(instance== nullptr)
            instance = new rests_api_get_status();
        return instance;

    };

    std::string get_and_produce();


    void add_and_wakeup(std::string req);

};

