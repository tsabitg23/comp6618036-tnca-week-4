#include "Store.hpp"

#include "Foundation.hpp"
#include "Inventory.hpp"

Store::Store() {
  inventoryMap = new HashMap<InventoryCode, Inventory>();
  inventoryStockMap = new HashMap<InventoryCode, UInt64>();
  orderMap = new HashMap<OrderIdentifier, Order>();

  // Prepare predefined data
  Inventory panadol = {.code = "OBT001", .name = "Panadol", .price = 10000.0};
  inventoryStockMap->set(panadol.code, 13);
  inventoryMap->set(panadol.code, panadol);
  inventories.push_back(panadol);

  Inventory sanmol = {.code = "OBT002", .name = "Sanmol", .price = 15000.0};
  inventoryStockMap->set(sanmol.code, 13);
  inventoryMap->set(sanmol.code, sanmol);
  inventories.push_back(sanmol);

  Inventory bodrex = {.code = "OBT003", .name = "Bodrex", .price = 11000.0};
  inventoryStockMap->set(bodrex.code, 13);
  inventoryMap->set(bodrex.code, bodrex);
  inventories.push_back(bodrex);
}

Store::~Store() {
  delete inventoryMap;
  delete inventoryStockMap;
  delete orderMap;
}

Vector<Inventory> Store::getInventories() {
  return inventories;
}

UInt16 Store::numberOfAvailableInventories(InventoryCode code) {
  return inventoryStockMap->get(code);
}

bool Store::contains(InventoryCode code) {
  return inventoryMap->contains(code);
}

Inventory Store::inventory(InventoryCode code) {
  return inventoryMap->get(code);
}

Order Store::purchase(Cart cart, User customer) {
  Order order = Order(customer, cart);
  orderMap->set(order.id, order);
  return order;
};

Cart Store::shop() {
  // Generate cart
  Cart cart;

  while (true) {
    InventoryCode code = readLineInput("Plese input the inventory code you want to purchase");
    if (contains(code) == false) {
      printError("Invalid inventory code. Please try again\n");
      continue;
    }

    // Get inventory with the amount
    Inventory inventory = this->inventory(code);
    int amount = readNumberInput("Plese input the number of " + inventory.name + " you want");

    UInt16 available = numberOfAvailableInventories(code);
    UInt16 currentAmount = cart.amountForInventory(code);

    if ((currentAmount + amount) > available) {
      printError("Insufficient stock for inventory with id" + code + "which only has " + std::to_string(available) + " in stock.\n");
    } else {
      cart.add(code, amount);
      if (!readBoolInput("Anything else")) {
        break;
      }
    }
  }

  return cart;
}

String Store::summary(Cart cart) {
  Vector<InventoryCode> keys = cart.allCodes();
  if (keys.empty()) {
    return "";
  }

  String summary;

  summary.append("==================================================\n");
  summary.append("Code\tName\tAmount\tPrice\t\tTotal\t\n");
  summary.append("--------------------------------------------------\n");

  Double total;

  // Print each row
  for (unsigned int i = 0; i < keys.size(); i++) {
    InventoryCode code = keys[i];
    Inventory inventory = this->inventory(code);
    UInt16 amount = cart.amountForInventory(code);
    total += (amount * inventory.price);
    summary.append(inventory.code + "\t" + inventory.name + "\t" + std::to_string(amount) + "\t" + formatCurrency("Rp", inventory.price) + "\t" + formatCurrency("Rp", (amount * inventory.price)) + "\n");
  }

  summary.append("--------------------------------------------------\n");
  summary.append("Total\t\t\t\t\t" + formatCurrency("Rp", total) + "\n");
  summary.append("==================================================\n");
  return summary;
}