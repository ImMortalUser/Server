#pragma once
#include <iostream>
#include <database.h>

static void terminal()
{
    const std::string command_list =
        "[0] exit\n"
        "[1] show registered users\n"
        "[2] show online users\n";

    short command;
    for (;;)
    {
        std::cout << command_list << "terminal> ";
        std::cin >> command;
        switch (command)
        {
        case 0:
        {
            exit(0);
            break;
        }

        case 1:
        {
            database::selectData();
            break;
        }

        case 2:
        {
            std::cout << "[no code]";
            break;
        }

        default:
        {
            std::cout << "Unknown command\n";
            break;
        }
        }
    }
}
