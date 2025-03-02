# Arguments (./spreadsheet r c)
ARGS = r c # random value

# Input file
INPUT = input.txt

# Output file
OUTPUT = output.txt

# Expected output file
EXPECTED = expected_output.txt

# Target directory
TARGET_DIR = target/release

.SILENT:

# Default command which will run
all: spreadsheet evaluator move

# Compile the source files
spreadsheet: main.o print_sheet.o operations_implementation.o
	gcc -o $@ $^ -lm  # Added -lm for math library

evaluator: evaluator.o
	gcc -o $@ $^

%.o: %.c
	gcc -c $< -o $@

# Ensure the target directory exists and move executables
move: | $(TARGET_DIR)
	mv spreadsheet $(TARGET_DIR)/
	mv evaluator $(TARGET_DIR)/

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# Cleaning up the executables and object files
clean:
	rm -f $(TARGET_DIR)/spreadsheet $(TARGET_DIR)/evaluator *.o  # Added *.o to clean object files too

.PHONY: all clean move
