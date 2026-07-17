echo CC = gcc > Makefile
echo CFLAGS = -Wall -Wextra -std=c99 >> Makefile
echo INCLUDES = -I include >> Makefile
echo SRC = src/main.c src/users.c src/authors.c src/categories.c src/books.c src/borrows.c src/returns.c src/reservations.c src/penalties.c src/stats.c src/history.c src/utils.c >> Makefile
echo. >> Makefile
echo all: bin/e-library.exe >> Makefile
echo. >> Makefile
echo bin/e-library.exe: $(SRC) >> Makefile
echo 	if not exist bin mkdir bin >> Makefile
echo 	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(SRC) >> Makefile
echo. >> Makefile
echo clean: >> Makefile
echo 	if exist bin\*.exe del /Q bin\*.exe >> Makefile