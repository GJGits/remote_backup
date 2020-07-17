FROM gjcode/gjcode-pds:1.0.6
LABEL author="GJCode"

# build app
COPY src /home/load-balancer
WORKDIR /home/load-balancer/
RUN g++ -std=c++17 -Wall -Werror -o load-balancer ./*.cpp ./test/*.cpp -pthread -lstdc++fs
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./load-balancer"]
