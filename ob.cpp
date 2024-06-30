#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip> // for std::setprecision

class OrderBook {
public:
    // Enumerations for order types and sides
    enum class OrderType { Market, Limit, Stop, GoodTillCanceled, FillOrKill_Limit, FillOrKill_Market };
    enum class Side { Buy, Sell };

    // Order class representing individual orders
    class Order {
    public:
        // Constructor to initialize an order
        Order(int id, OrderType type, Side side, double price, int quantity)
            : id(id), type(type), side(side), price(price), quantity(quantity) {}

        // Getter methods for order attributes
        int getId() const { return id; }
        OrderType getType() const { return type; }
        Side getSide() const { return side; }
        double getPrice() const { return price; }
        int getQuantity() const { return quantity; }

        // Method to update order quantity
        void setQuantity(int new_quantity) { quantity = new_quantity; }

    private:
        int id;
        OrderType type;
        Side side;
        double price;
        int quantity;
    };

    // Method to add an order to the order book
    void addOrder(const Order& order) {
        orders.push_back(order);
    }

    // Method to cancel an order by its ID
    void cancelOrder(int orderId) {
        orders.erase(
            std::remove_if(orders.begin(), orders.end(), 
                           [orderId](const Order& order) { return order.getId() == orderId; }), 
            orders.end());
    }

    // Method to match orders in the order book
    void matchOrders() {
        // Simple matching logic: match buy market and GTC orders with sell limit orders and vice versa
        for (auto it = orders.begin(); it != orders.end(); ) {
            if (it->getType() == OrderType::Market || it->getType() == OrderType::GoodTillCanceled ||
                it->getType() == OrderType::FillOrKill_Market) {
                // Match market, GTC, and FOK Market orders with limit orders
                auto matchIt = std::find_if(orders.begin(), orders.end(), 
                                            [&](const Order& order) {
                                                return order.getType() == OrderType::Limit && 
                                                       order.getSide() != it->getSide() &&
                                                       ((it->getSide() == Side::Buy && order.getPrice() <= it->getPrice()) ||
                                                        (it->getSide() == Side::Sell && order.getPrice() >= it->getPrice())) &&
                                                       order.getQuantity() >= it->getQuantity();
                                            });
                if (matchIt != orders.end()) {
                    // Calculate fill price from the matching order's price
                    double fillPrice = matchIt->getPrice();
                    // Print matched order IDs and fill price
                    std::cout << "Matched Order ID: " << it->getId() << " with Order ID: " << matchIt->getId() << " at Price: " << std::fixed << std::setprecision(2) << fillPrice << std::endl;
                    // Reduce quantity of the matching order
                    matchIt->setQuantity(matchIt->getQuantity() - it->getQuantity());
                    // Remove the matching order if fully executed
                    if (matchIt->getQuantity() == 0) {
                        orders.erase(matchIt);
                    }
                    // Remove the current order from the order book
                    it = orders.erase(it);
                } else {
                    // Move to the next order if no match found
                    ++it;
                }
            } else if (it->getType() == OrderType::Limit || it->getType() == OrderType::FillOrKill_Limit) {
                // Match limit and FOK Limit orders with market, GTC, and FOK Market orders
                auto matchIt = std::find_if(orders.begin(), orders.end(), 
                                            [&](const Order& order) {
                                                return (order.getType() == OrderType::Market || order.getType() == OrderType::GoodTillCanceled ||
                                                        order.getType() == OrderType::FillOrKill_Market) &&
                                                       order.getSide() != it->getSide() &&
                                                       ((order.getSide() == Side::Buy && it->getPrice() <= order.getPrice()) ||
                                                        (order.getSide() == Side::Sell && it->getPrice() >= order.getPrice())) &&
                                                       order.getQuantity() >= it->getQuantity();
                                            });
                if (matchIt != orders.end()) {
                    // Calculate fill price from the current order's price
                    double fillPrice = it->getPrice();
                    // Print matched order IDs and fill price
                    std::cout << "Matched Order ID: " << it->getId() << " with Order ID: " << matchIt->getId() << " at Price: " << std::fixed << std::setprecision(2) << fillPrice << std::endl;
                    // Reduce quantity of the matching order
                    matchIt->setQuantity(matchIt->getQuantity() - it->getQuantity());
                    // Remove the matching order if fully executed
                    if (matchIt->getQuantity() == 0) {
                        orders.erase(matchIt);
                    }
                    // Remove the current order from the order book
                    it = orders.erase(it);
                } else {
                    // Move to the next order if no match found
                    ++it;
                }
            } else {
                // Move to the next order if it's not a market, GTC, or limit order
                ++it;
            }
        }

        // Cancel Fill-Or-Kill orders that were not fully executed
        orders.erase(
            std::remove_if(orders.begin(), orders.end(), 
                           [](const Order& order) {
                               return order.getType() == OrderType::FillOrKill_Limit || order.getType() == OrderType::FillOrKill_Market;
                           }), 
            orders.end());
    }

    // Method to print all orders in the order book
    void printOrders() const {
        for (const auto& order : orders) {
            printOrder(order);
        }
    }

private:
    std::vector<Order> orders; // Vector to store orders in the order book

    // Helper method to print an individual order
    void printOrder(const Order& order) const {
        std::cout << "Order ID: " << order.getId() 
                  << ", Type: " << static_cast<int>(order.getType()) 
                  << ", Side: " << (order.getSide() == Side::Buy ? "Buy" : "Sell")
                  << ", Price: " << order.getPrice() 
                  << ", Quantity: " << order.getQuantity() << std::endl;
    }
};

int main() {
    // Create an instance of OrderBook
    OrderBook orderBook;

    // Create sample orders of different types
    OrderBook::Order order1(1, OrderBook::OrderType::Market, OrderBook::Side::Buy, 100.0, 10);
    OrderBook::Order order2(2, OrderBook::OrderType::Limit, OrderBook::Side::Sell, 110.0, 20);
    OrderBook::Order order3(3, OrderBook::OrderType::Limit, OrderBook::Side::Sell, 95.0, 5);
    OrderBook::Order order4(4, OrderBook::OrderType::Market, OrderBook::Side::Sell, 90.0, 15);
    OrderBook::Order order5(5, OrderBook::OrderType::GoodTillCanceled, OrderBook::Side::Buy, 110.0, 10);
    OrderBook::Order order6(6, OrderBook::OrderType::FillOrKill_Market, OrderBook::Side::Sell, 105.0, 8);
    OrderBook::Order order7(7, OrderBook::OrderType::FillOrKill_Limit, OrderBook::Side::Buy, 105.0, 12);

    // Add orders to the order book
    orderBook.addOrder(order1);
    orderBook.addOrder(order2);
    orderBook.addOrder(order3);
    orderBook.addOrder(order4);
    orderBook.addOrder(order5);
    orderBook.addOrder(order6);
    orderBook.addOrder(order7);

    // Print the order book before matching orders
    std::cout << "Order Book before matching:" << std::endl;
    orderBook.printOrders();

    // Match orders in the order book
    orderBook.matchOrders();

    // Print the order book after matching orders
    std::cout << "Order Book after matching:" << std::endl;
    orderBook.printOrders();

    return 0;
}