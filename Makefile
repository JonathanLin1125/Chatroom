all:server client
server:server.c
	gcc -ggdb -pedantic -Wall -Wextra -Werror -Wwrite-strings server.c -o server
client:client.c
	gcc -ggdb -pedantic -Wall -Wextra -Werror -Wwrite-strings client.c -o client
