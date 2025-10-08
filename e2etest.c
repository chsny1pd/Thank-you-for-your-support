#include "storemadebypd.h"
#include <assert.h>

void E2E_Test() {
    printf("\n[E2E Test] Start\n");
    CreateOrder();
    Order a={"401","E2E_A","Laptop","",""};
    Order b={"402","E2E_B","Phone","",""};
    Order c={"403","E2E_C","SSD","",""};
    getCurrentDateStr(a.OrderDate,20); getDateStrOffset(a.ShippingDate,20,1);
    getCurrentDateStr(b.OrderDate,20); getDateStrOffset(b.ShippingDate,20,2);
    getCurrentDateStr(c.OrderDate,20); getDateStrOffset(c.ShippingDate,20,0);
    assert(AddOrderToFile(a)==1);
    assert(AddOrderToFile(b)==1);
    assert(AddOrderToFile(c)==1);
    
    printf("\nAfter adding 3 orders:\n"); ReadOrders();

    Order up={"402","E2E_B_New","Monitor","",""};
    getCurrentDateStr(up.OrderDate,20); getDateStrOffset(up.ShippingDate,20,3);
    assert(UpdateOrderInFile("402",up)==1);
    
    printf("\nAfter update:\n"); ReadOrders();

    assert(DeleteOrderInFile("401")==1);
    printf("\nAfter delete 401:\n"); ReadOrders();
    printf("\n[E2E Test] Completed\n");
}