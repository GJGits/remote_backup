FROM gjcode/gjcode-pds:1.0.7
LABEL author="GJCode"

# build app
COPY . /home/server
WORKDIR /home/server/src
# -I/usr/include/cppconn -L/usr/lib -lmysqlcppcon
RUN g++ -g -std=c++17 -Wall -Werror -o server -I/usr/include/mysqlcppconn -L/usr/lib/mysqlcppconn -lmysqlcppconn $(find . -type f -iregex ".*\.cpp") -lstdc++fs -lboost_system -lcrypto -lpthread -lboost_thread
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./server"]
