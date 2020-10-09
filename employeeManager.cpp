// By: Nenad Ostojic

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <occi.h>
#include <cstring>

using oracle::occi::Environment;
using oracle::occi::Connection;

using namespace oracle::occi;
using namespace std;

struct Employee
{
    int employeeNumber;
    char lastName[50];
    char firstName[50];
    char email[100];
    char phone[50];
    char extension[10];
    char reportsTo[100];
    char jobTitle[50];
    char city[50];
};

int menu(void);
int findEmployee(Connection *conn, int employeeNumber, Employee *emp);
void displayEmployee(Connection *conn, Employee *emp);
void displayAllEmployees(Connection *conn);
void insertEmployee(Connection *conn, Employee *emp);
void updateEmployee(Connection *conn, int employeeNumber);
void deleteEmployee(Connection *conn, int employeeNumber);

int main(void)
{
    Environment *env = nullptr;
    Connection *conn = nullptr;
    
    string user = "xxxxxx"; // redacted
    string pass = "xxxxxx"; // redacted
    string constr = "xxxxxx"; // redacted
    
    try {

        env = Environment::createEnvironment(Environment::DEFAULT);
        conn = env->createConnection(user, pass, constr);
    
        Employee employee;
        int menuSelection;
        bool exit = false;

        while (!exit)
        {
            menuSelection = menu();
            switch (menuSelection)
            {
            case 1:
                cout << "Employee Number: ";
                cin >> employee.employeeNumber;
                displayEmployee(conn, &employee);
                break;
            case 2:
                displayAllEmployees(conn);
                break;
            case 3:
                cout << "Employee Number: ";
                cin >> employee.employeeNumber;
                insertEmployee(conn, &employee);
                break;
            case 4:
                cout << "Employee Number: ";
                cin >> employee.employeeNumber;
                updateEmployee(conn, employee.employeeNumber);
                break;
            case 5:
                cout << "Employee Number: ";
                cin >> employee.employeeNumber;
                deleteEmployee(conn, employee.employeeNumber);
                break;
            case 6:
                exit = true;
                break;
            default:
                break;
            }
        }
        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);

    }
    catch (SQLException &sqlExcp)
    {
        cout << "error";
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }

    return 0;
}

int menu(void)
{
    int selection;
    cout << "********************* HR Menu *********************" << endl;
    cout << "1) Find Employee" << endl;
    cout << "2) Employees Report" << endl;
    cout << "3) Add Employee" << endl;
    cout << "4) Update Employee" << endl;
    cout << "5) Remove Employee" << endl;
    cout << "6) Exit" << endl;
    cout << "> ";
    cin >> selection;

    while (selection < 1 || selection > 6)
    {
        cout << "Invalid entry, try again: ";
        cin >> selection;
    }

    return selection;
}

int findEmployee(Connection *conn, int employeeNumber, Employee *emp)
{
    int returnValue = 1;
    
    try
    {
        Statement *stmt = nullptr;
        ResultSet *rs = nullptr;

        stmt = conn->createStatement("SELECT e.employeenumber, e.lastname, e.firstname, e.email, o.phone, e.extension, m.firstname || ' ' || m.lastname AS REPORTSTO, e.jobtitle, o.city FROM EMPLOYEES e LEFT OUTER JOIN EMPLOYEES m ON e.reportsto = m.employeenumber INNER JOIN OFFICES o ON e.officecode = o.officecode WHERE e.employeenumber = :1");
        stmt->setInt(1, employeeNumber);
        rs = stmt->executeQuery();

        if (!rs->next())
        {
            returnValue = 0;
        }
        else
        {
            do
            {
                emp->employeeNumber = rs->getInt(1);
                strncpy(emp->lastName, rs->getString(2).c_str(), 50);
                strncpy(emp->firstName, rs->getString(3).c_str(), 50);
                strncpy(emp->email, rs->getString(4).c_str(), 100);
                strncpy(emp->phone, rs->getString(5).c_str(), 50);
                strncpy(emp->extension, rs->getString(6).c_str(), 10);
                strncpy(emp->reportsTo, rs->getString(7).c_str(), 100);
                strncpy(emp->jobTitle, rs->getString(8).c_str(), 50);
                strncpy(emp->city, rs->getString(9).c_str(), 50);
            } while (rs->next());
        }
    }
    catch (SQLException &sqlExcp)
    {
        cout << "Error finding employee.\nError ";
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }

    return returnValue;
}

void displayEmployee(Connection *conn, Employee *emp)
{
    int result = findEmployee(conn, emp->employeeNumber, emp);

    if (result == 0)
    {
        cout << "Employee " << emp->employeeNumber << " does not exist." << endl;
    }
    else
    {
        cout << "\nEmployee Information" << endl;
        cout << "------------------------------------------" << endl;
        cout << "employeeNumber = " << emp->employeeNumber << endl;
        cout << "lastName = " << emp->lastName << endl;
        cout << "firstName = " << emp->firstName << endl;
        cout << "email = " << emp->email << endl;
        cout << "phone = " << emp->phone << endl;
        cout << "extension = " << emp->extension << endl;
        cout << "reportsTo = " << emp->reportsTo << endl;
        cout << "jobTitle = " << emp->jobTitle << endl;
        cout << "city = " << emp->city << endl;
        cout << "------------------------------------------\n" << endl;
    }
}

