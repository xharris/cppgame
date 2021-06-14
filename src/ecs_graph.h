#ifndef ECS_GRAPH_H
#define ECS_GRAPH_H

#include <list>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <stdarg.h>
#include <string.h>
#include <tuple>

#include "sol.h"
#include "uuid.h"
#include "raylib.hpp"
#include "error.h"

typedef const char* propid;
typedef uint64_t propsig;
typedef uuid::uuid nodeid;
template <typename K,typename V> 
using umap = std::unordered_map<K,V>;
template <typename T>
using sptr = std::shared_ptr<T>;
typedef umap<propid, propsig> sigmap;

void bind_ecs(sol::state& lua);

class Node;
class System;

class Prop {
  public:
  static int count;
  static sigmap signatures;
  static umap<propid, umap<nodeid, Prop>> props;
  static propsig getSignature(propid);
  static void set(propid, sol::object, Node&);
  static void destroy(propid, Node&);

  Prop() {};
  Prop(propid, sol::object, nodeid);
  nodeid id;
  sol::object value;
  propsig signature;
};

class Node {
  public:
  static umap<nodeid, sptr<Node>> nodes;
  static Node root;

  Node();
  Node(sol::table);

  // PROPS

  std::vector<std::reference_wrapper<Node>> children;
  // pair<order, function>
  std::vector<std::tuple<propsig, int, sol::function>> renderers;
  nodeid id;
  propsig signature;
  int z;
  int last_z;
  Node* parent;

  // METHODS

  void remove(propid);
  sol::object get(sol::stack_object, sol::this_state);
  void set(sol::stack_object, sol::stack_object, sol::this_state);
  void set(propid, sol::object);
  bool has(propid);
  bool has(Node&);
  Node& add(Node&);
  Node& add(const Node& n) { return add(const_cast<Node&>(n)); }; // const_cast<Node&>(const_cast<const Node*>(this)->add(n)); };
  Node& add(sol::table);
  void removeChild(Node&);
  void removeChild(sol::table);
  void draw();
  void clearRenderers();
  void addRenderer(System&);
  void removeRenderer(System&);

  private:

  // PROPS

  struct Matrix transform;
  bool needs_sorting;

  // METHODS

  bool isPermanentProp(const char*);
};

class System {
  public:
  static std::vector<sptr<System>> systems;
  // check if node belongs/doesn't belong in any systems
  static void checkAll(Node&);
  static void updateAll(float);
  // TODO: deprecated
  static void drawAll();
  static System create(sol::table t) { return System(t); };

  System();
  System(sol::table);

  // PROPS

  propsig signature;
  umap<const char*, sol::function> callbacks;
  // nodes that fulfill prop requirement
  std::vector<nodeid> nodes;
  int order;

  // METHODS

  bool contains(Node& node);
  bool hasCallback(const char*);

  private:
  // check if node belongs in this system
  int check(Node&);
};

#endif 