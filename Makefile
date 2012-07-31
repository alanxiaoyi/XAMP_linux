CXX					:= g++
CXXFLAG_CMD			:= -g 
CXXFLAG_GUI			:= -g `pkg-config gtk+-2.0 --cflags --libs` -lgthread-2.0
OBJ_interface 		:=interface_exec.o menu_struct.o parse_config.o input_deduction.o 
OBJ_interface_gui   :=gui.o  parse_gui.o interface_gui.o input_deduction.o
OBJ_tinyxml			:=tinyxml.o tinyxmlerror.o tinyxmlparser.o tinystr.o

ifdef NO_GUI
all: xamp
	@echo "my work done here..."

else
all: xamp xamp_gui
	@echo "my work done here..."
endif
	
$(OBJ_interface): CXXFLAG := $(CXXFLAG_CMD)
$(OBJ_interface_gui): CXXFLAG := $(CXXFLAG_GUI)
	
xamp : $(OBJ_interface) $(OBJ_tinyxml) 
	$(CXX)  $(OBJ_interface) $(OBJ_tinyxml) -o xamp $(CXXFLAG_CMD)
	
ifdef NO_GUI

else
xamp_gui : $(OBJ_interface_gui) $(OBJ_tinyxml) 
	$(CXX)  $(OBJ_interface_gui) $(OBJ_tinyxml)  -o xamp_gui  $(CXXFLAG_GUI)
endif

 .cpp.o:
	$(CXX)  -c $*.cpp $(CXXFLAG)
	
	
	
clean:
	rm -f *.o xamp xamp_gui *.gch

clobber:
	rm -f *.o