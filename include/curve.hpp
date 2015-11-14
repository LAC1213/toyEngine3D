#ifndef CURVE_H
#define CURVE_H

#include <cmath>

template<class T>
class Curve
{
public:
    virtual void step( float dt )
    {
        _t += dt;
    }

    T getValue() const
    {
        return _value;
    }
    
    operator T()
    {
        return _value;
    }
    
protected:
    float _t;
    T _value;
};

template<class T>
class QuadraticCurve : public Curve<T>
{
public:
    QuadraticCurve() {}
    QuadraticCurve( T c, T l = T(), T q = T() ) : _constant(c), _linear(l), _quadratic(q) {
        this->_value = _constant;
    }
    
    virtual void step( float dt )
    {
        Curve<T>::step( dt );
        _linear += _quadratic*dt;
        this->_value += _linear*dt;
    }

    void setConstant( T c ) 
    {
        _constant = c;
        reset();
    }
    
    void setLinear( T l )
    {
        _linear = l;
        reset();
    }
    
    void setQuadratic( T q )
    {
        _quadratic = q;
        reset();
    }
    
    T getConstant() 
    {
        return _constant;
    }
    
    T getLinear()
    {
        return _linear;
    }
    
    T getQuadratic()
    {
        return _quadratic;
    }
    
private:
    T _constant;
    T _linear;
    T _quadratic;
    
    void reset()
    {
        this->_value = _constant;
        this->_t = 0;
    }
};

template<class T>
class SigmoidCurve : public Curve<T>
{
public:
    virtual void step( float dt )
    {
        Curve<T>::step( dt );
        this->_value = 1/(1 + exp(-this->_t));
    }
};

template<class T>
class SineCurve : public Curve<T>
{
public:
    virtual void step( float dt )
    {
        Curve<T>::step( dt );
        this->_value = _amplitude * sin( _phase + _frequency*2*M_PI*this->_t );
    }
private:
    T _frequency;
    T _phase;
    T _amplitude;
};

template<class T>
class SumCurve : public Curve<T>
{
public:
    virtual void step( float dt )
    {
        Curve<T>::step( dt );
        a->step( dt );
        b->step( dt );
        this->_value = a->getValue() + b->getValue();
    }
private:
    Curve<T> * a;
    Curve<T> * b;
};

template<class T>
class ProductCurve : public Curve<T>
{
public:
    virtual void step( float dt )
    {
        Curve<T>::step( dt );
        a->step( dt );
        b->step( dt );
        this->_value = a->getValue() * b->getValue();
    }
private:
    Curve<T> * a;
    Curve<T> * b;
    
};

#endif // CURVE_H