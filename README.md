# Bookstore Management System

A simple console-based Bookstore Management System developed in C as an introductory programming project.

The program allows users to manage a collection of books using arrays, functions, and file handling.

## Features

- Add new books with a bin number, name, and price.
- Remove books using their bin numbers.
- Search for books by bin number.
- Display all books in a formatted table.
- Automatically sort books by bin number.
- Save and load book information using a text file.
- Store up to 100 books.
- Validate user input and prevent duplicate bin numbers.

## Technologies Used

- C Programming Language
- Standard C Libraries
- File Handling

## How to Run

**1. Compile the program using GCC:**

```bash
gcc -std=c99 -Wall -Wextra main.c -o bookstore
```

**2. Run the program:**

Linux / macOS:

```bash
./bookstore
```

Windows (PowerShell):

```powershell
.\bookstore.exe
```

## Menu Options

1. Add a Book
2. Remove a Book
3. Search for a Book
4. Print Book List
5. Exit System

## Data Storage

The program stores book information in `books.txt`.

If the file does not exist, the program starts with an empty bookstore and creates the file when saving data upon exit.

Each record follows this format:

```text
Book Name  Bin Number  Price
```

Example:

```text
Harry Potter  5 12.50
```

Book names can contain spaces, with a maximum length of 49 characters.

If invalid records are detected during loading, the program attempts to create a backup named `books.txt.bak` and disables saving to protect the original file.

## Learning Objectives

This project demonstrates fundamental C programming concepts, including:

- Arrays and pointers
- Functions and loops
- Conditional statements
- Searching and sorted insertion
- File input and output
- Basic input validation and error handling

## Project Type

Introductory C Programming Project.
