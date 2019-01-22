# Makefile #

# Compilation #
CC = mpic++
EXECUTABLE = exec
CPPFLAGS = -Wall -Wextra -pedantic -g -std=c++14 -lpthread -Wreorder
LDFLAGS = -lglfw3 -lGLU -lGL -lglut -lX11 -lpthread -lXrandr -lXi -lXxf86vm -ldl -lm -lXinerama -lXcursor -Wno-literal-suffix -std=c++14 -lsfml-graphics -lsfml-window -lsfml-system
LIBS = 

# Directory #
OBJDIR = bin
SRCDIR = src
INCDIR = include

# Finding #
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRC:.cpp=.o)
OBJS := $(subst src/,,$(OBJS))
OBJS := $(addprefix $(OBJDIR)/, $(OBJS))


# Rules
all : $(EXECUTABLE)

$(EXECUTABLE) : $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@\


$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -I $(INCDIR) -o $@ -c $<

clean:
	@rm -rf $(OBJDIR)/*.o
	@rm -f $(EXECUTABLE)
	@rm -rf $(SRCDIR)/*.cpp~
	@rm -rf $(INCDIR)/*.h~
	@rm -rf $(OBJDIR)/*.o~
	@rm -rf *~
