#pragma once
#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP


enum class OrderType {
        buy,
        sell
};

struct Order {
    OrderType ordertype;
    int id;
    int price;
    int quantity;
    int timestamp;
    char symbol[8]; //Fixed-size Alpha Field
};

//Timestamps are represented as nanoseconds since midnight
//Prices are integerfields,suppliedwith an associatedprecision
//When convertedto a decimalformat, prices are in fixedpointformat,




#endif