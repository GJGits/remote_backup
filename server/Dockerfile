FROM gjcode/gjcode-pds:1.0.6
LABEL author="GJCode"

# build app
COPY src /home/server
WORKDIR /home/server/
# -I/usr/include/cppconn -L/usr/lib -lmysqlcppcon
RUN g++ -std=c++17 -Wall -Werror -o server -I/usr/include/mysqlcppconn -L/usr/lib/mysqlcppconn -lmysqlcppconn ./*.cpp ./test/*.cpp ./mysql/*.cpp -pthread -lstdc++fs
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./server"]
