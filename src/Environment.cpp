#include "../inc/Environment.hpp"
#include "../inc/Creature.hpp"
#include "../inc/Egg.hpp"
#include "../inc/Food.hpp"
#include "../inc/Meat.hpp"

Environment::Environment() : pool(THREADS)
{
}

void Environment::destroy(){
  eggs     .srcEntities.clear();
  creatures.srcEntities.clear();
  foods    .srcEntities.clear();
  meats    .srcEntities.clear();

  eggs     .dstEntities.clear();
  creatures.dstEntities.clear();
  foods    .dstEntities.clear();
  meats    .dstEntities.clear();
}

template <> Environment::storagevectors<Creature> &Environment::getStorage<Creature>(){
  return creatures;
}

template <> Environment::storagevectors<Egg> &Environment::getStorage<Egg>(){
  return eggs;
}

template <> Environment::storagevectors<Food> &Environment::getStorage<Food>(){
  return foods;
}

template <> Environment::storagevectors<Meat> &Environment::getStorage<Meat>(){
  return meats;
}




template <typename T> void Environment::sortDstToSrc()
{
  storagevectors<T> &st = getStorage<T>();
  std::vector<int> & idx = st.idx;
  std::vector<T> &src = st.srcEntities;
  std::vector<T> &dst = st.dstEntities;
  
  //clear
  for(auto it = idx.begin(); it != idx.end(); ++it){
    *it = 0;
  }
  //accumulate
  for(auto it = dst.begin(); it != dst.end(); ++it){
    if(it->exists)
    {
      agl::Vec<int, 2> tile = toGridPosition(it->position);
      idx[(tile.x + tile.y*gridResolution.x)%idx.size()]++;
    }
  }
  //prefix sum
  int accumulated = 0;
  for(auto it = idx.begin(); it != idx.end(); ++it){
    int tmp = *it;
    *it = accumulated;
    accumulated += tmp;
  }
  
  //grow for any newcomers
  src.reserve(accumulated);
  while(src.size() < accumulated){
    src.emplace_back();
  }
  //sort
  for(auto it = dst.begin(); it != dst.end(); ++it){
    agl::Vec<int, 2> tile = toGridPosition(it->position);
    int index = idx[tile.x + tile.y*gridResolution.x];
    src[index] = std::move(*it);
    idx[tile.x + tile.y*gridResolution.x]++;
  }
  
  //shrink such that any new comers will overwrite the old entities
  dst.erase(dst.begin()+accumulated, dst.end());  
}


void Environment::sortAllDstToSrc()
{
  sortDstToSrc<Creature>();
  sortDstToSrc<Food>();
  sortDstToSrc<Meat>();
  sortDstToSrc<Egg>();
}