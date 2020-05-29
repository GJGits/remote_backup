FROM gjcode/gjcode-pds:1.0.4
LABEL author="GJCode"
RUN g++ -g -Wall -Werror main.cpp -o client
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./client"]