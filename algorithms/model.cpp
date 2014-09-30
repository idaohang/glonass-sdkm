#include "model.h"

Model::Model(Estimator *est)
{
    this->estimator = est;
    this->estimator->model = this;
}

void Model::Run()
{
    this->estimator->Run();
}

void Model::add_visitor(ModelVisitor * visitor)
{
    this->visitors.push_back(visitor);
}

