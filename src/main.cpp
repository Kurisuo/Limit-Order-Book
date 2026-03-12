#include <iostream>
#include "Order.h"


void printOrder(const Order& order){
    std::cout << "ID: " << order.id
              << "Price: " << order.price
              << "Quantity: " << order.quantity
              << "Timestamp: " << order.timestamp
              << "Ticker: " << order.symbol
}