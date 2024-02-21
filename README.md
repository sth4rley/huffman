# Text Compression Project using Huffman Algorithm
I developed this project as part of the Algorithms and Data Structures I course.

## Description
The objective of this project is to implement the Huffman algorithm for text compression. The Huffman algorithm is an efficient compression technique that reduces the size of data without significant loss of information. It utilizes a coding tree where more frequent symbols are assigned shorter binary codes, resulting in efficient and space-saving text compression.

## Usage
```bash
g++ huffman.cpp -o huffman
./huffman [input_file_name] # If no input file name is provided, the program will default to "input.txt".
```

Output file: [input_file_name].huff