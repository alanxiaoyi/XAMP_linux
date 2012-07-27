CC := g++
CFLAG := -g 
TARGET:= memwall_ex
SRC:= memwall_main.cpp

all:$(TARGET)
	@echo "memory wall model done..."

$(TARGET):$(SRC)
	$(CC) $(CFLAG) $(SRC) -o $(TARGET)
	
clean:
	rm -f $(TARGET) 
