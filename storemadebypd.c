#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
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
void ListProduct();
int isValidProduct(const char *product);
void CreateOrder();
void AddOrder();
void ReadOrders();
void SearchOrder();
void UpdateOrder();
void DeleteOrder();
void E2E_Test();

int isValidDateFormat(const char *date);
int isShippingAfterOrderWithin7(const char *orderDate, const char *shippingDate);
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

// Helpers
void getCurrentDateStr(char *buf, size_t size);
void getDateStrOffset(char *buf, size_t size, int offsetDays);
int dateToEpoch(const char *date, time_t *out);

// ===== MAIN =====
int main() {
    int choice;
    do {
        DisplayMenu();
        printf("Please Select Menu: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }
        getchar();

        switch(choice) {
            case 1: ListProduct(); break;
            case 2: AddOrder(); break;
            case 3: ReadOrders(); break;
            case 4: SearchOrder(); break;
            case 5: UpdateOrder(); break;
            case 6: DeleteOrder(); break;
            case 7: CreateOrder(); break;
            case 8: UnitTest_AddOrder(); break;
            case 9: UnitTest_UpdateOrder(); break;
            case 10: UnitTest_DeleteOrder(); break;
            case 11: E2E_Test(); break;
            case 0: printf("Exit Program, Thank You\n"); break;
            default: printf("Incorrect Menu! Please Select Again\n");
        }
    } while(choice != 0);

    return 0;
}

// ===== Display Menu =====
void DisplayMenu() {
    printf("\n===== Welcome to Order and Delivery Information Management System =====\n");
    printf("1. List Product (15 Tech Items)\n");
    printf("2. Add Order\n");
    printf("3. Read Orders\n");
    printf("4. Search Order\n");
    printf("5. Update Order\n");
    printf("6. Delete Order\n");
    printf("7. Create New Order CSV File with Header\n");
    printf("8. Unit Test AddOrder\n");
    printf("9. Unit Test UpdateOrder\n");
    printf("10. Unit Test DeleteOrder\n");
    printf("11. E2E Test (Create->Add->Read->Update->Delete->Read)\n");
    printf("0. Exit Program\n");
}

// ===== Product List (Switch Case) =====
void ListProduct() {
    printf("\n--- Product List (15 Tech Items) ---\n");
    for (int i = 1; i <= 15; i++) {
        switch(i) {
            case 1:  printf("%2d. Laptop\n", i); break;
            case 2:  printf("%2d. Phone\n", i); break;
            case 3:  printf("%2d. Tablet\n", i); break;
            case 4:  printf("%2d. PC\n", i); break;
            case 5:  printf("%2d. Keyboard\n", i); break;
            case 6:  printf("%2d. Mouse\n", i); break;
            case 7:  printf("%2d. Monitor\n", i); break;
            case 8:  printf("%2d. Headphones\n", i); break;
            case 9:  printf("%2d. Webcam\n", i); break;
            case 10: printf("%2d. Router\n", i); break;
            case 11: printf("%2d. SSD\n", i); break;
            case 12: printf("%2d. HDD\n", i); break;
            case 13: printf("%2d. GPU\n", i); break;
            case 14: printf("%2d. Motherboard\n", i); break;
            case 15: printf("%2d. PowerSupply\n", i); break;
            default: break;
        }
    }
    printf("-----------------------------------\n");
}

// Check against the product list
int isValidProduct(const char *product) {
    const char *products[15] = {
        "Laptop","Phone","Tablet","PC","Keyboard","Mouse","Monitor",
        "Headphones","Webcam","Router","SSD","HDD","GPU","Motherboard","PowerSupply"
    };
    for (int i = 0; i < 15; i++) {
        if (strcasecmp(product, products[i]) == 0) return 1;
    }
    return 0;
}

// ===== Create CSV header =====
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

// ===== Helpers for dates =====
void getCurrentDateStr(char *buf, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(buf, size, "%Y-%m-%d", &tm);
}

// produce date string offset by offsetDays from today
void getDateStrOffset(char *buf, size_t size, int offsetDays) {
    time_t t = time(NULL) + offsetDays * 24 * 3600;
    struct tm tm = *localtime(&t);
    strftime(buf, size, "%Y-%m-%d", &tm);
}

// convert YYYY-MM-DD to epoch (midnight)
int dateToEpoch(const char *date, time_t *out) {
    struct tm tm = {0};
    if (sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3) return 0;
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0;
    *out = mktime(&tm);
    return (*out != (time_t)-1);
}

// ===== Validate Date Format YYYY-MM-DD =====
int isValidDateFormat(const char *date) {
    if (!date) return 0;
    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date[i])) return 0;
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

