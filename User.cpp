#include "User.h"

User::User(string name, string id, string userType, int borrowLimit) {
    this->name = name;
    this->id = id;
    this->userType = userType;
    this->borrowLimit = borrowLimit;
}

string User::getName() const { return name; }
string User::getId() const { return id; }
string User::getUserType() const { return userType; }
int User::getBorrowLimit() const { return borrowLimit; }

void User::setBorrowLimit(int cnt) { borrowLimit = cnt; }

void User::borrowBook(const string &isbn) {
    borrowedBooks.push_back(isbn);
}

void User::returnBook(const string &isbn) {
    for (auto it = borrowedBooks.begin(); it != borrowedBooks.end(); ++it) {
        if (*it == isbn) {
            borrowedBooks.erase(it);
            break;
        }
    }
}

bool User::hasBorrowed(const string &isbn) const {
    for (auto &b : borrowedBooks) {
        if (b == isbn) return true;
    }
    return false;
}

const vector<string> &User::getBorrowedBooks() const {
    return borrowedBooks;
}
