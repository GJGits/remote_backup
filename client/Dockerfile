FROM gjcode/gjcode-pds:1.0.7
LABEL author="GJCode"
COPY . /home/client
WORKDIR /home/client/
RUN g++ -g -Wall -Werror -std=c++17 $(find . -type f -iregex ".*\.cpp") -o client -lstdc++fs -lcrypto
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./client"]