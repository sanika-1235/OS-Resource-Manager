
#include <iostream>
#include "ProcessManager.h"

using namespace std;

int main() {

    ProcessManager manager;
    int choice;

    do {
        cout << "\n================================\n";
        cout << "       PROCESS MANAGEMENT\n";
        cout << "================================\n";

        cout << "1. Create Process\n";
        cout << "2. Display All Processes\n";
        cout << "3. Move Process to Ready\n";
        cout << "4. Move Process to Running\n";
        cout << "5. Move Process to Waiting\n";
        cout << "6. Terminate Process\n";
        cout << "0. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {

            case 1: {
                int pid, arrival, burst, priority;
                string name;

                cout << "Enter Process ID: ";
                cin >> pid;

                cout << "Enter Process Name: ";
                cin >> name;

                cout << "Enter Arrival Time: ";
                cin >> arrival;

                cout << "Enter Burst Time: ";
                cin >> burst;

                cout << "Enter Priority: ";
                cin >> priority;

                manager.createProcess(
                    pid, name, arrival, burst, priority
                );

                break;
            }

            case 2:
                manager.displayProcesses();
                break;

            case 3: {
                int pid;
                cout << "Enter PID: ";
                cin >> pid;

                manager.updateState(
                    pid, ProcessState::READY
                );
                break;
            }

            case 4: {
                int pid;
                cout << "Enter PID: ";
                cin >> pid;

                manager.updateState(
                    pid, ProcessState::RUNNING
                );
                break;
            }

            case 5: {
                int pid;
                cout << "Enter PID: ";
                cin >> pid;

                manager.updateState(
                    pid, ProcessState::WAITING
                );
                break;
            }

            case 6: {
                int pid;
                cout << "Enter PID: ";
                cin >> pid;

                manager.terminateProcess(pid);
                break;
            }

            case 0:
                cout << "Exiting Process Management...\n";
                break;

            default:
                cout << "Invalid choice! Try again.\n";
        }

    } while (choice != 0);

    return 0;
}
