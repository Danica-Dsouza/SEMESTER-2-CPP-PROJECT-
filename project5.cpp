/*
 * ============================================================
 *  Community Help Desk and Resource Allocation System
 *  Course : Programming in C (DBT7288) - C++ Enhancement
 *  Features: Full CRUD, File I/O, STL, OOP, Exception Handling
 * ============================================================
 */
 
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <limits>
 
using namespace std;
 
// ============================================================
//  CONSTANTS
// ============================================================
const string FILE_BENEFICIARIES = "beneficiaries.txt";
const string FILE_VOLUNTEERS    = "volunteers.txt";
const string FILE_RESOURCES     = "resources.txt";
const string FILE_ALLOCATIONS   = "allocations.txt";
 
// ============================================================
//  CUSTOM EXCEPTIONS
// ============================================================
class DuplicateIDException : public runtime_error {
public:
    explicit DuplicateIDException(int id)
        : runtime_error("ID " + to_string(id) + " already exists!") {}
};
 
class RecordNotFoundException : public runtime_error {
public:
    explicit RecordNotFoundException(int id)
        : runtime_error("Record with ID " + to_string(id) + " not found!") {}
};
 
class InsufficientStockException : public runtime_error {
public:
    InsufficientStockException(int avail, int req)
        : runtime_error("Insufficient stock! Available: " + to_string(avail)
                        + ", Requested: " + to_string(req)) {}
};
 
class InvalidInputException : public runtime_error {
public:
    explicit InvalidInputException(const string& msg)
        : runtime_error("Invalid input: " + msg) {}
};
 
// ============================================================
//  UTILITY FUNCTIONS
// ============================================================
void separator() {
    cout << "-------------------------------------------------------------\n";
}

/* 
u can pass null in * 
but & must be passed with an object reference (cannot be null)
called reference
if reference & , then pass variable normally
if pointer * , then  & address 
*/


void header(const string& title) {
    separator();
    cout << "  " << title << "\n";
    separator();
}
 
void pause() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

/*
void pause() {
“Ignore everything in the input buffer until you find a newline (\n).
But don’t stop unless you’ve ignored the maximum possible characters.”
getline ususally used every cin 
puase used to wait for user to press enter before going back to menu

*/
 
string readNonEmpty(const string& prompt) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (!s.empty()) return s;
        cout << "  [!] Input cannot be empty!\n";
    }
}
 
int readPositiveInt(const string& prompt) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        try {
            int val = stoi(line);
            if (val > 0) return val;
        } catch (...) {}  // used to prevent program from crashing
        // invalid argument , out of range 
        cout << "  [!] Please enter a positive integer.\n";
    }
}
 
int readNonNegInt(const string& prompt) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        try {
            int val = stoi(line);
            if (val >= 0) return val;
        } catch (...) {}
        cout << "  [!] Please enter a non-negative integer.\n";
    }
}
 
int readIntInRange(const string& prompt, int lo, int hi) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        try {
            int val = stoi(line);
            if (val >= lo && val <= hi) return val;
        } catch (...) {}
        cout << "  [!] Enter a number between " << lo << " and " << hi << ".\n";
    }
}
 
string priorityLabel(int p) {
    if (p == 1) return "Urgent";
    if (p == 2) return "Normal";
    return "Low";
}
 
// ============================================================
//  ABSTRACT BASE CLASS: Person
// ============================================================
class Person {
protected:
    int    id;
    string name;
    string phone;
public:
    Person() : id(0) {}  // initalise before constructor of derived class is called
    //This is initialization + assignment (two steps).  wheninside {}
    Person(int i, const string& n, const string& p)
        : id(i), name(n), phone(p) {}
    virtual ~Person() = default;
    /*
    If the destructor is not virtual, only Person’s destructor runs.
The derived class (Beneficiary, Volunteer, etc.) 
destructor will NOT run → memory leak.

cannot override 
I don’t want to write the function 
want to declare a function but dont wnat ot write 

virtual void display() const = 0;
inside the fucntion u cannot do any assigmnets 
const means:

“This function will NOT modify any member variables.”


Just use the built‑in version
*/
 
    int           getId()   const { return id;    }   // getter funct
    string        getName() const { return name;  }   // const not to change does not modify the object it belongs to
    string        getPhone()const { return phone; }
 
    virtual void   display()   const = 0;
    virtual string serialize() const = 0;  
    //serialize() converts the object into a string format for saving:
};
 
// ============================================================
//  BENEFICIARY
// ============================================================
class Beneficiary : public Person {
    // private by default
    string address;
    string needCategory;
    int    familySize;
    int    priorityLevel;   // 1=Urgent, 2=Normal, 3=Low
public:
    Beneficiary() : familySize(0), priorityLevel(2) {}
    // automatically calls default constuctor of base class 
    Beneficiary(int i, const string& n, const string& ph,
                const string& addr, const string& cat,
                int fam, int pri)
        : Person(i, n, ph), address(addr), needCategory(cat),
          familySize(fam), priorityLevel(pri) {}
 
    string getNeedCategory() const { return needCategory; }
    int    getPriority()     const { return priorityLevel; }
    int    getFamilySize()   const { return familySize;    }
 
    void display() const override {
        cout << left
             << "  ID        : " << id            << "\n"
             << "  Name      : " << name          << "\n"
             << "  Contact   : " << phone         << "\n"
             << "  Address   : " << address       << "\n"
             << "  Category  : " << needCategory  << "\n"
             << "  Family Sz : " << familySize    << "\n"
             << "  Priority  : " << priorityLabel(priorityLevel) << "\n";
    }
 
    string serialize() const override {
        return to_string(id)           + "|" + name         + "|" +
               phone                   + "|" + address      + "|" +
               needCategory            + "|" +
               to_string(familySize)   + "|" +
               to_string(priorityLevel);
    }
 
