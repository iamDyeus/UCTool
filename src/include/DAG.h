#pragma once
#include <string>
#include <vector>
#include <memory>

// DAG node for expression optimization
struct DAGNode {
    std::string op;
    std::string value;
    std::vector<std::shared_ptr<DAGNode>> children;
    std::string result;
    int id;
    DAGNode(const std::string& o, const std::string& v, int i);
};