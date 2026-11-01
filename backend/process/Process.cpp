
#include "Process.h"

string getStateName(ProcessState state) {

    switch (state) {

        case ProcessState::NEW:
            return "New";

        case ProcessState::READY:
            return "Ready";

        case ProcessState::RUNNING:
            return "Running";

        case ProcessState::WAITING:
            return "Waiting";

        case ProcessState::TERMINATED:
            return "Terminated";
    }

    return "Unknown";
}
