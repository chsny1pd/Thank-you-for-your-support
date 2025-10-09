# Thank-you-for-your-support
# 🏪 Order Management System (C Project)

> A simple CSV-based order management system written in C — with full validation, CRUD operations, and built-in unit & end-to-end testing.

---

## 📘 Overview

This project simulates an **Order Management System** that stores data in a CSV file.  
Users can **create, add, read, search, update, and delete orders**, with data validation (date, product, and order ID checks).

Additionally, it includes:
- ✅ Unit Tests for `Add`, `Update`, and `Delete` operations  
- ✅ End-to-End (E2E) Test for verifying full workflow  
- ✅ Interactive menu interface in the main program  

All data is stored in a local file: `orders.csv`

---

## 🧩 Project Structure
├── storemadebypd.c # Main program (contains menu and logic)    
├── storemadebypd.h # Header file with struct & function prototypes   
├── unittest.c # Unit Test for Add, Update, Delete   
├── e2etest.c # End-to-End system test   
└── build.bat # Windows build script (compile & run)   

## ⚙️ Build & Run Instructions

### 📦 Files Required  
Before building, make sure you have **all 4 source files** in the same folder:  
  
1.storemadebypd.c  
2.storemadebypd.h  
3.unittest.c  
4.e2etest.c  
5.build.bat  

These files must be downloaded or cloned before running the program.  

---  

### 🪟 On Windows (CMD)

1. Make sure you have **GCC** (via MinGW or similar).  
   You can test by running:
   ```bash
   gcc --version

2. Build and run automatically (recommended):
   Just double-click build.bat — it will:

   1.Compile all .c files together  
   2.Create store.exe  
   3.Launch the program automatically  

3. If you want to compile manually:

Copy code  
- gcc storemadebypd.c unittest.c e2etest.c -o store   
  
Then type   
- store or store.exe

### 💻 On Windows (PowerShell)

If using PowerShell, use:
- gcc storemadebypd.c unittest.c e2etest.c -o store.exe  
- .\store.exe  

### 🍎 On macOS / 🐧 Linux

Make sure GCC or Clang is installed (brew install gcc or sudo apt install gcc):  
- gcc storemadebypd.c unittest.c e2etest.c -o store  
- ./store  

## 🖥️ Program Menu

When you run the program (store.exe), you’ll see:

===== Order Management System =====
1. List Product
2. Add Order
3. Read Orders
4. Search Order
5. Update Order
6. Delete Order
7. Create CSV File
8. Unit Test: AddOrder
9. Unit Test: UpdateOrder
10. Unit Test: DeleteOrder
11. E2E Test (All-in-one)
0. Exit

## 🧪 Testing

## 🧭 Unit Tests
| Case | Function                 | Description                                 |
| ---- | ------------------------ | ------------------------------------------- |
| 8    | `UnitTest_AddOrder()`    | Tests valid & invalid order additions       |
| 9    | `UnitTest_UpdateOrder()` | Tests record update validation              |
| 10   | `UnitTest_DeleteOrder()` | Tests delete functionality and file rewrite |

## 🧩 End-to-End (Case 11)

-Creates a new CSV file  
-Adds 3 sample orders  
-Updates one record  
-Deletes one record  
-Prints all steps to the console  
Run via Menu option 11: “E2E Test (All-in-one)”  

## 📝 Example Console Output
===== Order Management System =====
1. List Product  
2. Add Order  
...  
11. E2E Test (All-in-one)  
0. Exit  
Please Select Menu: 11  
  
[INFO] Creating CSV file...  
CSV File Created Successfully with Header  
  
[INFO] Adding sample orders...  
Data Added Successfully: OrderID 101  
Data Added Successfully: OrderID 102  
Data Added Successfully: OrderID 103  
  
[INFO] Updating OrderID 102...  
Updated Successfully  
  
[INFO] Deleting OrderID 103...  
Deleted Successfully  
  
[INFO] Reading all orders...  
OrderID,CustomerName,ProductName,OrderDate,ShippingDate  
101,Alice,Laptop,2025-10-08,2025-10-09  
102,Bob,Phone,2025-10-08,2025-10-10  

## 📂 Example CSV File (orders.csv)
OrderID,CustomerName,ProductName,OrderDate,ShippingDate  
101,Alice,Laptop,2025-10-08,2025-10-09  
102,Bob,Phone,2025-10-08,2025-10-10  

## 🔍 Validation Rules
| Field            | Rule                                                                          | Max Length |
| ---------------- | ----------------------------------------------------------------------------- | ---------- |
| **OrderID**      | Must be numeric, non-empty, unique                                            | 10 chars   |
| **CustomerName** | Must be non-empty (letters/numbers/symbols allowed)                           | 50 chars   |
| **ProductName**  | Must match one of the predefined product list                                 | 49 chars   |
| **OrderDate**    | Valid format `YYYY-MM-DD`, auto-filled as current date                        | 10 chars   |
| **ShippingDate** | Valid format `YYYY-MM-DD`, must be same day or within 7 days after order date | 10 chars   |


## 🧠 Example Product List
Laptop, Phone, Tablet, PC, Keyboard, Mouse, Monitor,
Headphones, Webcam, Router, SSD, HDD, GPU, Motherboard, PowerSupply

## 🧰 Tech Details

● Language: C   
● Compiler: GCC (MinGW)  
● Data Storage: CSV file (orders.csv)  
● No external libraries required  

## ✨ Author & Credits

● Author: Setsin Chutsanayothin  
● Project Name: Thank-you-for-your-support  
● Inspired by: Testing-driven C development for learning purposes  