// ===== Check if ShippingDate >= OrderDate and within 7 days =====
int isShippingAfterOrderWithin7(const char *orderDate, const char *shippingDate) {
    time_t orderEpoch, shipEpoch;
    if (!dateToEpoch(orderDate, &orderEpoch)) return 0;
    if (!dateToEpoch(shippingDate, &shipEpoch)) return 0;

    double diff = difftime(shipEpoch, orderEpoch);
    if (diff < 0) return 0; // shipping before order
    if (diff > 7 * 24 * 3600) return 0; // more than 7 days after order
    return 1;
}

// ===== Helper: Check if string is numeric =====
int isNumeric(const char *str) {
    if (!str || *str == '\0') return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

// ===== Helper: Check duplicate OrderID =====
int isDuplicateOrderID(const char *orderID, const char *excludeID) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0; // file not exist = no duplicate

    char line[512];
    // read header if present
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        char id[32];
        // read until first comma
        sscanf(line, "%31[^,],", id);
        if (strcmp(id, orderID) == 0) {
            if (excludeID == NULL) {
                fclose(fp);
                return 1;
            } else {
                if (strcmp(id, excludeID) != 0) {
                    fclose(fp);
                    return 1;
                }
            }
        }
    }
    fclose(fp);
    return 0;
}

// ===== Core Functions =====
int AddOrderToFile(Order o) {
    // OrderDate expected to be set already (current date)
    if (!isNumeric(o.OrderID)) return 0;
    if (isDuplicateOrderID(o.OrderID, NULL)) return 0;
    if (!isValidProduct(o.ProductName)) return 0;
    if (!isValidDateFormat(o.OrderDate)) return 0;
    if (!isValidDateFormat(o.ShippingDate)) return 0;
    if (!isShippingAfterOrderWithin7(o.OrderDate, o.ShippingDate)) return 0;

    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) return 0;
    fprintf(fp, "%s,%s,%s,%s,%s\n", o.OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
    fclose(fp);
    return 1;
}

int UpdateOrderInFile(const char *oldID, Order o) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    FILE *temp = fopen("temp.csv", "w");
    if (!temp) { fclose(fp); return 0; }

    char line[512], id[32];
    int found = 0;

    // Copy header if exists
    if (fgets(line, sizeof(line), fp)) {
        fputs(line, temp);
    } else {
        // nothing in file
        fclose(fp); fclose(temp);
        remove("temp.csv");
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%31[^,],", id);
        if (strcmp(id, oldID) == 0) {
            found = 1;
            // Validate new data
            if (!isNumeric(o.OrderID) || isDuplicateOrderID(o.OrderID, oldID) ||
                !isValidProduct(o.ProductName) || !isValidDateFormat(o.OrderDate) ||
                !isValidDateFormat(o.ShippingDate) || !isShippingAfterOrderWithin7(o.OrderDate, o.ShippingDate)) {
                fclose(fp); fclose(temp); remove("temp.csv");
                return 0;
            }
            fprintf(temp, "%s,%s,%s,%s,%s\n", o.OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
        } else {
            fputs(line, temp);
        }
    }

    fclose(fp); fclose(temp);
    remove(FILE_NAME);
    rename("temp.csv", FILE_NAME);
    return found;
}

int DeleteOrderInFile(const char *id) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    FILE *temp = fopen("temp.csv", "w");
    if (!temp) { fclose(fp); return 0; }

    char line[512], curID[32];
    int found = 0;

    // copy header
    if (fgets(line, sizeof(line), fp)) {
        fputs(line, temp);
    } else {
        fclose(fp); fclose(temp);
        remove("temp.csv");
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%31[^,],", curID);
        if (strcmp(curID, id) == 0) {
            found = 1;
            continue;
        }
        fputs(line, temp);
    }

    fclose(fp); fclose(temp);
    remove(FILE_NAME);
    rename("temp.csv", FILE_NAME);
    return found;
}

