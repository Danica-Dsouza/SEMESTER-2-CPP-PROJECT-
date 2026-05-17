# Community Help Desk and Resource Allocation System
### C++ Version — project5.cpp
**Course:** DBT7288 – Programming in C (C++ Enhancement)
**Institution:** Symbiosis International University Dubai

---

## Overview

An object-oriented C++ rewrite of the Community Help Desk system. Built on the same problem domain as the C version, this program applies OOP principles — inheritance, polymorphism, templates, and exception handling — to produce a more maintainable, extensible, and robust application. All data persists across sessions via text files.

---

## Features

- **Beneficiary Management** — Add, view (sorted by priority), search (by ID, name, or category), edit, delete
- **Volunteer Management** — Add, view (sorted by availability), search (by ID, name, or skill), edit, delete
- **Resource Inventory** — Add, view (sorted by category), search (by ID, name, or category), edit/restock, delete
- **Resource Allocation** — Allocate resources with stock validation; records failed allocations automatically
- **Search Records** — Unified search menu across all three entity types with partial, case-insensitive matching
- **Reports**
  - Stock Summary (grouped by category using `std::map`)
  - Low Stock Alert
  - Beneficiaries Served
  - Failed Allocations
  - Volunteer Summary (with skill distribution)
- **File Persistence** — Auto-saves on exit; loads and validates records on startup; corrupt lines are skipped with a warning
- **Sample Data** — Option to seed 15+ demo records on first run

---

## File Structure

```
project5.cpp            # Main source file (all classes and logic in one file)
beneficiaries.txt       # Saved beneficiary records
volunteers.txt          # Saved volunteer records
resources.txt           # Saved resource records
allocations.txt         # Saved allocation records
```

---

## Class Design

```
Person  (abstract base class)
├── Beneficiary
└── Volunteer

Resource              (standalone class)
Allocation            (standalone class)

Repository<T>         (generic template base)
├── BeneficiaryManager
├── VolunteerManager
├── ResourceManager
└── AllocationManager
```

### Class Summary

| Class | Responsibility |
|---|---|
| `Person` | Abstract base — holds ID, name, phone; declares `display()` and `serialize()` as pure virtual |
| `Beneficiary` | Adds address, need category, family size, priority; implements CRUD display and file serialization |
| `Volunteer` | Adds skill and availability; implements display and serialization |
| `Resource` | Manages item stock with `deduct()` and `restock()` methods; tracks reorder level |
| `Allocation` | Records resource allocation attempts (success or failure) |
| `Repository<T>` | Template container wrapping `std::vector<T>`; provides `add`, `count`, `empty`, `getAll` |
| `*Manager` | Extends `Repository<T>` with CRUD operations, search, and a `menu()` method |

---

## Custom Exceptions

| Exception | Thrown When |
|---|---|
| `DuplicateIDException` | Adding a record with an ID that already exists |
| `RecordNotFoundException` | Searching or editing a non-existent ID |
| `InsufficientStockException` | Allocating more than the available stock quantity |
| `InvalidInputException` | Invalid input such as a non-positive restock quantity |

---

## How to Compile and Run

### Using G++ (Linux / macOS / WSL)
```bash
g++ -std=c++17 project5.cpp -o project5
./project5
```

### Using G++ (Windows Command Prompt)
```bash
g++ -std=c++17 project5.cpp -o project5.exe
project5.exe
```

### Using VS Code
1. Open the folder containing `project5.cpp`
2. Install the **C/C++ extension** by Microsoft
3. Set the C++ standard to C++17 in your build task
4. Press `Ctrl+Shift+B` to build, then run via the terminal

> Minimum standard required: **C++11** (C++17 recommended)

---

## How to Use

1. On first launch, choose `1` to load sample data (6 beneficiaries, 5 volunteers, 6 resources, 4 allocations)
2. Navigate using the numbered main menu (1–8)
3. All sub-menus follow the same structure — enter the number for your action
4. Enter `0` in any sub-menu to go back to the main menu
5. Choose option `8` (Exit) to save and quit — data is also auto-saved on exit
6. Use option `7` to manually save at any time

---

## Key Concepts Used

- Abstract base classes and pure virtual functions
- Inheritance and method overriding (`override` keyword)
- Polymorphism via virtual dispatch
- Generic programming with class templates (`Repository<T>`)
- STL containers: `std::vector`, `std::map`
- STL algorithms: `std::sort`, `std::find_if`, `std::any_of`, `std::transform`
- Custom exception classes inheriting from `std::runtime_error`
- Lambda functions for search predicates and sort comparators
- File I/O with `std::ifstream` and `std::ofstream`
- Object serialization and deserialization (pipe-delimited format)
- Input validation utilities (`readNonEmpty`, `readPositiveInt`, `readIntInRange`)

---

## File Format (pipe-delimited, no count header)

Each file stores one record per line with no header. Corrupt or malformed lines are skipped on load with a warning:

```
101|Aisha Rahman|0501234567|Block 3, Al Ain|Food|4|1
102|Carlos Mendez|0502345678|Villa 12, Abu Dhabi|Medicine|2|2
103|Priya Sharma|0503456789|Flat 5, Dubai|Clothing|6|1
```

---

## Known Limitations

- `BeneficiaryManager::edit()` does not preserve the address field — `Beneficiary` is missing a `getAddress()` getter, so editing always resets the address to blank. Add a getter to fix this.
- `reportBeneficiariesServed()` uses `const_cast` to call the non-const `find()` method — `find()` should be made `const` to avoid this.
- No date validation — allocation dates are accepted as free-text strings
- No undo functionality

---

## Priority Levels

| Level | Label |
|---|---|
| 1 | Urgent |
| 2 | Normal |
| 3 | Low |

---

## Comparison with C Version (project4.c)

| Feature | C (project4) | C++ (project5) |
|---|---|---|
| Paradigm | Procedural | Object-Oriented |
| Data storage | Global dynamic arrays | `std::vector` inside manager classes |
| Memory management | Manual (`malloc`/`realloc`/`free`) | Automatic (RAII via STL) |
| Error handling | `printf` + early `return` | Custom exception hierarchy |
| Search | ID-only (benef/vol) | Partial match, case-insensitive |
| Display order | Insertion order | Sorted (priority / availability / category) |
| File format | Count header + records | Records only (no count header) |
| Code reuse | Repeated patterns per module | `Repository<T>` template eliminates duplication |
