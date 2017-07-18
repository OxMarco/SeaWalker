###############################################################################
#
# Tests Makefile
#
###############################################################################


INTEGRATION_TEST_EXEC	= ais_integration-tests.run

###############################################################################
# Files
###############################################################################

SRC = $(CORE_SRC) $(HARDWARE_NODES_SRC) $(HARDWARE_SERVICES_SRC) $(CANAIS_INTEGRATION_TEST) WorldState/CollidableMgr/CollidableMgr.cpp 

# Object files
OBJECTS = $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))


###############################################################################
# Rules
###############################################################################


all: $(INTEGRATION_TEST_EXEC) stats

# Link and build
$(INTEGRATION_TEST_EXEC): $(OBJECTS)
	rm -f $(OBJECT_FILE)
	@echo -n " " $(OBJECTS) >> $(OBJECT_FILE)
	@echo Linking object files
	$(CXX) $(LDFLAGS) @$(OBJECT_FILE) -Wl,-rpath=./ -o $@ $(LIBS)

# Compile CPP files into the build folder
$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo Compiling CPP File: $@

	@$(CXX) -c $(CPPFLAGS) $(INC_DIR) -o ./$@ $< $(DEFINES) $(LIBS)

stats:$(INTEGRATION_TEST_EXEC)
	@echo Final executable size:
	$(SIZE) $(INTEGRATION_TEST_EXEC)
