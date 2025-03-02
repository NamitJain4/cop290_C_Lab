# Arguments (./sheet r c)
ARGS = r c # random value

# Input file
INPUT = input.txt

# Output file
OUTPUT = output.txt

# Expected output file
EXPECTED = expected_output.txt

.SILENT:

# Default command which will run
all: evaluator sheet
#	./sheet $(ARGS) < $(INPUT) > $(OUTPUT)
#	./evaluator $(OUTPUT) $(EXPECTED)

# compile the source files 
sheet: main.o print_sheet.o operations_implementation.o
	gcc -o $@ $^

evaluator: evaluator.o
	gcc -o $@ $^

%.o: %.c
    gcc -c $< -o $@	

# cleaning up the executbles produced
clean:
	rm -f sheet evaluator

.PHONY: all clean