void displayAllEmployees(Connection *conn)
{
    try
    {
        Statement *stmt = nullptr;
        ResultSet *rs = nullptr;

        stmt = conn->createStatement("SELECT e.employeenumber, e.firstname || ' ' || e.lastname AS NAME, e.email, o.phone, e.extension, m.firstname || ' ' || m.lastname AS MANAGER FROM EMPLOYEES e LEFT OUTER JOIN EMPLOYEES m ON e.reportsto = m.employeenumber INNER JOIN OFFICES o ON e.officecode = o.officecode ORDER BY e.employeenumber");
        rs = stmt->executeQuery();

        cout << "E       Employee Name            Email                                   Phone               Ext     Manager                  " << endl;
        cout << "-------------------------------------------------------------------------------------------------------------------------" << endl;
        while (rs->next())
        {
            cout.width(8);
            cout << left << rs->getInt(1);
            cout.width(25);
            cout << left << rs->getString(2);
            cout.width(40);
            cout << left << rs->getString(3);
            cout.width(20);
            cout << left << rs->getString(4);
            cout.width(8);
            cout << left << rs->getString(5);
            cout.width(20);
            cout << left << rs->getString(6) << endl;
        }
    }
    catch (SQLException &sqlExcp)
    {
        cout << "Error displaying employees.\nError ";
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
}

void insertEmployee(Connection *conn, Employee *emp)
{
    int result = findEmployee(conn, emp->employeeNumber, emp);

    if (result == 1)
    {
        cout << "An employee with the same employee number exists." << endl;
    }
    else
    {
        try
        {
            Statement *stmt = nullptr;
            ResultSet *rs = nullptr;
            
            char buffer[100];
            cin.ignore(1000, '\n');

            cout << "Last Name: ";
            cin.getline(buffer, 50);
            strncpy(emp->lastName, buffer, 50);

            cout << "First Name: ";
            cin.getline(buffer, 50);
            strncpy(emp->firstName, buffer, 50);

            cout << "Email: ";
            cin.getline(buffer, 100);
            strncpy(emp->email, buffer, 100);

            cout << "Extention: ";
            cin.getline(buffer, 10);
            strncpy(emp->extension, buffer, 10);

            cout << "Job Title: ";
            cin.getline(buffer, 50);
            strncpy(emp->jobTitle, buffer, 50);

            stmt = conn->createStatement("INSERT INTO employees (employeenumber, lastname, firstname, extension, email, officecode, reportsto, jobtitle) VALUES (:1, :2, :3, :4, :5, 1, 1002, :6)");
            stmt->setInt(1, emp->employeeNumber);
            stmt->setString(2, emp->lastName);
            stmt->setString(3, emp->firstName);
            stmt->setString(4, emp->extension);
            stmt->setString(5, emp->email);
            stmt->setString(6, emp->jobTitle);
            stmt->executeUpdate();

            cout << "The new employee is added successfully." << endl;
        }
        catch (SQLException &sqlExcp)
        {
            cout << "Error adding employee.\nError ";
            cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
        }

    }
}

void updateEmployee(Connection *conn, int employeeNumber)
{
    Employee emp;
    int result = findEmployee(conn, employeeNumber, &emp);

    if (result == 0)
    {
        cout << "Employee " << employeeNumber << " does not exist." << endl;
    }
    else
    {    
        try
        {
            Statement *stmt = nullptr;
            ResultSet *rs = nullptr;

            cout << "New Extension: ";
            cin.ignore(1000, '\n');
            cin >> emp.extension;

            stmt = conn->createStatement("UPDATE employees SET extension = :1 WHERE employeenumber = :2");
            stmt->setString(1, emp.extension);
            stmt->setInt(2, employeeNumber);
            stmt->executeUpdate();

            cout << "The employee is updated." << endl;
        }
        catch (SQLException &sqlExcp)
        {
            cout << "Error updating employee.\nError ";
            cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
        }
    }
}

void deleteEmployee(Connection *conn, int employeeNumber)
{
    Employee emp;
    int result = findEmployee(conn, employeeNumber, &emp);

    if (result == 0)
    {
        cout << "Employee " << employeeNumber << " does not exist." << endl;
    }
    else
    {
        try
        {
            Statement *stmt = nullptr;
            ResultSet *rs = nullptr;

            stmt = conn->createStatement("DELETE FROM employees WHERE employeenumber = :1");
            stmt->setInt(1, employeeNumber);
            stmt->executeUpdate();

            cout << "The employee is deleted." << endl;
        }
        catch (SQLException &sqlExcp)
        {
            cout << "Error deleting employee.\nError ";
            cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
        }
    }
}