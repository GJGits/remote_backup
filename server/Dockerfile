FROM gjcode/gjcode-pds:1.0.6
LABEL author="GJCode"

# build app
COPY logs /home/server/logs
COPY config /home/server/config
COPY src/ /home/server/src
WORKDIR /home/server/src
# -I/usr/include/cppconn -L/usr/lib -lmysqlcppcon
RUN g++ -std=c++17 -Wall -Werror -o server -I/usr/include/mysqlcppconn -L/usr/lib/mysqlcppconn -lmysqlcppconn $(find . -type f -iregex ".*\.cpp") -pthread -lstdc++fs
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./server"]
