FROM gjcode/gjcode-pds:1.0.5
LABEL author="GJCode"
COPY src /home/client
WORKDIR /home/client/
RUN g++ -g -Wall -Werror -std=c++17 ./*.cpp ./test/*.cpp -o client -lstdc++fs
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./client"]