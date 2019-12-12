#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct node {
    int ssn, y, m, d;
    double balance;
    string name;
    node *parent, *left, *right;

    void print() {
        if (ssn == 0) {
            cout << "AVL node is empty.\n";
            return;
        }
        cout << name << " " << ssn << " ";
        cout << y << " " << m << " " << d << " " << balance << "\n";
    }

    // Passed a line from database.txt, gets its properties and set
    // the relevant fields of the struct. Uses stringstream b/c
    // that's the easiest way I know to split the line by spaces.
    void read(string s) {
        string word;
        stringstream stream(s);
        int c = 0;
        vector<string> v;
        while (getline(stream, word, ' '))
            v.push_back(word);
        ssn = atoi(v.at(0).c_str()); d = atoi(v.at(1).c_str()); m = atoi(v.at(2).c_str());
        y = atoi(v.at(3).c_str()); name = v.at(4) + " " + v.at(5);
        balance = atof(v.at(6).c_str());
    }
};

int height(node * c) {
    if (c == NULL)
        return 0;
    if (c->left == NULL && c->right == NULL)
        return 1;
    int l = height(c->left), r = height(c->right);
    if (l > r)
        return 1 + l;
    return 1 + r;

}

int balancefactor(node * c) {
    return  height(c->right) - height(c->left);
}

node * right(node * x, node * z) {
    node *t2 = z->right;
    x->left = t2;
    if (t2 != NULL)
        t2->parent = x;
    z->right = x;
    x->parent = z;
    return z;
}

node * left(node * x, node * z) {
    node *t2 = z->left;
    x->right = t2;
    if (t2 != NULL)
        t2->parent = x;
    z->left = x;
    x->parent = z;
    return z;
}

node * rightleft(node * x, node * z) {
    node *y = z->left, *t2 = y->left, *t3 = y->right;
    z->left = t3;
    if (t3 != NULL)
        t3->parent = z;
    y->right = z;
    z->parent = y;
    x->right = t2;
    if (t2 != NULL)
        t2->parent = x;
    y->left = x;
    x->parent = y;
    return y;
}

node * leftright(node * x, node * z) {
    node *y = z->right, *t2 = y->left, *t3 = y->right;
    z->right = t2;
    if (t2 != NULL)
        t2->parent = z;
    y->left = z;
    z->parent = y;
    x->left = t3;
    if (t3 != NULL)
        t3->parent = x;
    y->right = x;
    x->parent = y;
    return y;
}


node * balance(node * x) {
    int bf = balancefactor(x);
    node * r = NULL, *p = x->parent;
    string s;
    if (p != NULL) {
        if (p->left == x)
            s = "left";
        else if (p->right == x)
            s = "right";
    }

    if (bf == -2) {
        int bz = balancefactor(x->left);
        if (bz == 1) // leftright
            r = leftright(x, x->left);
        else if (bz == -1) { // simple right
            r = right(x, x->left);
        }
    } else if (bf == 2) {
        int bz = balancefactor(x->right);
        if (bz == 1) // simple left
            r = left(x, x->right);
        else if (bz == -1) // rightleft
            r = rightleft(x, x->right);
    }

    if (p != NULL && r != NULL) {
        if (s == "left")
            p->left = r;
        else if (s == "right")
            p->right = r;
        r->parent = p;
        balance(r->parent);
    }
    if (p != NULL)
        balance(p);
}

class avl {
public:
    node * top;

    avl(node * t) {
        top = t;
    }

    void print(node * n = NULL, int f = 0) {
        if (n == NULL)
            n = top;
        if (n->left != NULL)
            print(n->left, f+1);
        for (int i = 0; i < f; i++)
            cout << "\t";
        cout << n->name << "(H" << height(n) << " B" << balancefactor(n) << ")\n";
        if (n->right != NULL)
            print(n->right, f+1);
    }

    // Add a node to the tree. If not passed a node to add to
    // defaults to the top node.
    void add(node * n, node * c = NULL) {
        if (c == NULL)
            c = top;
        if (n->name < c->name && c->left != NULL)
            add(n, c->left);
        else if (n->name > c->name && c->right != NULL)
            add(n, c->right);
        else if (n->name < c->name && c->left == NULL) {
            c->left = n;
            n->parent = c;
            balance(c);
            if (top->parent != NULL) {
                top = top->parent;
                top->parent = NULL;
            }
        } else if (n->name > c->name && c->right == NULL) {
            c->right = n;
            n->parent = c;
            balance(c);
            if (top->parent != NULL) {
                top = top->parent;
                top->parent = NULL;
            }
        }
    }

    node * findn(string name, node * c = NULL) {
        if (c == NULL)
            c = top;
        if (c->name == name)
            return c;

        if (name < c->name && c->left != NULL)
            return findn(name, c->left);
        if (name > c->name && c->right != NULL)
            return findn(name, c->right);
        return NULL;
    }

    bool verify(node * c = NULL) {
        if (c == NULL)
            c = top;
        bool x = true, y = true, z = true;
        if (c->left != NULL)
            y = verify(c->left);
        if (balancefactor(c) > 1 || balancefactor(c) < -1) {
            cout << "error, node " << c->name << " has balance of " << balancefactor(c) << "\n";
            x = false;
        }
        if (c->right != NULL)
            z = verify(c->right);
        if (x == false || y == false || z == false)
            return false;
        return true;
    }

    int getheight() {
        return height(top);
    }
};

int main() {
    string s;
    ifstream file("database.txt");
    getline(file, s);

    node * top = new node();
    top->read(s);
    avl * A = new avl(top);

    while (getline(file, s)) {
        node * t = new node();
        t->read(s);
        A->add(t);
    }

    while (s != "exit") {
        cout << "> ";
        cin >> s;
        if (s.substr(0, 4) == "find") {
            string f, l;
            cin >> f >> l;
            string name = f + " " + l;

            node * x = A->findn(name);
            if (x != NULL)
                x->print();
            else
                cout << "Person not found.\n";
        } else if (s == "verify") {
            bool c = A->verify();
            if (c == true)
                cout << "All nodes have acceptable balance factor.\n";
            else
                cout << "One or more nodes had incorrect balance factors.\n";
            cout << "Height: " << A->getheight() << "\n";
        }
    }

    return 0;
}
