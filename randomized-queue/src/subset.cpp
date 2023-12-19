#include "subset.h"

#include "randomized_queue.h"

void subset(unsigned long k, std::istream & in, std::ostream & out)
{
    std::string line;
    randomized_queue<std::string> queue;

    while (std::getline(in, line)) {
        queue.enqueue(line);
    }

    while (!queue.empty() && k--) {
        out << queue.dequeue() << std::endl;
    }
}