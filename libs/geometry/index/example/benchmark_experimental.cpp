// Boost.Geometry Index
// Additional tests

// Copyright (c) 2011-2013 Adam Wulkiewicz, Lodz, Poland.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_GEOMETRY_INDEX_DETAIL_EXPERIMENTAL
#define BOOST_GEOMETRY_INDEX_DETAIL_ENABLE_TYPE_ERASED_ITERATORS

#include <iostream>

#include <boost/chrono.hpp>
#include <boost/foreach.hpp>
#include <boost/random.hpp>

#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/segment.hpp>

namespace bg = boost::geometry;
namespace bgi = bg::index;

typedef bg::model::point<double, 2, bg::cs::cartesian> P;
typedef bg::model::box<P> B;
typedef bg::model::linestring<P> LS;
typedef bg::model::segment<P> S;

template <typename I1, typename I2, typename O>
void mycopy(I1 first, I2 last, O o)
{
    for ( ; first != last ; ++o, ++first )
        *o = *first;
}

//#define BOOST_GEOMETRY_INDEX_BENCHMARK_DEBUG

int main()
{
    typedef boost::chrono::thread_clock clock_t;
    typedef boost::chrono::duration<float> dur_t;

#ifndef BOOST_GEOMETRY_INDEX_BENCHMARK_DEBUG
    size_t values_count = 1000000;
    size_t queries_count = 100000;
    size_t nearest_queries_count = 10000;
    unsigned neighbours_count = 10;
    size_t path_queries_count = 2000;
    size_t path_queries_count2 = 10000;
    unsigned path_values_count = 10;
#else
    size_t values_count = 1000;
    size_t queries_count = 1;
    size_t nearest_queries_count = 1;
    unsigned neighbours_count = 10;
    size_t path_queries_count = 1;
    size_t path_queries_count2 = 1;
    unsigned path_values_count = 10;
#endif

    float max_val = static_cast<float>(values_count / 2);
    std::vector< std::pair<float, float> > coords;
    std::vector<B> values;

    //randomize values
    {
        boost::mt19937 rng;
        //rng.seed(static_cast<unsigned int>(std::time(0)));
        boost::uniform_real<float> range(-max_val, max_val);
        boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > rnd(rng, range);

        coords.reserve(values_count);

        std::cout << "randomizing data\n";
        for ( size_t i = 0 ; i < values_count ; ++i )
        {
            float x = rnd();
            float y = rnd();
            coords.push_back(std::make_pair(x, y));
            values.push_back(B(P(x - 0.5f, y - 0.5f), P(x + 0.5f, y + 0.5f)));
        }
        std::cout << "randomized\n";
    }

    //typedef bgi::rtree<B, bgi::linear<100, 50> > RT;
    //typedef bgi::rtree<B, bgi::quadratic<8, 3> > RT;
    typedef bgi::rtree<B, bgi::rstar<8, 3> > RT;

    std::cout << "sizeof rtree: " << sizeof(RT) << std::endl;

    for (;;)
    {
        std::vector<B> result;
        result.reserve(100);
        B result_one;

        // packing test
        {
            clock_t::time_point start = clock_t::now();

            RT t(values.begin(), values.end());

            dur_t time = clock_t::now() - start;
            std::cout << time << " - pack " << values_count << '\n';

            {
                clock_t::time_point start = clock_t::now();
                size_t temp = 0;
                for (size_t i = 0 ; i < queries_count ; ++i )
                {
                    float x = coords[i].first;
                    float y = coords[i].second;
                    result.clear();
                    t.query(bgi::intersects(B(P(x - 10, y - 10), P(x + 10, y + 10))), std::back_inserter(result));
                    temp += result.size();
                }
                dur_t time = clock_t::now() - start;
                std::cout << time << " - query(B) " << queries_count << " found " << temp << '\n';
            }
        }
        
        RT t;

        // inserting test
        {
            clock_t::time_point start = clock_t::now();
            t.insert(values);
            dur_t time = clock_t::now() - start;
            std::cout << time << " - insert " << values_count << '\n';
        }

        

        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < queries_count ; ++i )
            {
                float x = coords[i].first;
                float y = coords[i].second;
                result.clear();
                t.query(bgi::intersects(B(P(x - 10, y - 10), P(x + 10, y + 10))), std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - query(B) " << queries_count << " found " << temp << '\n';
        }

#ifdef BOOST_GEOMETRY_INDEX_DETAIL_EXPERIMENTAL
        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < queries_count ; ++i )
            {
                float x = coords[i].first;
                float y = coords[i].second;
                result.clear();
                std::copy(
                    t.qbegin(bgi::intersects(B(P(x - 10, y - 10), P(x + 10, y + 10)))),
                    t.qend(bgi::intersects(B(P(x - 10, y - 10), P(x + 10, y + 10)))),
                    std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - qbegin(B) qend(B) " << queries_count << " found " << temp << '\n';
        }
        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < queries_count ; ++i )
            {
                float x = coords[i].first;
                float y = coords[i].second;
                result.clear();
                mycopy(
                    t.qbegin(bgi::intersects(B(P(x - 10, y - 10), P(x + 10, y + 10)))),
                    t.qend(),
                    std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - qbegin(B) qend() " << queries_count << " found " << temp << '\n';
        }
#ifdef BOOST_GEOMETRY_INDEX_DETAIL_ENABLE_TYPE_ERASED_ITERATORS
        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < queries_count ; ++i )
            {
                float x = coords[i].first;
                float y = coords[i].second;
                result.clear();
                RT::const_query_iterator first = t.qbegin(bgi::intersects(B(P(x - 10, y - 10), P(x + 10, y + 10))));
                RT::const_query_iterator last = t.qend(bgi::intersects(B(P(x - 10, y - 10), P(x + 10, y + 10))));
                std::copy(first, last, std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - type-erased qbegin(B) qend(B) " << queries_count << " found " << temp << '\n';
        }
#endif
#endif

        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < queries_count / 2 ; ++i )
            {
                float x1 = coords[i].first;
                float y1 = coords[i].second;
                float x2 = coords[i+1].first;
                float y2 = coords[i+1].second;
                float x3 = coords[i+2].first;
                float y3 = coords[i+2].second;
                result.clear();
                t.query(
                    bgi::intersects(B(P(x1 - 10, y1 - 10), P(x1 + 10, y1 + 10)))
                    &&
                    !bgi::within(B(P(x2 - 10, y2 - 10), P(x2 + 10, y2 + 10)))
                    &&
                    !bgi::overlaps(B(P(x3 - 10, y3 - 10), P(x3 + 10, y3 + 10)))
                    ,
                    std::back_inserter(result)
                    );
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - query(i && !w && !o) " << queries_count << " found " << temp << '\n';
        }

        result.clear();

        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < nearest_queries_count ; ++i )
            {
                float x = coords[i].first + 100;
                float y = coords[i].second + 100;
                result.clear();
                temp += t.query(bgi::nearest(P(x, y), neighbours_count), std::back_inserter(result));
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - query(nearest(P, " << neighbours_count << ")) " << nearest_queries_count << " found " << temp << '\n';
        }

