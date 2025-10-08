#include "storemadebypd.h"
#include <assert.h>

void UnitTest_AddOrder() {
    printf("\n[Unit Test] AddOrder()\n");
    CreateOrder();
    Order o1={"101","John","Phone","",""};
    getCurrentDateStr(o1.OrderDate,20);
    getDateStrOffset(o1.ShippingDate,20,1);
    assert(AddOrderToFile(o1)==1);
    printf("AddOrder passed!\n");
}

void UnitTest_UpdateOrder() {
    printf("\n[Unit Test] UpdateOrder()\n");
    CreateOrder();
    Order o={"201","Alice","Laptop","",""};
    getCurrentDateStr(o.OrderDate,20);
    getDateStrOffset(o.ShippingDate,20,2);
    AddOrderToFile(o);
    Order n={"202","AliceNew","PC","",""};
    getCurrentDateStr(n.OrderDate,20);
    getDateStrOffset(n.ShippingDate,20,1);
    assert(UpdateOrderInFile("201",n)==1);
    printf("UpdateOrder passed!\n");
}

void UnitTest_DeleteOrder() {
    printf("\n[Unit Test] DeleteOrder()\n");
    CreateOrder();
    Order o={"301","Bob","Tablet","",""};
    getCurrentDateStr(o.OrderDate,20);
    getDateStrOffset(o.ShippingDate,20,1);
    AddOrderToFile(o);
    assert(DeleteOrderInFile("301")==1);
    printf("DeleteOrder passed!\n");
}