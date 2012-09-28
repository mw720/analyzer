### SET OPTIONAL #defines HERE ###
DEFINITIONS=-DDISPLAY_MODULES

### Variable definitions
SRC=$(PWD)/src
OBJ=$(PWD)/obj
CINT=$(PWD)/cint
DRLIB=$(PWD)/lib

ROOTGLIBS = $(shell root-config --glibs) -lXMLParser -lThread -lTreePlayer
RPATH    += -Wl,-rpath,$(ROOTSYS)/lib -Wl,-rpath,$(PWD)/lib
DYLIB=-shared
FPIC=-fPIC
INCFLAGS=-I$(SRC) -I$(CINT) -I$(USER) $(USER_INCLUDES)
DEBUG=-ggdb -O0 -DDEBUG
CXXFLAGS=$(DEBUG) $(INCFLAGS) -L$(PWD)/lib $(STOCK_BUFFERS) -DBUFFER_TYPE=$(USER_BUFFER_TYPE) $(DEFINITIONS)


ifdef ROOTSYS
ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread -lTreePlayer
CXXFLAGS += -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --cflags) -I$(ROOTSYS)/include
else
ROOTGLIBS = $(shell root-config --glibs --cflags) -lXMLParser -lThread -lTreePlayer
endif

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
CXXFLAGS += -DOS_LINUX -DOS_DARWIN
ifdef MIDASSYS
CXXFLAGS += -DMIDASSYS
MIDASLIBS = -L$(MIDASSYS)/darwin/lib -lmidas
INCFLAGS += -I$(MIDASSYS)/include
endif

DYLIB=-dynamiclib -single_module -undefined dynamic_lookup
FPIC=
RPATH=

endif

CXX=g++ -Wall
#CXX=clang++ -I/opt/local/include/ -I/opt/local/include/root

CC=gcc -Wall
#CC=clang -I/opt/local/include/ -I/opt/local/include/root

LINK=$(CXX) $(CXXFLAGS) $(ROOTGLIBS) $(RPATH) $(DEFAULTS) -I/user/gchristian/soft/develop/rootbeer/src -DMIDAS_BUFFERS

CXX+=$(CXXFLAGS) $(RPATH) $(DEF_EXT) $(DEFAULTS) -I/user/gchristian/soft/develop/rootbeer/src -DMIDAS_BUFFERS

CC+=$(CXXFLAGS) $(RPATH) $(DEF_EXT) $(DEFAULTS) -I/user/gchristian/soft/develop/rootbeer/src -DMIDAS_BUFFERS

ROOTCINT=rootcint

#### DRAGON LIBRARY ####
OBJECTS=                            	\
$(OBJ)/midas/internal/mxml.o        	\
$(OBJ)/midas/internal/strlcpy.o     	\
$(OBJ)/midas/Odb.o                  	\
$(OBJ)/midas/Xml.o                  	\
$(OBJ)/midas/internal/TMidasFile.o  	\
$(OBJ)/midas/internal/TMidasEvent.o 	\
$(OBJ)/midas/Event.o                	\
					\
$(OBJ)/utils/TStamp.o              	\
					\
$(OBJ)/vme/V792.o             	      	\
$(OBJ)/vme/V1190.o             	     	\
$(OBJ)/vme/IO32.o              	     	\
					\
$(OBJ)/dragon/Bgo.o                 	\
$(OBJ)/dragon/MCP.o                 	\
$(OBJ)/dragon/DSSSD.o               	\
$(OBJ)/dragon/Auxillary.o           	\
$(OBJ)/dragon/IonChamber.o          	\
$(OBJ)/dragon/SurfaceBarrier.o      	\
					\
$(OBJ)/dragon/Head.o                	\
$(OBJ)/dragon/Tail.o                	\
					\
$(OBJ)/dragon/Coinc.o
## END OBJECTS ##


