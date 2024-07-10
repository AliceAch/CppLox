CC=g++

MODE=debug
NAME=cpplox
SOURCE_DIR=./src/
INC_DIRS=./src/include/
#libraries are statically linked
DEP_DIRS=./src/dependencies/
#add any additional libraries here:
LIBS=libfmt.a
#DEPFLAGS=-MP -MD

CFLAGS=-std=c++17

# Mode configuration.
ifeq ($(MODE),debug)
	CFLAGS += -O0 -DDEBUG -g
	BUILD_DIR = build/debug
else
	CFLAGS += -O3 -flto
	BUILD_DIR = build/release
endif

# Files.
CFLAGS += -Wall -Wextra -Werror -g $(foreach D,$(INC_DIRS),-I$(D)) #$(DEPFLAGS)
LIBRARIES = $(foreach I,$(DEP_DIRS),-L$(I))
#CFILES=$(foreach D,$(SOURCE_DIR),$(wildcard $(D)/*.cpp))
CFILES=$(wildcard $(SOURCE_DIR)/*.cpp)

HEADERS=$(wildcard $(INC_DIRS)/*.h)
OBJECTS=$(addprefix $(BUILD_DIR)/$(NAME)/, $(notdir $(CFILES:.cpp=.o)))
#DEPFILES=$(patsubst %.cpp,%.d,$(CFILES))
LINK = -l:$(LIBS)

all: build/$(NAME)

build/$(NAME): $(OBJECTS)
	mkdir -p build
	$(CC) -o $@ $^ $(LIBRARIES) $(LINK) 
$(BUILD_DIR)/$(NAME)/%.o:$(SOURCE_DIR)/%.cpp $(HEADERS)
	mkdir -p $(BUILD_DIR)/$(NAME)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build/$(NAME) $(OBJECTS) #$(DEPFILES)

#-include $(DEPFILES)

.PHONY: all clean default
