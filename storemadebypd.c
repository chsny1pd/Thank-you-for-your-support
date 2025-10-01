#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define FILE_NAME "orders.csv"

typedef struct {
    char OrderID[10];
    char CustomerName[50];
    char ProductName[50];
    char OrderDate[20];
    char ShippingDate[20];
} Order;

// ===== Function Prototypes =====
void DisplayMenu();
void CreateOrder();
void AddOrder();
void ReadOrders();
void SearchOrder();
void UpdateOrder();
void DeleteOrder();
int isValidDateFormat(const char *date);
int isShippingAfterOrder(const char *orderDate, const char *shippingDate);
int isNumeric(const char *str);
int isDuplicateOrderID(const char *orderID, const char *excludeID);

// Core file operations for Unit Test
int AddOrderToFile(Order o);
int UpdateOrderInFile(const char *oldID, Order o);
int DeleteOrderInFile(const char *id);

// Unit Tests
void UnitTest_AddOrder();
void UnitTest_UpdateOrder();
void UnitTest_DeleteOrder();

// ===== MAIN =====
int main() {
    int choice;
    do {
        DisplayMenu();
        printf("Please Select Menu: ");
        scanf("%d", &choice);
        getchar();

        switch(choice) {
            case 1: AddOrder(); break;
            case 2: ReadOrders(); break;
            case 3: SearchOrder(); break;
            case 4: UpdateOrder(); break;
            case 5: DeleteOrder(); break;
            case 6: CreateOrder(); break;
            case 7: UnitTest_AddOrder(); break;
            case 8: UnitTest_UpdateOrder(); break;
            case 9: UnitTest_DeleteOrder(); break;
            case 0: printf("Exit Program, Thank You\n"); break;
            default: printf("Incorrect Menu! Please Select Again\n");
        }
    } while(choice != 0);

    return 0;
}

// ===== Display Menu =====
void DisplayMenu() {
    printf("\n===== Welcome to Order and Delivery Information Management System =====\n");
    printf("1. Add Order\n");
    printf("2. Read Orders\n");
    printf("3. Search Order\n");
    printf("4. Update Order\n");
    printf("5. Delete Order\n");
    printf("6. Create New Order CSV File with Header\n");
    printf("7. Unit Test AddOrder\n");
    printf("8. Unit Test UpdateOrder\n");
    printf("9. Unit Test DeleteOrder\n");
    printf("0. Exit Program\n");
}

// ===== Helper: Check if string is numeric =====
int isNumeric(const char *str) {
    for (int i = 0; str[i]; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

// ===== Helper: Check duplicate OrderID =====
int isDuplicateOrderID(const char *orderID, const char *excludeID) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0; // file not exist = no duplicate

    char line[200];
    fgets(line, sizeof(line), fp); // skip header

    while (fgets(line, sizeof(line), fp)) {
        char id[10];
        sscanf(line, "%[^,],", id);
        if (strcmp(id, orderID) == 0 && (!excludeID || strcmp(orderID, excludeID) != 0)) {
            fclose(fp);
            return 1; // duplicate found
        }
    }
    fclose(fp);
    return 0;
}

// ===== Validate Date Format YYYY-MM-DD =====
int isValidDateFormat(const char *date) {
    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (date[i] < '0' || date[i] > '9') return 0;
    }

    int year = atoi(date);
    int month = atoi(date + 5);
    int day = atoi(date + 8);

    if (year < 1900) return 0;
    if (month < 1 || month > 12) return 0;

    int daysInMonth;
    switch(month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            daysInMonth = 31; break;
        case 4: case 6: case 9: case 11:
            daysInMonth = 30; break;
        case 2:
            if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
                daysInMonth = 29;
            else
                daysInMonth = 28;
            break;
        default:
            return 0;
    }

    return (day >= 1 && day <= daysInMonth);
}

// ===== Check if ShippingDate >= OrderDate =====
int isShippingAfterOrder(const char *orderDate, const char *shippingDate) {
    int oY, oM, oD, sY, sM, sD;
    sscanf(orderDate, "%d-%d-%d", &oY, &oM, &oD);
    sscanf(shippingDate, "%d-%d-%d", &sY, &sM, &sD);

    int orderVal = oY * 10000 + oM * 100 + oD;
    int shipVal  = sY * 10000 + sM * 100 + sD;

    return shipVal >= orderVal;   // ✅ อนุญาตวันเดียวกันได้
}

// ===== Core Functions =====
void CreateOrder() {
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) {
        printf("Can't Create File\n");
        return;
    }
    fprintf(fp, "OrderID,CustomerName,ProductName,OrderDate,ShippingDate\n");
    fclose(fp);
    printf("CSV File Created Successfully with Header\n");
}

int AddOrderToFile(Order o) {
    if (!isNumeric(o.OrderID)) return 0;
    if (isDuplicateOrderID(o.OrderID, NULL)) return 0;
    if (!isValidDateFormat(o.OrderDate)) return 0;
    if (!isValidDateFormat(o.ShippingDate)) return 0;
    if (!isShippingAfterOrder(o.OrderDate, o.ShippingDate)) return 0;

    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) return 0;
    fprintf(fp, "%s,%s,%s,%s,%s\n", o.OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
    fclose(fp);
    return 1;
}