// ===== Interactive Versions =====
void AddOrder() {
    Order o;
    // OrderID
    while (1) {
        printf("Enter OrderID (numbers only): ");
        if (scanf("%9s", o.OrderID) != 1) { while (getchar() != '\n'); continue; }
        if (!isNumeric(o.OrderID)) {
            printf("Error: OrderID must be numeric only.\n");
            continue;
        }
        if (isDuplicateOrderID(o.OrderID, NULL)) {
            printf("Error: OrderID already exists. Please enter a new one.\n");
            continue;
        }
        break;
    }

    // Customer Name
    getchar();
    while (1) {
        printf("Enter Customer Name: ");
        fgets(o.CustomerName, sizeof(o.CustomerName), stdin);
        o.CustomerName[strcspn(o.CustomerName, "\n")] = 0;
        if (strlen(o.CustomerName) == 0) {
            printf("Error: Customer Name cannot be empty.\n");
            continue;
        }
        break;
    }

    // Product Name (must be in list)
    while (1) {
        printf("Enter Product Name (type 'list' to show products): ");
        fgets(o.ProductName, sizeof(o.ProductName), stdin);
        o.ProductName[strcspn(o.ProductName, "\n")] = 0;
        if (strcasecmp(o.ProductName, "list") == 0) {
            ListProduct();
            continue;
        }
        if (!isValidProduct(o.ProductName)) {
            printf("Error: Product not in list. Type 'list' to see available products.\n");
            continue;
        }
        break;
    }

    // Order Date: set to current date automatically
    getCurrentDateStr(o.OrderDate, sizeof(o.OrderDate));
    printf("Order Date set to current date: %s\n", o.OrderDate);

    // Shipping Date
    while (1) {
        printf("Enter Shipping Date (YYYY-MM-DD), must be within 7 days from Order Date: ");
        scanf("%19s", o.ShippingDate);
        if (!isValidDateFormat(o.ShippingDate)) {
            printf("Error: Invalid date format. Use YYYY-MM-DD.\n");
            continue;
        }
        if (!isShippingAfterOrderWithin7(o.OrderDate, o.ShippingDate)) {
            printf("Error: Shipping date must be same day or within 7 days after Order date.\n");
            continue;
        }
        break;
    }

    if (AddOrderToFile(o)) printf("Data Added Successfully\n");
    else printf("Add Failed\n");
}

void ReadOrders() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) {
        printf("File Not Found\n");
        return;
    }
    char line[512];
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
    char searchID[10], line[512];
    printf("Enter OrderID to Search: ");
    scanf("%9s", searchID);

    int found = 0;
    // skip header
    if (fgets(line, sizeof(line), fp)) {
        while (fgets(line, sizeof(line), fp)) {
            char id[32];
            sscanf(line, "%31[^,],", id);
            if (strcmp(id, searchID) == 0) {
                printf("Found: %s", line);
                found = 1;
            }
        }
    }
    if (!found) printf("Not Found\n");
    fclose(fp);
}

void UpdateOrder() {
    char searchID[10];
    Order o;
    printf("Enter OrderID to Update: ");
    scanf("%9s", searchID);

    // New OrderID
    while (1) {
        printf("Enter New OrderID (numbers only): ");
        if (scanf("%9s", o.OrderID) != 1) { while (getchar() != '\n'); continue; }
        if (!isNumeric(o.OrderID)) {
            printf("Error: OrderID must be numeric only.\n");
            continue;
        }
        if (isDuplicateOrderID(o.OrderID, searchID)) {
            printf("Error: OrderID already exists. Please enter a new one.\n");
            continue;
        }
        break;
    }

    // Customer Name
    getchar();
    while (1) {
        printf("Enter New Customer Name: ");
        fgets(o.CustomerName, sizeof(o.CustomerName), stdin);
        o.CustomerName[strcspn(o.CustomerName, "\n")] = 0;
        if (strlen(o.CustomerName) == 0) {
            printf("Error: Customer Name cannot be empty.\n");
            continue;
        }
        break;
    }

    // Product Name
    while (1) {
        printf("Enter New Product Name (type 'list' to show products): ");
        fgets(o.ProductName, sizeof(o.ProductName), stdin);
        o.ProductName[strcspn(o.ProductName, "\n")] = 0;
        if (strcasecmp(o.ProductName, "list") == 0) {
            ListProduct();
            continue;
        }
        if (!isValidProduct(o.ProductName)) {
            printf("Error: Product not in list. Type 'list' to see available products.\n");
            continue;
        }
        break;
    }

    // Order Date: set to current date automatically for update
    getCurrentDateStr(o.OrderDate, sizeof(o.OrderDate));
    printf("Order Date updated to current date: %s\n", o.OrderDate);

    // Shipping Date
    while (1) {
        printf("Enter New Shipping Date (YYYY-MM-DD), must be within 7 days from Order Date: ");
        scanf("%19s", o.ShippingDate);
        if (!isValidDateFormat(o.ShippingDate)) {
            printf("Error: Invalid date format. Use YYYY-MM-DD.\n");
            continue;
        }
        if (!isShippingAfterOrderWithin7(o.OrderDate, o.ShippingDate)) {
            printf("Error: Shipping date must be same day or within 7 days after Order date.\n");
            continue;
        }
        break;
    }

    if (UpdateOrderInFile(searchID, o)) printf("Updated Successfully\n");
    else printf("Update Failed\n");
}

