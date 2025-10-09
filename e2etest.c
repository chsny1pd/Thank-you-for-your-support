#include "storemadebypd.h"
#include <assert.h>

void E2E_Test() {
    printf("\n[E2E Test] Start\n");
    CreateOrder();

    Order a = {"401","E2E_A","Laptop","",""};
    Order b = {"402","E2E_B","Phone","",""};
    Order c = {"403","E2E_C","SSD","",""};
    getCurrentDateStr(a.OrderDate,20); getDateStrOffset(a.ShippingDate,20,1);
    getCurrentDateStr(b.OrderDate,20); getDateStrOffset(b.ShippingDate,20,2);
    getCurrentDateStr(c.OrderDate,20); getDateStrOffset(c.ShippingDate,20,0);

    // ✅ Add orders
    assert(AddOrderToFile(a)==1);
    assert(AddOrderToFile(b)==1);
    assert(AddOrderToFile(c)==1);

    printf("\nAfter adding 3 orders:\n"); ReadOrders();

    // ✅ Update order
    Order up={"402","E2E_B_New","Monitor","",""};
    getCurrentDateStr(up.OrderDate,20); getDateStrOffset(up.ShippingDate,20,3);
    assert(UpdateOrderInFile("402",up)==1);

    printf("\nAfter update:\n"); ReadOrders();

    // ✅ Delete order
    assert(DeleteOrderInFile("401")==1);

    printf("\nAfter delete 401:\n"); ReadOrders();

    // ❌ Try adding duplicate in E2E
    assert(AddOrderToFile(c)==0);

    // ❌ Try updating non-existing in E2E
    Order fail={"999","Fail","Laptop","",""};
    getCurrentDateStr(fail.OrderDate,20); getDateStrOffset(fail.ShippingDate,20,1);
    assert(UpdateOrderInFile("999",fail)==0);

    // ❌ Try deleting non-existing in E2E
    assert(DeleteOrderInFile("999")==0);

    printf("\n[E2E Test] Completed Successfully\n");
}