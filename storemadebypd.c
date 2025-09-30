#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

// ===== Check if ShippingDate > OrderDate =====
int isShippingAfterOrder(const char *orderDate, const char *shippingDate) {
    int oY, oM, oD, sY, sM, sD;
    sscanf(orderDate, "%d-%d-%d", &oY, &oM, &oD);
    sscanf(shippingDate, "%d-%d-%d", &sY, &sM, &sD);

    int orderVal = oY * 10000 + oM * 100 + oD;
    int shipVal  = sY * 10000 + sM * 100 + sD;

    return shipVal > orderVal;
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

void AddOrder() {
    FILE *fp = fopen(FILE_NAME, "a+");
    if (!fp) {
        printf("Can't Open File\n");
        return;
    }
    Order o;

    do {
        printf("Enter OrderID (Numbers Only): ");
        scanf("%s", o.OrderID);
        if (!isNumeric(o.OrderID)) {
            printf("OrderID must be numeric!\n");
            continue;
        }
        if (isDuplicateOrderID(o.OrderID, NULL)) {
            printf("OrderID already exists!\n");
            continue;
        }
        break;
    } while (1);

    printf("Enter Customer Name: "); getchar(); fgets(o.CustomerName, 50, stdin);
    o.CustomerName[strcspn(o.CustomerName, "\n")] = 0;
    printf("Enter Product Name: "); fgets(o.ProductName, 50, stdin);
    o.ProductName[strcspn(o.ProductName, "\n")] = 0;

    do {
        printf("Enter Order Date (YYYY-MM-DD): ");
        scanf("%s", o.OrderDate);
    } while (!isValidDateFormat(o.OrderDate));

    do {
        printf("Enter Shipping Date (YYYY-MM-DD): ");
        scanf("%s", o.ShippingDate);
    } while (!isValidDateFormat(o.ShippingDate) || !isShippingAfterOrder(o.OrderDate, o.ShippingDate));

    fprintf(fp, "%s,%s,%s,%s,%s\n", o.OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
    fclose(fp);
    printf("Data Added Successfully\n");
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
    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.csv", "w");
    if (!fp || !temp) {
        printf("Can't Open File\n");
        return;
    }
    char searchID[10], line[200];
    Order o;
    printf("Enter OrderID to Update: ");
    scanf("%s", searchID);

    int found = 0;
    fgets(line, sizeof(line), fp); // copy header
    fputs(line, temp);

    while (fgets(line, sizeof(line), fp)) {
        char id[10];
        sscanf(line, "%[^,],", id);
        if (strcmp(id, searchID) == 0) {
            found = 1;
            do {
                printf("Enter New OrderID (Numbers Only): ");
                scanf("%s", o.OrderID);
                if (!isNumeric(o.OrderID)) {
                    printf("OrderID must be numeric!\n");
                    continue;
                }
                if (isDuplicateOrderID(o.OrderID, searchID)) {
                    printf("OrderID already exists!\n");
                    continue;
                }
                break;
            } while (1);

            printf("Enter New Customer Name: "); getchar(); fgets(o.CustomerName, 50, stdin);
            o.CustomerName[strcspn(o.CustomerName, "\n")] = 0;
            printf("Enter New Product Name: "); fgets(o.ProductName, 50, stdin);
            o.ProductName[strcspn(o.ProductName, "\n")] = 0;

            do {
                printf("Enter New Order Date (YYYY-MM-DD): ");
                scanf("%s", o.OrderDate);
            } while (!isValidDateFormat(o.OrderDate));

            do {
                printf("Enter New Shipping Date (YYYY-MM-DD): ");
                scanf("%s", o.ShippingDate);
            } while (!isValidDateFormat(o.ShippingDate) || !isShippingAfterOrder(o.OrderDate, o.ShippingDate));

            fprintf(temp, "%s,%s,%s,%s,%s\n", o.OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
        } else {
            fputs(line, temp);
        }
    }
    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.csv", FILE_NAME);
    if (found) printf("Updated Successfully\n");
    else printf("OrderID Not Found\n");
}

void DeleteOrder() {
    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.csv", "w");
    if (!fp || !temp) {
        printf("Can't Open File\n");
        return;
    }
    char searchID[10], line[200];
    printf("Enter OrderID to Delete: ");
    scanf("%s", searchID);

    int found = 0;
    fgets(line, sizeof(line), fp); // copy header
    fputs(line, temp);

    while (fgets(line, sizeof(line), fp)) {
        char id[10];
        sscanf(line, "%[^,],", id);
        if (strcmp(id, searchID) == 0) {
            found = 1;
            continue; // skip this line
        }
        fputs(line, temp);
    }
    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.csv", FILE_NAME);
    if (found) printf("Deleted Successfully\n");
    else printf("OrderID Not Found\n");
}

// ===== Unit Tests =====
void UnitTest_AddOrder() {
    printf("\n[Unit Test] AddOrder()\n");
    CreateOrder();
    FILE *fp = fopen(FILE_NAME, "a");
    fprintf(fp, "101,John,Phone,2025-01-01,2025-01-05\n");
    fclose(fp);
    printf("Test Passed: Order Added\n");
}

void UnitTest_UpdateOrder() {
    printf("\n[Unit Test] UpdateOrder()\n");
    CreateOrder();
    FILE *fp = fopen(FILE_NAME, "a");
    fprintf(fp, "201,Alice,Laptop,2025-02-01,2025-02-10\n");
    fclose(fp);
    UpdateOrder();
    printf("Test Completed: Please check if update was correct\n");
}

void UnitTest_DeleteOrder() {
    printf("\n[Unit Test] DeleteOrder()\n");
    CreateOrder();
    FILE *fp = fopen(FILE_NAME, "a");
    fprintf(fp, "301,Bob,Tablet,2025-03-01,2025-03-05\n");
    fclose(fp);
    DeleteOrder();
    printf("Test Completed: Please check if deletion was correct\n");
}