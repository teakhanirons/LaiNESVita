#pragma once
#include <functional>
#include <string>
#include <vector>
#include "gui.hpp"

namespace GUI {


class Menu {
  public:
    void update(int a);
    void render();
    int *fb_base;
};

class FileMenu : public Menu {

  public:
    FileMenu();
};


}
