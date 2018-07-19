#ifndef CLOSED_HPP
#define CLOSED_HPP

#include <unordered_set>
#include <vector>
#include <algorithm>

// closed list
// requires node to have default hasher for equality comparison
template <typename Node>
struct Closed {

    std::unordered_set<Node> closed;

    // returns true if node needs to be expanded,
    // insert node if not already exist in closed, or if lower f-val than
    // existing closed node
    bool insert(Node node);

    std::vector<Node> getPath(Node const &node) const;
};

template <typename Node>
bool Closed<Node>::insert(Node node) {
    auto found = closed.find(node);
    if (found != closed.end()) {
        if (node.getF() >= found->getF()) {
            return false;
        }
        closed.erase(found); // reopening
    }
    closed.insert(std::move(node));
    return true;
}

template <typename Node>
std::vector<Node> Closed<Node>::getPath(Node const &node) const {
    std::vector<Node> path;
    auto found = closed.find(node);
    while (found != closed.end()) {
        path.push_back(*found);
        auto parent_ptr = found->getParentPtr();
        if (!parent_ptr) {
            break;
        }
        found = closed.find(*parent_ptr);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

#endif
