#include "ecs_graph.h"

#include <bitset>

const char* btos(propsig &sig)
{
  std::bitset<6> b(sig);
  return b.to_string().c_str();
}

int Prop::count = 0;
sigmap Prop::signatures;
umap<propid, umap<nodeid, Prop>> Prop::props;
std::vector<sptr<System>> System::systems;
umap<nodeid, sptr<Node>> Node::nodes;
Node Node::root;

Prop::Prop(propid k, sol::object v, nodeid id) 
{
  value = v;
  signature = Prop::getSignature(k);
  id = id;
  props[k].emplace(id, *this);
}

void Prop::set(propid k, sol::object v, Node& node)
{
  if (node.has(k))
  {
    props[k][node.id].value = v;
  }
  else 
  {
    Prop(k, v, node.id);
  }
}

void Prop::destroy(propid k, Node& node)
{
  if (node.has(k))
  {
    props[k].erase(node.id);
  }
}

propsig Prop::getSignature(propid key)
{
  propsig signature = 0;
  sigmap::iterator it = Prop::signatures.find(key);
  if (it != Prop::signatures.end())
  {
    signature = it->second;
  }
  // first time using prop, create a new signature for it
  else
  {
    signature = 1 << Prop::count;
    // TraceLog(LOG_INFO, "Prop(key=%s, signature=%s)", key, btos(signature));
    Prop::signatures.emplace(key, signature);
    Prop::count++;
  }
  return signature;
}

Node::Node() : signature(0), z(0), last_z(0), needs_sorting(false) {
  id = uuid::generate();
  root.add(*this);
  TraceLog(LOG_INFO, "Node(id=%d)", id);
}

Node::Node(sol::table props)
{
  id = uuid::generate();
  signature = 0;
  z = 0;
  last_z = 0;
  needs_sorting = false;

  // iterate components
  for (const auto& kv : props)
  {
    const char* key = kv.first.as<const char*>(); 
    set(key, kv.second.as<sol::object>());
  }
  root.add(*this);
  TraceLog(LOG_INFO, "Node(id=%d, signature=%s)", id, btos(signature));
}

bool Node::isPermanentProp(const char* key)
{
  return strlen(key) > 1 && strcmp(key, "id") == 0;
  // const char not_props1[0] = {};
  // const char *not_props2[1] = {"id"};
  // if (strlen(key) == 1)
  // {
  //   for (int p = 0; p < 0; p++)
  //   {
  //     if (not_props1[p] == key[0])
  //       return true;
  //   }
  // }
  // for (int p = 0; p < 1; p++)
  // {
  //   if (strcmp(not_props2[p], key) == 0)
  //     return true;
  // }
  // return false;
}

sol::object Node::get(propid key)
{
  return Prop::props[key][id].value;
}

sol::object Node::get(sol::stack_object key, sol::this_state state)
{
  propid property_id = key.as<propid>();
  if (has(property_id))
  { 
    return sol::object(state, sol::in_place, Prop::props[property_id][id].value);
  }
  return sol::object(state, sol::in_place, sol::nil);
}

void Node::set(sol::stack_object key, sol::stack_object val, sol::this_state state)
{
  set(key.as<propid>(), val);
}

void Node::set(propid key, sol::object val)
{
  if (!isPermanentProp(key))
  {
    propsig property_signature = Prop::getSignature(key);
    // add to signature
    if (val != sol::nil)
    {
      signature |= property_signature;
      Prop::set(key, val, *this);
    }
    // remove from signature
    else 
    {
      signature &= ~property_signature;
      TraceLog(LOG_DEBUG, "new signature Node(id=%d, sig=%s)", id, btos(signature));
      Prop::destroy(key, *this);
    }
    System::checkAll(*this);
  }
}

bool Node::has(propid key)
{
  // TraceLog(LOG_DEBUG, "node %d has %s? (%d, %d)", id, key, Prop::props.count(key), Prop::props[key].count(id));
  return Prop::props.count(key) > 0 && Prop::props[key].count(id) > 0;
}

bool Node::has(Node& node)
{
  auto it = std::find_if(children.begin(), children.end(), [&](const sptr<Node> &other){
    return id == other->id;
  });
  return it != children.end();
}

