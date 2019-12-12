#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// Acts as the tape, works as a linked list. Has prev() and
// next() functions. To check if a piece exists before or
// after, use prevt and nextt, as prev() and next() will
// create a new one in the event none exists.
struct Tape {
    char c;
    Tape *prevt, *nextt;

    Tape(char ch, Tape* p, Tape* n) {
        c = ch;
        prevt = p;
        nextt = n;
    }

    // Get the previous tape. If none exists, create a new one
    // with a # sign.
    Tape * prev() {
        if (prevt == NULL)
            prevt = new Tape('#', NULL, this);
        return prevt;
    }

    // Get the next tape. If none exists, create a new one
    // with a # sign.
    Tape * next() {
        if (nextt == NULL)
            nextt = new Tape('#', this, NULL);
        return nextt;
    }
};

// Turn a string into a Tape structure and return the lead struct.
Tape * newtape(string s) {
    Tape * first = new Tape(s[0], NULL, NULL);
    Tape * prev = first;
    for (int i = 1; i < s.length(); i++) {
        Tape * next = new Tape(s[i], prev, NULL);
        prev->nextt = next;
        prev = next;
    }
    return first;
}

// A Turing machine state, contains all the necessary info from
// a .tm file.
struct TState {
    int state, next;
    char curr, write, dir;
};

// A Turing machine executor. Use become("file.tm") to read in
// the Turing instructions and use execute(Tape*) to run the
// machine on a specific tape.
class TM {
private:
    // Store current state, stop state, current tape, and a vector
    // containing all states and what to do in each scenario.
    int state, stop;
    Tape * curr;
    vector<TState*> v;

public:
    // Read in a file and get its instructions.
    void become(string f) {
        ifstream file(f);
        string line;
        // Start each file w/ two lines:
        // start,n1
        // stop,n2
        // Where n1 and n2 are the respective start and stop states.
        while (getline(file, line)) {
            if (line.substr(0, 6) == "start,")
                state = atoi(line.substr(6, -1).c_str());
            else if (line.substr(0, 5) == "stop,")
                stop = atoi(line.substr(5, -1).c_str());
            else {
                stringstream l(line);
                string segment;
                vector<string> seglist;
                while (getline(l, segment, ','))
                    seglist.push_back(segment);
                TState * x = new TState;
                // Easiest way to split the line by comma and create
                // the TState to store the instruction.
                x->state = atoi(seglist[0].c_str());
                x->curr = seglist[1][0];
                x->next = atoi(seglist[2].c_str());
                x->dir = seglist[3][0];
                x->write = seglist[4][0];
                // Push it to the list of states.
                v.push_back(x);
            }
        }
        file.close();
    }

    // Called after become(), executes the Turing machine on a specific tape.
    void execute(Tape * t) {
        curr = t;
        while (state != stop) {
            // Print what you're doing for clarity.
            cout << "state " << state << ", read in " << curr->c;
            // Check each of the states listed in the vector v to see whether
            // the state and current in character match. While an array might be
            // quicker, it would also not support strings in the future and would
            // need to be changed.
            for (int i = 0; i < v.size(); i++) {
                if (v[i]->state == state && v[i]->curr == curr->c) {
                    cout << " moving to " << v[i]->state << ", tape in dir " << v[i]->dir;
                    cout << " writing " << v[i]->write << "\n";
                    // Write the character, unless it's a -, in which case write nothing.
                    if (v[i]->write != '-')
                        curr->c = v[i]->write;
                    // Move the tape left or right depending on what the state calls for.
                    if (v[i]->dir == 'R')
                        curr = curr->next();
                    else if (v[i]->dir == 'L')
                        curr = curr->prev();
                    // Then change the state based on the instruction and break to
                    // start the while loop again.
                    state = v[i]->next;
                    break;
                }
            }
        }
    }
};

// Print all of a tape.
void tapeprint(Tape * a) {
    cout << a->c;
    if (a->nextt != NULL)
        tapeprint(a->next());
    else
        cout << "\n";
}

int main() {
    // Create a tape from a string and print it so we know what it is.
    string s;
    cout << "Enter the tape: ";
    cin >> s;
    Tape * a = newtape(s);
    tapeprint(a);
    // Create a Turing machine with the instructions from sub.tm.txt,
    // which subtracts one from a number.
    TM sub;
    sub.become("sub.tm.txt");
    sub.execute(a);
    // Run the program and print the result.
    tapeprint(a);
    return 0;
}
