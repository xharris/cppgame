#ifndef ECS_GRAPH_H
#define ECS_GRAPH_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <stdarg.h>

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
  Node() : x(10), y(0), ox(0), oy(0), sx(1), sy(1), r(0), kx(0), ky(0) {
    id = uuid::generate();
  };
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

  System() {};
  System(sol::table);
  // PROPS
  propsig signature;
  umap<const char*, sol::function> callbacks;
  // nodes that fulfill prop requirement
  std::vector<nodeid> nodes;

  private:
  void add(Node);
  // check if node belongs in this system
  bool check(Node&);
};

#endif 