    static Beneficiary deserialize(const string& line) {
        vector<string> t;
        string tok;
        stringstream ss(line);  // stringstream allows us to treat a string like a stream (like cin or file stream)
        while (getline(ss, tok, '|')) t.push_back(tok);
        if (t.size() < 7) throw runtime_error("Corrupt beneficiary record.");  
        return Beneficiary(stoi(t[0]), t[1], t[2], t[3], t[4],
                           stoi(t[5]), stoi(t[6]));
    }
    // means belong to class and not to object 
    //Beneficiary::deserialize(line);
    //It converts one line of text (from a file) back into a Beneficiary object.

};

//So throw is used to stop the current function
// and jump to the nearest catch block.

/*looks for a catch block in the caller function

if not found, it goes up another level

and another

until it finds a matching catch*/
 
// ============================================================
//  VOLUNTEER
// ============================================================
class Volunteer : public Person {
    string skill;
    bool   isAvailable;
public:
    Volunteer() : isAvailable(true) {}
    Volunteer(int i, const string& n, const string& ph,
              const string& sk, bool av)
        : Person(i, n, ph), skill(sk), isAvailable(av) {}
 
    string getSkill()       const { return skill;       }
    bool   getAvailable()   const { return isAvailable; }
 
    void display() const override {
        cout << left
             << "  ID        : " << id                              << "\n"
             << "  Name      : " << name                            << "\n"
             << "  Contact   : " << phone                           << "\n"
             << "  Skill     : " << skill                           << "\n"
             << "  Available : " << (isAvailable ? "Yes" : "No")   << "\n";
    }
 
    string serialize() const override {
        return to_string(id) + "|" + name  + "|" + phone + "|" +
               skill         + "|" + (isAvailable ? "1" : "0");
    }
 
    static Volunteer deserialize(const string& line) {
        vector<string> t;
        string tok;
        stringstream ss(line);
        while (getline(ss, tok, '|')) t.push_back(tok);
        if (t.size() < 5) throw runtime_error("Corrupt volunteer record.");
        return Volunteer(stoi(t[0]), t[1], t[2], t[3], t[4] == "1");
    }
};
 
// ============================================================
//  RESOURCE
// ============================================================
class Resource {
    int    itemId;
    string itemName;
    string category;
    int    quantityAvailable;
    int    reorderLevel;
public:
    Resource() : itemId(0), quantityAvailable(0), reorderLevel(0) {}
    Resource(int id, const string& name, const string& cat,
             int qty, int reorder)
        : itemId(id), itemName(name), category(cat),
          quantityAvailable(qty), reorderLevel(reorder) {}
 
    int    getId()       const { return itemId;            }
    string getName()     const { return itemName;          }
    string getCategory() const { return category;          }
    int    getQuantity() const { return quantityAvailable; }
    int    getReorder()  const { return reorderLevel;      }
    bool   isLowStock()  const { return quantityAvailable <= reorderLevel; }
 
    void deduct(int qty) {
        if (qty > quantityAvailable)
            throw InsufficientStockException(quantityAvailable, qty);
        quantityAvailable -= qty;
    }
 
    void restock(int qty) {
        if (qty <= 0) throw InvalidInputException("Restock quantity must be positive.");
        quantityAvailable += qty;
    }
 
    void display() const {
        cout << left
             << "  Item ID   : " << itemId            << "\n"
             << "  Name      : " << itemName          << "\n"
             << "  Category  : " << category          << "\n"
             << "  Qty Avail : " << quantityAvailable << "\n"
             << "  Reorder @ : " << reorderLevel
             << (isLowStock() ? "  [!] LOW STOCK" : "")   << "\n";
    }
 
    string serialize() const {
        return to_string(itemId)            + "|" + itemName  + "|" +
               category                     + "|" +
               to_string(quantityAvailable) + "|" +
               to_string(reorderLevel);
    }
 
    static Resource deserialize(const string& line) {
        vector<string> t;
        string tok;
        stringstream ss(line);
        while (getline(ss, tok, '|')) t.push_back(tok);
        if (t.size() < 5) throw runtime_error("Corrupt resource record.");
        return Resource(stoi(t[0]), t[1], t[2], stoi(t[3]), stoi(t[4]));
    }
};
 
// ============================================================
//  ALLOCATION
// ============================================================
class Allocation {
    int    allocationId;
    int    beneficiaryId;
    int    itemId;
    int    requestedQty;
    int    allocatedQty;
    string allocationDate;
    string status;   // "Success" | "Failed"
public:
    Allocation() : allocationId(0), beneficiaryId(0), itemId(0),
                   requestedQty(0), allocatedQty(0), status("Failed") {}
    Allocation(int aid, int bid, int iid, int rq, int aq,
               const string& date, const string& st)
        : allocationId(aid), beneficiaryId(bid), itemId(iid),
          requestedQty(rq), allocatedQty(aq),
          allocationDate(date), status(st) {}
 
    int    getAllocId()   const { return allocationId;  }
    int    getBenefId()  const { return beneficiaryId; }
    int    getItemId()   const { return itemId;         }
    int    getAllocQty() const { return allocatedQty;   }
    string getStatus()  const { return status;          }
 
    bool isFailed() const { return allocatedQty == 0; }
 
    void display() const {
        cout << left
             << "  Alloc ID  : " << allocationId  << "\n"
             << "  Benef ID  : " << beneficiaryId << "\n"
             << "  Item ID   : " << itemId        << "\n"
             << "  Requested : " << requestedQty  << "\n"
             << "  Allocated : " << allocatedQty  << "\n"
             << "  Date      : " << allocationDate<< "\n"
             << "  Status    : " << status        << "\n";
    }
 
    string serialize() const {
        return to_string(allocationId)  + "|" +
               to_string(beneficiaryId) + "|" +
               to_string(itemId)        + "|" +
               to_string(requestedQty)  + "|" +
               to_string(allocatedQty)  + "|" +
               allocationDate           + "|" + status;
    }
 
    static Allocation deserialize(const string& line) {
        vector<string> t;
        string tok;
        stringstream ss(line);
        while (getline(ss, tok, '|')) t.push_back(tok);
        if (t.size() < 7) throw runtime_error("Corrupt allocation record.");
        return Allocation(stoi(t[0]), stoi(t[1]), stoi(t[2]),
                          stoi(t[3]), stoi(t[4]), t[5], t[6]);
    }
};
 
