#include "function.h"

// Function to remove commas from a string
void removeComma(string &command){
    string line = "";
    bool insideQuotes = false;

    for (char letter : command){
        if (letter == '"')
            insideQuotes = !insideQuotes;

        if (!insideQuotes && letter == ',')
            line += ' ';
        else
            line += letter;
    }

    command = line;
}

// Function to split a string into words based on space or comma
vector<string> splitLine(string line) {
  vector<string> words;
  string currentWord;

  // Iterate through each character in the input line
  for (char c : line) {
    if (c == ' ' || c == ',') {
      // Found a space or comma, consider the current word as complete
      if (!currentWord.empty()) {
        words.push_back(currentWord);
        currentWord.clear();  // Reset current word for the next word
      }
    } else {
      // Add the character to the current word
      currentWord += c;
    }
  }

  // If there's a word at the end of the line, add it
  if (!currentWord.empty()) {
    words.push_back(currentWord);
  }

  return words;
}

void printRegisters(ostream &output, string registers[]){
    output << "Registers: ";

    for (int i = 0; i < REGISTER_SIZE; i++){
        try{
            int r0 = stoi(registers[i]);
            if (r0 == 0 && i != REGISTER_SIZE - 1){
                output << setw(4) << setfill('0') << r0 << " ";
            }
            else{
                output << r0 << " ";
            }
            registers[i] = to_string(r0);
        }
        catch (const invalid_argument &){
            output << registers[i] << setw(4) << setfill('0') << "0";
            registers[i] = "0";
        }
    }

    output << "#\n";
}

// Function to print flags and PC
void printFlagsAndPC(ostream &output){
    output << "Flags    : 0 0 0 0 #" << endl;
    output << "PC       : 25" << endl;
}

// Function to print memory
void printMemory(ostream &output, const vector<int> &memory){
    vector<int> M(64, 0);
    for (int i = 0; i < MEMORY_SIZE; i++)
        M[i] = memory[i];

    output << "Memory:" << endl;
    for (int i = 0; i < MEMORY_SIZE; i++){
        if (M[i] == 0 && (i % 8) == 0 && i == 0){
            output << setw(4) << setfill('0') << M[i] << " ";
        }
        else if (M[i] == 0 && (i % 8) == 0 && i != 0){
            output << endl;
            output << setw(4) << setfill('0') << M[i] << " ";
        }
        else if (M[i] == 0 && (i % 8) != 0){
            output << setw(4) << setfill('0') << M[i] << " ";
        }
        else if (M[i] > 0 && (i % 8) != 0){
            output << setw(4) << setfill(' ') << M[i] << " ";
        }
    }

    output << endl
           << "#\n";
}

int main(){
    // Initialize registers and memory
    for (int count = 0; count < REGISTER_SIZE; count++)
        registers[count] = " ";

    ifstream input;

    string line;

    // Open input and output files
     input.open("filleInput4.asm");
    if (!input.is_open()){
        cerr << "Error: Unable to open input file." << endl;
        return 1; // Return an error code
    }
    output.open("fileOutput2.txt");



    // Process each line in the input file
    while (getline(input, line)){
        // Clean up commas in the line
        removeComma(line);
        // Split the line into individual words (commands)
        vector<string> command = splitLine(line);

        if (!command.empty()){
            cout << line << endl;

            // Execute the command and update the state
            execute(command);

            // Display the updated state
            displayRegisters();
            displayFlags();
            displayMemory();

            // Reset flags after display
            for (int i = 0; i < FLAGS_SIZE; i++)
                flags[i] = 0;

            cout << endl;
        }
        else{
            cerr << "Error: Invalid command on line " << counter << endl;
        }
    }

    printRegisters(output, registers);
    printFlagsAndPC(output);
    printMemory(output, vector<int>(memory, memory + MEMORY_SIZE));


    // Close input and output files
    input.close();
    output.close();
    return 0;
}
