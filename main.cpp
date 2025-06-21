#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
using namespace std;

class Book {
    string title, author, isbn;

public:
    Book(string t = "", string a = "", string i = "") : title(t), author(a), isbn(i) {}
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getISBN() const { return isbn; }
};

class User {
protected:
    string name, id;

public:
    User(string n = "", string i = "") : name(n), id(i) {}
    virtual ~User() {}
    virtual int borrowLimit() const = 0;

    string getName() const { return name; }
    string getID() const { return id; }
};

class Student : public User {
public:
    Student(string n, string i) : User(n, i) {}
    int borrowLimit() const override { return 3; }
};

class Teacher : public User {
public:
    Teacher(string n, string i) : User(n, i) {}
    int borrowLimit() const override { return 5; }
};

struct BookItem {
    Book book;
    int quantity;

    BookItem(const Book &b, int q) : book(b), quantity(q) {}
};

class Library {
    vector<BookItem> books;
    vector<User*> users;
    map<string, vector<Book*>> borrowedBooks;

public:
    ~Library() {
        for (auto u : users) delete u;
    }

    void loadData() {
        loadBooks();
        loadUsers();
        loadBorrowed();
    }

    void saveData() {
        saveBooks();
        saveUsers();
        saveBorrowed();
    }

    void addBook() {
        string title, author, isbn;
        int quantity;

        cout << "Enter book title: ";
        getline(cin, title);
        cout << "Enter book author: ";
        getline(cin, author);
        cout << "Enter book ISBN: ";
        getline(cin, isbn);
        cout << "Enter quantity: ";
        cin >> quantity;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        for (auto &item : books) {
            if (item.book.getISBN() == isbn) {
                item.quantity += quantity;
                cout << "Updated quantity for existing book.\n";
                saveBooks();
                return;
            }
        }

        books.push_back({ Book(title, author, isbn), quantity });
        cout << "Book added!\n";
        saveBooks();
    }

    void removeBook() {
        string isbn;

        cout << "Enter ISBN of book to remove: ";
        getline(cin, isbn);

        for (auto &pair : borrowedBooks) {
            for (auto *b : pair.second) {
                if (b->getISBN() == isbn) {
                    cout << "Cannot remove this book as it's currently borrowed.\n";
                    return;
                }
            }
        }

        for (auto it = books.begin(); it != books.end(); ++it) {
            if (it->book.getISBN() == isbn) {
                books.erase(it);
                cout << "Book removed!\n";
                saveBooks();
                return;
            }
        }

        cout << "Book not found.\n";
    }

    void addUser() {
        string name, id, userType;

        cout << "Enter user name: ";
        getline(cin, name);
        cout << "Enter user ID: ";
        getline(cin, id);
        cout << "Enter user type (Student(S)/Teacher(T)): ";
        getline(cin, userType);

        User *user = nullptr;

        if (userType == "Student" || userType == "student" || userType == "S") {
            user = new Student(name, id);
        } else if (userType == "Teacher" || userType == "teacher" || userType == "T") {
            user = new Teacher(name, id);
        } else {
            cout << "Invalid user type.\n";
            return;
        }

        users.push_back(user);
        cout << "User added!\n";
        saveUsers();
    }

    void listBooks() const {
        cout << "\n=== Book List ===\n";
        for (auto &item : books) {
            cout << "------------------------------\n";
            cout << "Title            : " << item.book.getTitle() << '\n';
            cout << "Author           : " << item.book.getAuthor() << '\n';
            cout << "ISBN             : " << item.book.getISBN() << '\n';
            cout << "Available Copies : " << item.quantity << '\n';
        }
        cout << "------------------------------\n";
    }

    void listUsers() const {
        cout << "\n=== User List ===\n";
        for (auto u : users) {
            string id = u->getID();
            int borrowed = borrowedBooks.count(id) ? borrowedBooks.at(id).size() : 0;

            cout << "------------------------------\n";
            cout << "Name              : " << u->getName() << '\n';
            cout << "ID                : " << u->getID() << '\n';
            cout << "Borrow Limit      : " << u->borrowLimit() << '\n';
            cout << "Currently Borrowed: " << borrowed << '\n';
            if (borrowedBooks.find(id) != borrowedBooks.end()) {
                cout << "Borrowed books:\n";
                for (auto b : borrowedBooks.at(id))
                    cout << " - " << b->getTitle() << " (ISBN: " << b->getISBN() << ")\n";
            }
        }
        cout << "------------------------------\n";
    }

