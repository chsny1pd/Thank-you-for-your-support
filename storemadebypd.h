#ifndef STOREMADEBYPD_H
#define STOREMADEBYPD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define FILE_NAME "orders.csv"

typedef struct {
    char OrderID[11];
    char CustomerName[51];
    char ProductName[50];
    char OrderDate[20];
    char ShippingDate[20];
} Order;

// Core operations
int AddOrderToFile(Order o);
int UpdateOrderInFile(const char *oldID, Order o);
int DeleteOrderInFile(const char *id);
int isDuplicateOrderID(const char *orderID, const char *excludeID);
void CreateOrder();

// Utility & validation
int getValidOrderID(char *buf, size_t size, const char *action);
void safeInput(char *buf, size_t size);
int isNumeric(const char *str);
int isValidDateFormat(const char *date);
int isValidProduct(const char *product);
int isShippingAfterOrderWithin7(const char *orderDate, const char *shippingDate);

// ===== Helper: date/time =====
void getCurrentDateStr(char *buf, size_t size);
void getDateStrOffset(char *buf, size_t size, int offsetDays);
int dateToEpoch(const char *date, time_t *out);


// For interactive mode
void ListProduct();
void AddOrder();
void ReadOrders();
void SearchOrder();
void UpdateOrder();
void DeleteOrder();

// For tests
void UnitTest_AddOrder();
void UnitTest_UpdateOrder();
void UnitTest_DeleteOrder();
void E2E_Test();

#endif