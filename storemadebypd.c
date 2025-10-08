#include "storemadebypd.h"

// ===== Helper: date/time =====
void getCurrentDateStr(char *buf, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(buf, size, "%Y-%m-%d", &tm);
}

void getDateStrOffset(char *buf, size_t size, int offsetDays) {
    time_t t = time(NULL) + offsetDays * 24 * 3600;
    struct tm tm = *localtime(&t);
    strftime(buf, size, "%Y-%m-%d", &tm);
}

int dateToEpoch(const char *date, time_t *out) {
    struct tm tm = {0};
    if (sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3) return 0;
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    *out = mktime(&tm);
    return (*out != (time_t)-1);
}

// ===== Validation =====
int isNumeric(const char *s) {
    if (!s || !*s) return 0;
    for (int i = 0; s[i]; i++)
        if (!isdigit((unsigned char)s[i])) return 0;
    return 1;
}

int isValidDateFormat(const char *d) {
    if (!d || strlen(d) != 10 || d[4] != '-' || d[7] != '-') return 0;
    int y, m, da;
    sscanf(d, "%d-%d-%d", &y, &m, &da);
    if (y < 1900 || m < 1 || m > 12 || da < 1 || da > 31) return 0;
    return 1;
}

int isShippingAfterOrderWithin7(const char *order, const char *ship) {
    time_t o, s;
    if (!dateToEpoch(order, &o) || !dateToEpoch(ship, &s)) return 0;
    double diff = difftime(s, o);
    return (diff >= 0 && diff <= 7 * 24 * 3600);
}

int isValidProduct(const char *product) {
    const char *list[] = {"Laptop","Phone","Tablet","PC","Keyboard","Mouse","Monitor",
        "Headphones","Webcam","Router","SSD","HDD","GPU","Motherboard","PowerSupply"};
    for (int i=0;i<15;i++) if (strcasecmp(product, list[i])==0) return 1;
    return 0;
}

// ===== File Operations =====
void CreateOrder() {
    FILE *fp = fopen(FILE_NAME, "w");
    fprintf(fp, "OrderID,CustomerName,ProductName,OrderDate,ShippingDate\n");
    fclose(fp);
    printf("CSV File Created Successfully with Header\n");
}

int isDuplicateOrderID(const char *id, const char *exclude) {
    FILE *fp = fopen(FILE_NAME, "r"); if (!fp) return 0;
    char line[256], cur[32];
    fgets(line, sizeof(line), fp);
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%31[^,],", cur);
        if (strcmp(cur, id) == 0 && (!exclude || strcmp(cur, exclude) != 0)) {
            fclose(fp); return 1;
        }
    }
    fclose(fp);
    return 0;
}

int AddOrderToFile(Order o) {
    if (!isNumeric(o.OrderID) || isDuplicateOrderID(o.OrderID, NULL) ||
        !isValidProduct(o.ProductName) || !isValidDateFormat(o.OrderDate) ||
        !isValidDateFormat(o.ShippingDate) || 
        !isShippingAfterOrderWithin7(o.OrderDate, o.ShippingDate)) return 0;
    
    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) return 0;
    fprintf(fp, "%s,%s,%s,%s,%s\n", o.OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
    fclose(fp);
    return 1;
}

int UpdateOrderInFile(const char *oldID, Order o) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    FILE *tmp = fopen("tmp.csv", "w");
    if (!tmp) { fclose(fp); return 0; }

    char line[256], id[32];
    int found = 0;
    fgets(line, sizeof(line), fp);
    fputs(line, tmp);
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%31[^,],", id);
        if (strcmp(id, oldID) == 0) {
            found = 1;
            if (!isNumeric(o.OrderID) || isDuplicateOrderID(o.OrderID, oldID) ||
                !isValidProduct(o.ProductName) || !isValidDateFormat(o.OrderDate) ||
                !isValidDateFormat(o.ShippingDate) || 
                !isShippingAfterOrderWithin7(o.OrderDate, o.ShippingDate)) {
                fclose(fp); fclose(tmp); remove("tmp.csv"); return 0;
            }
            fprintf(tmp, "%s,%s,%s,%s,%s\n", o.OrderID, o.CustomerName, o.ProductName, o.OrderDate, o.ShippingDate);
        } else fputs(line, tmp);
    }
    fclose(fp); fclose(tmp);
    remove(FILE_NAME); rename("tmp.csv", FILE_NAME);
    return found;
}

