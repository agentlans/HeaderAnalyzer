#ifndef BOOK_LIBRARY_H
#define BOOK_LIBRARY_H

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

// Constants
const int MAX_TITLE_LENGTH = 100;
const int MAX_AUTHOR_LENGTH = 50;
const int MAX_BOOKS = 100;

// Enum for book genres
typedef enum {
    FICTION,
    NON_FICTION,
    MYSTERY,
    FANTASY,
    SCIENCE_FICTION,
    BIOGRAPHY,
    HISTORY,
    ROMANCE,
    OTHER
} Genre;

// Struct for a book
typedef struct {
    char title[MAX_TITLE_LENGTH];
    char author[MAX_AUTHOR_LENGTH];
    Genre genre;
    int year_published;
} Book;

// Typedef for a collection of books
typedef struct {
    Book books[MAX_BOOKS];
    int count;
} BookCollection;

// Function declarations
void initializeCollection(BookCollection *collection);
int addBook(BookCollection *collection, const char *title, const char *author, Genre genre, int year);
void printBooks(const BookCollection *collection);
void findBooksByAuthor(const BookCollection *collection, const char *author);

BookCollection shelf[];

// Global variable
extern BookCollection globalBookCollection;

#endif // BOOK_LIBRARY_H