HEADERS=                  \
$(SRC)/midas/*.hxx        \
$(SRC)/midas/internal/*.h \
$(SRC)/utils/*.hxx        \
$(SRC)/utils/*.h          \
$(SRC)/vme/*.hxx          \
$(SRC)/dragon/*.hxx       \

### DRAGON LIBRARY ###
all: $(DRLIB)/libDragon.so

lib: $(DRLIB)/libDragon.so

$(DRLIB)/libDragon.so: $(CINT)/DragonDictionary.cxx $(OBJECTS)
	$(LINK) $(DYLIB) $(FPIC) -o $@ $(MIDASLIBS) $(OBJECTS) \
-p $(CINT)/DragonDictionary.cxx  \


### OBJECT FILES ###

$(OBJ)/*/%.o: $(SRC)/*/%.cxx $(CINT)/DragonDictionary.cxx
	$(CXX) $(FPIC) -c \
-o $@ -p $< \

$(OBJ)/*/%.o: $(SRC)/*/%.c $(CINT)/DragonDictionary.cxx
	$(CC) $(FPIC) -c \
-o $@ -p $< \

### CINT DICTIONARY ###
dict: $(CINT)/DragonDictionary.cxx
$(CINT)/DragonDictionary.cxx:  $(HEADERS) $(CINT)/Linkdef.h
	rootcint -f $@ -c $(CXXFLAGS) -p $(HEADERS) $(CINT)/Linkdef.h \


definitions:
	scp dragon@ladd06.triumf.ca:/home/dragon/online/src/definitions.h \
$(PWD)/src/utils/


### --- ROOTBEER --- ###

RBINC=/Users/gchristian/soft/develop/rootbeer/src

$(OBJ)/rootbeer/%.o: $(SRC)/*/%.cxx $(SRC)/*/*.hxx
	$(CXX) $(FPIC) -c \
-o $@ -p $< \

Timestamp: $(OBJ)/rootbeer/Timestamp.o
MidasBuffer: $(OBJ)/rootbeer/MidasBuffer.o


#### REMOVE EVERYTHING GENERATED BY MAKE ####

clean:
	rm -f $(DRLIB)/*.so rootbeer $(CINT)/DragonDictionary.* $(OBJECTS) $(OBJ)/rootbeer/*.o


#### FOR DOXYGEN ####

doc::
	doxygen doc/Doxyfile

docclean::
	rm -fr /Volumes/public/gchristian/public_html/dragon/analyzer/html \
/Volumes/public/gchristian/public_html/dragon/analyzer/latex


#### FOR UNIT TESTING ####

mxml.o:           $(OBJ)/midas/internal/mxml.o
strlcpy.o:        $(OBJ)/midas/internal/strlcpy.o
Odb.o:            $(OBJ)/midas/Odb.o
Xml.o:            $(OBJ)/midas/Xml.o
TMidasFile.o:     $(OBJ)/midas/internal/TMidasFile.o
TMidasEvent.o:    $(OBJ)/midas/internal/TMidasEvent.o
Event.o:          $(OBJ)/midas/Event.o

TStamp.o:         $(OBJ)/tstamp/TStamp.o

V792.o:           $(OBJ)/vme/V792.o
V1190.o:          $(OBJ)/vme/V1190.o
IO32.o:           $(OBJ)/vme/IO32.o

Bgo.o:            $(OBJ)/dragon/Bgo.o
MCP.o:            $(OBJ)/dragon/MCP.o
DSSSD.o:          $(OBJ)/dragon/DSSSD.o
Auxillary.o:      $(OBJ)/dragon/Auxillary.o
IonChamber.o:     $(OBJ)/dragon/IonChamber.o
SurfaceBarrier.o: $(OBJ)/dragon/SurfaceBarrier.o

Head.o:           $(OBJ)/dragon/Head.o
Tail.o:           $(OBJ)/dragon/Tail.o

odbtest: $(DRLIB)/libDragon.so
	$(LINK) src/midas/Odb.cxx -o odbtest -DMIDAS_BUFFERS -lDragon -L$(DRLIB) -DODB_TEST
