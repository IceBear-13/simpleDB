// main.cpp
#include <iostream>
#include "../includes/query_processor.h"
#include "../includes/storage.h"


int main() {
    std::cout << "SimpleDBMS - A minimal DBMS written in C++\n";
    std::cout << "Type 'EXIT' to quit, 'HELP' for commands\n";
    
    Storage storage("./data");
    QueryProcessor processor(storage);
    
    std::string input;
    while (true) {
        std::cout << "simpledb> ";
        std::getline(std::cin, input);
        
        if (input == "EXIT" || input == "exit") {
            break;
        } else if (input == "HELP" || input == "help") {
            std::cout << "Supported commands:\n";
            std::cout << "  CREATE TABLE table_name\n";
            std::cout << "  INSERT INTO table_name VALUES (val1, val2, ...)\n";
            std::cout << "  SELECT * FROM table_name\n";
        } else if (!input.empty()) {
            processor.execute(input);
        }
    }
    
    return 0;
}