// ============================================================
//  GENERIC REPOSITORY TEMPLATE
// ============================================================
template<typename T>
class Repository {
protected:
    vector<T> records;
public:
    void         add(const T& item)  { records.push_back(item); }
    void         clear()             { records.clear(); }
    vector<T>&   getAll()            { return records; }
    const vector<T>& getAll() const  { return records; }
    size_t       count()   const     { return records.size(); }
    bool         empty()   const     { return records.empty(); }
};

// wont store object only if called 

 
// ============================================================
//  BENEFICIARY MANAGER
// ============================================================
class BeneficiaryManager : public Repository<Beneficiary> {
public:
    bool idExists(int id) const {
        return any_of(records.begin(), records.end(),
                      [id](const Beneficiary& b){ return b.getId() == id; });
    }
    /*
    records.begin() returns an iterator pointing to the first element of the vector.
    records.end() returns an iterator pointing just past
    the last element of the vector (not a valid element, but a marker for the end).
    lanbda function is an anonymous function that can capture variables from the surrounding scope.
    [id] captures the id variable by value, allowing us to use it inside the lambda.
    beneficiary& b is the parameter representing each element in the vector as we iterate through it.
    
    */
 
    Beneficiary* find(int id) {
        auto it = find_if(records.begin(), records.end(),
                          [id](const Beneficiary& b){ return b.getId() == id; });
        if (it == records.end()) return nullptr;
        return &(*it);
    }
 
    void addBeneficiary() {
        header("ADD BENEFICIARY");
        int id = readPositiveInt("  Enter Beneficiary ID : ");
        if (idExists(id)) throw DuplicateIDException(id);
 
        string n  = readNonEmpty("  Enter Name           : ");
        string p  = readNonEmpty("  Enter Contact        : ");
        string a  = readNonEmpty("  Enter Address        : ");
        string c  = readNonEmpty("  Enter Need Category  : ");
        int    f  = readPositiveInt("  Enter Family Size    : ");
        int    pr = readIntInRange("  Priority (1-Urgent 2-Normal 3-Low): ", 1, 3);
 
        add(Beneficiary(id, n, p, a, c, f, pr));
        cout << "\n  [OK] Beneficiary #" << id << " added successfully!\n";
    }
 
    void displayAll() const {
        header("ALL BENEFICIARIES");
        if (records.empty()) { cout << "  No beneficiaries found.\n"; return; }
 
        // Sort by priority for display (copy, sort, display)
        vector<Beneficiary> sorted = records;
        sort(sorted.begin(), sorted.end(),
             [](const Beneficiary& a, const Beneficiary& b){
                 return a.getPriority() < b.getPriority();
             });
 
        int n = 1;
        for (const auto& b : sorted) {
            cout << "\n  --- Record " << n++ << " ---\n";
            b.display();
        }
        cout << "\n  Total: " << records.size() << " beneficiary(ies).\n";
    }
 
    void search() const {
        header("SEARCH BENEFICIARY");
        cout << "  1. Search by ID\n  2. Search by Name\n  3. Search by Category\n";
        int ch = readIntInRange("  Choice: ", 1, 3);
        bool found = false;
 
        if (ch == 1) {
            int id = readPositiveInt("  Enter ID: ");
            for (const auto& b : records)
                if (b.getId() == id) { cout << "\n"; b.display(); found = true; break; }
        } else if (ch == 2) {
            string nm; cout << "  Enter Name (partial): "; getline(cin, nm);
            string nmL = nm;
            transform(nmL.begin(), nmL.end(), nmL.begin(), ::tolower);
            for (const auto& b : records) {
                string bn = b.getName();
                transform(bn.begin(), bn.end(), bn.begin(), ::tolower);
                if (bn.find(nmL) != string::npos) { cout << "\n"; b.display(); found = true; }
            }
        } else {
            string cat; cout << "  Enter Category (partial): "; getline(cin, cat);
            string catL = cat;
            transform(catL.begin(), catL.end(), catL.begin(), ::tolower);
            for (const auto& b : records) {
                string bc = b.getNeedCategory();
                transform(bc.begin(), bc.end(), bc.begin(), ::tolower);
                if (bc.find(catL) != string::npos) { cout << "\n"; b.display(); found = true; }
            }
        }
        if (!found) cout << "  [!] No matching record found.\n";
    }
 
    void edit() {
        header("EDIT BENEFICIARY");
        int id = readPositiveInt("  Enter Beneficiary ID to edit: ");
        Beneficiary* b = find(id);
        if (!b) throw RecordNotFoundException(id);
 
        cout << "\n  Current record:\n"; b->display();
        cout << "\n  Enter new values (leave blank to keep current):\n";
 
        auto prompt = [](const string& label, const string& current) -> string {
            cout << "  " << label << " [" << current << "]: ";
            string s; getline(cin, s);
            return s.empty() ? current : s;
        };
 
        // Rebuild with updated values
        string newName  = prompt("Name",     b->getName());
        string newPhone = prompt("Contact",  b->getPhone());
 
        cout << "  Priority 1=Urgent 2=Normal 3=Low [" << b->getPriority() << "]: ";
        string priStr; getline(cin, priStr);
        int newPri = priStr.empty() ? b->getPriority() : stoi(priStr);
 
        // Replace record
        *b = Beneficiary(id, newName, newPhone,
                         prompt("Address",  ""), // address getter not exposed, reuse or keep
                         prompt("Category", b->getNeedCategory()),
                         readPositiveInt("  New Family Size: "),
                         (newPri >= 1 && newPri <= 3) ? newPri : b->getPriority());
        cout << "\n  [OK] Beneficiary #" << id << " updated.\n";
    }
 
    void remove() {
        header("DELETE BENEFICIARY");
        int id = readPositiveInt("  Enter Beneficiary ID to delete: ");
        auto it = find_if(records.begin(), records.end(),
                          [id](const Beneficiary& b){ return b.getId() == id; });
        if (it == records.end()) throw RecordNotFoundException(id);
 
        cout << "\n  Record to delete:\n"; it->display();
        int confirm = readIntInRange("\n  Confirm delete? (1=Yes 0=No): ", 0, 1);
        if (confirm == 1) {
            records.erase(it);
            cout << "  [OK] Beneficiary #" << id << " deleted.\n";
        } else {
            cout << "  [--] Deletion cancelled.\n";
        }
    }
 