void DeleteOrder() {
    char id[10];
    printf("Enter OrderID to Delete: ");
    scanf("%9s", id);
    if (DeleteOrderInFile(id)) printf("Deleted Successfully\n");
    else printf("Delete Failed\n");
}

// ===== Unit Tests =====
void UnitTest_AddOrder() {
    printf("\n[Unit Test] AddOrder()\n");
    CreateOrder();

    Order o1;
    strcpy(o1.OrderID, "101");
    strcpy(o1.CustomerName, "John");
    strcpy(o1.ProductName, "Phone");
    getCurrentDateStr(o1.OrderDate, sizeof(o1.OrderDate));
    getDateStrOffset(o1.ShippingDate, sizeof(o1.ShippingDate), 4); // +4 days
    assert(AddOrderToFile(o1) == 1);

    Order oDup;
    strcpy(oDup.OrderID, "101");
    strcpy(oDup.CustomerName, "Dup");
    strcpy(oDup.ProductName, "Laptop");
    getCurrentDateStr(oDup.OrderDate, sizeof(oDup.OrderDate));
    getDateStrOffset(oDup.ShippingDate, sizeof(oDup.ShippingDate), 1);
    assert(AddOrderToFile(oDup) == 0); // duplicate ID

    Order oBadProduct;
    strcpy(oBadProduct.OrderID, "102");
    strcpy(oBadProduct.CustomerName, "Kate");
    strcpy(oBadProduct.ProductName, "NonExistProduct"); // invalid product
    getCurrentDateStr(oBadProduct.OrderDate, sizeof(oBadProduct.OrderDate));
    getDateStrOffset(oBadProduct.ShippingDate, sizeof(oBadProduct.ShippingDate), 2);
    assert(AddOrderToFile(oBadProduct) == 0);

    Order oShipBefore;
    strcpy(oShipBefore.OrderID, "103");
    strcpy(oShipBefore.CustomerName, "Mike");
    strcpy(oShipBefore.ProductName, "TV"); // TV not in list -> should be invalid, so use "Monitor" instead
    strcpy(oShipBefore.ProductName, "Monitor");
    getCurrentDateStr(oShipBefore.OrderDate, sizeof(oShipBefore.OrderDate));
    getDateStrOffset(oShipBefore.ShippingDate, sizeof(oShipBefore.ShippingDate), -2); // before order
    assert(AddOrderToFile(oShipBefore) == 0);

    Order oSameDay;
    strcpy(oSameDay.OrderID, "104");
    strcpy(oSameDay.CustomerName, "Jane");
    strcpy(oSameDay.ProductName, "Book"); // not in list -> use "Tablet" as valid
    strcpy(oSameDay.ProductName, "Tablet");
    getCurrentDateStr(oSameDay.OrderDate, sizeof(oSameDay.OrderDate));
    getDateStrOffset(oSameDay.ShippingDate, sizeof(oSameDay.ShippingDate), 0); // same day
    assert(AddOrderToFile(oSameDay) == 1);

    // test shipping >7 days
    Order oTooLate;
    strcpy(oTooLate.OrderID, "105");
    strcpy(oTooLate.CustomerName, "LateGuy");
    strcpy(oTooLate.ProductName, "GPU");
    getCurrentDateStr(oTooLate.OrderDate, sizeof(oTooLate.OrderDate));
    getDateStrOffset(oTooLate.ShippingDate, sizeof(oTooLate.ShippingDate), 10); // +10 days -> fail
    assert(AddOrderToFile(oTooLate) == 0);

    printf("All AddOrder tests passed!\n");
}

