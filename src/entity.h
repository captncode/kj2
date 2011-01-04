#pragma once

template <class T>
class Countable
{
public:
  Countable() {
    count++;
  }
  Countable( const Countable & r ) {
    count++;
  }
  ~Countable() {
    count--;
  }
  unsigned int getCount() { return count; };
protected:

private:
  static unsigned int count;
}; // koniec Countable

template<class T>
unsigned int Countable<T>::count = 0;


template <class T>
class UniqueComparableID
{
  friend class Entity;
public:
  UniqueComparableID() {
    id = last++;
  }
  UniqueComparableID( const UniqueComparableID & r ): id( r.id ) {
    //last++;
    //id = last;
  }

  ~UniqueComparableID() {
    //id = 0;
  }
  unsigned int getLast() const {
    return last;
  }
  unsigned int getId() const {
    return id;
  }
  /*! w przypadku gdy obiekt jest towrzony przez konstruktor kopiujacy,
      mozna tego uzyc aby otrzymac nowe id;
  */
  void getNewId() {
    id = last++;
  }

  bool operator < ( const UniqueComparableID & r ) const {
    return this->id < r.id;
  }
  bool operator == ( const UniqueComparableID & r ) const {
    return this->id == r.id;
  }

protected:

private:
  static unsigned int last;
  unsigned int id;
}; // koniec UniqueComparableID

template<class T>
unsigned int UniqueComparableID<T>::last = 0;

template<class T>
class Singielton
{
protected:
  Singielton() {}
public:
  static T & inst() {
    static T instance;
    return instance;
  }
  ~Singielton() {}
}; // koniec Singielton

//class Entity : public UniqueComparableID<Entity>
//{
//  friend class Game;
//protected:
//  Entity() {}
//public:
//  ~Entity() {}
//
//  //potencjalnie niebezpieczny konstruktor
//  explicit Entity( int i )
//  {
//    --UniqueComparableID<Entity>::last;
//    uint32_t & last = UniqueComparableID<Entity>::last;
////nalezy tu uwazac
////    if( i > last ){
////      last = i;
////    }
//    this->id = i;
//  }
//}; // koniec Entity

template <uint32_t I>
class EntityT
{
  friend class Game;
//  template<uint32_t K>
//    friend EntityT<K> createEntityT();

public:
  EntityT()
  {
    id = 0;
    saveIt = true;
  }
  explicit EntityT( uint32_t e )
  {
    id = e;
    saveIt = true;
  }

  EntityT( const EntityT<I>& r ) : id( r.id ),saveIt(r.saveIt) {};
  ~EntityT() {};

  uint32_t getId() const  { return id; }

  bool operator < ( const EntityT<I> & r ) const {
    return this->id < r.id;
  }
  bool operator == ( const EntityT<I> & r ) const {
    return this->id == r.id;
  }
  bool operator != ( const EntityT<I> & r ) const {
    return this->id != r.id;
  }

protected:
  uint32_t id : 31;
public:
  uint32_t saveIt : 1;
  //static uint32_t last;
};

//template<uint32_t I>
//uint32_t EntityT<I>::last = 0;

typedef EntityT<0> Entity;

typedef Entity     *    Entity_p;
typedef const Entity  & Entity_cref;

template<int I>
inline
EntityT<I> readEntityTFormText(const char *line ){
  uint32_t e =0;
  sscanf(line,"%u",&e);
  assert(e);
  return EntityT<I>(e);
}

inline
Entity readEntityFormText(const char *line ){
  uint32_t e =0;
  sscanf(line,"%u",&e);
  //assert(e);
  return Entity(e);
}

inline
const char * getEntityAsText(const Entity e)
{
  static char tmp [50];
  memset(tmp,0,sizeof(tmp) );
  sprintf(tmp,"%u",e.getId() );
  return tmp;
}