int DeleteOrderInFile(const char *id) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    FILE *tmp = fopen("tmp.csv", "w");
    if (!tmp) { fclose(fp); return 0; }

    char line[256], cur[32];
    int found = 0;
    fgets(line, sizeof(line), fp);
    fputs(line, tmp);
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%31[^,],", cur);
        if (strcmp(cur, id) == 0) { found = 1; continue; }
        fputs(line, tmp);
    }
    fclose(fp); fclose(tmp);
    remove(FILE_NAME); rename("tmp.csv", FILE_NAME);
    return found;
}

// ===== Menu and Interactive =====
void DisplayMenu() {
    printf("\n===== Order Management System =====\n");
    printf("1. List Product\n");
    printf("2. Add Order\n");
    printf("3. Read Orders\n");
    printf("4. Search Order\n");
    printf("5. Update Order\n");
    printf("6. Delete Order\n");
    printf("7. Create CSV File\n");
    printf("8. Unit Test: AddOrder\n");
    printf("9. Unit Test: UpdateOrder\n");
    printf("10. Unit Test: DeleteOrder\n");
    printf("11. E2E Test (All-in-one)\n");
    printf("0. Exit\n");
}

void ListProduct() {
    const char *items[] = {
        "Laptop","Phone","Tablet","PC","Keyboard","Mouse","Monitor",
        "Headphones","Webcam","Router","SSD","HDD","GPU","Motherboard","PowerSupply"
    };
    printf("\n--- Product List ---\n");
    for (int i=0;i<15;i++) printf("%2d. %s\n", i+1, items[i]);
}

