CC := g++
CFLAG := -g 
TARGET:= bandwidth_ex
SRC:= bandwidth_main.cpp

all:$(TARGET)
	@echo "bandwidth partition model done..."

$(TARGET):$(SRC)
	$(CC) $(CFLAG) $(SRC) -o $(TARGET)

	
clean:
	rm -f $(TARGET)
