#ifndef DOUBLE_CLOSED_HPP
#define DOUBLE_CLOSED_HPP

#include <memory>
#include <vector>
#include "search.hpp"
#include "open_array.hpp"
#include "closed_open_address.hpp"
#include "tabulation.hpp"

/* Generic A* Search, using lazy duplicate detection (duplicate detection is
 * only done when nodes are popped from the open list.
*/

template <typename Node, typename Heuristic, typename HashFunction,
          size_t ClosedEntries = 512927357,
          typename Closed = ClosedOpenAddress<Node, HashFunction, ClosedEntries> >
struct DoubleClosed : public Search<Node> {

    Heuristic heuristic;
    OpenArray<Node> open;
    Closed closed;
    Closed closed2;
    size_t counter;

    // perform A* search and returns solution path
    std::vector<Node>
    search(Node initial_node) override final {

        evalH(initial_node, heuristic);
        ++Search<Node>::generated;
        open.push(std::move(initial_node));

        while (!open.empty()) {
            auto node = open.pop();
            if (closed2.insert(node)) ++counter;
            if (closed.insert(node)) {
                // check goal node
                if (isGoal(node)) {
                    return closed.getPath(node);
                }
                auto child_nodes = getChildNodes(node);
                ++Search<Node>::expanded;
                for (auto child_node : child_nodes) {
                    if (child_node.has_value()) {
                        ++Search<Node>::generated;
                        evalH(*child_node, heuristic);
                        open.push(std::move(*child_node));
                    }
                }
            }
        }
        return std::vector<Node>(); // no path found
    }

    std::ostream&  print(std::ostream& os) const override final {
        os << closed;
        os << "counter : " << counter;
        return os;
    }
};

#endif