// ===== Interactive Versions =====
void AddOrder() {
    Order o;

    // ===== OrderID =====
    while (1) {
        printf("Enter OrderID (numbers only, up to 9 digits): ");
        fgets(o.OrderID, sizeof(o.OrderID), stdin);
        o.OrderID[strcspn(o.OrderID, "\n")] = 0;

        if (strlen(o.OrderID) == 0) {
            printf("Error: OrderID cannot be empty.\n");
            continue;
        }
        if (strlen(o.OrderID) > 9) {
            printf("Error: OrderID must not exceed 9 digits.\n");
            continue;
        }
        if (!isNumeric(o.OrderID)) {
            printf("Error: OrderID must be numeric only.\n");
            continue;
        }
        if (isDuplicateOrderID(o.OrderID, NULL)) {
            printf("Error: OrderID already exists.\n");
            continue;
        }
        break;
    }

    // ===== Customer Name =====
    while (1) {
        printf("Enter Customer Name (max 49 characters): ");
        fgets(o.CustomerName, sizeof(o.CustomerName), stdin);
        o.CustomerName[strcspn(o.CustomerName, "\n")] = 0;

        if (strlen(o.CustomerName) == 0) {
            printf("Error: Customer Name cannot be empty.\n");
            continue;
        }
        if (strlen(o.CustomerName) > 49) {
            printf("Error: Customer Name must not exceed 49 characters.\n");
            continue;
        }
        break;
    }

    // ===== Product Name =====
    while (1) {
        printf("Enter Product Name (type 'list' to show products, max 49 chars): ");
        fgets(o.ProductName, sizeof(o.ProductName), stdin);
        o.ProductName[strcspn(o.ProductName, "\n")] = 0;

        if (strlen(o.ProductName) == 0) {
            printf("Error: Product Name cannot be empty.\n");
            continue;
        }
        if (strlen(o.ProductName) > 49) {
            printf("Error: Product Name must not exceed 49 characters.\n");
            continue;
        }
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

    // ===== Order Date =====
    getCurrentDateStr(o.OrderDate, sizeof(o.OrderDate));
    printf("Order Date set to current date: %s\n", o.OrderDate);

    // ===== Shipping Date =====
    while (1) {
        printf("Enter Shipping Date (YYYY-MM-DD, within 7 days from Order Date): ");
        fgets(o.ShippingDate, sizeof(o.ShippingDate), stdin);
        o.ShippingDate[strcspn(o.ShippingDate, "\n")] = 0;

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

    // ===== Search OrderID =====
    while (1) {
        printf("Enter OrderID to Update (up to 9 digits): ");
        fgets(searchID, sizeof(searchID), stdin);
        searchID[strcspn(searchID, "\n")] = 0;

        if (strlen(searchID) == 0) {
            printf("Error: OrderID cannot be empty.\n");
            continue;
        }
        if (strlen(searchID) > 9) {
            printf("Error: OrderID must not exceed 9 digits.\n");
            continue;
        }
        break;
    }

    // ===== New OrderID =====
    while (1) {
        printf("Enter New OrderID (numbers only, up to 9 digits): ");
        fgets(o.OrderID, sizeof(o.OrderID), stdin);
        o.OrderID[strcspn(o.OrderID, "\n")] = 0;

        if (strlen(o.OrderID) == 0) {
            printf("Error: OrderID cannot be empty.\n");
            continue;
        }
        if (strlen(o.OrderID) > 9) {
            printf("Error: OrderID must not exceed 9 digits.\n");
            continue;
        }
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

    // ===== Customer Name =====
    while (1) {
        printf("Enter New Customer Name (max 49 characters): ");
        fgets(o.CustomerName, sizeof(o.CustomerName), stdin);
        o.CustomerName[strcspn(o.CustomerName, "\n")] = 0;

        if (strlen(o.CustomerName) == 0) {
            printf("Error: Customer Name cannot be empty.\n");
            continue;
        }
        if (strlen(o.CustomerName) > 49) {
            printf("Error: Customer Name must not exceed 49 characters.\n");
            continue;
        }
        break;
    }

    // ===== Product Name =====
    while (1) {
        printf("Enter New Product Name (type 'list' to show products, max 49 chars): ");
        fgets(o.ProductName, sizeof(o.ProductName), stdin);
        o.ProductName[strcspn(o.ProductName, "\n")] = 0;

        if (strlen(o.ProductName) == 0) {
            printf("Error: Product Name cannot be empty.\n");
            continue;
        }
        if (strlen(o.ProductName) > 49) {
            printf("Error: Product Name must not exceed 49 characters.\n");
            continue;
        }
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

    // ===== Order Date =====
    getCurrentDateStr(o.OrderDate, sizeof(o.OrderDate));
    printf("Order Date updated to current date: %s\n", o.OrderDate);

    // ===== Shipping Date =====
    while (1) {
        printf("Enter New Shipping Date (YYYY-MM-DD, within 7 days): ");
        fgets(o.ShippingDate, sizeof(o.ShippingDate), stdin);
        o.ShippingDate[strcspn(o.ShippingDate, "\n")] = 0;

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

    while (1) {
        printf("Enter OrderID to Delete (up to 9 digits): ");
        fgets(id, sizeof(id), stdin);
        id[strcspn(id, "\n")] = 0;

        if (strlen(id) == 0) {
            printf("Error: OrderID cannot be empty.\n");
            continue;
        }
        if (strlen(id) > 9) {
            printf("Error: OrderID must not exceed 9 digits.\n");
            continue;
        }
        break;
    }

    if (DeleteOrderInFile(id)) printf("Deleted Successfully\n");
    else printf("Delete Failed\n");
}

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

        switch (choice) {
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
    } while (choice != 0);
    return 0;
}