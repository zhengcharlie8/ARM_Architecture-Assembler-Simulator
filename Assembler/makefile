EXE := assembler-sim

SRCDIR := src
OBJDIR := .obj

$(shell mkdir -p $(OBJDIR))

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.d)

CXXFLAGS := --std=c++14

all : $(EXE)

$(EXE) : $(OBJS)
	$(CXX) $^ -o $@

clean : 
	$(RM) -rf $(OBJDIR) $(EXE)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)
