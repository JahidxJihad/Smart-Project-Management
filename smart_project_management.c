#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PRODUCTS 200
#define MAX_CUSTOMERS 200
#define MAX_SUPPLIERS 100
#define LOW_STOCK_THRESHOLD 10

#define PRODUCTS_FILE   "products.txt"
#define CUSTOMERS_FILE  "customers.txt"
#define SUPPLIERS_FILE  "suppliers.txt"
#define SALES_FILE      "sales.txt"
#define BILLS_FILE      "bills.txt"

typedef struct {
    int code;
    char name[50];
    double price;
    int quantity;
    char category[30];
} Product;

typedef struct {
    int id;
    char name[50];
    char phone[20];
    int totalPurchases;
    double totalSpent;
} Customer;

typedef struct {
    int id;
    char company[60];
    char contact[25];
    char catalog[50];
    int restockCount;
} Supplier;

typedef struct {
    int billId;
    int customerId;
    char date[20];
    double grossAmount;
    double discountPercent;
    double discountAmount;
    double finalTotal;
} SaleRecord;

typedef struct {
    int productCode;
    char productName[50];
    double unitPrice;
    int qty;
    double subtotal;
} BillItem;

/* Global Arrays & Counters */
Product products[MAX_PRODUCTS];
int productCount = 0;

Customer customers[MAX_CUSTOMERS];
int customerCount = 0;

Supplier suppliers[MAX_SUPPLIERS];
int supplierCount = 0;

/* Utility Prototypes */
void clearInputBuffer(void);
void getCurrentDate(char *buffer, size_t size);
void pauseScreen(void);
int toLowerCaseStr(const char *src, char *dest, size_t size);

/* File Persistence Prototypes */
void loadAllData(void);
void saveProducts(void);
void loadProducts(void);
void saveCustomers(void);
void loadCustomers(void);
void saveSuppliers(void);
void loadSuppliers(void);
void appendSaleRecord(const SaleRecord *rec);
void appendBillReceipt(const SaleRecord *rec, const Customer *cust, const BillItem *items, int itemCount);

/* Lookup Helpers */
int findProductIndexByCode(int code);
int findCustomerIndexById(int id);
int findSupplierIndexById(int id);

/* Menu & Module Prototypes */
void showWelcomeScreen(void);
void mainMenu(void);
void productManagementMenu(void);
void customerManagementMenu(void);
void supplierManagementMenu(void);
void billingSystemMenu(void);
void reportsMenu(void);
void showDashboard(void);

/* Product Operations */
void addProduct(void);
void displayProducts(const Product *arr, int count);
void editProduct(void);
void deleteProduct(void);
void searchProductMenu(void);
void sortProductsMenu(void);
void checkLowStock(void);

/* Customer Operations */
void addCustomer(void);
void displayCustomers(void);
void searchCustomer(void);

/* Supplier Operations */
void addSupplier(void);
void displaySuppliers(void);
void orderRestockFromSupplier(void);

/* Billing & Reports */
double calculateDiscountRate(double grossAmount);
void createNewBill(void);
void viewSalesReport(void);
void viewInventoryValuation(void);
void viewCategoryBreakdown(void);
void viewSavedBills(void);

int main(void) {
    loadAllData();
    showWelcomeScreen();
    mainMenu();
    saveProducts();
    saveCustomers();
    saveSuppliers();
    printf("\nAll data saved. Thank you for using SmartMart!\n");
    return 0;
}

/* ================= UTILITY FUNCTIONS ================= */

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* discard */
    }
}

void getCurrentDate(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    if (tm_info != NULL) {
        strftime(buffer, size, "%Y-%m-%d %H:%M", tm_info);
    } else {
        snprintf(buffer, size, "2026-01-01 00:00");
    }
}

void pauseScreen(void) {
    printf("\nPress Enter to continue...");
    getchar();
}

int toLowerCaseStr(const char *src, char *dest, size_t size) {
    size_t i = 0;
    if (size == 0) return -1;
    while (src[i] != '\0' && i < size - 1) {
        char ch = src[i];
        if (ch >= 'A' && ch <= 'Z') {
            ch = (char)(ch + ('a' - 'A'));
        }
        dest[i] = ch;
        i++;
    }
    dest[i] = '\0';
    return 0;
}

/* ================= FILE HANDLING ================= */

void loadAllData(void) {
    loadProducts();
    loadCustomers();
    loadSuppliers();
}

