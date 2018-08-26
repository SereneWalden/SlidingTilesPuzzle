#ifndef ASTAR_POOL_HPP
#define ASTAR_POOL_HPP

#include <memory>
#include <vector>
#include "MemoryPool.h"
#include "search.hpp"
#include "open_array_ptr.hpp"
#include "closed_open_address_ptr.hpp"

/* A* Search, using lazy duplicate detection (duplicate detection is only done
 * when nodes are popped from the open list.
 * 
 * In addition, a memory pool is used for node allocation. This allows for a
 * more compact closed list (holds pointers instead of nodes), but incurs an
 * additional pointer for each node in the open and closed list.
 *
 * Requires versions of the open and closed list that stores pointers instead of
 * nodes.
 */
template <typename Node, typename Heuristic,
          typename HashFunction, size_t ClosedEntries = 512927357,
          typename Closed = ClosedOpenAddressPtr<Node, HashFunction, ClosedEntries>,
          typename Open = OpenArrayPtr<Node> >
struct AStarPool : public Search<Node> {

    Open open;
    Closed closed;
    Heuristic  heuristic;
    MemoryPool<Node> pool; // memory pool

    // perform A* search and returns solution path
    std::vector<Node>
    search(Node initial_node) override final {
        auto initial_node_ptr = pool.newElement(initial_node);

        evalH(*initial_node_ptr, heuristic);
        ++Search<Node>::generated;
        open.push(initial_node_ptr);

        while (!open.empty()) {
            auto node_ptr = open.pop();

            if (closed.insert(node_ptr)) { // need to expand
                // check goal node
                if (isGoal(*node_ptr)) {
                    return closed.getPath(node_ptr);
                }
                auto child_nodes = getChildNodes(*node_ptr);
                ++Search<Node>::expanded;
                for (auto child_node : child_nodes) {
                    // push all valid child nodes into open
                    if (child_node.has_value()) {
                        ++Search<Node>::generated;
                        auto child_node_ptr =
                            pool.newElement(child_node.value());
                        evalH(*child_node_ptr, heuristic);
                        open.push(child_node_ptr);
                    }
                }
            } else {
                pool.deleteElement(node_ptr); // duplicate in closed, deallocate
            }
        }
        return std::vector<Node>(); // no path found
    }

    std::ostream& print(std::ostream& os) const override final {
        os << closed;
        return os;
    }
};

#endif
