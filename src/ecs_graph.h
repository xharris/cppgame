#ifndef ECS_GRAPH_H
#define ECS_GRAPH_H

#include <list>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <stdarg.h>
#include <string.h>

#include "sol.h"
#include "uuid.h"
#include "raylib.hpp"

typedef const char* propid;
typedef uint64_t propsig;
typedef uuid::uuid nodeid;
template <typename K,typename V> 
using umap = std::unordered_map<K,V>;
typedef umap<propid, propsig> sigmap;

void bind_ecs(sol::state& lua);

class Node;

class Prop {
  public:
  static int count;
  static sigmap signatures;
  static umap<propid, umap<nodeid, Prop>> props;
  static propsig getSignature(propid);
  static void set(propid, sol::object, Node&);

  Prop() {};
  Prop(propid, sol::object, nodeid);
  nodeid id;
  sol::object value;
  propsig signature;
};

class Node {
  public:
  static umap<nodeid, std::shared_ptr<Node>> nodes;

  Node();
  Node(sol::table);
  // props
  std::vector<std::reference_wrapper<Node>> children;
  float x, y, ox, oy, sx, sy, r, kx, ky;
  nodeid id;
  propsig signature;
  // methods
  void remove(propid);
  sol::object get(sol::stack_object, sol::this_state);
  void set(sol::stack_object, sol::stack_object, sol::this_state);
  void set(propid, sol::object);
  bool has(propid);
  Node& add(Node&);
  Node& add(const Node& n) { return add(const_cast<Node&>(n)); }; // const_cast<Node&>(const_cast<const Node*>(this)->add(n)); };
  Node& add(sol::table);

  private:
  // PROPS
  struct Matrix transform;
  // METHODS
  bool isPermanentProp(const char*);
};

class System {
  public:
  static std::vector<System> systems;
  // check if node belongs/doesn't belong in any systems
  static void checkAll(Node&);
  static void updateAll();
  static void drawAll();
  static System create(sol::table t) { return System(t); };

  System() {};
  System(sol::table);
  // PROPS
  propsig signature;
  umap<const char*, sol::function> callbacks;
  // nodes that fulfill prop requirement
  std::vector<nodeid> nodes;

  private:
  // check if node belongs in this system
  bool check(Node&);
};

#endif 