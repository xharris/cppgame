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

Node::Node() : x(10), y(0), ox(0), oy(0), sx(1), sy(1), r(0), kx(0), ky(0), signature(0) {
  id = uuid::generate();
  TraceLog(LOG_INFO, "Node(id=%d)", id);
}

Node::Node(sol::table props)
{
  id = uuid::generate();
  x = props['x'].get_or(0);
  y = props['y'].get_or(0);
  ox = props["ox"].get_or(0);
  oy = props["oy"].get_or(0);
  sx = props["sx"].get_or(1);
  sy = props["sy"].get_or(1);
  r = props['r'].get_or(0);
  kx = props["kx"].get_or(0);
  ky = props["ky"].get_or(0);
  signature = 0;

  // iterate components
  for (const auto& kv : props)
  {
    const char* key = kv.first.as<const char*>(); 
    set(key, kv.second.as<sol::object>());
  }
  TraceLog(LOG_INFO, "Node(id=%d, signature=%s)", id, btos(signature));
}

bool Node::isPermanentProp(const char* key)
{
  const char not_props1[3] = {'x','y','r'};
  const char *not_props2[7] = {"ox", "oy", "sx", "sy", "kx", "ky", "id"};
  if (strlen(key) == 1)
  {
    for (int p = 0; p < 3; p++)
    {
      if (not_props1[p] == key[0])
        return true;
    }
  }
  for (int p = 0; p < 7; p++)
  {
    if (strcmp(not_props2[p], key) == 0)
      return true;
  }
  return false;
}

sol::object Node::get(sol::stack_object key, sol::this_state state)
{
  propid property_id = key.as<propid>();
  if (!isPermanentProp(property_id) && has(property_id))
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
  return Prop::props.find(key) != Prop::props.end() && Prop::props[key].find(id) != Prop::props[key].end();
}

Node& Node::add(Node& node)
{
  int _id = id;
  auto it = std::find_if(children.begin(), children.end(), [&_id](const std::reference_wrapper<Node> &other){
    return _id == other.get().id;
  });
  if (it == children.end())
  {
    children.push_back(std::ref(node));
    TraceLog(LOG_DEBUG, "add %d to %d", node.id, _id);
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

System::System(sol::table t)
{
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
    TraceLog(LOG_DEBUG, "add");
    nodes.push_back(node.id);
    return 1;
  }
  // remove node from system
  if (!belongs && found)
  {
    TraceLog(LOG_DEBUG, "remove");
    nodes.erase(it);
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
  for (sptr<System> sys : systems)
  {
    if (sys->hasCallback("draw"))
    {
      sol::function fn = sys->callbacks["draw"];
      // iterate nodes in system
      for (nodeid n : sys->nodes)
      {
        auto rs = fn(Node::nodes[n]);
        Error::check(rs);
      }
    }
  }
}

void bind_ecs(sol::state& lua)
{
  sol::usertype<Node> node_type = lua.new_usertype<Node>("Node",
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
    sol::meta_function::index, &Node::get,
    sol::meta_function::new_index, sol::resolve<void(sol::stack_object, sol::stack_object, sol::this_state)>(&Node::set),
    sol::meta_function::addition, sol::overload(
      sol::resolve<Node&(Node&)>(&Node::add),
      sol::resolve<Node&(const Node&)>(&Node::add),
      sol::resolve<Node&(sol::table)>(&Node::add)
    ),
    // sol::meta_function::equal_to, [](const Node& lhs, const Node& rhs) { return lhs == rhs; },
    "id", sol::readonly(&Node::id),
    "x", &Node::x, "y", &Node::y, "ox", &Node::ox, "oy", &Node::oy,
    "sx", &Node::sx, "sy", &Node::sy, "r", &Node::r, "kx", &Node::kx, "ky", &Node::ky,
    "add", [](Node& self, sol::variadic_args va) {
      for (auto v : va) {
        Node node = v;
        self.add(node);
      }
    }
  );

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