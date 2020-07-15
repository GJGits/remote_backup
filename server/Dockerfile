FROM gjcode/gjcode-pds:1.0.6
LABEL author="GJCode"

# build app
COPY src /home/server
WORKDIR /home/server/
# -I/usr/include/cppconn -L/usr/lib -lmysqlcppcon
RUN g++ -g -Wall -Werror -o server -I/usr/include/cppconn -L/usr/lib -std=c++17 ./*.cpp ./test/*.cpp -pthread -lstdc++fs -lmysqlcppcon 
CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./server"]
