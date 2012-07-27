CC := g++
CFLAG := -g 
TARGET:= sharing_ex
SRC:= sharing_main.cpp

all:$(TARGET)
	@echo "data sharing model done..."

$(TARGET):$(SRC)
	$(CC) $(CFLAG) $(SRC) -o $(TARGET)

	
clean:
	rm -f $(TARGET)
