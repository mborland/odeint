/*
 [auto_generated]
 libs/numeric/odeint/test/step_size_limitation.cpp

 [begin_description]
 Tests the step size limitation functionality
 [end_description]

 Copyright 2015 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define BOOST_TEST_MODULE odeint_integrate_times

#include <boost/test/unit_test.hpp>

#include <utility>
#include <iostream>
#include <vector>

#include <boost/numeric/odeint.hpp>

using namespace boost::unit_test;
using namespace boost::numeric::odeint;

typedef double value_type;
typedef std::vector< value_type > state_type;


void lorenz( const state_type &x , state_type &dxdt , const value_type t )
{
    BOOST_CHECK( t >= 0.0 );

    const value_type sigma( 10.0 );
    const value_type R( 28.0 );
    const value_type b( value_type( 8.0 ) / value_type( 3.0 ) );

    dxdt[0] = sigma * ( x[1] - x[0] );
    dxdt[1] = R * x[0] - x[1] - x[0] * x[2];
    dxdt[2] = -b * x[2] + x[0] * x[1];
}

struct push_back_time
{
    std::vector< double >& m_times;

    push_back_time( std::vector< double > &times )
    :  m_times( times ) { }

    void operator()( const state_type &x , double t )
    {
        m_times.push_back( t );
    }
};

BOOST_AUTO_TEST_SUITE( step_size_limitation_test )

BOOST_AUTO_TEST_CASE( test_error_checker )
{
    // first use checker without step size limitation
    default_error_checker<double, double, range_algebra, default_operations> checker;
    const double dt = 0.1;
    double dt_new = checker.decrease_step(dt, 1.5, 2);
    BOOST_CHECK(dt_new < dt*2.0/3.0);

    dt_new = checker.increase_step(dt, 0.8, 1);
    // for errors > 0.5 no increase is performed
    BOOST_CHECK(dt_new == dt);

    dt_new = checker.increase_step(dt, 0.4, 1);
    // smaller errors should lead to step size increase
    std::cout << dt_new << std::endl;
    BOOST_CHECK(dt_new > dt);


    // now test with step size limitation max_dt = 0.1
    default_error_checker<double, double, range_algebra, default_operations>
        limited_checker(1E-6, 1E-6, 1, 1, dt);

    dt_new = limited_checker.decrease_step(dt, 1.5, 2);
    // decreasing works as before
    BOOST_CHECK(dt_new < dt*2.0/3.0);

    dt_new = limited_checker.decrease_step(2*dt, 1.1, 2);
    // decreasing a large step size should give max_dt
    BOOST_CHECK(dt_new == dt);

    dt_new = limited_checker.increase_step(dt, 0.8, 1);
    // for errors > 0.5 no increase is performed, still valid
    BOOST_CHECK(dt_new == dt);

    dt_new = limited_checker.increase_step(dt, 0.4, 1);
    // but even for smaller errors, we should at most get 0.1
    BOOST_CHECK(dt_new == dt);

    dt_new = limited_checker.increase_step(0.9*dt, 0.1, 1);
    std::cout << dt_new << std::endl;
    // check that we don't increase beyond max_dt
    BOOST_CHECK(dt_new == dt);
}


BOOST_AUTO_TEST_CASE( test_controlled )
{
    state_type x( 3 );
    x[0] = x[1] = x[2] = 10.0;
    const double max_dt = 0.01;

    std::vector<double> times;

    integrate_adaptive(make_controlled(1E-2, 1E-2, max_dt, runge_kutta_dopri5<state_type>()),
                       lorenz, x, 0.0, 1.0, max_dt, push_back_time(times));
    // check that dt remains at exactly max_dt
    for( size_t i=0 ; i<times.size() ; ++i )
        // check if observer was called at times 0,1,2,...
        BOOST_CHECK_SMALL( times[i] - static_cast<double>(i)*max_dt , 1E-15);
    times.clear();

    // this should also work when we provide some bigger initial dt
    integrate_adaptive(make_controlled(1E-2, 1E-2, max_dt, runge_kutta_dopri5<state_type>()),
                       lorenz, x, 0.0, 1.0, 0.1, push_back_time(times));
    // check that dt remains at exactly max_dt
    for( size_t i=0 ; i<times.size() ; ++i )
    // check if observer was called at times 0,1,2,...
    BOOST_CHECK_SMALL( times[i] - static_cast<double>(i)*max_dt , 1E-15);
    times.clear();

    // check this behavior with cash karp
    integrate_adaptive(make_controlled(1E-2, 1E-2, max_dt, runge_kutta_cash_karp54<state_type>()),
                       lorenz, x, 0.0, 1.0, max_dt, push_back_time(times));
    // check that dt remains at exactly max_dt
    for( size_t i=0 ; i<times.size() ; ++i )
        // check if observer was called at times 0,1,2,...
        BOOST_CHECK_SMALL( times[i] - static_cast<double>(i)*max_dt , 1E-15);
    times.clear();

    // this should also work when we provide some bigger initial dt
    integrate_adaptive(make_controlled(1E-2, 1E-2, max_dt, runge_kutta_cash_karp54<state_type>()),
                       lorenz, x, 0.0, 1.0, 0.1, push_back_time(times));
    // check that dt remains at exactly max_dt
    for( size_t i=0 ; i<times.size() ; ++i )
        // check if observer was called at times 0,1,2,...
        BOOST_CHECK_SMALL( times[i] - static_cast<double>(i)*max_dt , 1E-15);
    times.clear();

}



BOOST_AUTO_TEST_CASE( test_dense_out )
{
    state_type x( 3 );
    x[0] = x[1] = x[2] = 10.0;
    const double max_dt = 0.01;

    std::vector<double> times;

    integrate_adaptive(make_dense_output(1E-2, 1E-2, max_dt, runge_kutta_dopri5<state_type>()),
                      lorenz, x, 0.0, 1.0, max_dt, push_back_time(times));
    // check that dt remains at exactly max_dt
    for( size_t i=0 ; i<times.size() ; ++i )
        // check if observer was called at times 0,1,2,...
        BOOST_CHECK_SMALL( times[i] - static_cast<double>(i)*max_dt , 1E-15);
    times.clear();

    // this should also work when we provide some bigger initial dt
    integrate_adaptive(make_dense_output(1E-2, 1E-2, max_dt, runge_kutta_dopri5<state_type>()),
                       lorenz, x, 0.0, 1.0, 0.1, push_back_time(times));
    // check that dt remains at exactly max_dt
    for( size_t i=0 ; i<times.size() ; ++i )
        // check if observer was called at times 0,1,2,...
        BOOST_CHECK_SMALL( times[i] - static_cast<double>(i)*max_dt , 1E-15);
    times.clear();
}

BOOST_AUTO_TEST_SUITE_END()
