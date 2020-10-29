FROM gjcode/gjcode-pds:1.0.8
LABEL author="GJCode"
COPY . /home/client
WORKDIR /home/client/
# ritorna a -g per debug
RUN g++ -Wall -Werror -std=c++17 $(find . -type f -iregex ".*\.cpp") -o client -lstdc++fs -lcrypto -pthread
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./client"]