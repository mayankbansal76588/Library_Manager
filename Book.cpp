#include "Book.h"

Book::Book(string title, string author, string isbn, int copies) {
    this->title = title;
    this->author = author;
    this->isbn = isbn;
    this->copies = copies;
}

string Book::getTitle() const { return title; }
string Book::getAuthor() const { return author; }
string Book::getISBN() const { return isbn; }
int Book::getCopies() const { return copies; }
void Book::setCopies(int cnt) { copies = cnt; }
