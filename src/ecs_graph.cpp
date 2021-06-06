#include "ecs_graph.h"

int Prop::count = 0;
sigmap Prop::signatures;
umap<propid, umap<nodeid, Prop>> Prop::props;
std::vector<System> System::systems;

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

propsig Prop::getSignature(propid key)
{
  propsig signature;
  sigmap::iterator it = Prop::signatures.find(key);
  if (it != Prop::signatures.end())
  {
    signature = it->second;
    Prop::count++;
  }
  else
  {
    signature = 1 << Prop::count;
  }
  return signature;
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

  // iterate components
  for (const auto& kv : props)
  {
    const char* key = kv.first.as<const char*>(); 
    set(key, kv.second.as<sol::object>());
  }
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
    // a system callback?
    for (int c = 0; c < len_callbacks; c++)
    {
      if (strcmp(kv.first.as<const char*>(), sys_callbacks[c]))
        callbacks.emplace(sys_callbacks[c], kv.second.as<sol::function>());
    }
    // a property?
    if (kv.second.is<const char*>())
    {
      signature |= Prop::getSignature(kv.second.as<const char*>());
    }
  }
}

bool System::check(Node& node)
{
  bool belongs = (bool)(signature & node.signature);
  auto found = std::find(nodes.begin(), nodes.end(), node.id);
  // add node to system
  if (belongs && found == nodes.end())
    nodes.push_back(node.id);
  // remove node from system
  if (!belongs && found != nodes.end())
    nodes.erase(found);

  return belongs; 
}

void System::checkAll(Node& node)
{
  for (System sys : systems)
  {
    sys.check(node);
  }
}

void bind_ecs(sol::state& lua)
{
  sol::usertype<Node> node_type = lua.new_usertype<Node>("Node",
    sol::call_constructor, sol::constructors<Node(), Node(sol::table)>(),
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

  lua.set_function("System", [](sol::table t){ return System(t); });
  // sol::usertype<System> sys_type = lua.new_usertype<System>("System",
  //   sol::call_constructor, sol::constructors<System(sol::table)>()
  //   // sol::factories(
  //   //   [](sol::table t){ return System(t); }
  //   // )
  // );
}