void saveProducts(void) {
    FILE *fp = fopen(PRODUCTS_FILE, "w");
    if (!fp) {
        perror("Error opening products.txt for writing");
        return;
    }
    for (int i = 0; i < productCount; i++) {
        fprintf(fp, "%d|%.2f|%d|%s|%s\n",
                products[i].code,
                products[i].price,
                products[i].quantity,
                products[i].category,
                products[i].name);
    }
    fclose(fp);
}

void loadProducts(void) {
    FILE *fp = fopen(PRODUCTS_FILE, "r");
    if (!fp) {
        return; /* Will be created on save */
    }
    productCount = 0;
    while (productCount < MAX_PRODUCTS) {
        Product p;
        int read = fscanf(fp, "%d|%lf|%d|%29[^|]|%49[^\n]\n",
                          &p.code, &p.price, &p.quantity, p.category, p.name);
        if (read == 5) {
            products[productCount++] = p;
        } else {
            break;
        }
    }
    fclose(fp);
}

void saveCustomers(void) {
    FILE *fp = fopen(CUSTOMERS_FILE, "w");
    if (!fp) {
        perror("Error opening customers.txt for writing");
        return;
    }
    for (int i = 0; i < customerCount; i++) {
        fprintf(fp, "%d|%d|%.2f|%s|%s\n",
                customers[i].id,
                customers[i].totalPurchases,
                customers[i].totalSpent,
                customers[i].phone,
                customers[i].name);
    }
    fclose(fp);
}

void loadCustomers(void) {
    FILE *fp = fopen(CUSTOMERS_FILE, "r");
    if (!fp) {
        return;
    }
    customerCount = 0;
    while (customerCount < MAX_CUSTOMERS) {
        Customer c;
        int read = fscanf(fp, "%d|%d|%lf|%19[^|]|%49[^\n]\n",
                          &c.id, &c.totalPurchases, &c.totalSpent, c.phone, c.name);
        if (read == 5) {
            customers[customerCount++] = c;
        } else {
            break;
        }
    }
    fclose(fp);
}

void saveSuppliers(void) {
    FILE *fp = fopen(SUPPLIERS_FILE, "w");
    if (!fp) {
        perror("Error opening suppliers.txt for writing");
        return;
    }
    for (int i = 0; i < supplierCount; i++) {
        fprintf(fp, "%d|%d|%s|%s|%s\n",
                suppliers[i].id,
                suppliers[i].restockCount,
                suppliers[i].contact,
                suppliers[i].catalog,
                suppliers[i].company);
    }
    fclose(fp);
}

void loadSuppliers(void) {
    FILE *fp = fopen(SUPPLIERS_FILE, "r");
    if (!fp) {
        return;
    }
    supplierCount = 0;
    while (supplierCount < MAX_SUPPLIERS) {
        Supplier s;
        int read = fscanf(fp, "%d|%d|%24[^|]|%49[^|]|%59[^\n]\n",
                          &s.id, &s.restockCount, s.contact, s.catalog, s.company);
        if (read == 5) {
            suppliers[supplierCount++] = s;
        } else {
            break;
        }
    }
    fclose(fp);
}

void appendSaleRecord(const SaleRecord *rec) {
    FILE *fp = fopen(SALES_FILE, "a");
    if (!fp) {
        perror("Error opening sales.txt");
        return;
    }
    fprintf(fp, "%d|%d|%.2f|%.1f|%.2f|%.2f|%s\n",
            rec->billId,
            rec->customerId,
            rec->grossAmount,
            rec->discountPercent,
            rec->discountAmount,
            rec->finalTotal,
            rec->date);
    fclose(fp);
}

void appendBillReceipt(const SaleRecord *rec, const Customer *cust, const BillItem *items, int itemCount) {
    FILE *fp = fopen(BILLS_FILE, "a");
    if (!fp) {
        perror("Error opening bills.txt");
        return;
    }
    fprintf(fp, "========================================================\n");
    fprintf(fp, "               SMARTMART RECEIPT (BILL #%d)\n", rec->billId);
    fprintf(fp, "Date     : %s\n", rec->date);
    fprintf(fp, "Customer : %s (ID: %d | Phone: %s)\n", cust->name, cust->id, cust->phone);
    fprintf(fp, "--------------------------------------------------------\n");
    fprintf(fp, "%-6s %-22s %6s %9s %10s\n", "Code", "Item", "Qty", "Price", "Subtotal");
    for (int i = 0; i < itemCount; i++) {
        fprintf(fp, "%-6d %-22s %6d %9.2f %10.2f\n",
                items[i].productCode,
                items[i].productName,
                items[i].qty,
                items[i].unitPrice,
                items[i].subtotal);
    }
    fprintf(fp, "--------------------------------------------------------\n");
    fprintf(fp, "Gross Amount : %10.2f\n", rec->grossAmount);
    fprintf(fp, "Discount (%.0f%%): %10.2f\n", rec->discountPercent, rec->discountAmount);
    fprintf(fp, "FINAL TOTAL  : %10.2f\n", rec->finalTotal);
    fprintf(fp, "========================================================\n\n");
    fclose(fp);
}

