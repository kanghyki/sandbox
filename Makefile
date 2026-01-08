# -----( SRC / OBJ )-----
SRC_DIR   = src
OBJ_DIR   = obj

SRC_FILES := $(shell find $(SRC_DIR) -name '*.cpp')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# -----( LIB )-----
LIB_PATH := lib
LIBS     :=

LIB_ARCS := $(foreach L,$(LIBS),$(LIB_PATH)/$(L)/lib$(L).a)
LIB_INCS := $(foreach L,$(LIBS),-I$(LIB_PATH)/$(L))

# -----( RULES )-----
NAME     = sandbox
CXX      = c++

CPPFLAGS = -I$(SRC_DIR) $(LIB_INCS)
CXXFLAGS = -Wall -Wextra -Werror -std=c++11

LDFLAGS  = -framework OpenGL -framework AppKit
LDLIBS   = $(LIB_ARCS)

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJ_FILES) $(LDLIBS)
	$(CXX) $(OBJ_FILES) $(LDLIBS) $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

.PHONY: all clean fclean re
