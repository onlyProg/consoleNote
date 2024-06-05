
TARGET = note
CC = gcc
SRC_FILE_EXTENSOIN = c
RUN_FLAGS =
FLAGS_LINK = -Wall
FLAGS_COMPIL =


PREF_SRC = ./src/
PREF_OBJ = ./obj/


SRC = $(wildcard $(PREF_SRC)*.$(SRC_FILE_EXTENSOIN))
OBJ = $(patsubst $(PREF_SRC)%.$(SRC_FILE_EXTENSOIN), $(PREF_OBJ)%.o, $(SRC))


$(TARGET) : $(OBJ)
	$(CC) $(FLAGS_LINK) $(OBJ) -o $(TARGET)

$(PREF_OBJ)%.o : $(PREF_SRC)%.$(SRC_FILE_EXTENSOIN)
	$(CC) $(FLAGS_COMPIL) -c $< -o $@

run :
	@test -e "$(TARGET)" && ./$(TARGET) $(RUN_FLAGS)

install : $(TARGET)
	sudo cp $(TARGET) /bin

uninstall:
	sudo rm /bin/$(TARGET)

clean :
	rm -v $(TARGET) $(PREF_OBJ)*.o

