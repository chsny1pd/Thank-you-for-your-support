#include "storemadebypd.h"

// ===== Helper =====
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
    if (sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday)!=3) return 0;
    tm.tm_year -= 1900; tm.tm_mon -= 1;
    *out = mktime(&tm);
    return (*out != (time_t)-1);
}

// ===== Validation =====
int isNumeric(const char *s) {
    if (!s || !*s) return 0;
    for (int i=0; s[i]; i++) if (!isdigit((unsigned char)s[i])) return 0;
    return 1;
}

int isValidDateFormat(const char *d) {
    if (!d || strlen(d)!=10 || d[4]!='-' || d[7]!='-') return 0;
    int y,m,day; sscanf(d,"%d-%d-%d",&y,&m,&day);
    if (y<1900||m<1||m>12||day<1||day>31) return 0;
    return 1;
}

int isShippingAfterOrderWithin7(const char *order, const char *ship) {
    time_t o,s;
    if (!dateToEpoch(order,&o) || !dateToEpoch(ship,&s)) return 0;
    double diff = difftime(s,o);
    return (diff>=0 && diff<=7*24*3600);
}

int isValidProduct(const char *product) {
    const char *list[]={"Laptop","Phone","Tablet","PC","Keyboard","Mouse","Monitor",
        "Headphones","Webcam","Router","SSD","HDD","GPU","Motherboard","PowerSupply"};
    for(int i=0;i<15;i++) if (strcasecmp(product,list[i])==0) return 1;
    return 0;
}

// ===== File Operations =====
void CreateOrder() {
    FILE *fp=fopen(FILE_NAME,"w");
    if (!fp){printf("Error creating file\n"); return;}
    fprintf(fp,"OrderID,CustomerName,ProductName,OrderDate,ShippingDate\n");
    fclose(fp);
    printf("CSV File Created Successfully\n");
}

int isDuplicateOrderID(const char *id, const char *exclude) {
    FILE *fp=fopen(FILE_NAME,"r"); if (!fp) return 0;
    char line[256],cur[32];
    fgets(line,sizeof(line),fp); // skip header
    while(fgets(line,sizeof(line),fp)){
        sscanf(line,"%31[^,],",cur);
        if(strcmp(cur,id)==0 && (!exclude || strcmp(cur,exclude)!=0)){
            fclose(fp); return 1;
        }
    }
    fclose(fp); return 0;
}

int AddOrderToFile(Order o) {
    if (!isNumeric(o.OrderID) || isDuplicateOrderID(o.OrderID,NULL) ||
        !isValidProduct(o.ProductName) || !isValidDateFormat(o.OrderDate) ||
        !isValidDateFormat(o.ShippingDate) ||
        !isShippingAfterOrderWithin7(o.OrderDate,o.ShippingDate)) return 0;

    FILE *fp=fopen(FILE_NAME,"a"); if (!fp) return 0;
    fprintf(fp,"%s,%s,%s,%s,%s\n",o.OrderID,o.CustomerName,o.ProductName,o.OrderDate,o.ShippingDate);
    fclose(fp);
    return 1;
}

int UpdateOrderInFile(const char *oldID, Order o) {
    FILE *fp=fopen(FILE_NAME,"r"); if(!fp) return 0;
    FILE *tmp=fopen("tmp.csv","w"); if(!tmp){fclose(fp); return 0;}
    char line[256],id[32]; int found=0;
    fgets(line,sizeof(line),fp); fputs(line,tmp); // header
    while(fgets(line,sizeof(line),fp)){
        sscanf(line,"%31[^,],",id);
        if(strcmp(id,oldID)==0){
            found=1;
            if(!isNumeric(o.OrderID)||isDuplicateOrderID(o.OrderID,oldID)||
               !isValidProduct(o.ProductName)||!isValidDateFormat(o.OrderDate)||
               !isValidDateFormat(o.ShippingDate)||!isShippingAfterOrderWithin7(o.OrderDate,o.ShippingDate)){
                fclose(fp); fclose(tmp); remove("tmp.csv"); return 0;
            }
            fprintf(tmp,"%s,%s,%s,%s,%s\n",o.OrderID,o.CustomerName,o.ProductName,o.OrderDate,o.ShippingDate);
        } else fputs(line,tmp);
    }
    fclose(fp); fclose(tmp); remove(FILE_NAME); rename("tmp.csv",FILE_NAME);
    return found;
}