    void menu() {
        int ch;
        do {
            header("BENEFICIARY MANAGEMENT");
            cout << "  1. Add Beneficiary\n"
                 << "  2. View All\n"
                 << "  3. Search\n"
                 << "  4. Edit\n"
                 << "  5. Delete\n"
                 << "  0. Back\n";
            ch = readIntInRange("  Choice: ", 0, 5);
            try {
                switch (ch) {
                    case 1: addBeneficiary(); break;
                    case 2: displayAll();     break;
                    case 3: search();         break;
                    case 4: edit();           break;
                    case 5: remove();         break;
                }
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            if (ch != 0) pause();
        } while (ch != 0);
    }
};
 
// ============================================================
//  VOLUNTEER MANAGER
// ============================================================
class VolunteerManager : public Repository<Volunteer> {
public:
    bool idExists(int id) const {
        return any_of(records.begin(), records.end(),
                      [id](const Volunteer& v){ return v.getId() == id; });
    }
 
    Volunteer* find(int id) {
        auto it = find_if(records.begin(), records.end(),
                          [id](const Volunteer& v){ return v.getId() == id; });
        if (it == records.end()) return nullptr;
        return &(*it);
    }
 
    void addVolunteer() {
        header("ADD VOLUNTEER");
        int id = readPositiveInt("  Enter Volunteer ID  : ");
        if (idExists(id)) throw DuplicateIDException(id);
 
        string n  = readNonEmpty("  Enter Name          : ");
        string p  = readNonEmpty("  Enter Contact       : ");
        string sk = readNonEmpty("  Enter Skill         : ");
        int    av = readIntInRange("  Available? (1=Yes 0=No): ", 0, 1);
 
        add(Volunteer(id, n, p, sk, av == 1));
        cout << "\n  [OK] Volunteer #" << id << " added successfully!\n";
    }
 
    void displayAll() const {
        header("ALL VOLUNTEERS");
        if (records.empty()) { cout << "  No volunteers found.\n"; return; }
 
        // Sort: available first
        vector<Volunteer> sorted = records;
        sort(sorted.begin(), sorted.end(),
             [](const Volunteer& a, const Volunteer& b){
                 return a.getAvailable() > b.getAvailable();
             });
 
        int n = 1;
        for (const auto& v : sorted) {
            cout << "\n  --- Record " << n++ << " ---\n";
            v.display();
        }
        cout << "\n  Total: " << records.size() << " volunteer(s).\n";
    }
 
    void search() const {
        header("SEARCH VOLUNTEER");
        cout << "  1. Search by ID\n  2. Search by Name\n  3. Search by Skill\n";
        int ch = readIntInRange("  Choice: ", 1, 3);
        bool found = false;
 
        if (ch == 1) {
            int id = readPositiveInt("  Enter ID: ");
            for (const auto& v : records)
                if (v.getId() == id) { cout << "\n"; v.display(); found = true; break; }
        } else if (ch == 2) {
            string nm; cout << "  Enter Name (partial): "; getline(cin, nm);
            string nmL = nm;
            transform(nmL.begin(), nmL.end(), nmL.begin(), ::tolower);
            for (const auto& v : records) {
                string vn = v.getName();
                transform(vn.begin(), vn.end(), vn.begin(), ::tolower);
                if (vn.find(nmL) != string::npos) { cout << "\n"; v.display(); found = true; }
            }
        } else {
            string sk; cout << "  Enter Skill (partial): "; getline(cin, sk);
            string skL = sk;
            transform(skL.begin(), skL.end(), skL.begin(), ::tolower);
            for (const auto& v : records) {
                string vs = v.getSkill();
                transform(vs.begin(), vs.end(), vs.begin(), ::tolower);
                if (vs.find(skL) != string::npos) { cout << "\n"; v.display(); found = true; }
            }
        }
        if (!found) cout << "  [!] No matching record found.\n";
    }
 
    void edit() {
        header("EDIT VOLUNTEER");
        int id = readPositiveInt("  Enter Volunteer ID to edit: ");
        Volunteer* v = find(id);
        if (!v) throw RecordNotFoundException(id);
 
        cout << "\n  Current record:\n"; v->display();
 
        string newName  = readNonEmpty("  New Name    : ");
        string newPhone = readNonEmpty("  New Contact : ");
        string newSkill = readNonEmpty("  New Skill   : ");
        int    newAv    = readIntInRange("  Available? (1=Yes 0=No): ", 0, 1);
 
        *v = Volunteer(id, newName, newPhone, newSkill, newAv == 1);
        cout << "\n  [OK] Volunteer #" << id << " updated.\n";
    }
 
    void remove() {
        header("DELETE VOLUNTEER");
        int id = readPositiveInt("  Enter Volunteer ID to delete: ");
        auto it = find_if(records.begin(), records.end(),
                          [id](const Volunteer& v){ return v.getId() == id; });
        if (it == records.end()) throw RecordNotFoundException(id);
 
        cout << "\n  Record to delete:\n"; it->display();
        int confirm = readIntInRange("\n  Confirm delete? (1=Yes 0=No): ", 0, 1);
        if (confirm == 1) {
            records.erase(it);
            cout << "  [OK] Volunteer #" << id << " deleted.\n";
        } else {
            cout << "  [--] Deletion cancelled.\n";
        }
    }
 
    void menu() {
        int ch;
        do {
            header("VOLUNTEER MANAGEMENT");
            cout << "  1. Add Volunteer\n"
                 << "  2. View All\n"
                 << "  3. Search\n"
                 << "  4. Edit\n"
                 << "  5. Delete\n"
                 << "  0. Back\n";
            ch = readIntInRange("  Choice: ", 0, 5);
            try {
                switch (ch) {
                    case 1: addVolunteer(); break;
                    case 2: displayAll();   break;
                    case 3: search();       break;
                    case 4: edit();         break;
                    case 5: remove();       break;
                }
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            if (ch != 0) pause();
        } while (ch != 0);
    }
};
 
// ============================================================
//  RESOURCE MANAGER
// ============================================================
class ResourceManager : public Repository<Resource> {
public:
    bool idExists(int id) const {
        return any_of(records.begin(), records.end(),
                      [id](const Resource& r){ return r.getId() == id; });
    }
 
