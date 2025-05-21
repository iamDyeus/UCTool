#include "DAG.h"

DAGNode::DAGNode(const std::string& o, const std::string& v, int i)
    : op(o), value(v), result(""), id(i) {}