int DeleteOrderInFile(const char *id) {
    FILE *fp=fopen(FILE_NAME,"r"); if(!fp) return 0;
    FILE *tmp=fopen("tmp.csv","w"); if(!tmp){fclose(fp); return 0;}
    char line[256],cur[32]; int found=0;
    fgets(line,sizeof(line),fp); fputs(line,tmp); // header
    while(fgets(line,sizeof(line),fp)){
        sscanf(line,"%31[^,],",cur);
        if(strcmp(cur,id)==0){found=1; continue;}
        fputs(line,tmp);
    }
    fclose(fp); fclose(tmp); remove(FILE_NAME); rename("tmp.csv",FILE_NAME);
    return found;
}

// ===== Menu =====
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
    printf("0. Exit Program\n");
}

// ===== Interactive Helpers =====
void ListProduct() {
    const char *items[]={"Laptop","Phone","Tablet","PC","Keyboard","Mouse","Monitor",
        "Headphones","Webcam","Router","SSD","HDD","GPU","Motherboard","PowerSupply"};
    printf("\n--- Product List ---\n");
    for(int i=0;i<15;i++) printf("%2d. %s\n",i+1,items[i]);
}

void safeInput(char *buf,size_t size){
    if(!fgets(buf,size,stdin)) buf[0]=0;
    if(strchr(buf,'\n')==NULL){int c;while((c=getchar())!='\n'&&c!=EOF);}
    else buf[strcspn(buf,"\n")]=0;
}

// ===== Add/Read/Search/Update/Delete =====
void AddOrder() {
    Order o;
    char temp[256]; // buffer ชั่วคราว

    // OrderID
    while(1){
        printf("Enter OrderID (numeric, max 10 chars): ");
        safeInput(temp,sizeof(temp));
        if(strlen(temp)==0){printf("OrderID cannot be empty\n"); continue;}
        if(strlen(temp)>10){printf("OrderID must <=10 chars\n"); continue;}
        if(!isNumeric(temp)){printf("OrderID must numeric\n"); continue;}
        if(isDuplicateOrderID(temp,NULL)){printf("OrderID already exists\n"); continue;}
        strcpy(o.OrderID,temp);
        break;
    }

    // CustomerName
    while(1){
        printf("Enter Customer Name (max 50 chars): ");
        safeInput(o.CustomerName,sizeof(o.CustomerName));
        if(strlen(o.CustomerName)==0){printf("Customer Name cannot be empty\n"); continue;}
        if(strlen(o.CustomerName)>50){printf("Customer Name must <=50 chars\n"); continue;}
        break;
    }

    // ProductName
    while(1){
        printf("Enter Product Name ('list' to show products): ");
        safeInput(o.ProductName,sizeof(o.ProductName));
        if(strlen(o.ProductName)==0){printf("Product Name cannot be empty\n"); continue;}
        if(strlen(o.ProductName)>49){printf("Product Name must <=49 chars\n"); continue;}
        if(strcasecmp(o.ProductName,"list")==0){ListProduct(); continue;}
        if(!isValidProduct(o.ProductName)){printf("Product not in list\n"); continue;}
        break;
    }

    getCurrentDateStr(o.OrderDate,sizeof(o.OrderDate));
    printf("Order Date: %s\n",o.OrderDate);

    // ShippingDate
    while(1){
        printf("Enter Shipping Date (YYYY-MM-DD within 7 days): ");
        safeInput(o.ShippingDate,sizeof(o.ShippingDate));
        if(!isValidDateFormat(o.ShippingDate)){printf("Invalid date\n"); continue;}
        if(!isShippingAfterOrderWithin7(o.OrderDate,o.ShippingDate)){printf("Shipping must be within 7 days\n"); continue;}
        break;
    }

    if(AddOrderToFile(o)) printf("Added Successfully\n"); else printf("Add Failed\n");
}

void ReadOrders() {
    FILE *fp=fopen(FILE_NAME,"r");
    if(!fp){printf("File Not Found\n"); return;}
    char line[512]; printf("\n--- Orders ---\n");
    while(fgets(line,sizeof(line),fp)) printf("%s",line);
    fclose(fp);
}

void SearchOrder() {
    char searchID[11]; // max 10 chars + null terminator
    while (1) {
        printf("Enter OrderID to search (max 10 chars): ");
        safeInput(searchID, sizeof(searchID));
        if (strlen(searchID) == 0) {
            printf("OrderID cannot be empty\n");
            continue;
        }
        if (strlen(searchID) > 10) {
            printf("OrderID must be <= 10 characters\n");
            continue;
        }
        if (!isNumeric(searchID)) {
            printf("OrderID must be numeric\n");
            continue;
        }
        break;
    }

    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) {
        printf("File Not Found\n");
        return;
    }

    char line[512];
    fgets(line, sizeof(line), fp); // skip header
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        char id[32];
        sscanf(line, "%31[^,],", id);
        if (strcmp(id, searchID) == 0) {
            printf("Found: %s", line);
            found = 1;
        }
    }
    if (!found) printf("Not Found\n");
    fclose(fp);
}

