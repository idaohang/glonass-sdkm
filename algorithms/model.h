#ifndef MODEL_H
#define MODEL_H

#include <boost/numeric/ublas/matrix.hpp>
#include "modelvisitor.h"
#include "string"

// Base estimating algotithms
class Estimator;

//!  Класс описывающий ММ, котоаря работает с Фильтрм Калмана
class Model
{
public:

    //!  Возвращает размер текущего вектора состояния
    virtual int get_state_size(unsigned t) = 0;

     //!  Возвращает размер текущего вектора измерений
    virtual int get_observation_size(unsigned t) = 0;

     //!  Возвращает ветор состояния на шаг t
    virtual bool get_data(boost::numeric::ublas::matrix<double> &data, unsigned t) = 0;

    //!  Матрица Ф для шага t
    virtual boost::numeric::ublas::matrix<double> get_F(unsigned t) = 0;    

    //!  Матрица H для шага t
    virtual boost::numeric::ublas::matrix<double> get_H(unsigned t) = 0;

    //!  Матрица H для шага t и текущего вектора состояния cur_state (для нелинейных моделей)
    virtual boost::numeric::ublas::matrix<double> get_H(boost::numeric::ublas::matrix<double> cur_state, unsigned t) = 0;

    //!  Возвращает посчитанный вектор измерений для шага t и текущего вектора состояния cur_state
    virtual boost::numeric::ublas::matrix<double> get_Y(boost::numeric::ublas::matrix<double> cur_state, unsigned t) = 0;

    //!  Матрица D
    virtual boost::numeric::ublas::matrix<double> get_D(unsigned t) = 0;

    //!  Матрица Q
    virtual boost::numeric::ublas::matrix<double> get_Q(unsigned t) = 0;

    // Initial Conditions
    //!  начальыне условия -- вектор состояния
    virtual boost::numeric::ublas::matrix<double> get_initial_state() = 0;

    //!  начальные условия -- матрица ковариации
    virtual boost::numeric::ublas::matrix<double> get_initial_cov() = 0;

    // Needed ikf observable components are changing
    //!  Возвращает актуальный вектор состояния для шага t
    virtual boost::numeric::ublas::matrix<double> actual_state(boost::numeric::ublas::matrix<double> cur_state, unsigned t) = 0;

    //!  Возвращает актуальную матрицу ковариации для шага t
    virtual boost::numeric::ublas::matrix<double> actual_cov(boost::numeric::ublas::matrix<double> cur_state, unsigned t) = 0;

    // Output visitors
    void add_visitor(ModelVisitor * visitor);
    virtual void write_est_data(boost::numeric::ublas::matrix<double> cur_state, unsigned t) = 0;

    Model(Estimator *est);
    void Run();

protected:

    //!  Алгоритм апостериорной обработки
    Estimator *estimator;
    std::vector<ModelVisitor *> visitors;
};

//!  Абстрактный класс, описывающий алгоритм апостериороной обработки данных
class Estimator
{
    public:
        virtual void Run() = 0;
        Model *model;
};

#endif // MODEL_H
