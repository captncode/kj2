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
public:
  UniqueComparableID() {
    //bez zera
    id = ++last;
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
    id = ++last;
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

class Entity : public UniqueComparableID<Entity>
{
public:
  Entity() {}
  ~Entity() {}
}; // koniec Entity
typedef Entity    *     Entity_p;
typedef const Entity &  Entity_cref;

