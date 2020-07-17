FROM gjcode/gjcode-pds:1.1.0
LABEL author="GJCode"

# build app
COPY src /home/db-attivo
WORKDIR /home/db-attivo/
# -I/usr/include/cppconn -L/usr/lib -lmysqlcppcon
RUN g++ -std=c++17 -Wall -Werror -o db-attivo -I/usr/include/mysqlcppconn -L/usr/lib/mysqlcppconn -lmysqlcppconn ./*.cpp ./test/*.cpp -pthread -lstdc++fs
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./db-attivo"]