    Resource* find(int id) {
        auto it = find_if(records.begin(), records.end(),
                          [id](const Resource& r){ return r.getId() == id; });
        if (it == records.end()) return nullptr;
        return &(*it);
    }
 
    void addResource() {
        header("ADD RESOURCE ITEM");
        int id = readPositiveInt("  Enter Item ID       : ");
        if (idExists(id)) throw DuplicateIDException(id);
 
        string n  = readNonEmpty("  Enter Item Name     : ");
        string c  = readNonEmpty("  Enter Category      : ");
        int    q  = readNonNegInt("  Enter Quantity      : ");
        int    r  = readNonNegInt("  Enter Reorder Level : ");
 
        add(Resource(id, n, c, q, r));
        cout << "\n  [OK] Resource #" << id << " added successfully!\n";
    }
 
    void displayAll() const {
        header("ALL RESOURCES");
        if (records.empty()) { cout << "  No resources found.\n"; return; }
 
        // Sort by category
        vector<Resource> sorted = records;
        sort(sorted.begin(), sorted.end(),
             [](const Resource& a, const Resource& b){
                 return a.getCategory() < b.getCategory();
             });
 
        int n = 1;
        for (const auto& r : sorted) {
            cout << "\n  --- Item " << n++ << " ---\n";
            r.display();
        }
        cout << "\n  Total: " << records.size() << " resource item(s).\n";
    }
 
    void search() const {
        header("SEARCH RESOURCE");
        cout << "  1. Search by ID\n  2. Search by Name\n  3. Search by Category\n";
        int ch = readIntInRange("  Choice: ", 1, 3);
        bool found = false;
 
        if (ch == 1) {
            int id = readPositiveInt("  Enter Item ID: ");
            for (const auto& r : records)
                if (r.getId() == id) { cout << "\n"; r.display(); found = true; break; }
        } else if (ch == 2) {
            string nm; cout << "  Enter Name (partial): "; getline(cin, nm);
            string nmL = nm;
            transform(nmL.begin(), nmL.end(), nmL.begin(), ::tolower);
            for (const auto& r : records) {
                string rn = r.getName();
                transform(rn.begin(), rn.end(), rn.begin(), ::tolower);
                if (rn.find(nmL) != string::npos) { cout << "\n"; r.display(); found = true; }
            }
        } else {
            string cat; cout << "  Enter Category (partial): "; getline(cin, cat);
            string catL = cat;
            transform(catL.begin(), catL.end(), catL.begin(), ::tolower);
            for (const auto& r : records) {
                string rc = r.getCategory();
                transform(rc.begin(), rc.end(), rc.begin(), ::tolower);
                if (rc.find(catL) != string::npos) { cout << "\n"; r.display(); found = true; }
            }
        }
        if (!found) cout << "  [!] No matching record found.\n";
    }
 
    void edit() {
        header("EDIT / RESTOCK RESOURCE");
        int id = readPositiveInt("  Enter Item ID to edit: ");
        Resource* r = find(id);
        if (!r) throw RecordNotFoundException(id);
 
        cout << "\n  Current record:\n"; r->display();
        cout << "\n  1. Update all fields\n  2. Restock quantity only\n";
        int ch = readIntInRange("  Choice: ", 1, 2);
 
        if (ch == 1) {
            string newName = readNonEmpty("  New Item Name     : ");
            string newCat  = readNonEmpty("  New Category      : ");
            int    newQty  = readNonNegInt("  New Quantity       : ");
            int    newRe   = readNonNegInt("  New Reorder Level  : ");
            *r = Resource(id, newName, newCat, newQty, newRe);
            cout << "\n  [OK] Resource #" << id << " updated.\n";
        } else {
            int addQty = readPositiveInt("  Quantity to add: ");
            r->restock(addQty);
            cout << "\n  [OK] Restocked. New quantity: " << r->getQuantity() << "\n";
        }
    }
 
    void remove() {
        header("DELETE RESOURCE");
        int id = readPositiveInt("  Enter Item ID to delete: ");
        auto it = find_if(records.begin(), records.end(),
                          [id](const Resource& r){ return r.getId() == id; });
        if (it == records.end()) throw RecordNotFoundException(id);
 
        cout << "\n  Record to delete:\n"; it->display();
        int confirm = readIntInRange("\n  Confirm delete? (1=Yes 0=No): ", 0, 1);
        if (confirm == 1) {
            records.erase(it);
            cout << "  [OK] Resource #" << id << " deleted.\n";
        } else {
            cout << "  [--] Deletion cancelled.\n";
        }
    }
 
    void menu() {
        int ch;
        do {
            header("RESOURCE INVENTORY MANAGEMENT");
            cout << "  1. Add Resource Item\n"
                 << "  2. View All\n"
                 << "  3. Search\n"
                 << "  4. Edit / Restock\n"
                 << "  5. Delete\n"
                 << "  0. Back\n";
            ch = readIntInRange("  Choice: ", 0, 5);
            try {
                switch (ch) {
                    case 1: addResource(); break;
                    case 2: displayAll();  break;
                    case 3: search();      break;
                    case 4: edit();        break;
                    case 5: remove();      break;
                }
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            if (ch != 0) pause();
        } while (ch != 0);
    }
};
 
// ============================================================
//  ALLOCATION MANAGER
// ============================================================
class AllocationManager : public Repository<Allocation> {
public:
    bool idExists(int id) const {
        return any_of(records.begin(), records.end(),
                      [id](const Allocation& a){ return a.getAllocId() == id; });
    }
 