int UpdateOrderInFile(const char *oldID, Order o) {
    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.csv", "w");
    if (!fp || !temp) return 0;

    char line[200], id[10];
    int found = 0;
    fgets(line, sizeof(line), fp); fputs(line, temp); // copy header

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],", id);
        if (strcmp(id, oldID) == 0) {
            found = 1;
            if (!isNumeric(o.OrderID) || isDuplicateOrderID(o.OrderID, oldID) ||
                !isValidDateFormat(o.OrderDate) || !isValidDateFormat(o.ShippingDate) ||
                !isShippingAfterOrder(o.OrderDate, o.ShippingDate)) {
                fclose(fp); fclose(temp); remove("temp.csv");
                return 0;
            }
            fprintf(temp, "%s,%s,%s,%s,%s\n", o.OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
        } else {
            fputs(line, temp);
        }
    }
    fclose(fp); fclose(temp);
    remove(FILE_NAME); rename("temp.csv", FILE_NAME);
    return found;
}

int DeleteOrderInFile(const char *id) {
    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.csv", "w");
    if (!fp || !temp) return 0;

    char line[200], curID[10];
    int found = 0;
    fgets(line, sizeof(line), fp); fputs(line, temp);

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],", curID);
        if (strcmp(curID, id) == 0) {
            found = 1;
            continue;
        }
        fputs(line, temp);
    }
    fclose(fp); fclose(temp);
    remove(FILE_NAME); rename("temp.csv", FILE_NAME);
    return found;
}

// ===== Interactive Versions =====
void AddOrder() {
    Order o;
    printf("Enter OrderID: "); scanf("%s", o.OrderID);
    printf("Enter Customer Name: "); getchar(); fgets(o.CustomerName, 50, stdin);
    o.CustomerName[strcspn(o.CustomerName, "\n")] = 0;
    printf("Enter Product Name: "); fgets(o.ProductName, 50, stdin);
    o.ProductName[strcspn(o.ProductName, "\n")] = 0;
    printf("Enter Order Date (YYYY-MM-DD): "); scanf("%s", o.OrderDate);
    printf("Enter Shipping Date (YYYY-MM-DD): "); scanf("%s", o.ShippingDate);

    if (AddOrderToFile(o)) printf("Data Added Successfully\n");
    else printf("Add Failed\n");
}

void ReadOrders() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) {
        printf("File Not Found\n");
        return;
    }
    char line[200];
    printf("\n----- Order Information -----\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

void SearchOrder() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) {
        printf("File Not Found\n");
        return;
    }
    char searchID[10], line[200];
    printf("Enter OrderID to Search: ");
    scanf("%s", searchID);

    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, searchID)) {
            printf("Found: %s", line);
            found = 1;
        }
    }
    if (!found) printf("Not Found\n");
    fclose(fp);
}

void UpdateOrder() {
    char searchID[10];
    Order o;
    printf("Enter OrderID to Update: ");
    scanf("%s", searchID);
    printf("Enter New OrderID: "); scanf("%s", o.OrderID);
    printf("Enter New Customer Name: "); getchar(); fgets(o.CustomerName, 50, stdin);
    o.CustomerName[strcspn(o.CustomerName, "\n")] = 0;
    printf("Enter New Product Name: "); fgets(o.ProductName, 50, stdin);
    o.ProductName[strcspn(o.ProductName, "\n")] = 0;
    printf("Enter New Order Date (YYYY-MM-DD): "); scanf("%s", o.OrderDate);
    printf("Enter New Shipping Date (YYYY-MM-DD): "); scanf("%s", o.ShippingDate);

    if (UpdateOrderInFile(searchID, o)) printf("Updated Successfully\n");
    else printf("Update Failed\n");
}

void DeleteOrder() {
    char id[10];
    printf("Enter OrderID to Delete: ");
    scanf("%s", id);
    if (DeleteOrderInFile(id)) printf("Deleted Successfully\n");
    else printf("Delete Failed\n");
}

// ===== Unit Tests =====
void UnitTest_AddOrder() {
    printf("\n[Unit Test] AddOrder()\n");
    CreateOrder();

    Order o1 = {"101", "John", "Phone", "2025-01-01", "2025-01-05"};
    assert(AddOrderToFile(o1) == 1);

    Order oDup = {"101", "Dup", "Laptop", "2025-01-01", "2025-01-02"};
    assert(AddOrderToFile(oDup) == 0);

    Order oBadDate = {"102", "Kate", "Tablet", "2025-13-01", "2025-01-05"};
    assert(AddOrderToFile(oBadDate) == 0);

    Order oShipBefore = {"103", "Mike", "TV", "2025-01-10", "2025-01-05"};
    assert(AddOrderToFile(oShipBefore) == 0);

    // ✅ ทดสอบ ShippingDate เท่ากับ OrderDate
    Order oSameDay = {"104", "Jane", "Book", "2025-01-15", "2025-01-15"};
    assert(AddOrderToFile(oSameDay) == 1);

    printf("All AddOrder tests passed!\n");
}

void UnitTest_UpdateOrder() {
    printf("\n[Unit Test] UpdateOrder()\n");
    CreateOrder();
    Order o1 = {"201", "Alice", "Laptop", "2025-02-01", "2025-02-10"};
    AddOrderToFile(o1);

    Order newO = {"202", "AliceNew", "PC", "2025-02-02", "2025-02-12"};
    assert(UpdateOrderInFile("201", newO) == 1);

    Order dupO = {"202", "Bob", "Monitor", "2025-03-01", "2025-03-10"};
    assert(UpdateOrderInFile("202", dupO) == 1);

    assert(UpdateOrderInFile("999", newO) == 0);

    printf("All UpdateOrder tests passed!\n");
}

void UnitTest_DeleteOrder() {
    printf("\n[Unit Test] DeleteOrder()\n");
    CreateOrder();
    Order o1 = {"301", "Bob", "Tablet", "2025-03-01", "2025-03-05"};
    AddOrderToFile(o1);

    assert(DeleteOrderInFile("301") == 1);
    assert(DeleteOrderInFile("301") == 0);

    printf("All DeleteOrder tests passed!\n");
}