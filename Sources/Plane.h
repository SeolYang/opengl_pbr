#pragma once
#include "Model.h"

class Plane : public Model
{
public:
   Plane(std::string_view name);

private:
   void Construct();

};
