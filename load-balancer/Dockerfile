FROM gjcode/gjcode-pds:1.0.6
LABEL author="GJCode"

# build app
COPY . /home/load-balancer
WORKDIR /home/load-balancer/src
RUN g++ -std=c++17 -Wall -Werror -o load-balancer $(find . -type f -iregex ".*\.cpp") -pthread -lstdc++fs -lboost_system
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./load-balancer"]
