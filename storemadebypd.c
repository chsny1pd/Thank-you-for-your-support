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

void DisplayMenu();
void AddOrder();
void ReadOrders();
void SearchOrder();
void UpdateOrder();
void DeleteOrder();

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
            case 0: printf("Exit Program, Thank You\n"); break;
            default: printf("Incorrect Menu! Please Select Again\n");
        }
    } while(choice != 0);

    return 0;
}

void DisplayMenu() {
    printf("\n===== Welcome to Order and Delivery Information Management System =====\n");
    printf("1. AddOrder\n");
    printf("2. ReadOrders\n");
    printf("3. SearchOrder\n");
    printf("4. UpdateOrder\n");
    printf("5. DeleteOrder\n");
    printf("0. Exit Program\n");
}

// ----------------- Main Function -----------------

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
    printf("Enter Order Date (YYYY-MM-DD): "); scanf("%s", o.OrderDate);
    printf("Enter Shipping Date (YYYY-MM-DD): "); scanf("%s", o.ShippingDate);

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
            printf("Enter New Order Date: "); scanf("%s", o.OrderDate);
            printf("Enter New Shipping Date: "); scanf("%s", o.ShippingDate);

            fprintf(temp, "%s,%s,%s,%s,%s\n", OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
        } else {
            fputs(line, temp);
        }
    }
    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.csv", FILE_NAME);
    if (found) printf("Updated Succesfully\n");
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