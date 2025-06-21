#ifndef BOOK_H
#define BOOK_H

#include <string>
using namespace std;

class Book {
public:
    Book(string title = "", string author = "", string isbn = "", int copies = 0);
    string getTitle() const;
    string getAuthor() const;
    string getISBN() const;
    int getCopies() const;

    void setCopies(int cnt);

private:
    string title;
    string author;
    string isbn;
    int copies;
};

#endif
