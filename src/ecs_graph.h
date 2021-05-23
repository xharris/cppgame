#ifndef ECS_GRAPH_H
#define ECS_GRAPH_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <stdarg.h>

#include "engine.h"

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
  std::vector<std::reference_wrapper<Node>> children;
  float x, y, ox, oy, sx, sy, r, kx, ky;
  nodeid id;
  propsig signature;
  Node() : x(10), y(0), ox(0), oy(0), sx(1), sy(1), r(0), kx(0), ky(0) {
    id = uuid::generate();
  };
  // bool operator ==(const Node &b) const { return b.id == id; };
  Node(sol::table);
  void remove(propid);
  sol::object get(sol::stack_object, sol::this_state);
  void set(sol::stack_object, sol::stack_object, sol::this_state);
  void set(propid, sol::object);
  bool has(propid);
  Node& add(Node&);

  private:
  struct Matrix transform;
  bool isPermanentProp(const char*);
};

class System {
  public:
  static std::vector<System> systems;
  System(sol::table);
  propsig signature;
  umap<const char*, sol::function> callbacks;
  // nodes that fulfill prop requirement
  std::vector<nodeid> nodes;
  // check if node belongs/doesn't belong in all systems
  static void checkAll(Node&);

  private:
  void add(Node);
  // check if node belongs in this system
  bool check(Node&);
};

#endif 