    void allocate(BeneficiaryManager& bm, ResourceManager& rm) {
        header("ALLOCATE RESOURCE");
 
        int aid = readPositiveInt("  Enter Allocation ID  : ");
        if (idExists(aid)) throw DuplicateIDException(aid);
 
        int bid = readPositiveInt("  Enter Beneficiary ID : ");
        if (!bm.find(bid)) throw RecordNotFoundException(bid);
 
        int iid = readPositiveInt("  Enter Item ID        : ");
        Resource* res = rm.find(iid);
        if (!res) throw RecordNotFoundException(iid);
 
        cout << "\n  Item: " << res->getName()
             << " | Available: " << res->getQuantity() << "\n";
 
        int rq = readPositiveInt("  Quantity Requested   : ");
 
        string date = readNonEmpty("  Allocation Date (YYYY-MM-DD): ");
 
        try {
            res->deduct(rq);
            add(Allocation(aid, bid, iid, rq, rq, date, "Success"));
            cout << "\n  [OK] Allocation #" << aid << " successful! "
                 << rq << " unit(s) issued.\n"
                 << "  Remaining stock: " << res->getQuantity() << "\n";
            if (res->isLowStock())
                cout << "  [!] WARNING: Item is now at or below reorder level!\n";
        } catch (const InsufficientStockException& e) {
            // Record a failed allocation
            add(Allocation(aid, bid, iid, rq, 0, date, "Failed"));
            cout << "\n  [!!] Allocation FAILED: " << e.what() << "\n"
                 << "  A failed allocation record has been saved.\n";
        }
    }
 
    void displayAll() const {
        header("ALL ALLOCATIONS");
        if (records.empty()) { cout << "  No allocation records found.\n"; return; }
        int n = 1;
        for (const auto& a : records) {
            cout << "\n  --- Allocation " << n++ << " ---\n";
            a.display();
        }
        cout << "\n  Total: " << records.size() << " allocation(s).\n";
    }
 
