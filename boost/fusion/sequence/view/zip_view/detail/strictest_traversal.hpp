/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman
    Copyright (c) 2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_STRICTEST_TRAVERSAL_20060123_2101)
#define FUSION_STRICTEST_TRAVERSAL_20060123_2101

#include <boost/mpl/or.hpp>
#include <boost/mpl/if.hpp>
#include <boost/fusion/support/category_of.hpp>
#include <boost/fusion/sequence/intrinsic/mpl.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_convertible.hpp>

namespace boost { namespace fusion
{
    struct forward_traversal_tag;
    struct bidirectional_traversal_tag;
    struct random_access_traversal_tag;

    namespace detail
    {
        template<typename Tag1, typename Tag2,
            bool Tag1Stricter = boost::is_convertible<Tag2,Tag1>::value>
        struct stricter_traversal
        {
            typedef Tag1 type;
        };

        template<typename Tag1, typename Tag2>
        struct stricter_traversal<Tag1,Tag2,false>
        {
            typedef Tag2 type;
        };

        struct strictest_traversal_impl
        {
            template<typename Sig>
            struct result;

            template<typename Next, typename StrictestSoFar>
            struct result<strictest_traversal_impl(Next, StrictestSoFar)>
            {
                typedef typename remove_reference<Next>::type next_value;
                typedef typename remove_reference<StrictestSoFar>::type strictest_so_far;

                typedef strictest_so_far tag1;
                typedef typename traits::category_of<next_value>::type tag2;

                typedef typename stricter_traversal<tag1,tag2>::type type;
            };
        };

        template<typename Sequence>
        struct strictest_traversal
            : result_of::fold<
            Sequence, fusion::random_access_traversal_tag,
            strictest_traversal_impl>
        {};

    }
}}

#endif
