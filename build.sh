#/bin/bash

BASE_DIR=$(dirname "$(readlink -f "$0")")
cd "$BASE_DIR/shared"
g++ -c *.cpp
ar rvs ../shared.a *.o
rm *.o
echo "shared built"
cd "$BASE_DIR/client"
g++ *.cpp -I ../shared ../shared.a -lpthread -o ../client.out
echo "client built"
cd "$BASE_DIR/server"
g++ *.cpp -I ../shared ../shared.a -lpthread -o ../server.out
echo "server built"
