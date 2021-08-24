#include "ecs_graph.h"

#include <bitset>

const char* btos(propsig &sig)
{
  std::bitset<6> b(sig);
  return b.to_string().c_str();
}

std::vector<sptr<System>> System::systems;
umaph<std::pair<entityid, propid>, EntityProp, hash_pair> EntityProp::props;
EntityList Entity::root_children;
umap<entityid, sptr<Entity>> Entity::entities;

EntityProp::EntityProp() {}

EntityProp::EntityProp(const EntityProp& ep)
  : id(ep.id), value(ep.value) {}

EntityProp::EntityProp(EntityProp&& ep)
  : id(ep.id), value(ep.value) {}

EntityProp::EntityProp(propid id, sol::object v)
  : id(id), value(v) {}

void EntityProp::operator=(const EntityProp& rhs)
{
  id = rhs.id;
  value = rhs.value;
}

EntityList::EntityList() {}

void EntityList::add(Entity& e)
{
  list.push_back(e.id);
}

void EntityList::remove(Entity& e)
{
  auto rem = std::remove_if(list.begin(), list.end(),
    [&e](entityid& id) { return e.id == id; });
  list.erase(rem, list.end());
}

Entity::Entity()
  : id(uuid::generate()), parent(0)
{
  TraceLog(LOG_DEBUG, "create %d", id);
  entities[id] = std::make_shared<Entity>(*this);
  root_children.add(*this);
}

template<typename... Prop>
Entity::Entity(Prop... prop)
  : id(uuid::generate()), parent(0)
{
  TraceLog(LOG_DEBUG, "create %d", id);
  entities[id] = std::make_shared<Entity>(*this);
  (set(prop), ...);
  root_children.add(*this);
}

template<typename... ArgEntity>
Entity& Entity::add(Entity& entity, ArgEntity... e)
{
  entity.setParent(*this);
  (add(e), ...);
  return *this;
}

void Entity::set(EntityProp prop)
{
  std::pair<entityid, propid> p(id, prop.id);
  EntityProp::props[p] = prop;
}

EntityProp& Entity::get(propid key)
{
  std::pair<entityid, propid> p(id, key);
  return EntityProp::props[p];
}

sptr<Entity> Entity::getParent()
{
  return entities[parent];
}

// set this entity's parent to p
void Entity::setParent(Entity& p)
{
  if (parent != p.id)
  {
    TraceLog(LOG_DEBUG, "%d removed from %d", id, parent);
    // remove from root
    if (!parent)
    {
      root_children.remove(*this);
    }
    // remove from current parent
    else
    {
      p.getParent()->removeChild(p);
    }
    p.children.add(*this);
    parent = p.id;
    TraceLog(LOG_DEBUG, "%d added to %d", id, p.id);
  }
}

void Entity::removeChild(Entity& c)
{
  children.remove(c);
}

void System::drawAll()
{

}

void System::updateAll(float dt)
{

}

struct CTransform {
  float x;
  float y;
};

void bind_ecs(sol::state& lua)
{  
  entt::registry reg;
  const auto entity = reg.create();
  reg.emplace<CTransform>(entity, 1.f, 1.f);
  
  // System(
  //   {"Transform"},
  //   {"draw", [](Entity &n){ TraceLog(LOG_DEBUG, "drawing %d (%p)", n.id, &n); }}
  // );

  // System::drawAll();
}