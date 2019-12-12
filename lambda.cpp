#include <iostream>
#include <string>

using namespace std;

// $ = pair, # = lambda, v = variable
struct lambda {
    char type, ch; // ch used for lambda or variable
    lambda *c1, *c2, *p; // lambda has 1 child, pair has 2
    // p is parent

    lambda(char t, lambda *par = NULL) {
        type = t;
        c1 = NULL;
        c2 = NULL;
        p = par;
    }
};

// Turn a lambda into a string and return.
string printlambda(lambda * x) {
    if (x->type == '$') {
        return "(" + printlambda(x->c1) + " " + printlambda(x->c2) + ")";
    } else if (x->type == '#') {
        string b = "";
        b = b + x->ch;
        return "#" + b + "." + printlambda(x->c1);
    } else if (x->type == 'v') {
        string b = "";
        b = b + x->ch;
        return b;
    }
}

// Create a copy of a lambda and return it.
lambda *copylambda(lambda * x) {
    lambda *n = new lambda(x->type);
    if (n->type == '$') {
        n->c1 = copylambda(x->c1);
        n->c2 = copylambda(x->c2);
        n->c1->p = n;
        n->c2->p = n;
    } else if (n->type == '#') {
        n->ch = x->ch;
        n->c1 = copylambda(x->c1);
        n->c1->p = n;
    } else if (n->type == 'v') {
        n->ch = x->ch;
    }
    return n;
}

// Search a lambda and all its descendants and replace every
// variable with ch equal to x with b.
void copyto(char x, lambda *a, lambda *b) {
    if (a->type == 'v' && a->ch == x) {
        lambda *n = copylambda(b);
        if (a->p != NULL) {
            if (a->p->c1 == a) {
                a->p->c1 = n;
            } else if (a->p->c2 == a) {
                a->p->c2 = n;
            }
        }
    } else if (a->type == '$') {
        copyto(x, a->c1, b);
        copyto(x, a->c2, b);
    } else if (a->type == '#') {
        copyto(x, a->c1, b);
    }
}

// Call reducelambda on every lambda in a tree. Starts w/ those located
// farthest down the tree and works its way up.
lambda *reducelambda(lambda *x) {
    if (x->type == 'v') {
        return x;
    } else if (x->type == '#') {
        x->c1 = reducelambda(x->c1);
        return x;
    } else if (x->type == '$') {
        // Replacements start at the bottom of the tree
        x->c1 = reducelambda(x->c1);
        x->c2 = reducelambda(x->c2);
        // Perform lambda reduction if c1 is #
        if (x->c1->type == '#') {
            copyto(x->c1->ch, x->c1, x->c2);
            lambda *b = x->c1->c1;
            b->p = NULL;
            return b;
        }
        return x;
    }
}

// Call reducelambda on x until no changes are made.
lambda *reduce(lambda *x) {
    string a = printlambda(x);
    x = reducelambda(x);
    string b = printlambda(x);
    if (a.compare(b)) {
        x = reduce(x);
    }
    return x;
}

// Turn string into lambda structure.
lambda *stringtolambda(string s, lambda *x) {
    if (s.substr(0,1) == "(") {
        // Locate the splitting space character (not enclosed in parentheses).
        int j = 1;
        int c = 0;
        while (j < s.size()) {
            if (s.substr(j, 1) == "(")
                c++;
            if (s.substr(j, 1) == ")")
                c--;
            if (s.substr(j, 1) == " " && c == 0) {
                break;
            }
            j++;
        }
        if (j == 1) {
            cout << "Error: parentheses not closed\n";
            return NULL;
        }
        x->type = '$';

        string s1 = s.substr(1, j - 1);
        string s2 = s.substr(j+1);
        s2 = s2.substr(0, s2.size()-1);
        //cout << "*" << s1 << "*" << s2 << "*\n";

        // run stringtolambda on both substrings
        x->c1 = stringtolambda(s1, new lambda('a'));
        x->c2 = stringtolambda(s2, new lambda('a'));
        x->c1->p = x;
        x->c2->p = x;
        return x;
    } else if (s.substr(0,1) == "#") {
        x->type = '#';
        x->ch = s[1];
        x->c1 = stringtolambda(s.substr(3), new lambda('a'));
        x->c1->p = x;
        return x;
    } else {
        x->type = 'v';
        x->ch = s[0];
        return x;
    }
}

int main() {
    string s = "A";
    while (s != "") {
        cout << "> ";
        getline(cin, s);
        lambda *a = stringtolambda(s, new lambda('a'));
        a = reduce(a);
        cout << printlambda(a) << "\n";
    }

    return 0;
}
