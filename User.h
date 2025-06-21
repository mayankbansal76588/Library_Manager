#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
using namespace std;

class User {
public:
    User(string name = "", string id = "", string userType = "", int borrowLimit = 3);

    string getName() const;
    string getId() const;
    string getUserType() const;
    int getBorrowLimit() const;

    void setBorrowLimit(int cnt);
    void borrowBook(const string &isbn);
    void returnBook(const string &isbn);
    bool hasBorrowed(const string &isbn) const;
    const vector<string> &getBorrowedBooks() const;

private:
    string name;
    string id;
    string userType;
    int borrowLimit;
    vector<string> borrowedBooks;
};

#endif

