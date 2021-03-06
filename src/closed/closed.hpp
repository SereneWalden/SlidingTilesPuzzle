#ifndef CLOSED_HPP
#define CLOSED_HPP

#include <unordered_set>
#include <vector>
#include <algorithm>
#include <ostream>

/* Minimal Closed List Implementation
 * Requires node to have default hasher for equality comparison.
 */
template <typename Node>
struct Closed {

    std::unordered_set<Node> closed;

    // returns true if node needs to be expanded,
    // insert node if not already exist in closed, or if lower f-val than
    // existing closed node
    bool insert(Node node);

    // given node, return path in closed list by tracing parent nodes
    // assumes node is in the closed list; otherwise returns empty path
    std::vector<Node> getPath(Node const &node) const;
};

template <typename Node>
bool Closed<Node>::insert(Node node) {
    auto found = closed.find(node);
    if (found != closed.end()) {
        if (getF(node) >= getF(*found)) {
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
        auto parent = getParent(*found);
        if (!parent.has_value()) {
            break;
        }
        found = closed.find(*parent);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename Node>
std::ostream &operator<<(std::ostream& os, Closed<Node> const & closed) {
    os <<  "closed list load factor: "
       << closed.closed.load_factor() << "\n";
    return os;
}

#endif
