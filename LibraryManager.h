#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "Book.h"
#include "User.h"
#include <vector>
#include <string>
using namespace std;

class LibraryManager {
public:
    void loadData();
    void saveData();
    void showMenu();

private:
    vector<Book> books;
    vector<User> users;

    void addBook();
    void removeBook();
    void listBooks() const;

    void addUser();
    void listUsers() const;

    void borrowBook();
    void returnBook();

    Book* findBook(const string &isbn);
    User* findUser(const string &userId);
};

#endif
