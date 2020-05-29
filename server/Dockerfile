FROM gjcode/gjcode-pds:1.0.4
LABEL author="GJCode"
WORKDIR /home/src
RUN g++ -g -Wall -Werror main.cpp -o server
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./server"]