    void menu(BeneficiaryManager& bm, ResourceManager& rm) {
        int ch;
        do {
            header("RESOURCE ALLOCATION");
            cout << "  1. Allocate Resource to Beneficiary\n"
                 << "  2. View All Allocations\n"
                 << "  0. Back\n";
            ch = readIntInRange("  Choice: ", 0, 2);
            try {
                switch (ch) {
                    case 1: allocate(bm, rm); break;
                    case 2: displayAll();     break;
                }
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            if (ch != 0) pause();
        } while (ch != 0);
    }
};
 
// ============================================================
//  SEARCH MENU (cross-module)
// ============================================================
void searchMenu(BeneficiaryManager& bm, VolunteerManager& vm,
                ResourceManager& rm) {
    int ch;
    do {
        header("SEARCH RECORDS");
        cout << "  1. Search Beneficiary\n"
             << "  2. Search Volunteer\n"
             << "  3. Search Resource\n"
             << "  0. Back\n";
        ch = readIntInRange("  Choice: ", 0, 3);
        try {
            if (ch == 1) bm.search();
            else if (ch == 2) vm.search();
            else if (ch == 3) rm.search();
        } catch (const exception& e) {
            cout << "\n  [ERROR] " << e.what() << "\n";
        }
        if (ch != 0) pause();
    } while (ch != 0);
}
 
// ============================================================
//  REPORTS
// ============================================================
void reportStockSummary(const ResourceManager& rm) {
    header("REPORT: STOCK SUMMARY");
    if (rm.empty()) { cout << "  No resource data.\n"; return; }
 
    // Group by category using map
    map<string, vector<const Resource*>> catMap;
    for (const auto& r : rm.getAll())
        catMap[r.getCategory()].push_back(&r);
 
    for (const auto& kv : catMap) {
        cout << "\n  Category: " << kv.first << "\n";
        cout << "  " << string(50, '-') << "\n";
        cout << left << setw(6) << "  ID"
             << setw(25) << "Name"
             << setw(10) << "Qty"
             << "Reorder\n";
        for (const Resource* r : kv.second) {
            cout << "  " << setw(6) << r->getId()
                 << setw(25) << r->getName()
                 << setw(10) << r->getQuantity()
                 << r->getReorder()
                 << (r->isLowStock() ? " [!] LOW" : "")
                 << "\n";
        }
    }
    cout << "\n  Total items: " << rm.count() << "\n";
}
 
void reportLowStock(const ResourceManager& rm) {
    header("REPORT: LOW STOCK ALERT");
    bool any = false;
    for (const auto& r : rm.getAll()) {
        if (r.isLowStock()) {
            cout << "\n  [!] Item ID " << r.getId()
                 << " | " << r.getName()
                 << " | Qty: " << r.getQuantity()
                 << " | Reorder level: " << r.getReorder() << "\n";
            any = true;
        }
    }
    if (!any) cout << "  All items are adequately stocked.\n";
}
 
void reportBeneficiariesServed(const BeneficiaryManager& bm,
                                const AllocationManager& am) {
    header("REPORT: BENEFICIARIES SERVED");
    map<int, int> servCount;  // beneficiaryId -> count of successful allocations
    for (const auto& a : am.getAll())
        if (!a.isFailed()) servCount[a.getBenefId()]++;
 
    if (servCount.empty()) { cout << "  No successful allocations recorded.\n"; return; }
 
    cout << left << setw(10) << "  BenefID"
         << setw(30) << "Name"
         << "Allocations\n";
    cout << "  " << string(50, '-') << "\n";
 
    for (const auto& kv : servCount) {
        const Beneficiary* b = const_cast<BeneficiaryManager&>(bm).find(kv.first);
        string nm = b ? b->getName() : "(unknown)";
        cout << "  " << setw(10) << kv.first
             << setw(30) << nm
             << kv.second << "\n";
    }
}
 
void reportPendingFailed(const AllocationManager& am) {
    header("REPORT: FAILED ALLOCATIONS");
    bool any = false;
    for (const auto& a : am.getAll()) {
        if (a.isFailed()) {
            a.display();
            cout << "\n";
            any = true;
        }
    }
    if (!any) cout << "  No failed allocations on record.\n";
}
 
void reportVolunteerSummary(const VolunteerManager& vm) {
    header("REPORT: VOLUNTEER SUMMARY");
    int total = (int)vm.count(), avail = 0;
    map<string, int> skillMap;
 
    for (const auto& v : vm.getAll()) {
        if (v.getAvailable()) avail++;
        skillMap[v.getSkill()]++;
    }
 
    cout << "  Total Volunteers : " << total << "\n"
         << "  Available        : " << avail << "\n"
         << "  Unavailable      : " << (total - avail) << "\n\n"
         << "  Skill Distribution:\n";
 
    for (const auto& kv : skillMap)
        cout << "    " << left << setw(25) << kv.first
             << kv.second << " volunteer(s)\n";
}
 
void reportsMenu(BeneficiaryManager& bm, VolunteerManager& vm,
                 ResourceManager& rm, AllocationManager& am) {
    int ch;
    do {
        header("REPORTS");
        cout << "  1. Stock Summary (by category)\n"
             << "  2. Low Stock Alert\n"
             << "  3. Beneficiaries Served\n"
             << "  4. Failed Allocations\n"
             << "  5. Volunteer Summary\n"
             << "  0. Back\n";
        ch = readIntInRange("  Choice: ", 0, 5);
        switch (ch) {
            case 1: reportStockSummary(rm);              break;
            case 2: reportLowStock(rm);                  break;
            case 3: reportBeneficiariesServed(bm, am);   break;
            case 4: reportPendingFailed(am);             break;
            case 5: reportVolunteerSummary(vm);          break;
        }
        if (ch != 0) pause();
    } while (ch != 0);
}
 
// ============================================================
//  FILE I/O — SAVE
// ============================================================
void saveData(BeneficiaryManager& bm, VolunteerManager& vm,
              ResourceManager& rm, AllocationManager& am) {
 
    // --- Beneficiaries ---
    {
        ofstream f(FILE_BENEFICIARIES);
        if (!f) throw runtime_error("Cannot open " + FILE_BENEFICIARIES);
        for (const auto& b : bm.getAll())
            f << b.serialize() << "\n";
    }
 
    // --- Volunteers ---
    {
        ofstream f(FILE_VOLUNTEERS);
        if (!f) throw runtime_error("Cannot open " + FILE_VOLUNTEERS);
        for (const auto& v : vm.getAll())
            f << v.serialize() << "\n";
    }
 
    // --- Resources ---
    {
        ofstream f(FILE_RESOURCES);
        if (!f) throw runtime_error("Cannot open " + FILE_RESOURCES);
        for (const auto& r : rm.getAll())
            f << r.serialize() << "\n";
    }
 
    // --- Allocations ---
    {
        ofstream f(FILE_ALLOCATIONS);
        if (!f) throw runtime_error("Cannot open " + FILE_ALLOCATIONS);
        for (const auto& a : am.getAll())
            f << a.serialize() << "\n";
    }
 
    cout << "  [OK] All data saved to files successfully.\n";
}
 
// ============================================================
//  FILE I/O — LOAD
// ============================================================
void loadData(BeneficiaryManager& bm, VolunteerManager& vm,
              ResourceManager& rm, AllocationManager& am) {
    int loaded = 0;
 
    // --- Beneficiaries ---
    {
        ifstream f(FILE_BENEFICIARIES);
        if (f.is_open()) {
            string line;
            while (getline(f, line)) {
                if (line.empty()) continue;
                try { bm.add(Beneficiary::deserialize(line)); loaded++; }
                catch (...) { cerr << "  [WARN] Skipped bad beneficiary record.\n"; }
            }
        }
    }
 
    // --- Volunteers ---
    {
        ifstream f(FILE_VOLUNTEERS);
        if (f.is_open()) {
            string line;
            while (getline(f, line)) {
                if (line.empty()) continue;
                try { vm.add(Volunteer::deserialize(line)); loaded++; }
                catch (...) { cerr << "  [WARN] Skipped bad volunteer record.\n"; }
            }
        }
    }
 
    // --- Resources ---
    {
        ifstream f(FILE_RESOURCES);
        if (f.is_open()) {
            string line;
            while (getline(f, line)) {
                if (line.empty()) continue;
                try { rm.add(Resource::deserialize(line)); loaded++; }
                catch (...) { cerr << "  [WARN] Skipped bad resource record.\n"; }
            }
        }
    }
 
    // --- Allocations ---
    {
        ifstream f(FILE_ALLOCATIONS);
        if (f.is_open()) {
            string line;
            while (getline(f, line)) {
                if (line.empty()) continue;
                try { am.add(Allocation::deserialize(line)); loaded++; }
                catch (...) { cerr << "  [WARN] Skipped bad allocation record.\n"; }
            }
        }
    }
 
    if (loaded > 0)
        cout << "  [OK] Loaded " << loaded << " records from files.\n";
}
 
// ============================================================
//  SEED SAMPLE DATA (15+ records)
// ============================================================
void seedSampleData(BeneficiaryManager& bm, VolunteerManager& vm,
                    ResourceManager& rm, AllocationManager& am) {
    // -- 6 Beneficiaries --
    bm.add(Beneficiary(101, "Aisha Rahman",   "0501234567", "Block 3, Al Ain",     "Food",     4, 1));
    bm.add(Beneficiary(102, "Carlos Mendez",  "0502345678", "Villa 12, Abu Dhabi", "Medicine", 2, 2));
    bm.add(Beneficiary(103, "Priya Sharma",   "0503456789", "Flat 5, Dubai",       "Clothing",  6, 1));
    bm.add(Beneficiary(104, "Omar Abdullah",  "0504567890", "House 7, Sharjah",    "Books",     3, 3));
    bm.add(Beneficiary(105, "Li Wei",         "0505678901", "Room 2, Ajman",       "Food",      5, 2));
    bm.add(Beneficiary(106, "Fatima Al Zaabi","0506789012", "Street 9, Al Ain",    "Medicine",  1, 1));
 
    // -- 5 Volunteers --
    vm.add(Volunteer(201, "Ahmed Hassan",     "0551112222", "Driving",    true));
    vm.add(Volunteer(202, "Sarah Johnson",    "0552223333", "Data Entry", true));
    vm.add(Volunteer(203, "Ravi Kumar",       "0553334444", "Packing",    false));
    vm.add(Volunteer(204, "Maria Santos",     "0554445555", "Field Visit",true));
    vm.add(Volunteer(205, "James Okafor",     "0555556666", "Driving",    true));
 
    // -- 6 Resources --
    rm.add(Resource(301, "Rice (5kg bag)",     "Food",     50,  10));
    rm.add(Resource(302, "Cooking Oil (2L)",   "Food",     30,   5));
    rm.add(Resource(303, "Paracetamol 500mg",  "Medicine", 200, 20));
    rm.add(Resource(304, "Bandage Rolls",      "Medicine",  80, 15));
    rm.add(Resource(305, "Winter Jackets",     "Clothing",  25,  5));
    rm.add(Resource(306, "School Notebooks",   "Books",    100, 10));
 
    // -- 4 Allocations --
    // Directly deduct resource quantities for seeded allocations
    Resource* r1 = rm.find(301);
    Resource* r2 = rm.find(303);
    Resource* r3 = rm.find(306);
    // r4 (jackets) left at full stock — intentional, allocation 404 is a failed record
    if (r1) r1->deduct(5);
    if (r2) r2->deduct(10);
    if (r3) r3->deduct(20);
 
    am.add(Allocation(401, 101, 301,  5,  5, "2026-04-01", "Success"));
    am.add(Allocation(402, 103, 303, 10, 10, "2026-04-02", "Success"));
    am.add(Allocation(403, 105, 306, 20, 20, "2026-04-03", "Success"));
    am.add(Allocation(404, 102, 305, 30,  0, "2026-04-04", "Failed")); // intentional fail
 
    cout << "  [OK] Sample data loaded: 6 beneficiaries, 5 volunteers, "
            "6 resources, 4 allocations.\n";
}
 
// ============================================================
//  MAIN
// ============================================================
int main() {
    BeneficiaryManager bm;
    VolunteerManager   vm;
    ResourceManager    rm;
    AllocationManager  am;
 
    header("COMMUNITY HELP DESK AND RESOURCE ALLOCATION SYSTEM");
    cout << "  Symbiosis International University Dubai\n"
         << "  Course: DBT7288 - Programming in C\n";
    separator();
 
    // Load persistent data
    try { loadData(bm, vm, rm, am); }
    catch (const exception& e) {
        cerr << "  [WARN] Load error: " << e.what() << "\n";
    }
 
    // Offer sample data if nothing loaded
    if (bm.empty() && vm.empty() && rm.empty()) {
        cout << "\n  No existing data found.\n";
        if (readIntInRange("  Load sample data? (1=Yes 0=No): ", 0, 1) == 1) {
            seedSampleData(bm, vm, rm, am);
        }
    } else {
        cout << "  Records loaded: "
             << bm.count() << " beneficiaries, "
             << vm.count() << " volunteers, "
             << rm.count() << " resources, "
             << am.count() << " allocations.\n";
    }
 
    int choice;
    do {
        header("MAIN MENU");
        cout << "  1. Beneficiary Management\n"
             << "  2. Volunteer Management\n"
             << "  3. Resource Inventory Management\n"
             << "  4. Resource Allocation\n"
             << "  5. Search Records\n"
             << "  6. Reports\n"
             << "  7. Save Data\n"
             << "  8. Exit\n";
        choice = readIntInRange("  Choice: ", 1, 8);
 
        try {
            switch (choice) {
                case 1: bm.menu();               break;
                case 2: vm.menu();               break;
                case 3: rm.menu();               break;
                case 4: am.menu(bm, rm);         break;
                case 5: searchMenu(bm, vm, rm);  break;
                case 6: reportsMenu(bm, vm, rm, am); break;
                case 7:
                    saveData(bm, vm, rm, am);
                    pause();
                    break;
                case 8:
                    cout << "\n  Saving data before exit...\n";
                    saveData(bm, vm, rm, am);
                    cout << "\n  Thank you for using the Community Help Desk System!\n";
                    cout << "  Goodbye!\n";
                    separator();
                    break;
            }
        } catch (const exception& e) {
            cout << "\n  [ERROR] " << e.what() << "\n";
            pause();
        }
    } while (choice != 8);
 
    return 0;
}
 
/*
 * ============================================================
 *  TEST CASES DOCUMENTATION
 * ============================================================
 *
 *  TC01 - Normal: Add beneficiary with valid data
 *         Input: ID=201, Name=Test User, valid fields
 *         Expected: "Beneficiary #201 added successfully!"
 *
 *  TC02 - Duplicate ID: Add beneficiary with existing ID
 *         Input: ID already in system
 *         Expected: "[ERROR] ID X already exists!"
 *
 *  TC03 - Invalid input: Enter negative family size
 *         Input: -3 at family size prompt
 *         Expected: "[!] Please enter a positive integer." (re-prompt)
 *
 *  TC04 - Normal: Allocate resource with sufficient stock
 *         Input: Valid BenefID, ItemID, qty within stock
 *         Expected: "Allocation successful!", stock decremented
 *
 *  TC05 - Boundary: Allocate exactly all remaining stock
 *         Input: qty == available quantity
 *         Expected: "Allocation successful!", stock = 0, low-stock warning
 *
 *  TC06 - Insufficient stock: Allocate more than available
 *         Input: qty > available
 *         Expected: "Allocation FAILED: Insufficient stock!", failed record saved
 *
 *  TC07 - Not found: Search/edit non-existent ID
 *         Input: ID that doesn't exist
 *         Expected: "[ERROR] Record with ID X not found!"
 *
 *  TC08 - Empty file handling: Load from empty/missing files
 *         Expected: System starts cleanly, offers sample data
 *
 *  TC09 - File persistence: Save and reload all records
 *         Expected: All records identical after save->restart->load
 *
 *  TC10 - Empty string: Enter blank name
 *         Input: Press Enter at name prompt
 *         Expected: "[!] Input cannot be empty!" (re-prompt)
 * ============================================================
 */