// ===== Update/Delete =====
void UpdateOrder() {
    char searchID[11];
    Order o;
    char temp[256]; // buffer ชั่วคราว

    // รับ OrderID ที่จะอัปเดต
    while(1){
        printf("Enter OrderID to Update (max 10 chars): ");
        safeInput(temp,sizeof(temp));
        if(strlen(temp)==0){printf("OrderID cannot be empty\n"); continue;}
        if(strlen(temp)>10){printf("OrderID must <=10 chars\n"); continue;}
        if(!isNumeric(temp)){printf("OrderID must numeric\n"); continue;}
        strcpy(searchID,temp);
        break;
    }

    // New OrderID
    while(1){
        printf("Enter New OrderID (numeric, max 10 chars): ");
        safeInput(temp,sizeof(temp));
        if(strlen(temp)==0){printf("OrderID cannot be empty\n"); continue;}
        if(strlen(temp)>10){printf("OrderID must <=10 chars\n"); continue;}
        if(!isNumeric(temp)){printf("OrderID must numeric\n"); continue;}
        if(isDuplicateOrderID(temp,searchID)){printf("OrderID already exists\n"); continue;}
        strcpy(o.OrderID,temp);
        break;
    }

    // CustomerName
    while(1){
        printf("Enter New Customer Name (max 50 chars): ");
        safeInput(o.CustomerName,sizeof(o.CustomerName));
        if(strlen(o.CustomerName)==0){printf("Customer Name cannot be empty\n"); continue;}
        if(strlen(o.CustomerName)>50){printf("Customer Name must <=50 chars\n"); continue;}
        break;
    }

    // ProductName
    while(1){
        printf("Enter New Product Name ('list' to show products): ");
        safeInput(o.ProductName,sizeof(o.ProductName));
        if(strlen(o.ProductName)==0){printf("Product Name cannot be empty\n"); continue;}
        if(strlen(o.ProductName)>49){printf("Product Name must <=49 chars\n"); continue;}
        if(strcasecmp(o.ProductName,"list")==0){ListProduct(); continue;}
        if(!isValidProduct(o.ProductName)){printf("Product not in list\n"); continue;}
        break;
    }

    // OrderDate ใช้วันที่ปัจจุบัน
    getCurrentDateStr(o.OrderDate,sizeof(o.OrderDate));
    printf("Order Date updated to: %s\n",o.OrderDate);

    // ShippingDate
    while(1){
        printf("Enter New Shipping Date (YYYY-MM-DD within 7 days): ");
        safeInput(o.ShippingDate,sizeof(o.ShippingDate));
        if(!isValidDateFormat(o.ShippingDate)){printf("Invalid date\n"); continue;}
        if(!isShippingAfterOrderWithin7(o.OrderDate,o.ShippingDate)){printf("Shipping must be within 7 days\n"); continue;}
        break;
    }

    if(UpdateOrderInFile(searchID,o)) printf("Updated Successfully\n");
    else printf("Update Failed\n");
}

void DeleteOrder() {
    char id[11]; // max 10 chars + null terminator
    while (1) {
        printf("Enter OrderID to Delete (max 10 chars): ");
        safeInput(id, sizeof(id));
        if (strlen(id) == 0) {
            printf("OrderID cannot be empty\n");
            continue;
        }
        if (strlen(id) > 10) {
            printf("OrderID must be <= 10 characters\n");
            continue;
        }
        if (!isNumeric(id)) {
            printf("OrderID must be numeric\n");
            continue;
        }
        break;
    }

    if (DeleteOrderInFile(id)) printf("Deleted Successfully\n");
    else printf("Delete Failed\n");
}

// ===== Main =====
void UnitTest_AddOrder(); void UnitTest_UpdateOrder(); void UnitTest_DeleteOrder(); void E2E_Test();

int main(){
    int choice;
    do{
        DisplayMenu();
        printf("Select Menu: ");
        if(scanf("%d",&choice)!=1){while(getchar()!='\n'); choice=-1;}
        getchar();
        switch(choice){
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
            case 0: printf("Exit Program, Thank You\n\n"); break;
            default: printf("Incorrect Menu! Please Select Again\n");
        }
    }while(choice!=0);
    return 0;
}