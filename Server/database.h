#pragma once
#include <string>
#include "database.h"
#include <string>
#include <lib/sqlite-amalgamation-3430000/sqlite3.h>
#include <iostream>
#include <vector>

static const std::string db_dir = "lastochka.db";

class database
{
public:

    static int createDB()
    {
        sqlite3* DB;
        int exit = 0;
        exit = sqlite3_open(db_dir.c_str(), &DB);
        sqlite3_close(DB);
        return 0;
    }

    static int createTableUsers()
    {
        sqlite3* DB;
        std::string sql = "CREATE TABLE IF NOT EXISTS USERS("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "NAME TEXT NOT NULL, "
                          "PASSWORD TEXT NOT NULL, "
                          "IP TEXT NOT NULL, "
                          "ONLINE BIT NOT NULL);";

        try
        {
            int exit = 0;
            exit = sqlite3_open(db_dir.c_str(), &DB);

            char* message_error;
            exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &message_error);

            if (exit != SQLITE_OK)
            {
                std::cerr << "Error create table" << std::endl;
                sqlite3_free(message_error);
            }
            else
            {
                std::cout << "Table USERS created successfully" << std::endl;
            }
            sqlite3_close(DB);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what();
        }

        return 0;
    }

    static int createTableMessages()
    {
        sqlite3* DB;
        std::string sql = "CREATE TABLE IF NOT EXISTS MESSAGES("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "RECEIVERNAME TEXT NOT NULL, "
                          "SENDERNAME TEXT NOT NULL, "
                          "MESSAGE TEXT NOT NULL, "
                          "TIMESTAMP TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
        try
        {
            int exit = 0;
            exit = sqlite3_open(db_dir.c_str(), &DB);

            char* message_error;
            exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &message_error);

            if (exit != SQLITE_OK)
            {
                std::cerr << "Error create table" << std::endl;
                sqlite3_free(message_error);
            }
            else
            {
                std::cout << "Table CHATS created successfully" << std::endl;
            }
            sqlite3_close(DB);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what();
        }

        return 0;
    }

    static int addUserToTable(const std::string user_name, const std::string user_password, const std::string user_ip)
    {
        sqlite3* DB;
        std::string sql = "INSERT INTO USERS ("
                          "NAME, PASSWORD, IP, ONLINE) "
                          "VALUES (\"" + user_name + "\", \"" + user_password + "\", \"" + user_ip + "\", 0);";
        try
        {
            int exit = 0;
            exit = sqlite3_open(db_dir.c_str(), &DB);

            char* message_error;
            exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &message_error);

            if (exit != SQLITE_OK)
            {
                std::cerr << "Error add user" << std::endl;
                sqlite3_free(message_error);
            }
            else
            {
                std::cout << "User added successfully" << std::endl;
            }
            sqlite3_close(DB);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what();
        }

        return 0;
    }

    static int addMessageToMessages(const std::string& receiver_name, const std::string& sender_name, const std::string& message_text)
    {
        if (message_text == ""); // Should be replaced in main code, probably
        sqlite3* DB;
        std::string sql = "INSERT INTO MESSAGES (RECEIVERNAME, SENDERNAME, MESSAGE) "
                          "VALUES ('" + receiver_name + "', '" + sender_name + "', '" + message_text + "');";
        try
        {
            int exit = 0;
            exit = sqlite3_open(db_dir.c_str(), &DB);

            char* message_error;
            exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &message_error);

            if (exit != SQLITE_OK)
            {
                std::cerr << "Error adding message to MESSAGES table: " << message_error << std::endl;
                sqlite3_free(message_error);
            }
            else
            {
                std::cout << "Message added to MESSAGES table successfully" << std::endl;
            }
            sqlite3_close(DB);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what();
        }

        return 0;
    }

    static int selectData()
    {
        sqlite3* DB;
        int exit = sqlite3_open(db_dir.c_str(), &DB);
        std::string sql =
            "SELECT * FROM USERS;";
        sqlite3_exec(DB, sql.c_str(), callback, NULL, NULL);
        return 0;
    }

    static bool userExists(const std::string& name, const std::string& password) {
        sqlite3* db;
        int rc = sqlite3_open(db_dir.c_str(), &db);

        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        std::string query = "SELECT * FROM users WHERE NAME = ? AND PASSWORD = ?";
        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return false;
        }

        rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        rc = sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind parameters: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return false;
        }

        rc = sqlite3_step(stmt);

        bool userExists = (rc == SQLITE_ROW);

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return userExists;
    }

    static bool userExists(const std::string& name) {
        sqlite3* db;
        int rc = sqlite3_open(db_dir.c_str(), &db);

        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        std::string query = "SELECT * FROM users WHERE NAME = ?";
        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return false;
        }

        rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind parameters: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return false;
        }

        rc = sqlite3_step(stmt);

        bool userExists = (rc == SQLITE_ROW);

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return userExists;
    }

    static bool userOnline(const std::string& name) {
        sqlite3* db;
        int rc = sqlite3_open(db_dir.c_str(), &db);

        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        std::string query = "SELECT * FROM Users WHERE NAME = ? AND ONLINE = 1";
        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return false;
        }

        rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind parameters: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return false;
        }

        rc = sqlite3_step(stmt);

        bool isOnline = (rc == SQLITE_ROW);

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return isOnline;
    }


    static bool ToggleOnlineStatus(const std::string& username, bool onlineStatus) {
        sqlite3* DB;
        int exit = sqlite3_open(db_dir.c_str(), &DB);
        std::string sql = "UPDATE Users SET ONLINE = ? WHERE NAME = ?";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            // Ïðèâÿçûâàåì ïàðàìåòðû ê çàïðîñó
            sqlite3_bind_int(stmt, 1, onlineStatus ? 1 : 0); // 1 äëÿ true, 0 äëÿ false
            sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                std::cout << "Online status toggled successfully." << std::endl;
                sqlite3_finalize(stmt);
                return true;
            }
        }

        std::cerr << "Error toggling online status: " << sqlite3_errmsg(DB) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    static std::vector<std::pair<std::string, std::string>> getMessages(const std::string& sender, const std::string& receiver) {
        std::vector<std::pair<std::string, std::string>> messages;

        sqlite3* db;
        int rc = sqlite3_open(db_dir.c_str(), &db);

        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return messages;
        }

        std::string query = "SELECT MESSAGE, TIMESTAMP FROM MESSAGES WHERE SENDERNAME = ? AND RECEIVERNAME = ?";
        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return messages;
        }

        rc = sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_STATIC);
        rc = sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_STATIC);

        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind parameters: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return messages;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            std::string message(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            std::string timestamp(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            messages.push_back(std::make_pair(message, timestamp));
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return messages;
    }




private:


    static int callback(void* NotUsed, int argc, char** argv, char** azColName)
    {
        for (int i = 0; i < argc; i++)
        {
            std::cout << azColName[i] << ": " << argv[i] << std::endl;
        }
        std::cout << std::endl;
        return 0;
    }


};
