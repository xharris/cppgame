#include "ecs_graph.h"

int Prop::count = 0;
sigmap Prop::signatures;
umap<propid, umap<nodeid, Prop>> Prop::props;
umap<nodeid, Node> Node::nodes;
std::vector<System> System::systems;

Prop::Prop(propid k, sol::object v, nodeid id) {
  value = v;
  signature = Prop::getSignature(k);
  id = id;
  props[k][id] = *this;
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
  Node node;
  TraceLog(LOG_DEBUG, "here we are");
  node.x = props['x'].get_or(0);
  node.y = props['y'].get_or(0);
  node.ox = props["ox"].get_or(0);
  node.oy = props["oy"].get_or(0);
  node.sx = props["sx"].get_or(1);
  node.sy = props["sy"].get_or(1);
  node.r = props['r'].get_or(0);
  node.kx = props["kx"].get_or(0);
  node.ky = props["ky"].get_or(0);

  // iterate components
  for (const auto& kv : props)
  {
    const char* key = kv.first.as<const char*>(); 
    TraceLog(LOG_DEBUG, "add property", key);
    node.set(key, kv.second.as<sol::object>());
  }
}

Node Node::create()
{
  nodeid id = uuid::generate();
  nodes[id] = Node();
  nodes[id].id = id;
  return nodes[id];
}

Node Node::from_table(sol::table props)
{
  nodeid id = uuid::generate();
  nodes[id] = Node(props);
  nodes[id].id = id;
  return nodes[id];
}

bool Node::isPermanentProp(const char* key)
{
  const char not_props1[3] = {'x','y','r'};
  const char *not_props2[7] = {"ox", "oy", "sx", "sy", "kx", "ky", "id"};
  bool permanent = false;
  for (int p = 0; p < 3; p++)
  {
    if (not_props1[p] == key[0])
      permanent = true;
  }
  for (int p = 0; p < 7; p++)
  {
    if (strcmp(not_props2[p], key))
      permanent = true;
  }
  return permanent;
}

sol::object Node::get(sol::stack_object key, sol::this_state state)
{
  propid property_id = key.as<propid>();
  if (!isPermanentProp(property_id) && has(property_id))
  { 
    return sol::object(state, sol::in_place, Prop::props[property_id][id].value);
  }
  // TODO: how to return default property (x, y, etc..)?
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
      Prop(key, val, id);
    }
    // remove from signature
    else 
      signature &= ~property_signature;
    System::checkAll(*this);
  }
}

bool Node::has(propid key)
{
  return Prop::props.find(key) != Prop::props.end() && Prop::props[key].find(id) != Prop::props[key].end();
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
  sol::usertype<Node> node_type = lua.new_usertype<Node>("node",
    sol::call_constructor, // sol::constructors<Node(), Node(sol::table)>(),
    sol::factories(
      &Node::create,
      &Node::from_table
    ),
    sol::meta_function::index, &Node::get,
    sol::meta_function::new_index, sol::resolve<void(sol::stack_object, sol::stack_object, sol::this_state)>(&Node::set),
    "id", sol::readonly(&Node::id),
    "x", &Node::x, "y", &Node::y, "ox", &Node::ox, "oy", &Node::oy,
    "sx", &Node::sx, "sy", &Node::sy, "r", &Node::r, "kx", &Node::kx, "ky", &Node::ky
  );

  sol::usertype<System> sys_type = lua.new_usertype<System>("system",
    sol::call_constructor,
    sol::factories(
      [](sol::table t){ return System(t); }
    )
  );
}