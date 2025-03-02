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

TEXFILE = report
PDFFILE = $(TEXFILE).pdf

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

test: parser_test unittest_printsheet testcases_op_impl
	./parser_test
	./unittest_printsheet
	./testcases_op_impl

parser_test: parser_test.c
	gcc -o parser_test parser_test.c

unittest_printsheet: unittest_printsheet.c 
	gcc -o unittest_printsheet unittest_printsheet.c 

testcases_op_impl: testcases_op_impl.c 
	gcc -o testcases_op_impl testcases_op_impl.c 

report: $(PDFFILE)

# Compile LaTeX file into PDF
$(PDFFILE): $(TEXFILE).tex
	pdflatex $(TEXFILE).tex
	pdflatex $(TEXFILE).tex  

# Ensure the target directory exists and move executables
move: | $(TARGET_DIR)
	mv spreadsheet $(TARGET_DIR)/
	mv evaluator $(TARGET_DIR)/

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# Cleaning up the executables and object files
clean:
	rm -f $(TARGET_DIR)/spreadsheet $(TARGET_DIR)/evaluator *.o  # Added *.o to clean object files too
	rm -f $(PDFFILE) parser_test unittest_printsheet testcases_op_impl

.PHONY: all clean move test report
