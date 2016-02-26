#pragma once

#include <iostream>

#include <functional>
#include <stack>
#include <vector>

namespace caprica { namespace papyrus {

namespace expressions { struct PapyrusExpression; }
namespace statements { struct PapyrusStatement; }

enum class PapyrusControlFlowNodeEdgeType
{
  None,
  Continue,
  Break,
  Return,
  /**
   * All children of this node are edges.
   */
  Children,
};

struct PapyrusControlFlowNode final
{
  int id{ };
  PapyrusControlFlowNodeEdgeType edgeType{ PapyrusControlFlowNodeEdgeType::None };
  std::vector<const statements::PapyrusStatement*> statements{ };
  std::vector<PapyrusControlFlowNode*> children{ };
  PapyrusControlFlowNode* nextSibling{ nullptr };

  PapyrusControlFlowNode() {
    static int i = 0;
    id = i++;
  }
  ~PapyrusControlFlowNode() {
    for (auto c : children)
      delete c;
    if (nextSibling)
      delete nextSibling;
  }

  void iterate(int depth);
};

struct PapyrusCFG final
{
  PapyrusCFG() : root(new PapyrusControlFlowNode()) {
    nodeStack.push(root);
  }
  ~PapyrusCFG() {
    if (root)
      delete root;
  }

  bool processCommonLoopBody(const std::vector<statements::PapyrusStatement*>& stmts) {
    pushBreakTerminal();
    addLeaf();
    bool wasTerminal = processStatements(stmts);
    bool isTerminal = !popBreakTerminal() && wasTerminal;
    if (isTerminal)
      terminateNode(PapyrusControlFlowNodeEdgeType::Children);
    else
      createSibling();
    return isTerminal;
  }
  bool processStatements(const std::vector<statements::PapyrusStatement*>& stmts);

  void appendStatement(const statements::PapyrusStatement* stmt) {
    nodeStack.top()->statements.push_back(stmt);
  }

  void terminateNode(PapyrusControlFlowNodeEdgeType tp) {
    nodeStack.top()->edgeType = tp;
    nodeStack.pop();
  }

  void addLeaf() {
    auto n = new PapyrusControlFlowNode();
    nodeStack.top()->children.push_back(n);
    nodeStack.push(n);
  }

  void createSibling() {
    auto n = new PapyrusControlFlowNode();
    nodeStack.top()->nextSibling = n;
    nodeStack.pop();
    nodeStack.push(n);
  }

  void debugDump() {
    root->iterate(0);
  }

  void pushBreakTerminal() {
    breakTargetStack.push(false);
  }

  void markBreakTerminal() {
    breakTargetStack.top() = true;
  }

  bool popBreakTerminal() {
    bool b = breakTargetStack.top();
    breakTargetStack.pop();
    return b;
  }

private:
  PapyrusControlFlowNode* root{ nullptr };
  std::stack<PapyrusControlFlowNode*> nodeStack{ };
  std::stack<bool> breakTargetStack{ };
};

}}
