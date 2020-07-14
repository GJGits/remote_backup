FROM gjcode/gjcode-pds:1.0.5
LABEL author="GJCode"
COPY src /home/server
WORKDIR /home/server/
RUN g++ -g -Wall -Werror -std=c++17 ./*.cpp ./test/*.cpp -o server -lstdc++fs
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./server"]