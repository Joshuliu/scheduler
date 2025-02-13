CC = g++
# CFLAGS = -Wall -g
CFLAGS = -Wall -g -w
SRCDIR = source
OBJDIR = obj
BINDIR = bin
OUTPUTDIR = outputs
TARGET = $(BINDIR)/schedsim
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
INPUT_FILE = sample_input.txt

.PHONY: all clean clean-all test-FCFS test-RR test-MLFQ debug-MLFQ

# Default target to compile the program
all: $(TARGET)

# Rule to link object files and create the binary
$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -rf $(OBJDIR) $(BINDIR)
	rm -f $(OBJ)

# Test with FCFS (no quantum required)
test-FCFS: all
	@mkdir -p $(OUTPUTDIR)
	@$(TARGET) $(INPUT_FILE) $(OUTPUTDIR)/output_fcfs.txt 0

# Test with RR (requires QUANTUM)
test-RR: all
	@if [ -z "$(QUANTUM)" ]; then \
		echo "Error: Please provide QUANTUM (e.g., make test-RR QUANTUM=1)"; \
		exit 1; \
	fi
	@mkdir -p $(OUTPUTDIR)
	@$(TARGET) $(INPUT_FILE) $(OUTPUTDIR)/output_rr$(QUANTUM).txt 2 $(QUANTUM)

# Test with MLFQ (requires QUEUES, QUANTA, and AGING)
test-MLFQ: CFLAGS = -Wall -g
test-MLFQ: all
	@if [ -z "$(QUEUES)" ] || [ -z "$(QUANTA)" ] || [ -z "$(AGING)" ]; then \
		echo "Error: Please provide QUEUES, QUANTA, and AGING (e.g., make test-MLFQ QUEUES=3 QUANTA='1 2 3' AGING='8 16')"; \
		exit 1; \
	fi
	@mkdir -p $(OUTPUTDIR)
	@$(TARGET) $(INPUT_FILE) $(OUTPUTDIR)/output_mlfq$(QUEUES).txt 4 $(QUEUES) $(QUANTA) $(AGING)

# Evaluate MLFQ with fixed parameters
evaluate-MLFQ: CFLAGS = -Wall -g
evaluate-MLFQ: all
	@if [ -z "$(QUEUES)" ] || [ -z "$(QUANTA)" ] || [ -z "$(AGING)" ]; then \
		echo "Error: Please provide QUEUES, QUANTA, and AGING (e.g., make evaluate-MLFQ QUEUES=3 QUANTA='1 2 3' AGING='8 16')"; \
		exit 1; \
	fi
	@mkdir -p $(OUTPUTDIR)
	@echo "Running MLFQ with QUEUES=$(QUEUES), QUANTA=$(QUANTA), AGING=$(AGING)..."
	@$(TARGET) $(INPUT_FILE) $(OUTPUTDIR)/output_mlfq.txt 4 $(QUEUES) $(QUANTA) $(AGING)
	./sample_schedsim $(INPUT_FILE) sample_output.txt 4 $(QUEUES) $(QUANTA) $(AGING)
	@python3 compare_metrics.py $(OUTPUTDIR)/output_mlfq.txt sample_output.txt
	
# Test with MLFQ and debug enabled
debug: clean $(TARGET)
debug: CFLAGS += -DDEBUG

debug-MLFQ: debug
	@if [ -z "$(QUEUES)" ] || [ -z "$(QUANTA)" ] || [ -z "$(AGING)" ]; then \
		echo "Error: Please provide QUEUES, QUANTA, and AGING (e.g., make debug-MLFQ QUEUES=3 QUANTA='1 2 3' AGING='8 16')"; \
		exit 1; \
	fi
	@mkdir -p $(OUTPUTDIR)
	@$(TARGET) $(INPUT_FILE) $(OUTPUTDIR)/output_mlfq$(QUEUES).txt 4 $(QUEUES) $(QUANTA) $(AGING)
