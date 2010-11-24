#ifndef MAP_H
#define MAP_H

//#include <component.h>


class Map// : public BaseComponent<>
{
  public:
    Map();
    uint32_t load(const char * name);
    uint32_t save(const char * name);
    void generate();

    virtual ~Map();
  protected:
  private:
};

#endif // MAP_H