/* ================= LOOKUP HELPERS ================= */

int findProductIndexByCode(int code) {
    for (int i = 0; i < productCount; i++) {
        if (products[i].code == code) return i;
    }
    return -1;
}

int findCustomerIndexById(int id) {
    for (int i = 0; i < customerCount; i++) {
        if (customers[i].id == id) return i;
    }
    return -1;
}

int findSupplierIndexById(int id) {
    for (int i = 0; i < supplierCount; i++) {
        if (suppliers[i].id == id) return i;
    }
    return -1;
}

/* ================= WELCOME & MAIN MENU ================= */

void showWelcomeScreen(void) {
    printf("\n");
    printf("************************************************************\n");
    printf("*        WELCOME TO SMARTMART MANAGEMENT & BILLING         *\n");
    printf("*      Structured C Supermarket & Inventory Solution       *\n");
    printf("************************************************************\n");
    printf("  Loaded Products  : %d\n", productCount);
    printf("  Loaded Customers : %d\n", customerCount);
    printf("  Loaded Suppliers : %d\n", supplierCount);
    pauseScreen();
}

void mainMenu(void) {
    int choice;
    do {
        printf("\n==================== SMARTMART MAIN MENU ====================\n");
        printf("  1. Product Management\n");
        printf("  2. Customer Management\n");
        printf("  3. Supplier Management\n");
        printf("  4. Billing System\n");
        printf("  5. Reports\n");
        printf("  6. Live Dashboard\n");
        printf("  0. Save & Exit\n");
        printf("=============================================================\n");
        printf("Select an option: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Invalid input! Please enter a number.\n");
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                productManagementMenu();
                break;
            case 2:
                customerManagementMenu();
                break;
            case 3:
                supplierManagementMenu();
                break;
            case 4:
                billingSystemMenu();
                break;
            case 5:
                reportsMenu();
                break;
            case 6:
                showDashboard();
                break;
            case 0:
                break;
            default:
                printf("Invalid choice! Please select 0-6.\n");
        }
    } while (choice != 0);
}

/* ================= PRODUCT MANAGEMENT ================= */

void productManagementMenu(void) {
    int choice;
    do {
        printf("\n--- PRODUCT MANAGEMENT ---\n");
        printf("1. Add Product\n");
        printf("2. Display All Products\n");
        printf("3. Edit Product\n");
        printf("4. Delete Product\n");
        printf("5. Search Product (Code / Name / Category)\n");
        printf("6. Sort Products (Price / Name)\n");
        printf("7. Low-Stock & Restock Alerts\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: addProduct(); break;
            case 2: displayProducts(products, productCount); pauseScreen(); break;
            case 3: editProduct(); break;
            case 4: deleteProduct(); break;
            case 5: searchProductMenu(); break;
            case 6: sortProductsMenu(); break;
            case 7: checkLowStock(); pauseScreen(); break;
            case 0: break;
            default: printf("Invalid option.\n");
        }
    } while (choice != 0);
}

void addProduct(void) {
    if (productCount >= MAX_PRODUCTS) {
        printf("Inventory full! Cannot add more products.\n");
        return;
    }
    Product p;
    printf("\nEnter Product Code (integer): ");
    if (scanf("%d", &p.code) != 1) {
        clearInputBuffer();
        printf("Invalid code.\n");
        return;
    }
    clearInputBuffer();

    if (findProductIndexByCode(p.code) != -1) {
        printf("A product with code %d already exists!\n", p.code);
        return;
    }

    printf("Enter Product Name: ");
    fgets(p.name, sizeof(p.name), stdin);
    p.name[strcspn(p.name, "\n")] = '\0';

    printf("Enter Category: ");
    fgets(p.category, sizeof(p.category), stdin);
    p.category[strcspn(p.category, "\n")] = '\0';

    printf("Enter Price: ");
    if (scanf("%lf", &p.price) != 1 || p.price < 0) {
        clearInputBuffer();
        printf("Invalid price.\n");
        return;
    }

    printf("Enter Quantity in Stock: ");
    if (scanf("%d", &p.quantity) != 1 || p.quantity < 0) {
        clearInputBuffer();
        printf("Invalid quantity.\n");
        return;
    }
    clearInputBuffer();

    products[productCount++] = p;
    saveProducts();
    printf("Product added successfully!\n");
}

