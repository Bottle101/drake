#ifndef _PENDULUM_H_
#define _PENDULUM_H_

#include <iostream>
#include <cmath>
#include "DrakeSystem.h"
//#include "LCMCoordinateFrame.h"
//#include "BotVisualizer.h"

using namespace std;

template <typename ScalarType = double>
class PendulumState  { // models the Drake::Vector concept
public:
  PendulumState(void) : theta(0), thetadot(0) {};
  PendulumState(const Eigen::Matrix<ScalarType,2,1>& x) : theta(x(0)), thetadot(x(1)) {};

  operator Eigen::Matrix<ScalarType,2,1> () const {
    Eigen::Matrix<ScalarType,2,1> x;
    x << theta, thetadot;
    return x;
  }

  friend std::ostream& operator<<(std::ostream& os, const PendulumState& x)
  {
    os << "  theta = " << x.theta << endl;
    os << "  thetadot = " << x.thetadot << endl;
    return os;
  }

  static std::size_t size() { return 2; }
  static const std::size_t size_at_compile = 2;

  ScalarType theta;
  ScalarType thetadot;
};

template <template<typename> class Vector>
struct traits
{
  enum {
    RowsAtCompileTime = 2
  };
};

template <typename ScalarType = double>
class PendulumInput {
public:
  PendulumInput(void) : tau(0) {};
  PendulumInput(const Eigen::Matrix<ScalarType,1,1>& x) : tau(x(0)) {};

  operator Eigen::Matrix<ScalarType,1,1> () const {
    Eigen::Matrix<ScalarType,1,1> x;
    x << tau;
    return x;
  }

  friend std::ostream& operator<<(std::ostream& os, const PendulumInput& x)
  {
    os << "  tau = " << x.tau << endl;
    return os;
  }

  static unsigned int size() { return 1; }
  static const std::size_t size_at_compile = 1;

  ScalarType tau;
};

class Pendulum : public Drake::System<Pendulum,PendulumState,PendulumInput,PendulumState> {
public:
  Pendulum() :
          m(1.0), // kg
          l(.5),  // m
          b(0.1), // kg m^2 /s
          lc(.5), // m
          I(.25), // m*l^2; % kg*m^2
          g(9.81) // m/s^2
  {}
  virtual ~Pendulum(void) {};

//  virtual bool isTimeInvariant() const override { return true; }
//  virtual bool isDirectFeedthrough() const override { return false; }

/*
  DrakeSystemPtr balanceLQR() {
    Eigen::MatrixXd Q(2,2);  Q << 10, 0, 0, 1;
    Eigen::MatrixXd R(1,1);  R << 1;
    Eigen::VectorXd xG(2);   xG << M_PI, 0;
    Eigen::VectorXd uG(1);   uG << 0;

    return timeInvariantLQR(xG,uG,Q,R);
  }
*/

  template <typename ScalarType>
  PendulumState<ScalarType> dynamicsImplementation(ScalarType t, const PendulumState<ScalarType>& x, const PendulumInput<ScalarType>& u) const {
    PendulumState<ScalarType> dot;
    dot.theta = x.thetadot;
    dot.thetadot = (u.tau - m*g*lc*sin(x.theta) - b*x.thetadot)/I;
    return dot;
  }


  template <typename ScalarType>
  PendulumState<ScalarType> outputImplementation(ScalarType t, const PendulumState<ScalarType>& x, const PendulumInput<ScalarType>& u) const {
    return x;
  }

private:
  double m,l,b,lc,I,g;  // pendulum parameters (initialized in the constructor)
};

/*
class PendulumWithBotVis : public Pendulum {
public:
  PendulumWithBotVis(const std::shared_ptr<lcm::LCM>& lcm) : Pendulum(lcm), botvis(lcm,"Pendulum.urdf",DrakeJoint::FIXED) {}

  virtual Eigen::VectorXd output(double t, const Eigen::VectorXd& x, const Eigen::VectorXd& u) const override {
    botvis.output(t,Eigen::VectorXd::Zero(0),x);
    return Pendulum::output(t,x,u);
  }

  BotVisualizer botvis;
};

class PendulumEnergyShaping : public DrakeSystem {
public:
  PendulumEnergyShaping(const Pendulum& pendulum)
          : DrakeSystem("PendulumEnergyShaping"),
            m(pendulum.m),
            l(pendulum.l),
            b(pendulum.b),
            g(pendulum.g)
  {
    input_frame = pendulum.output_frame;
    output_frame = pendulum.input_frame;
  };

  virtual Eigen::VectorXd output(double t, const Eigen::VectorXd& unused, const Eigen::VectorXd& u) const override {
    double Etilde = .5 * m*l*l*u(1)*u(1) - m*g*l*cos(u(0)) - 1.1*m*g*l;
    Eigen::VectorXd y(1);
    y << b*u(1) - .1*u(1)*Etilde;
    return y;
  }

  virtual bool isTimeInvariant() const override { return true; }
  virtual bool isDirectFeedthrough() const override { return true; }

  double m,l,b,g;  // pendulum parameters (initialized in the constructor)
};
*/

#endif // _PENDULUM_H_