    void borrowBook() {
        string userId, bookIsbn;

        cout << "Enter User ID: ";
        getline(cin, userId);
        cout << "Enter Book ISBN: ";
        getline(cin, bookIsbn);

        User *user = findUser(userId);
        BookItem *bookItem = findBook(bookIsbn);

        if (!user || !bookItem) {
            cout << "User or Book not found.\n";
            return;
        }

        auto &userList = borrowedBooks[userId];
        if ((int)userList.size() >= user->borrowLimit()) {
            cout << "Borrow limit reached.\n";
            return;
        }

        for (auto *b : userList) {
            if (b->getISBN() == bookIsbn) {
                cout << "User already borrowed this book.\n";
                return;
            }
        }

        if (bookItem->quantity <= 0) {
            cout << "No copies available.\n";
            return;
        }

        bookItem->quantity--;  
        userList.push_back(&(bookItem->book));
        cout << "Book borrowed!\n";
        saveData();
    }

    void returnBook() {
        string userId, bookIsbn;

        cout << "Enter User ID: ";
        getline(cin, userId);
        cout << "Enter Book ISBN: ";
        getline(cin, bookIsbn);

        if (borrowedBooks.find(userId) == borrowedBooks.end()) {
            cout << "User has no borrowed books.\n";
            return;
        }

        auto &userList = borrowedBooks[userId];
        for (auto it = userList.begin(); it != userList.end(); ++it) {
            if ((*it)->getISBN() == bookIsbn) {
                BookItem *bookItem = findBook(bookIsbn);
                if (bookItem) {
                    bookItem->quantity++;
                }
                userList.erase(it);
                cout << "Book returned!\n";
                saveData();
                return;
            }
        }

        cout << "Book not found in user's borrowed list.\n";
    }

private:
    User* findUser(const string &userId) {
        for (auto u : users) {
            if (u->getID() == userId) return u;
        }
        return nullptr;
    }

    BookItem* findBook(const string &isbn) {
        for (auto &item : books) {
            if (item.book.getISBN() == isbn) return &item;
        }
        return nullptr;
    }

    void loadBooks() {
        ifstream fin("books.txt");
        if (!fin.is_open()) {
            ofstream create("books.txt"); // create an empty file
            return;
        }

        books.clear();
        string line;

        while (getline(fin, line)) {
            istringstream ss(line);
            string title, author, isbn;
            int quantity;

            getline(ss, title, '|');
            getline(ss, author, '|');
            getline(ss, isbn, '|');
            ss >> quantity;

            books.push_back({ Book(title, author, isbn), quantity });
        }
    }

    void saveBooks() {
        ofstream fout("books.txt");
        for (auto &item : books) {
            fout << item.book.getTitle() << '|'
                 << item.book.getAuthor() << '|'
                 << item.book.getISBN() << '|'
                 << item.quantity << '\n';
        }
    }

    void loadUsers() {
        ifstream fin("users.txt");
        if (!fin.is_open()) {
            ofstream create("users.txt"); // create an empty file
            return;
        }

        users.clear();
        string line;

        while (getline(fin, line)) {
            istringstream ss(line);
            string name, id, userType;

            getline(ss, name, '|');
            getline(ss, id, '|');
            getline(ss, userType, '|');

            if (userType == "Student") {
                users.push_back(new Student(name, id));
            } else if (userType == "Teacher") {
                users.push_back(new Teacher(name, id));
            }
        }
    }

    void saveUsers() {
        ofstream fout("users.txt");
        for (auto u : users) {
            if (Student *s = dynamic_cast<Student*>(u))
                fout << u->getName() << '|' << u->getID() << "|Student\n";
            else if (Teacher *t = dynamic_cast<Teacher*>(u))
                fout << u->getName() << '|' << u->getID() << "|Teacher\n";
        }
    }

    void loadBorrowed() {
        ifstream fin("borrowed.txt");
        if (!fin.is_open()) {
            ofstream create("borrowed.txt"); // create an empty file
            return;
        }

        borrowedBooks.clear();
        string userId, bookIsbn;

        while (fin >> userId >> bookIsbn) {
            BookItem *bookItem = findBook(bookIsbn);
            if (bookItem) {
                borrowedBooks[userId].push_back(&(bookItem->book));
            }
        }
    }

    void saveBorrowed() {
        ofstream fout("borrowed.txt");
        for (auto &pair : borrowedBooks) {
            for (auto *book : pair.second) {
                fout << pair.first << " " << book->getISBN() << '\n';
            }
        }
    }
};

int main() {
    Library lib;
    lib.loadData();

    while (true) {
        cout << "\n=== Library Menu ===\n";
        cout << "1. Add Book\n2. Remove Book\n3. Add User\n4. List Books\n5. List Users\n6. Borrow Book\n7. Return Book\n0. Exit\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: lib.addBook(); break;
            case 2: lib.removeBook(); break;
            case 3: lib.addUser(); break;
            case 4: lib.listBooks(); break;
            case 5: lib.listUsers(); break;
            case 6: lib.borrowBook(); break;
            case 7: lib.returnBook(); break;
            case 0:
                cout << "Exiting.\n";
                lib.saveData();
                return 0;
            default:
                cout << "Invalid choice.\n"; break;
        }
    }
}