Node& Node::add(Node& node)
{
  if (&node != this && !has(node))
  {
    children.push_back(std::shared_ptr<Node>(&node));
    needs_sorting = true;
    last_z = z;
    // remove from last parent
    if (node.parent != NULL)
      node.parent->removeChild(node);
    node.parent = this;
  }
  return *this;
}

Node& Node::add(sol::table node_list)
{
  for (const auto& kv : node_list)
  {
    if (kv.second.is<Node>())
    {
      add(kv.second.as<Node>());
    }
  }
  return *this;
}

void Node::removeChild(Node& node)
{
  auto rem = std::remove_if(children.begin(), children.end(),
    [&node](sptr<Node>& n) { return n->id == node.id; });
  children.erase(rem, children.end());
}

void Node::removeChild(sol::table node_list)
{
  for (const auto& kv : node_list)
  {
    if (kv.second.is<Node>())
    {
      removeChild(kv.second.as<Node>());
    }
  }
}

void Node::clearRenderers()
{
  renderers.clear();
}

void Node::addRenderer(System& sys)
{
  if (sys.hasCallback("draw"))
  {
    renderers.push_back(
      std::make_tuple(sys.signature, sys.order, sys.callbacks["draw"])
    );
    std::sort(renderers.begin(), renderers.end(), 
      [](const std::tuple<propsig, int, sol::function> &a,
         const std::tuple<propsig, int, sol::function> &b)
         -> bool { return std::get<1>(a) < std::get<1>(b); });
  }
}

void Node::removeRenderer(System& sys)
{
  auto rem = std::remove_if(renderers.begin(), renderers.end(),
    [sys](auto const x) { return std::get<0>(x) == sys.signature; });
  renderers.erase(rem, renderers.end());
}

void Node::draw()
{
  // do the child nodes need to be sorted?
  if (needs_sorting)
  {
    std::sort(children.begin(), children.end(),
      [](sptr<Node>& a, sptr<Node>& b)
      -> bool { return a->z < b->z; } );
    needs_sorting = false;
  }
  rlPushMatrix();
  // do transformations
  // TraceLog(LOG_DEBUG, "draw node %d", id);
  if (has("Transform"))
  {
    sol::table t = get("Transform");
    TraceLog(LOG_DEBUG, "x %d", t['x'].get_or(0.f));
    rlScalef(t["sx"].get_or(0.f), t["sy"].get_or(0.f), t["sz"].get_or(0.f));
    rlRotatef(t["rx"].get_or(0.f), 1.f, 0.f, 0.f);
    rlRotatef(t["ry"].get_or(0.f), 0.f, 1.f, 0.f);
    rlRotatef(t["rz"].get_or(t['r'].get_or(0.f)), 0.f, 0.f, 1.f);
    rlTranslatef(t['x'].get_or(0.f), t['y'].get_or(0.f), t['z'].get_or(0.f));
  }
  // draw this node
  for (auto& rend : renderers)
  {
    std::get<2>(rend)(*this);
  }
  // iterate child nodes
  for (sptr<Node>& child : children)
  {
    child->draw();
  }
  rlPopMatrix();
}

System::System() : order(0), signature(0)
{}

System::System(sol::table t)
{
  order = 0;
  signature = 0;
  const char *sys_callbacks[] = {"create", "draw", "update", "destroy"};
  int len_callbacks = *(&sys_callbacks + 1) - sys_callbacks;
  
  for (const auto& kv : t)
  {
    // a property?
    if (kv.first.is<int>() && kv.second.is<const char*>())
    {
      signature |= Prop::getSignature(kv.second.as<const char*>());
    }
    // a system callback?
    else if (kv.first.is<const char*>() && kv.second.is<sol::function>())
    {
      for (int c = 0; c < len_callbacks; c++)
      {
        if (!strcmp(kv.first.as<const char*>(), sys_callbacks[c]))
        {
          callbacks.emplace(sys_callbacks[c], kv.second.as<sol::function>());
        }
      }
    }
  }

  // check ALL nodes 
  for (auto const& p : Node::nodes)
  {
    check(*p.second);
  }

  std::ostringstream o;
  o << signature;
  TraceLog(LOG_INFO, "System(signature=%s, nodes=%d)", btos(signature), nodes.size());
  for (const auto& kv : callbacks)
  {
    TraceLog(LOG_INFO, "\thas %s", kv.first);
  }
}

