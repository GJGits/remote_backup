FROM gjcode/gjcode-pds:1.0.8
LABEL author="GJCode"
COPY . /home/client
WORKDIR /home/client/
# ritorna a -g per debug
RUN g++ -g -Wall -Werror -std=c++17 $(find . -type f -iregex ".*\.cpp") -o client -lstdc++fs -lcrypto -pthread
RUN chmod -R a+rwx sync
RUN chmod -R a+rwx sync2
# lanciare applicazione con valgrind
#CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./client"]
# lanciare app senza valgrind
CMD [ "./client" ]