#ifdef BOOST_GEOMETRY_INDEX_DETAIL_EXPERIMENTAL
        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < nearest_queries_count ; ++i )
            {
                float x = coords[i].first + 100;
                float y = coords[i].second + 100;
                result.clear();
                std::copy(
                    t.qbegin(bgi::nearest(P(x, y), neighbours_count)),
                    t.qend(bgi::nearest(P(x, y), neighbours_count)),
                    std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - qbegin(nearest(P, " << neighbours_count << ")) qend(n) " << nearest_queries_count << " found " << temp << '\n';
        }
        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < nearest_queries_count ; ++i )
            {
                float x = coords[i].first + 100;
                float y = coords[i].second + 100;
                result.clear();
                mycopy(
                    t.qbegin(bgi::nearest(P(x, y), neighbours_count)),
                    t.qend(),
                    std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - qbegin(nearest(P, " << neighbours_count << ")) qend() " << nearest_queries_count << " found " << temp << '\n';
        }
#ifdef BOOST_GEOMETRY_INDEX_DETAIL_ENABLE_TYPE_ERASED_ITERATORS
        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < nearest_queries_count ; ++i )
            {
                float x = coords[i].first;
                float y = coords[i].second;
                result.clear();
                RT::const_query_iterator first = t.qbegin(bgi::nearest(P(x, y), neighbours_count));
                RT::const_query_iterator last = t.qend(bgi::nearest(P(x, y), neighbours_count));
                std::copy(first, last, std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - type-erased qbegin(nearest(P, " << neighbours_count << ")) qend(n) " << nearest_queries_count << " found " << temp << '\n';
        }
#endif

        {
            LS ls;
            ls.resize(6);
            
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < path_queries_count ; ++i )
            {
                float x = coords[i].first;
                float y = coords[i].second;
                for ( int i = 0 ; i < 3 ; ++i )
                {
                    float foo = i*max_val/300;
                    ls[2*i] = P(x, y+foo);
                    ls[2*i+1] = P(x+max_val/100, y+foo);
                }                
                result.clear();
                t.query(bgi::path(ls, path_values_count), std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - query(path(LS6, " << path_values_count << ")) " << path_queries_count << " found " << temp << '\n';
        }

        {
            LS ls;
            ls.resize(2);

            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < path_queries_count2 ; ++i )
            {
                float x = coords[i].first;
                float y = coords[i].second;
                ls[0] = P(x, y);
                ls[1] = P(x+max_val/100, y+max_val/100);
                result.clear();
                t.query(bgi::path(ls, path_values_count), std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - query(path(LS2, " << path_values_count << ")) " << path_queries_count2 << " found " << temp << '\n';
        }

        {
            clock_t::time_point start = clock_t::now();
            size_t temp = 0;
            for (size_t i = 0 ; i < path_queries_count2 ; ++i )
            {
                float x = coords[i].first;
                float y = coords[i].second;
                S seg(P(x, y), P(x+max_val/100, y+max_val/100));
                result.clear();
                t.query(bgi::path(seg, path_values_count), std::back_inserter(result));
                temp += result.size();
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - query(path(S, " << path_values_count << ")) " << path_queries_count2 << " found " << temp << '\n';
        }
#endif
        {
            clock_t::time_point start = clock_t::now();
            for (size_t i = 0 ; i < values_count / 10 ; ++i )
            {
                float x = coords[i].first;
                float y = coords[i].second;
                B b(P(x - 0.5f, y - 0.5f), P(x + 0.5f, y + 0.5f));

                t.remove(b);
            }
            dur_t time = clock_t::now() - start;
            std::cout << time << " - remove " << values_count / 10 << '\n';
        }

        std::cout << "------------------------------------------------\n";
    }

    return 0;
}
