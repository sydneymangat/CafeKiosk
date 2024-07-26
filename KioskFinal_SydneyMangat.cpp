/*
 * Name: Sydney Mangat
 * Purpose: The Cafe Kiosk is an interface that allows for management intervention and customer purchases.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

//Constants
const int MAX_MENU_ITEMS = 30;  //Maximum number of items that can be stored in the menu
const double TAX_RATE = 0.07;   //Sales tax for calculating the total price of an order

//Function prototypes to verify functions are called properly
void displayHomeMenu();
void managerMode();
void customerMode();
bool userAuthentication();
void getCredentials(const string& prompt, string& data);
string trim(const string& str);
void displayCart(const string menuItems[], int cartIndices[], int cartSize, double& totalCost);
void displayMenu();
void displayFormattedMenu(const string menuItems[], int numItems);
int readMenuItems(string menuItems[MAX_MENU_ITEMS]);
void toggleMaintenanceMode();
void addMenuItem();
void removeMenuItem();
bool isMaintenanceMode();
void placeOrder();
void writeMenuItems(const string menuItems[], int numItems);

//Global variable to track if system is in maintenance mode
static bool maintenanceMode = false;

//Entry point of program
int main() {
    displayHomeMenu();
    return 0;
}

//Displays the initial menu and handles user interaction
void displayHomeMenu() {
    char choice;
    do {
        cout << "Welcome to the Cafe Kiosk System\n";
        cout << "A. Manager\n";
        cout << "B. Customer\n";
        cout << "C. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  //Clears input buffer

        switch (choice) {
            case 'A':
            case 'a':
                managerMode();  //Access management functions
                break;
            case 'B':
            case 'b':
                customerMode();  //Access customer functions
                break;
            case 'C':
            case 'c':
                cout << "Exiting application...\n";  //Exit application
                return;
            default:
                cout << "Invalid choice. Please enter a valid option.\n";
                break;
        }
    } while (choice != 'C' && choice != 'c');  //Continue until 'C' or 'c' is entered
}

//Manager mode allows management tasks like editing the menu and toggling maintenance mode
void managerMode() {
    if (!userAuthentication()) {
        cout << "Authentication failed. Returning to home menu.\n";
        return;  //Return to home menu if authentication fails
    }
    char choice;
    do {
        cout << "Manager Mode:\n";
        cout << "A. Display Menu\n";
        cout << "B. Toggle Maintenance Mode\n";
        cout << "C. Add Menu Item\n";
        cout << "D. Remove Menu Item\n";
        cout << "E. Go Back (Main Menu)\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 'A':
            case 'a':
                displayMenu();  //Display current menu
                break;
            case 'B':
            case 'b':
                toggleMaintenanceMode();  //Enable or disable maintenance mode
                break;
            case 'C':
            case 'c':
                addMenuItem();  //Add a new item to menu
                break;
            case 'D':
            case 'd':
                removeMenuItem();  //Remove an item from menu
                break;
            case 'E':
            case 'e':
                return;  //Return to main menu
            default:
                cout << "Invalid choice. Please enter a valid option.\n";
                break;
        }
    } while (choice != 'E' && choice != 'e');  //Stay in manager mode until 'E' or 'e' is entered
}

//Customer mode: Allows customers to view menu and place orders
void customerMode() {
    if (isMaintenanceMode()) {
        cout << "The kiosk is currently undergoing repairs. We are unable to take any orders at this time. We apologize for the inconvenience.\n";
        return;  //Exit if the system is in maintenance mode
    }

    int choice;
    do {
        cout << "\nCustomer Mode:\n";
        cout << "1. Display Menu\n";
        cout << "2. Place Order\n";
        cout << "3. Go Back\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                displayMenu();  //Display current menu
                break;
            case 2:
                placeOrder();  //Allow customer to place order
                break;
            case 3:
                return;  //Return to main menu
            default:
                cout << "Invalid choice. Please enter a valid option.\n";
                break;
        }
    } while (choice != 3);  //Continue until '3' is entered
}

//Authenticates a manager user against stored credentials file
bool userAuthentication() {
    string userName, password;
    getCredentials("What is your username: ", userName);
    getCredentials("What is your password: ", password);

    ifstream adminFile("credentials.txt");
    if (!adminFile.is_open()) {
        cout << "Invalid Credentials" << endl;
        return false;
    }

    string line;
    bool isAuthenticated = false;
    while (getline(adminFile, line)) {
        istringstream ss(line);
        string fileUserName, filePassword;
        getline(ss, fileUserName, ';');
        getline(ss, filePassword, ';');

        //Trim strings for consistency
        fileUserName = trim(fileUserName);
        filePassword = trim(filePassword);
        userName = trim(userName);
        password = trim(password);

        //Check if input matches stored credentials
        if (fileUserName == userName && filePassword == password) {
            isAuthenticated = true;
            cout << "User Authenticated" << endl;
            break;
        }
    }
    adminFile.close();
    return isAuthenticated;
}

//Get credentials from user
void getCredentials(const string& prompt, string& data) {
    cout << prompt;
    getline(cin, data);  // Read a line of text
}

//Trim whitespace from both ends of string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    if (first == string::npos || last == string::npos)
        return "";
    return str.substr(first, (last - first + 1));
}


//Display formatted menu item
void displayFormattedItem(int itemNumber, const string& category, const string& item, const string& price, const string& description) {
    cout << "---------------------------------------------------------" << endl;
    cout << "No.   Category            Price      Item" << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << itemNumber << "     " << category;
    for (int i = category.length(); i < 20; i++) cout << " ";  // Align category
    cout << price;
    for (int i = price.length(); i < 10; i++) cout << " ";  // Align price
    cout << item << " - " << description << endl;
    cout << "---------------------------------------------------------" << endl;
}

//Add a new menu item
void addMenuItem() {
    string menuItems[MAX_MENU_ITEMS];
    int itemCount = 0;
    ifstream file("menu.txt");
    string line;

    //Read existing items
    while (getline(file, line) && itemCount < MAX_MENU_ITEMS) {
        menuItems[itemCount++] = line;
    }
    file.close();

    //Check if maximum number of items is reached
    if (itemCount >= MAX_MENU_ITEMS) {
        cout << "Cannot add more items. Maximum of 30 reached.\n";
        return;
    }

    string category, item, price, description;
    cout << "Enter category: ";
    getline(cin, category);
    cout << "Enter item name: ";
    getline(cin, item);
    cout << "Enter price: ";
    getline(cin, price);
    cout << "Enter description: ";
    getline(cin, description);

    //Ensure proper formatting
    category = trim(category);
    item = trim(item);
    price = trim(price);
    description = trim(description);

    //Write formatted data to file
    ofstream outFile("menu.txt", ios::app);
    if (outFile) {
        outFile << category << ";" << item << ";" << price << ";" << description << "\n";
        outFile.close();
        cout << "Item added successfully.\n";
    } else {
        cout << "Failed to open the menu file for writing.\n";
    }
}


//Read menu items from file into array
int readMenuItems(string menuItems[MAX_MENU_ITEMS]) {
    ifstream file("menu.txt");
    string line;
    int count = 0;
    while (getline(file, line) && count < MAX_MENU_ITEMS) {
        menuItems[count++] = line;
    }
    file.close();
    return count;
}

//Display menu items with index numbers
void displayMenu(const string menuItems[], int itemCount) {
    cout << "Current Menu Items:" << endl;
    for (int i = 0; i < itemCount; i++) {
        cout << i + 1 << ": " << menuItems[i] << endl;
    }
}

//Write updated list back to the file
void writeMenuItems(const string menuItems[], int itemCount) {
    ofstream file("menu.txt");
    for (int i = 0; i < itemCount; i++) {
        file << menuItems[i] << endl;
    }
    file.close();
}

//Function to remove a menu item
void removeMenuItem() {
    string menuItems[MAX_MENU_ITEMS];
    int itemCount = readMenuItems(menuItems);
    
    displayMenu(menuItems, itemCount);
    
    cout << "Enter the number of the item to remove: ";
    int indexToRemove;
    cin >> indexToRemove;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (indexToRemove < 1 || indexToRemove > itemCount) {
        cout << "Invalid item number!" << endl;
        return;
    }

    //Shift items down to remove selected item
    for (int i = indexToRemove - 1; i < itemCount - 1; i++) {
        menuItems[i] = menuItems[i + 1];
    }

    //Write updated list back to file
    writeMenuItems(menuItems, itemCount - 1);

    cout << "Item removed successfully." << endl;
}

void displayMenu() {
    ifstream file("menu.txt");
    if (!file) {
        cout << "Failed to open the menu file." << endl;
        return;
    }

    string line;
    int itemNumber = 0;

    //Header
    cout << "****************MENU****************" << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "No.   Category            Price      Item" << endl;
    cout << "---------------------------------------------------------" << endl;

    //Read each line from file
    while (getline(file, line)) {
        itemNumber++;
        //Find positions of semicolons
        size_t firstSemi = line.find(';');
        size_t secondSemi = line.find(';', firstSemi + 1);
        size_t thirdSemi = line.find(';', secondSemi + 1);

        //Extract data between semicolons
        string category = line.substr(0, firstSemi);
        string item = line.substr(firstSemi + 1, secondSemi - firstSemi - 1);
        string price = line.substr(secondSemi + 1, thirdSemi - secondSemi - 1);
        string description = line.substr(thirdSemi + 1);

        //Output data
        cout << itemNumber << "     " << category;
        //Make sure category field is even add spaces for user experience
        for (int i = category.length(); i < 20; i++) {
            cout << " ";
        }
        cout << price;
        //Align price field by adding spaces
        for (int i = price.length(); i < 10; i++) {
            cout << " ";
        }
        cout << item << endl;
        //Print description on the next line with consistent indentation for user experience
        cout << "                                      " << description << endl;
    }

    cout << "*****************************************************" << endl;

    file.close();
}


void toggleMaintenanceMode() {
    maintenanceMode = !maintenanceMode;
    cout << "Maintenance Mode is now " << (maintenanceMode ? "enabled" : "disabled") << ".\n";
}

bool isMaintenanceMode() {
    return maintenanceMode;
}
void displayFormattedMenu(const string menuItems[], int numItems) {


    for (int i = 0; i < numItems; i++) {
        stringstream ss(menuItems[i]);
        string category, item, price, description;
        getline(ss, category, ';');
        getline(ss, item, ';');
        getline(ss, price, ';');
        getline(ss, description);
        cout << i + 1 << " - " << item << " (" << category << ") - $" << price << " - " << description << endl;
    }

    cout << "---------------------------------------------------------" << endl;
}

//Display cart function extra credit to show customer what is in their cart and their total
void displayCart(const string menuItems[], int cartIndices[], int cartSize, double& totalCost) {
    cout << "\nCurrent Cart:" << endl;
    for (int i = 0; i < cartSize; i++) {
        stringstream ss(menuItems[cartIndices[i]]);
        string category, item, price, description;
        getline(ss, category, ';');
        getline(ss, item, ';');
        getline(ss, price, ';');
        getline(ss, description);
        cout << "- " << item << " at $" << price << endl;
    }
    cout << "Total Cost: $" << fixed << setprecision(2) << totalCost << endl << endl;
}
void placeOrder() {
    string menuItems[MAX_MENU_ITEMS];
    int itemCount = readMenuItems(menuItems);
    int cartIndices[MAX_MENU_ITEMS];
    int cartSize = 0;
    int itemChoice;
    double totalCost = 0.0;
    char choice;

    do {
        displayMenu();  //Display formatted menu
        cout << "Select the number of the dish to add to your cart, 0 to review cart: ";
        int itemChoice;
        cin >> itemChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (itemChoice > 0 && itemChoice <= itemCount) {
            string itemDetails = menuItems[itemChoice - 1];
            stringstream ss(itemDetails);
            string category, item, price, description;
            getline(ss, category, ';');
            getline(ss, item, ';');
            getline(ss, price, ';');
            getline(ss, description);
            double itemPrice = stod(price);

            cartIndices[cartSize++] = itemChoice - 1;  //Add item index to cart
            totalCost += itemPrice;
            cout << "You selected " << item << ": $" << fixed << setprecision(2) << itemPrice << endl;
        } else if (itemChoice == 0) {
            displayCart(menuItems, cartIndices, cartSize, totalCost);  //Display cart
        } else {
            cout << "Invalid choice. Please try again.\n";
        }

        cout << "Order Options:\nA. Done\nB. Add another item\nC. Cancel\nEnter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 'A' || choice == 'a') {
            totalCost += totalCost * TAX_RATE;  // Apply tax
            displayCart(menuItems, cartIndices, cartSize, totalCost);  //Display cart before payment
            cout << "Your order has been taken. Please proceed to the register to pay $" 
                << fixed << setprecision(2) << totalCost 
                << " and collect your food. Thank you, and please visit again.\n";
            break;
        } else if (choice == 'C' || choice == 'c') {
            cout << "Order canceled. Returning to main menu.\n";
            return;
        }
    } while (choice == 'B' || choice == 'b' || itemChoice != -1);
}
