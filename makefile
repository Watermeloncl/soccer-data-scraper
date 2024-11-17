all: writer adder parser

writer: write.c
	gcc -o writer write.c fillCommands.c
adder: adder.c
	gcc -o adder adder.c
parser: parser.c
	gcc -o parser parser.c

clean:
	rm -f writer
	rm -f adder
	rm -f parser
