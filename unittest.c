#include "storemadebypd.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void UnitTest_AddOrder() {
    printf("\n[Unit Test] AddOrder()\n");
    CreateOrder();

    // ✅ Success case
    Order o1 = {"101", "John", "Phone", "", ""};
    getCurrentDateStr(o1.OrderDate, 20);
    getDateStrOffset(o1.ShippingDate, 20, 1);
    assert(AddOrderToFile(o1) == 1);

    // ❌ Fail case: duplicate OrderID
    Order o2 = {"101", "SomeoneElse", "Laptop", "", ""};
    getCurrentDateStr(o2.OrderDate, 20);
    getDateStrOffset(o2.ShippingDate, 20, 1);
    assert(AddOrderToFile(o2) == 0);

    // ❌ Fail case: OrderID ไม่ใช่ตัวเลข
    Order o3 = {"10A", "Alice", "Phone", "", ""};
    getCurrentDateStr(o3.OrderDate, 20);
    getDateStrOffset(o3.ShippingDate, 20, 1);
    assert(AddOrderToFile(o3) == 0);

    // ❌ Fail case: OrderID ยาวเกิน 10
    Order o4 = {"12345678901", "Bob", "Laptop", "", ""};
    getCurrentDateStr(o4.OrderDate, 20);
    getDateStrOffset(o4.ShippingDate, 20, 1);
    assert(AddOrderToFile(o4) == 0);

    // ❌ Fail case: ProductName ผิด
    Order o5 = {"102", "Charlie", "InvalidProduct", "", ""};
    getCurrentDateStr(o5.OrderDate, 20);
    getDateStrOffset(o5.ShippingDate, 20, 1);
    assert(AddOrderToFile(o5) == 0);

    // ❌ Fail case: ShippingDate เกิน 7 วัน
    Order o6 = {"103", "David", "Laptop", "", ""};
    getCurrentDateStr(o6.OrderDate, 20);
    getDateStrOffset(o6.ShippingDate, 20, 10);
    assert(AddOrderToFile(o6) == 0);

    // ❌ Fail case: Invalid date format
    Order o7 = {"104", "Eve", "Phone", "2025/10/10", "2025/10/11"};
    assert(AddOrderToFile(o7) == 0);

    printf("UnitTest_AddOrder: All cases passed!\n");
}

void UnitTest_UpdateOrder() {
    printf("\n[Unit Test] UpdateOrder()\n");

    CreateOrder();

    // 1️⃣ Add two orders: 201 และ 202
    Order o1 = {"201", "Alice", "Laptop", "", ""};
    getCurrentDateStr(o1.OrderDate, 20);
    getDateStrOffset(o1.ShippingDate, 20, 2);
    assert(AddOrderToFile(o1) == 1);

    Order o2 = {"202", "Bob", "Phone", "", ""};
    getCurrentDateStr(o2.OrderDate, 20);
    getDateStrOffset(o2.ShippingDate, 20, 1);
    assert(AddOrderToFile(o2) == 1);

    // ✅ Success case: update 202 เป็น order ใหม่ที่ valid
    Order n = {"203", "BobNew", "PC", "", ""};
    getCurrentDateStr(n.OrderDate, 20);
    getDateStrOffset(n.ShippingDate, 20, 1);
    assert(UpdateOrderInFile("202", n) == 1);

    // ❌ Fail case: Update 203 ให้ OrderID ซ้ำกับ 201
    Order dup = {"201", "DupTest", "Phone", "", ""};
    getCurrentDateStr(dup.OrderDate, 20);
    getDateStrOffset(dup.ShippingDate, 20, 1);
    assert(UpdateOrderInFile("203", dup) == 0);

    // ❌ Fail case: ProductName ผิด
    Order wrongProd = {"204", "Test", "WrongProduct", "", ""};
    getCurrentDateStr(wrongProd.OrderDate, 20);
    getDateStrOffset(wrongProd.ShippingDate, 20, 1);
    assert(UpdateOrderInFile("203", wrongProd) == 0);

    // ❌ Fail case: ShippingDate เกิน 7 วัน
    Order lateShip = {"204", "Test", "Laptop", "", ""};
    getCurrentDateStr(lateShip.OrderDate, 20);
    getDateStrOffset(lateShip.ShippingDate, 20, 10);
    assert(UpdateOrderInFile("203", lateShip) == 0);

    // ❌ Fail case: Update order ที่ไม่พบ
    Order nonExist = {"999", "Nobody", "Phone", "", ""};
    getCurrentDateStr(nonExist.OrderDate, 20);
    getDateStrOffset(nonExist.ShippingDate, 20, 1);
    assert(UpdateOrderInFile("999", nonExist) == 0);

    printf("UnitTest_UpdateOrder: All cases passed!\n");
}

void UnitTest_DeleteOrder() {
    printf("\n[Unit Test] DeleteOrder()\n");

    CreateOrder();

    Order o;
    strcpy(o.OrderID, "301");
    strcpy(o.CustomerName, "Bob");
    strcpy(o.ProductName, "Tablet");
    getCurrentDateStr(o.OrderDate, 20);
    getDateStrOffset(o.ShippingDate, 20, 1);
    AddOrderToFile(o);

    // ✅ Success case
    assert(DeleteOrderInFile("301") == 1);

    // ❌ Fail case: Delete OrderID ไม่อยู่
    assert(DeleteOrderInFile("999") == 0);

    printf("UnitTest_DeleteOrder: All cases passed!\n");
}