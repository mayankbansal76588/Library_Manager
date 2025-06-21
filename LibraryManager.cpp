#include "LibraryManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
using namespace std;

void LibraryManager::loadData() {
    struct stat buffer;
    if (stat("books.txt", &buffer) != 0) {
        ofstream("books.txt"); // create if not present
    }
    if (stat("users.txt", &buffer) != 0) {
        ofstream("users.txt"); // create if not present
    }
    if (stat("borrowed.txt", &buffer) != 0) {
        ofstream("borrowed.txt"); // create if not present
    }

    ifstream bookFile("books.txt");
    string line;
    while (getline(bookFile, line)) {
        string title, author, isbn;
        int copies;
        stringstream ss(line);
        getline(ss, title, '|');
        getline(ss, author, '|');
        getline(ss, isbn, '|');
        ss >> copies;
        books.emplace_back(title, author, isbn, copies);
    }
    bookFile.close();

    ifstream userFile("users.txt");
    while (getline(userFile, line)) {
        string name, id, userType;
        int borrowLimit;
        stringstream ss(line);
        getline(ss, name, '|');
        getline(ss, id, '|');
        getline(ss, userType, '|');
        ss >> borrowLimit;
        users.emplace_back(name, id, userType, borrowLimit);
    }
    userFile.close();

    ifstream borrowedFile("borrowed.txt");
    while (getline(borrowedFile, line)) {
        string userId, isbn;
        stringstream ss(line);
        ss >> userId >> isbn;

        User* user = findUser(userId);
        if (user) {
            user->borrowBook(isbn);
        }
    }
    borrowedFile.close();
}

void LibraryManager::saveData() {
    ofstream bookFile("books.txt");
    for (auto &b : books) {
        bookFile << b.getTitle() << "|" << b.getAuthor() << "|"
                 << b.getISBN() << "|" << b.getCopies() << '\n';
    }
    bookFile.close();

    ofstream userFile("users.txt");
    for (auto &u : users) {
        userFile << u.getName() << "|" << u.getId() << "|"
                 << u.getUserType() << "|" << u.getBorrowLimit() << '\n';
    }
    userFile.close();

    ofstream borrowedFile("borrowed.txt");
    for (auto &user : users) {
        for (auto &isbn : user.getBorrowedBooks()) {
            borrowedFile << user.getId() << " " << isbn << '\n';
        }
    }
    borrowedFile.close();
}

void LibraryManager::showMenu() {
    while (true) {
        cout << "========================\n";
cout << "       Library Menu     \n";
cout << "========================\n";
cout << "1. Add Book\n";
cout << "2. Remove Book\n";
cout << "3. Add User\n";
cout << "4. List Books\n";
cout << "5. List Users\n";
cout << "6. Borrow Book\n";
cout << "7. Return Book\n";
cout << "0. Exit\n";
cout << "------------------------\n";
cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: addBook(); break;
            case 2: removeBook(); break;
            case 3: addUser(); break;
            case 4: listBooks(); break;
            case 5: listUsers(); break;
            case 6: borrowBook(); break;
            case 7: returnBook(); break;
            case 0:
                saveData();
                cout<< "Exiting the library system.\n";
                return;
            default:
                cout << "Invalid choice!\n";
        }
    }
}

void LibraryManager::addBook() {
    string title, author, isbn;
    int copies;
    cout << "Enter title: "; cin.ignore();
    getline(cin, title);
    cout << "Enter author: "; getline(cin, author);
    cout << "Enter ISBN: "; getline(cin, isbn);
    cout << "Enter number of copies: "; cin >> copies;

    books.emplace_back(title, author, isbn, copies);
    cout << "Book added!\n";
}

void LibraryManager::removeBook() {
    string isbn;
    cout << "Enter ISBN of book to remove: ";
    cin >> isbn;

    for (auto it = books.begin(); it != books.end(); ++it) {
        if (it->getISBN() == isbn) {
            books.erase(it);
            cout << "Book removed!\n";
            return;
        }
    }
    cout << "Book not found.\n";
}

void LibraryManager::listBooks() const {
    cout << "===== Book List =====\n";
    for (auto &b : books) {
        cout << "Title : " << b.getTitle() << '\n';
        cout << "Author: " << b.getAuthor() << '\n';
        cout << "ISBN  : " << b.getISBN() << '\n';
        cout << "Copies: " << b.getCopies() << '\n';
        cout << "-----------------------------\n";
    }
}


void LibraryManager::addUser() {
    cout << "Enter User Name: ";
    string name;
    cin.ignore();
    getline(cin, name);

    cout << "Enter User ID: ";
    string id;
    getline(cin, id);

    cout << "Enter User Type (student/faculty): ";
    string userType;
    getline(cin, userType);

    // Set borrow limit based on user type
    int borrowLimit = 0;
    if (userType == "student") {
        borrowLimit = 3;
    } else if (userType == "faculty") {
        borrowLimit = 5;
    } else {
        cout << "Invalid user type! Must be 'student' or 'faculty'.\n";
        return;
    }

    User newUser(name, id, userType, borrowLimit);
    users.push_back(newUser);
    //saveUsers();
    cout << "User added!\n";
}

void LibraryManager::listUsers() const {
    cout << "===== User List =====\n";
    for (auto &user : users) {
        cout << "Name       : " << user.getName() << '\n';
        cout << "ID         : " << user.getId() << '\n';
        cout << "User Type  : " << user.getUserType() << '\n';
        cout << "Borrow Left: " << user.getBorrowLimit() << '\n';
        cout << "Borrowed Books:\n";
        for (auto &isbn : user.getBorrowedBooks()) {
            cout << "  - " << isbn << '\n';
        }
        cout << "-----------------------------\n";
    }
}


void LibraryManager::borrowBook() {
    string userId, isbn;
    cout << "Enter user ID: "; cin >> userId;
    cout << "Enter book ISBN: "; cin >> isbn;

    User* user = findUser(userId);
    Book* book = findBook(isbn);

    if (!user) {
        cout << "Invalid user ID.\n"; return;
    }
    if (!book) {
        cout << "Invalid book ISBN.\n"; return;
    }
    if (user->getBorrowLimit() <= 0) {
        cout << "Borrow limit exceeded.\n"; return;
    }
    if (user->hasBorrowed(isbn)) {
        cout << "User has already borrowed this book.\n"; return;
    }
    if (book->getCopies() <= 0) {
        cout << "No copies available.\n"; return;
    }

    book->setCopies(book->getCopies() - 1);
    user->setBorrowLimit(user->getBorrowLimit() - 1);
    user->borrowBook(isbn);
    cout << "Book borrowed successfully!\n";
}

void LibraryManager::returnBook() {
    string userId, isbn;
    cout << "Enter user ID: "; cin >> userId;
    cout << "Enter book ISBN: "; cin >> isbn;

    User* user = findUser(userId);
    Book* book = findBook(isbn);

    if (!user || !book) {
        cout << "Invalid user or book.\n"; return;
    }
    if (!user->hasBorrowed(isbn)) {
        cout << "User has not borrowed this book.\n"; return;
    }

    book->setCopies(book->getCopies() + 1);
    user->setBorrowLimit(user->getBorrowLimit() + 1);
    user->returnBook(isbn);
    cout << "Book returned successfully!\n";
}

Book* LibraryManager::findBook(const string &isbn) {
    for (auto &book : books) {
        if (book.getISBN() == isbn) {
            return &book;
        }
    }
    return nullptr;
}

User* LibraryManager::findUser(const string &userId) {
    for (auto &user : users) {
        if (user.getId() == userId) {
            return &user;
        }
    }
    return nullptr;
}