void UnitTest_UpdateOrder() {
    printf("\n[Unit Test] UpdateOrder()\n");
    CreateOrder();
    Order o1;
    strcpy(o1.OrderID, "201");
    strcpy(o1.CustomerName, "Alice");
    strcpy(o1.ProductName, "Laptop");
    getCurrentDateStr(o1.OrderDate, sizeof(o1.OrderDate));
    getDateStrOffset(o1.ShippingDate, sizeof(o1.ShippingDate), 5);
    AddOrderToFile(o1);

    // valid update
    Order newO;
    strcpy(newO.OrderID, "202");
    strcpy(newO.CustomerName, "AliceNew");
    strcpy(newO.ProductName, "PC");
    getCurrentDateStr(newO.OrderDate, sizeof(newO.OrderDate));
    getDateStrOffset(newO.ShippingDate, sizeof(newO.ShippingDate), 3);
    assert(UpdateOrderInFile("201", newO) == 1);

    // try updating existing ID 202 to same ID (should be allowed since excludeID)
    Order dupO;
    strcpy(dupO.OrderID, "202");
    strcpy(dupO.CustomerName, "Bob");
    strcpy(dupO.ProductName, "Monitor");
    getCurrentDateStr(dupO.OrderDate, sizeof(dupO.OrderDate));
    getDateStrOffset(dupO.ShippingDate, sizeof(dupO.ShippingDate), 2);
    assert(UpdateOrderInFile("202", dupO) == 1);

    // update non-existing
    Order notExist;
    strcpy(notExist.OrderID, "999");
    strcpy(notExist.CustomerName, "NoOne");
    strcpy(notExist.ProductName, "Keyboard");
    getCurrentDateStr(notExist.OrderDate, sizeof(notExist.OrderDate));
    getDateStrOffset(notExist.ShippingDate, sizeof(notExist.ShippingDate), 1);
    assert(UpdateOrderInFile("888", notExist) == 0);

    // try invalid product in update
    Order badProd;
    strcpy(badProd.OrderID, "203");
    strcpy(badProd.CustomerName, "BadProd");
    strcpy(badProd.ProductName, "UnknownThing");
    getCurrentDateStr(badProd.OrderDate, sizeof(badProd.OrderDate));
    getDateStrOffset(badProd.ShippingDate, sizeof(badProd.ShippingDate), 1);
    // first add a valid record to update
    Order base;
    strcpy(base.OrderID, "204");
    strcpy(base.CustomerName, "Base");
    strcpy(base.ProductName, "SSD");
    getCurrentDateStr(base.OrderDate, sizeof(base.OrderDate));
    getDateStrOffset(base.ShippingDate, sizeof(base.ShippingDate), 1);
    assert(AddOrderToFile(base) == 1);
    // now attempt update with bad product
    assert(UpdateOrderInFile("204", badProd) == 0);

    printf("All UpdateOrder tests passed!\n");
}

void UnitTest_DeleteOrder() {
    printf("\n[Unit Test] DeleteOrder()\n");
    CreateOrder();
    Order o1;
    strcpy(o1.OrderID, "301");
    strcpy(o1.CustomerName, "Bob");
    strcpy(o1.ProductName, "Tablet");
    getCurrentDateStr(o1.OrderDate, sizeof(o1.OrderDate));
    getDateStrOffset(o1.ShippingDate, sizeof(o1.ShippingDate), 3);
    AddOrderToFile(o1);

    assert(DeleteOrderInFile("301") == 1);
    assert(DeleteOrderInFile("301") == 0);

    printf("All DeleteOrder tests passed!\n");
}

// ===== E2E Test (Case 11) =====
void E2E_Test() {
    printf("\n[E2E Test] Start\n");
    CreateOrder();
    // Add 3 orders
    Order a,b,c;
    strcpy(a.OrderID, "401"); strcpy(a.CustomerName, "E2E_A"); strcpy(a.ProductName, "Laptop");
    getCurrentDateStr(a.OrderDate, sizeof(a.OrderDate)); getDateStrOffset(a.ShippingDate, sizeof(a.ShippingDate), 1);
    assert(AddOrderToFile(a) == 1);

    strcpy(b.OrderID, "402"); strcpy(b.CustomerName, "E2E_B"); strcpy(b.ProductName, "Phone");
    getCurrentDateStr(b.OrderDate, sizeof(b.OrderDate)); getDateStrOffset(b.ShippingDate, sizeof(b.ShippingDate), 2);
    assert(AddOrderToFile(b) == 1);

    strcpy(c.OrderID, "403"); strcpy(c.CustomerName, "E2E_C"); strcpy(c.ProductName, "SSD");
    getCurrentDateStr(c.OrderDate, sizeof(c.OrderDate)); getDateStrOffset(c.ShippingDate, sizeof(c.ShippingDate), 0);
    assert(AddOrderToFile(c) == 1);

    printf("After adding 3 orders:\n");
    ReadOrders();

    // Update one order
    Order up;
    strcpy(up.OrderID, "402"); strcpy(up.CustomerName, "E2E_B_New"); strcpy(up.ProductName, "Monitor");
    getCurrentDateStr(up.OrderDate, sizeof(up.OrderDate)); getDateStrOffset(up.ShippingDate, sizeof(up.ShippingDate), 3);
    assert(UpdateOrderInFile("402", up) == 1);
    printf("After update:\n"); ReadOrders();

    // Delete one
    assert(DeleteOrderInFile("401") == 1);
    printf("After delete 401:\n"); ReadOrders();

    printf("[E2E Test] Completed\n");
}