bool System::hasCallback(const char* callback)
{
  return callbacks.count(callback) > 0;
}

int System::check(Node& node)
{
  bool belongs = (bool)((signature & node.signature) == signature);
  auto it = std::find(nodes.begin(), nodes.end(), node.id);
  bool found = it != nodes.end();

  // TraceLog(LOG_DEBUG, "Checking node(id=%d,%s)", node.id, btos(node.signature));
  // TraceLog(LOG_DEBUG, "with system(%s)", btos(signature));
  // TraceLog(LOG_DEBUG, "belongs:%d, found:%d", belongs, found);

  // add node to system
  if (belongs && !found)
  {
    nodes.push_back(node.id);
    node.addRenderer(*this);
    return 1;
  }
  // remove node from system
  if (!belongs && found)
  {
    nodes.erase(it);
    node.removeRenderer(*this);
    return -1;
  }
  return 0;
}

bool System::contains(Node& node)
{
  return std::find(nodes.begin(), nodes.end(), node.id) != nodes.end();
}

void System::checkAll(Node& node)
{
  int adds = 0, removes = 0, status = 0;
  for (sptr<System> sys : systems)
  {
    status = sys->check(node);
    if (status == 1) adds++;
    if (status == -1) removes++;
  }

  if (adds > 0) TraceLog(LOG_INFO, "Node(id=%d) added to %d system%c", node.id, adds, adds > 1 ? 's' : ' ');
  if (removes > 0) TraceLog(LOG_INFO, "Node(id=%d) removed from %d system%c", node.id, removes, removes > 1 ? 's' : ' ');
}

void System::updateAll(float dt)
{
  for (sptr<System> sys : systems)
  {
    if (sys->hasCallback("update"))
    {
      sol::function fn = sys->callbacks["update"];
      // iterate nodes in system
      for (nodeid n : sys->nodes)
      {
        auto rs = fn(Node::nodes[n], dt);
        Error::check(rs);
      }
    }
  }
}

void System::drawAll()
{
  Node::root.draw();
}

void bind_ecs(sol::state& lua)
{
  sol::usertype<Node> node_type = lua.new_usertype<Node>("Entity",
    sol::call_constructor, sol::factories(
      []() -> sptr<Node> {
        auto node = std::make_shared<Node>();
        Node::nodes.emplace((*node).id, node);
        return node;
      },
      [](sol::table t) -> sptr<Node> {
        auto node = std::make_shared<Node>(t);
        Node::nodes.emplace((*node).id, node);
        return node;
      }
    ),
    sol::meta_function::index, sol::resolve<sol::object(sol::stack_object, sol::this_state)>(&Node::get),
    sol::meta_function::new_index, sol::resolve<void(sol::stack_object, sol::stack_object, sol::this_state)>(&Node::set),
    sol::meta_function::addition, sol::overload(
      sol::resolve<Node&(Node&)>(&Node::add),
      sol::resolve<Node&(const Node&)>(&Node::add),
      sol::resolve<Node&(sol::table)>(&Node::add)
    ),
    // sol::meta_function::equal_to, [](const Node& lhs, const Node& rhs) { return lhs == rhs; },
    "id", sol::readonly(&Node::id),
    "add", [](Node& self, sol::variadic_args va) {
      for (auto v : va) {
        Node node = v;
        self.add(node);
      }
    },
    "children", &Node::children
  );
  // root node
  sol::table t_node = lua["Entity"];
  t_node.set("root", &Node::root);

  sol::usertype<System> sys_type = lua.new_usertype<System>("System",
    sol::call_constructor, sol::factories(
      []() -> sptr<System> {
        auto sys = std::make_shared<System>();
        System::systems.push_back(sys);
        return sys;
      },
      [](sol::table t) -> sptr<System> {
        auto sys = std::make_shared<System>(t);
        System::systems.push_back(sys);
        return sys;
      }
    ),
    "contains", &System::contains
  );
}