void displayProducts(const Product *arr, int count) {
    if (count == 0) {
        printf("\nNo products available.\n");
        return;
    }
    printf("\n%-8s %-24s %-16s %10s %8s %10s\n",
           "Code", "Name", "Category", "Price", "Stock", "Alert");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        const char *alert = (arr[i].quantity <= LOW_STOCK_THRESHOLD) ? "LOW STOCK" : "OK";
        printf("%-8d %-24s %-16s %10.2f %8d %10s\n",
               arr[i].code, arr[i].name, arr[i].category,
               arr[i].price, arr[i].quantity, alert);
    }
}

void editProduct(void) {
    int code;
    printf("\nEnter Product Code to edit: ");
    if (scanf("%d", &code) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    int idx = findProductIndexByCode(code);
    if (idx == -1) {
        printf("Product not found.\n");
        return;
    }

    Product *p = &products[idx];
    printf("Editing '%s' (Current Price: %.2f, Stock: %d)\n", p->name, p->price, p->quantity);

    printf("New Name: ");
    fgets(p->name, sizeof(p->name), stdin);
    p->name[strcspn(p->name, "\n")] = '\0';

    printf("New Category: ");
    fgets(p->category, sizeof(p->category), stdin);
    p->category[strcspn(p->category, "\n")] = '\0';

    printf("New Price: ");
    scanf("%lf", &p->price);

    printf("New Quantity: ");
    scanf("%d", &p->quantity);
    clearInputBuffer();

    saveProducts();
    printf("Product updated successfully!\n");
}

void deleteProduct(void) {
    int code;
    printf("\nEnter Product Code to delete: ");
    if (scanf("%d", &code) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    int idx = findProductIndexByCode(code);
    if (idx == -1) {
        printf("Product not found.\n");
        return;
    }

    for (int i = idx; i < productCount - 1; i++) {
        products[i] = products[i + 1];
    }
    productCount--;
    saveProducts();
    printf("Product deleted successfully.\n");
}

void searchProductMenu(void) {
    int opt;
    printf("\nSearch by:\n1. Code\n2. Name\n3. Category\nChoice: ");
    if (scanf("%d", &opt) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (opt == 1) {
        int code;
        printf("Enter Code: ");
        scanf("%d", &code);
        clearInputBuffer();
        int idx = findProductIndexByCode(code);
        if (idx != -1) {
            displayProducts(&products[idx], 1);
        } else {
            printf("No product found with code %d.\n", code);
        }
    } else if (opt == 2 || opt == 3) {
        char query[50], lowerQuery[50], lowerField[50];
        Product matches[MAX_PRODUCTS];
        int matchCount = 0;

        printf("Enter search keyword: ");
        fgets(query, sizeof(query), stdin);
        query[strcspn(query, "\n")] = '\0';
        toLowerCaseStr(query, lowerQuery, sizeof(lowerQuery));

        for (int i = 0; i < productCount; i++) {
            const char *field = (opt == 2) ? products[i].name : products[i].category;
            toLowerCaseStr(field, lowerField, sizeof(lowerField));
            if (strstr(lowerField, lowerQuery) != NULL) {
                matches[matchCount++] = products[i];
            }
        }
        displayProducts(matches, matchCount);
    } else {
        printf("Invalid search option.\n");
    }
    pauseScreen();
}

void sortProductsMenu(void) {
    int opt;
    printf("\nSort Products by:\n1. Price (Low to High)\n2. Price (High to Low)\n3. Name (A-Z)\nChoice: ");
    if (scanf("%d", &opt) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    for (int i = 0; i < productCount - 1; i++) {
        for (int j = 0; j < productCount - i - 1; j++) {
            int swapNeeded = 0;
            if (opt == 1 && products[j].price > products[j + 1].price) swapNeeded = 1;
            else if (opt == 2 && products[j].price < products[j + 1].price) swapNeeded = 1;
            else if (opt == 3 && strcmp(products[j].name, products[j + 1].name) > 0) swapNeeded = 1;

            if (swapNeeded) {
                Product temp = products[j];
                products[j] = products[j + 1];
                products[j + 1] = temp;
            }
        }
    }
    displayProducts(products, productCount);
    pauseScreen();
}

void checkLowStock(void) {
    printf("\n=== LOW STOCK ALERTS (Threshold <= %d) ===\n", LOW_STOCK_THRESHOLD);
    int found = 0;
    for (int i = 0; i < productCount; i++) {
        if (products[i].quantity <= LOW_STOCK_THRESHOLD) {
            printf("  [ALERT] Code: %d | %-20s | Stock: %d | Category: %s\n",
                   products[i].code, products[i].name, products[i].quantity, products[i].category);
            found = 1;
        }
    }
    if (!found) {
        printf("  All products have healthy stock levels.\n");
    }
}

/* ================= CUSTOMER MANAGEMENT ================= */

void customerManagementMenu(void) {
    int choice;
    do {
        printf("\n--- CUSTOMER MANAGEMENT ---\n");
        printf("1. Add Customer\n");
        printf("2. Display All Customers\n");
        printf("3. Search Customer & Purchase History\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: addCustomer(); break;
            case 2: displayCustomers(); pauseScreen(); break;
            case 3: searchCustomer(); pauseScreen(); break;
            case 0: break;
            default: printf("Invalid option.\n");
        }
    } while (choice != 0);
}

void addCustomer(void) {
    if (customerCount >= MAX_CUSTOMERS) {
        printf("Customer directory full.\n");
        return;
    }
    Customer c;
    printf("\nEnter Customer ID: ");
    if (scanf("%d", &c.id) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (findCustomerIndexById(c.id) != -1) {
        printf("Customer ID already exists!\n");
        return;
    }

    printf("Enter Customer Name: ");
    fgets(c.name, sizeof(c.name), stdin);
    c.name[strcspn(c.name, "\n")] = '\0';

    printf("Enter Phone Number: ");
    fgets(c.phone, sizeof(c.phone), stdin);
    c.phone[strcspn(c.phone, "\n")] = '\0';

    c.totalPurchases = 0;
    c.totalSpent = 0.0;

    customers[customerCount++] = c;
    saveCustomers();
    printf("Customer registered successfully!\n");
}

void displayCustomers(void) {
    if (customerCount == 0) {
        printf("\nNo customers registered.\n");
        return;
    }
    printf("\n%-6s %-24s %-16s %10s %12s\n",
           "ID", "Name", "Phone", "Bills", "Total Spent");
    printf("------------------------------------------------------------------------\n");
    for (int i = 0; i < customerCount; i++) {
        printf("%-6d %-24s %-16s %10d %12.2f\n",
               customers[i].id, customers[i].name, customers[i].phone,
               customers[i].totalPurchases, customers[i].totalSpent);
    }
}

void searchCustomer(void) {
    int id;
    printf("\nEnter Customer ID to search: ");
    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    int idx = findCustomerIndexById(id);
    if (idx == -1) {
        printf("Customer not found.\n");
        return;
    }

    Customer *c = &customers[idx];
    printf("\nCustomer Details:\n");
    printf("  ID           : %d\n", c->id);
    printf("  Name         : %s\n", c->name);
    printf("  Phone        : %s\n", c->phone);
    printf("  Total Bills  : %d\n", c->totalPurchases);
    printf("  Total Spent  : %.2f\n", c->totalSpent);
    if (c->totalPurchases >= 5) {
        printf("  Status       : Frequent Shopper\n");
    }
}

/* ================= SUPPLIER MANAGEMENT ================= */

void supplierManagementMenu(void) {
    int choice;
    do {
        printf("\n--- SUPPLIER MANAGEMENT ---\n");
        printf("1. Add Supplier\n");
        printf("2. Display All Suppliers\n");
        printf("3. Restock Product from Supplier\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: addSupplier(); break;
            case 2: displaySuppliers(); pauseScreen(); break;
            case 3: orderRestockFromSupplier(); break;
            case 0: break;
            default: printf("Invalid option.\n");
        }
    } while (choice != 0);
}

void addSupplier(void) {
    if (supplierCount >= MAX_SUPPLIERS) {
        printf("Supplier list full.\n");
        return;
    }
    Supplier s;
    printf("\nEnter Supplier ID: ");
    if (scanf("%d", &s.id) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (findSupplierIndexById(s.id) != -1) {
        printf("Supplier ID already exists.\n");
        return;
    }

    printf("Enter Company Name: ");
    fgets(s.company, sizeof(s.company), stdin);
    s.company[strcspn(s.company, "\n")] = '\0';

    printf("Enter Contact Number: ");
    fgets(s.contact, sizeof(s.contact), stdin);
    s.contact[strcspn(s.contact, "\n")] = '\0';

    printf("Enter Supplied Catalog / Category: ");
    fgets(s.catalog, sizeof(s.catalog), stdin);
    s.catalog[strcspn(s.catalog, "\n")] = '\0';

    s.restockCount = 0;
    suppliers[supplierCount++] = s;
    saveSuppliers();
    printf("Supplier added successfully!\n");
}

void displaySuppliers(void) {
    if (supplierCount == 0) {
        printf("\nNo suppliers registered.\n");
        return;
    }
    printf("\n%-6s %-24s %-15s %-18s %10s\n",
           "ID", "Company", "Contact", "Catalog", "Orders");
    printf("-------------------------------------------------------------------------------\n");
    for (int i = 0; i < supplierCount; i++) {
        printf("%-6d %-24s %-15s %-18s %10d\n",
               suppliers[i].id, suppliers[i].company, suppliers[i].contact,
               suppliers[i].catalog, suppliers[i].restockCount);
    }
}

void orderRestockFromSupplier(void) {
    int supId, prodCode, addQty;
    displaySuppliers();
    printf("\nEnter Supplier ID: ");
    if (scanf("%d", &supId) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();

    int sIdx = findSupplierIndexById(supId);
    if (sIdx == -1) {
        printf("Supplier not found.\n");
        return;
    }

    displayProducts(products, productCount);
    printf("\nEnter Product Code to restock: ");
    if (scanf("%d", &prodCode) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();

    int pIdx = findProductIndexByCode(prodCode);
    if (pIdx == -1) {
        printf("Product not found.\n");
        return;
    }

    printf("Enter quantity to add: ");
    if (scanf("%d", &addQty) != 1 || addQty <= 0) {
        clearInputBuffer();
        printf("Invalid quantity.\n");
        return;
    }
    clearInputBuffer();

    products[pIdx].quantity += addQty;
    suppliers[sIdx].restockCount++;
    saveProducts();
    saveSuppliers();
    printf("Restocked %d units of '%s' from '%s'. New stock: %d\n",
           addQty, products[pIdx].name, suppliers[sIdx].company, products[pIdx].quantity);
}

/* ================= BILLING SYSTEM ================= */

double calculateDiscountRate(double grossAmount) {
    if (grossAmount >= 5000.0) {
        return 10.0;
    } else if (grossAmount >= 3000.0) {
        return 5.0;
    } else {
        return 0.0;
    }
}

void billingSystemMenu(void) {
    int choice;
    do {
        printf("\n--- BILLING SYSTEM ---\n");
        printf("1. Create New Customer Bill\n");
        printf("2. View Saved Bills (bills.txt)\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: createNewBill(); break;
            case 2: viewSavedBills(); pauseScreen(); break;
            case 0: break;
            default: printf("Invalid option.\n");
        }
    } while (choice != 0);
}

void createNewBill(void) {
    if (productCount == 0) {
        printf("\nNo products in inventory to bill!\n");
        return;
    }

    int custId;
    Customer walkIn = {0, "Walk-in Customer", "N/A", 0, 0.0};
    Customer *activeCust = &walkIn;

    if (customerCount > 0) {
        displayCustomers();
        printf("\nEnter Customer ID (or 0 for Walk-in Customer): ");
        if (scanf("%d", &custId) != 1) { clearInputBuffer(); return; }
        clearInputBuffer();

        if (custId != 0) {
            int cIdx = findCustomerIndexById(custId);
            if (cIdx != -1) {
                activeCust = &customers[cIdx];
            } else {
                printf("Customer not found. Proceeding as Walk-in Customer.\n");
            }
        }
    }

    BillItem items[MAX_PRODUCTS];
    int itemCount = 0;
    double grossAmount = 0.0;

    while (1) {
        displayProducts(products, productCount);
        int code, qty;
        printf("\nEnter Product Code to add (or 0 to finish bill): ");
        if (scanf("%d", &code) != 1) { clearInputBuffer(); continue; }
        clearInputBuffer();

        if (code == 0) break;

        int pIdx = findProductIndexByCode(code);
        if (pIdx == -1) {
            printf("Invalid product code!\n");
            continue;
        }

        printf("Available stock for '%s': %d | Price: %.2f\n",
               products[pIdx].name, products[pIdx].quantity, products[pIdx].price);
        printf("Enter Quantity: ");
        if (scanf("%d", &qty) != 1 || qty <= 0) {
            clearInputBuffer();
            printf("Invalid quantity.\n");
            continue;
        }
        clearInputBuffer();

        if (qty > products[pIdx].quantity) {
            printf("Insufficient stock! Only %d available.\n", products[pIdx].quantity);
            continue;
        }

        /* Deduct stock immediately for this bill */
        products[pIdx].quantity -= qty;

        double subtotal = products[pIdx].price * qty;
        items[itemCount].productCode = products[pIdx].code;
        strncpy(items[itemCount].productName, products[pIdx].name, sizeof(items[itemCount].productName) - 1);
        items[itemCount].productName[sizeof(items[itemCount].productName) - 1] = '\0';
        items[itemCount].unitPrice = products[pIdx].price;
        items[itemCount].qty = qty;
        items[itemCount].subtotal = subtotal;
        itemCount++;

        grossAmount += subtotal;
        printf("Added %d x %s (Subtotal: %.2f)\n", qty, products[pIdx].name, subtotal);
    }

    if (itemCount == 0) {
        printf("\nNo items added. Bill cancelled.\n");
        loadProducts(); /* Restore any uncommitted changes */
        return;
    }

    SaleRecord rec;
    srand((unsigned int)time(NULL));
    rec.billId = 1000 + (rand() % 9000);
    rec.customerId = activeCust->id;
    getCurrentDate(rec.date, sizeof(rec.date));
    rec.grossAmount = grossAmount;
    rec.discountPercent = calculateDiscountRate(grossAmount);
    rec.discountAmount = (grossAmount * rec.discountPercent) / 100.0;
    rec.finalTotal = grossAmount - rec.discountAmount;

    /* Update customer stats if registered */
    if (activeCust->id != 0) {
        activeCust->totalPurchases++;
        activeCust->totalSpent += rec.finalTotal;
        saveCustomers();
    }

    saveProducts();
    appendSaleRecord(&rec);
    appendBillReceipt(&rec, activeCust, items, itemCount);

    /* Print receipt to console */
    printf("\n==================== FINAL RECEIPT ====================\n");
    printf("Bill ID      : #%d\n", rec.billId);
    printf("Date         : %s\n", rec.date);
    printf("Customer     : %s\n", activeCust->name);
    printf("-------------------------------------------------------\n");
    for (int i = 0; i < itemCount; i++) {
        printf("  %-20s %3d x %8.2f = %9.2f\n",
               items[i].productName, items[i].qty, items[i].unitPrice, items[i].subtotal);
    }
    printf("-------------------------------------------------------\n");
    printf("Gross Amount : %10.2f\n", rec.grossAmount);
    printf("Discount (%.0f%%): %10.2f\n", rec.discountPercent, rec.discountAmount);
    printf("FINAL TOTAL  : %10.2f\n", rec.finalTotal);
    printf("=======================================================\n");
    pauseScreen();
}

/* ================= REPORTS & DASHBOARD ================= */

void reportsMenu(void) {
    int choice;
    do {
        printf("\n--- REPORTS ---\n");
        printf("1. Sales Report (sales.txt)\n");
        printf("2. Inventory Valuation & Low Stock\n");
        printf("3. Category Breakdown\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: viewSalesReport(); pauseScreen(); break;
            case 2: viewInventoryValuation(); pauseScreen(); break;
            case 3: viewCategoryBreakdown(); pauseScreen(); break;
            case 0: break;
            default: printf("Invalid option.\n");
        }
    } while (choice != 0);
}

void viewSalesReport(void) {
    FILE *fp = fopen(SALES_FILE, "r");
    if (!fp) {
        printf("\nNo sales records found yet.\n");
        return;
    }

    SaleRecord rec;
    double totalRevenue = 0.0;
    double totalDiscounts = 0.0;
    int txCount = 0;

    printf("\n%-8s %-8s %11s %8s %11s %-16s\n",
           "BillID", "CustID", "Gross", "Disc%", "Final", "Date");
    printf("------------------------------------------------------------------------\n");

    while (fscanf(fp, "%d|%d|%lf|%lf|%lf|%lf|%19[^\n]\n",
                  &rec.billId, &rec.customerId, &rec.grossAmount,
                  &rec.discountPercent, &rec.discountAmount, &rec.finalTotal, rec.date) == 7) {
        printf("%-8d %-8d %11.2f %7.1f%% %11.2f %-16s\n",
               rec.billId, rec.customerId, rec.grossAmount,
               rec.discountPercent, rec.finalTotal, rec.date);
        totalRevenue += rec.finalTotal;
        totalDiscounts += rec.discountAmount;
        txCount++;
    }
    fclose(fp);

    printf("------------------------------------------------------------------------\n");
    printf("Total Transactions: %d | Total Discounts: %.2f | Net Revenue: %.2f\n",
           txCount, totalDiscounts, totalRevenue);
}

void viewInventoryValuation(void) {
    int totalUnits = 0;
    double totalValue = 0.0;
    int lowStockItems = 0;

    for (int i = 0; i < productCount; i++) {
        totalUnits += products[i].quantity;
        totalValue += products[i].price * products[i].quantity;
        if (products[i].quantity <= LOW_STOCK_THRESHOLD) {
            lowStockItems++;
        }
    }

    printf("\n=== INVENTORY VALUATION ===\n");
    printf("Total Product Types : %d\n", productCount);
    printf("Total Stock Units   : %d\n", totalUnits);
    printf("Total Stock Value   : %.2f\n", totalValue);
    printf("Low-Stock Alerts    : %d\n", lowStockItems);
}

void viewCategoryBreakdown(void) {
    if (productCount == 0) {
        printf("\nNo products available for category breakdown.\n");
        return;
    }

    char seenCategories[MAX_PRODUCTS][30];
    int catCount = 0;

    printf("\n%-20s %10s %12s %14s\n", "Category", "Products", "Units", "Stock Value");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < productCount; i++) {
        int alreadySeen = 0;
        for (int k = 0; k < catCount; k++) {
            if (strcmp(seenCategories[k], products[i].category) == 0) {
                alreadySeen = 1;
                break;
            }
        }
        if (alreadySeen) continue;

        strncpy(seenCategories[catCount], products[i].category, 29);
        seenCategories[catCount][29] = '\0';
        catCount++;

        int prodInCat = 0;
        int unitsInCat = 0;
        double valInCat = 0.0;

        for (int j = 0; j < productCount; j++) {
            if (strcmp(products[j].category, seenCategories[catCount - 1]) == 0) {
                prodInCat++;
                unitsInCat += products[j].quantity;
                valInCat += products[j].price * products[j].quantity;
            }
        }

        printf("%-20s %10d %12d %14.2f\n",
               seenCategories[catCount - 1], prodInCat, unitsInCat, valInCat);
    }
}

void viewSavedBills(void) {
    FILE *fp = fopen(BILLS_FILE, "r");
    if (!fp) {
        printf("\nNo saved bills found in %s.\n", BILLS_FILE);
        return;
    }
    char line[256];
    printf("\n");
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }
    fclose(fp);
}

void showDashboard(void) {
    FILE *fp = fopen(SALES_FILE, "r");
    double netSales = 0.0;
    int totalBills = 0;

    if (fp) {
        SaleRecord rec;
        while (fscanf(fp, "%d|%d|%lf|%lf|%lf|%lf|%19[^\n]\n",
                      &rec.billId, &rec.customerId, &rec.grossAmount,
                      &rec.discountPercent, &rec.discountAmount, &rec.finalTotal, rec.date) == 7) {
            netSales += rec.finalTotal;
            totalBills++;
        }
        fclose(fp);
    }

    int totalUnits = 0, lowStock = 0;
    double stockValue = 0.0;
    for (int i = 0; i < productCount; i++) {
        totalUnits += products[i].quantity;
        stockValue += products[i].price * products[i].quantity;
        if (products[i].quantity <= LOW_STOCK_THRESHOLD) lowStock++;
    }

    printf("\n==================== SMARTMART LIVE DASHBOARD ====================\n");
    printf("  Products in Catalog : %-6d | Low-Stock Alerts : %d\n", productCount, lowStock);
    printf("  Total Stock Units   : %-6d | Inventory Value  : %.2f\n", totalUnits, stockValue);
    printf("  Registered Customers: %-6d | Suppliers        : %d\n", customerCount, supplierCount);
    printf("  Completed Bills     : %-6d | Net Sales Total  : %.2f\n", totalBills, netSales);
    printf("==================================================================\n");
    pauseScreen();
}
