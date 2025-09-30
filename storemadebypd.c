#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "orders.csv"

typedef struct {
    char OrderID[10];
    char CustomerName[50];
    char ProductName[50];
    char OrderDate[20];
    char ShippingDate[20];
} Order;

// ====== Function Prototype ======
void DisplayMenu();
void CreateOrder();
void AddOrder();
void ReadOrders();
void SearchOrder();
void UpdateOrder();
void DeleteOrder();
int isValidDateFormat(const char *date);
int isShippingAfterOrder(const char *orderDate, const char *shippingDate);

// ====== MAIN ======
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
            case 0: printf("Exit Program, Thank You\n"); break;
            default: printf("Incorrect Menu! Please Select Again\n");
        }
    } while(choice != 0);

    return 0;
}

// ====== DISPLAY MENU ======
void DisplayMenu() {
    printf("\n===== Welcome to Order and Delivery Information Management System =====\n");
    printf("1. AddOrder\n");
    printf("2. ReadOrders\n");
    printf("3. SearchOrder\n");
    printf("4. UpdateOrder\n");
    printf("5. DeleteOrder\n");
    printf("6. Create New Order CSV File with Header\n"); 
    printf("0. Exit Program\n");
}

// ====== Validate Date Format YYYY-MM-DD (with real days in month) ======
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

    // ตรวจสอบจำนวนวันในเดือน
    int daysInMonth;
    switch(month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            daysInMonth = 31; break;
        case 4: case 6: case 9: case 11:
            daysInMonth = 30; break;
        case 2:
            // ตรวจสอบ leap year
            if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
                daysInMonth = 29;
            else
                daysInMonth = 28;
            break;
        default:
            return 0;
    }

    if (day < 1 || day > daysInMonth) return 0;

    return 1; // ✅ valid date
}

// ====== Check if ShippingDate > OrderDate ======
int isShippingAfterOrder(const char *orderDate, const char *shippingDate) {
    int orderY, orderM, orderD;
    int shipY, shipM, shipD;

    sscanf(orderDate, "%d-%d-%d", &orderY, &orderM, &orderD);
    sscanf(shippingDate, "%d-%d-%d", &shipY, &shipM, &shipD);

    int orderVal = orderY * 10000 + orderM * 100 + orderD;
    int shipVal  = shipY * 10000 + shipM * 100 + shipD;

    return shipVal > orderVal; 
}


// ----------------- Main Function -----------------

void CreateOrder() {
    FILE *fp = fopen(FILE_NAME, "w"); // ใช้ "w" เพื่อเขียนทับ/สร้างใหม่
    if (!fp) {
        printf("Can't Create File\n");
        return;
    }
    fprintf(fp, "OrderID,CustomerName,ProductName,OrderDate,ShippingDate\n");
    fclose(fp);
    printf("CSV File Created Successfully with Header\n");
}

void AddOrder() {
    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) {
        printf("Can't Open File\n");
        return;
    }
    Order o;
    printf("Enter OrderID: "); scanf("%s", o.OrderID);
    printf("Enter Customer Name: "); getchar(); fgets(o.CustomerName, 50, stdin);
    o.CustomerName[strcspn(o.CustomerName, "\n")] = 0;
    printf("Enter Product Name: "); fgets(o.ProductName, 50, stdin);
    o.ProductName[strcspn(o.ProductName, "\n")] = 0;

    // ตรวจสอบ OrderDate
    do {
        printf("Enter Order Date (YYYY-MM-DD): ");
        scanf("%s", o.OrderDate);
        if (!isValidDateFormat(o.OrderDate)) {
            printf("Invalid Date! Please Try Again (YYYY-MM-DD)\n");
        }
    } while (!isValidDateFormat(o.OrderDate));

    // ตรวจสอบ ShippingDate (ต้องอยู่หลัง OrderDate)
    do {
        printf("Enter Shipping Date (YYYY-MM-DD): ");
        scanf("%s", o.ShippingDate);
        if (!isValidDateFormat(o.ShippingDate)) {
            printf("Invalid Date! Please Try Again (YYYY-MM-DD)\n");
            continue;
        }
        if (!isShippingAfterOrder(o.OrderDate, o.ShippingDate)) {
            printf("Shipping Date must be AFTER Order Date! Please Try Again\n");
        }
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
    printf("\n----- Order information -----\n");
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
    printf("Enter OrderID You Want to Search For: ");
    scanf("%s", searchID);

    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, searchID)) {
            printf("Found Info: %s", line);
            found = 1;
        }
    }
    if (!found) printf("Not Found Info\n");
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
    printf("Enter OrderID You Want to Update For: ");
    scanf("%s", searchID);

    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        char OrderID[10];
        sscanf(line, "%[^,],", OrderID);
        if (strcmp(OrderID, searchID) == 0) {
            found = 1;
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
                if (!isValidDateFormat(o.ShippingDate)) {
                    printf("Invalid Date! Please Try Again (YYYY-MM-DD)\n");
                    continue;
                }
                if (!isShippingAfterOrder(o.OrderDate, o.ShippingDate)) {
                    printf("Shipping Date must be AFTER Order Date! Please Try Again\n");
                }
            } while (!isValidDateFormat(o.ShippingDate) || !isShippingAfterOrder(o.OrderDate, o.ShippingDate));

            fprintf(temp, "%s,%s,%s,%s,%s\n", OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
        } else {
            fputs(line, temp);
        }
    }
    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.csv", FILE_NAME);
    if (found) printf("Updated Successfully\n");
    else printf("Not Found OrderID\n");
}

void DeleteOrder() {
    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.csv", "w");
    if (!fp || !temp) {
        printf("Can't Open File\n");
        return;
    }
    char searchID[10], line[200];
    printf("Enter OrderID You Want to Delete: ");
    scanf("%s", searchID);

    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        char OrderID[10];
        sscanf(line, "%[^,],", OrderID);
        if (strcmp(OrderID, searchID) == 0) {
            found = 1;
            continue; // ข้ามบรรทัดนี้ (ไม่เขียนลงไฟล์ใหม่)
        }
        fputs(line, temp);
    }
    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.csv", FILE_NAME);
    if (found) printf("Delete Successfuly\n");
    else printf("Not Found OrderID\n");
}