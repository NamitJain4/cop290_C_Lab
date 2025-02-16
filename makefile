# Arguments (./sheet r c)
ARGS = r c

# Input file
INPUT = input.txt

# Output file
OUTPUT = output.txt

# Expected output file
EXPECTED = expected_output.txt

.SILENT:

# Default command which will run
all: evaluator sheet
	./sheet $(ARGS) < $(INPUT) > $(OUTPUT)
	./evaluator $(OUTPUT) $(EXPECTED)

# compile the source files 
sheet: main.c
	gcc -o sheet main.c

evaluator: evaluator.c
	gcc -o evaluator evaluator.c

# cleaning up the executbles produced
clean:
	rm -f sheet evaluator

.PHONY: all clean
